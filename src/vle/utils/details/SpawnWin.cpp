/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <tchar.h>

#include <Windows.h>

#include <vle/utils/details/UtilsWin.hpp>

#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <strsafe.h>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/details/ShellUtils.hpp>
#include <vle/utils/details/UtilsWin.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace utils {

/* Max default size of command line buffer. See the CreateProcess function
 * in msdn. */
const unsigned long int Spawn::default_buffer_size = 32768;

/* Envp is used to store the environment variables in utf-8. */
using Envp = std::vector<std::pair<std::string, std::string>>;

static std::wstring
win32_quote(const std::wstring& arg)
{
    if (not arg.empty() and arg.find_first_of(L" \t\n\v\"") == arg.npos)
        return arg;

    std::wstring result;
    result += L'"';

    for (auto it = arg.begin();; ++it) {
        unsigned NumberBackslashes = 0;

        while (it != arg.end() and *it == L'\\') {
            ++it;
            ++NumberBackslashes;
        }

        if (it == arg.end()) {
            result.append(NumberBackslashes * 2, L'\\');
            break;
        } else if (*it == '"') {
            result.append(NumberBackslashes * 2 + 1, L'\\');
            result.push_back(*it);
        } else {
            result.append(NumberBackslashes, L'\\');
            result.push_back(*it);
        }
    }

    result += L'"';

    return result;
}

static std::wstring
build_command_line(const Path& exe, const std::vector<std::string>& args)
{
    std::wstring ret;
    ret.reserve(Spawn::default_buffer_size);

    ret = win32_quote(exe.wstring());
    ret += L' ';

    for (auto elem : args) {
        ret += win32_quote(from_utf8_to_wide(elem));
        ret += L' ';
    }

    /* Remove the latest ` ' to avoid bad variable string. */
    ret.resize(ret.size() - 1);

    return ret;
}

struct Spawn::Pimpl
{
    ContextPtr m_context;
    HANDLE hOutputRead;
    HANDLE hErrorRead;

    PROCESS_INFORMATION m_pi;
    DWORD m_status;
    std::string m_msg;
    std::chrono::milliseconds m_waitchildtimeout;
    bool m_start;
    bool m_finish;

    std::ofstream ouputfs, errorfs;

    Pimpl(ContextPtr ctx, std::chrono::milliseconds waitchildtimeout)
      : m_context(ctx)
      , hOutputRead(INVALID_HANDLE_VALUE)
      , hErrorRead(INVALID_HANDLE_VALUE)
      , m_status(0)
      , m_waitchildtimeout(waitchildtimeout)
      , m_start(false)
      , m_finish(true)
    {
        utils::Path tmp(utils::Path::temp_directory_path());

        utils::Path file_out(tmp);
        file_out /= "vlespawn-out.txt";
        ouputfs.open(file_out.string());

        utils::Path file_err(tmp);
        file_err /= "vlespawn-error.txt";
        errorfs.open(file_err.string());
    }

    void init(std::chrono::milliseconds waitchildtimeout)
    {
        if (m_start and not m_finish)
            wait();

        hOutputRead = INVALID_HANDLE_VALUE;
        hErrorRead = INVALID_HANDLE_VALUE;
        m_status = 0;
        m_waitchildtimeout = waitchildtimeout;

        m_finish = true;
        m_start = false;
    }

    ~Pimpl()
    {
        if (m_start) {
            if (not m_finish)
                wait();
        }
    }

    bool is_running()
    {
        if (m_start == false or m_finish == true)
            return false;

        if (GetExitCodeProcess(m_pi.hProcess, &m_status)) {
            if (m_status == STILL_ACTIVE) {
                m_finish = false;
                return true;
            }
        }

        m_finish = true;
        return false;
    }

    bool get(std::string* output, std::string* error)
    {
        assert(m_start);

        is_running();

        const auto size = 4096;
        auto buffer = std::make_unique<char[]>(size);
        unsigned long bread;
        unsigned long avail;

        {
            PeekNamedPipe(
              hOutputRead, buffer.get(), size - 1, &bread, &avail, NULL);

            if (bread) {
                ouputfs << "get PeekNamedPipe success " << bread << " "
                        << avail << "\n";

                std::fill(buffer.get(), buffer.get() + size, '\0');
                if (avail > size - 1) {
                    ouputfs << "get PeekNamedPipe " << avail << "\n"
                            << size - 1 << "\n";
                    while (bread >= size - 1) {
                        ReadFile(
                          hOutputRead, buffer.get(), size - 1, &bread, NULL);

                        ouputfs << "get: " << bread << "\n";

                        if (bread > 0) {
                            unsigned long sz = std::min(
                              bread, static_cast<unsigned long>(size - 1));

                            output->append(buffer.get(), sz);
                            ouputfs.write(buffer.get(), sz);
                        }

                        std::fill(buffer.get(), buffer.get() + size, '\0');
                    }
                } else {
                    ouputfs << "get PeekNamedPipe success (else) " << bread
                            << " " << avail << "\n";

                    ReadFile(
                      hOutputRead, buffer.get(), size - 1, &bread, NULL);

                    ouputfs << "get: " << bread << "\n";

                    if (bread > 0) {
                        unsigned long sz = std::min(
                          bread, static_cast<unsigned long>(size - 1));

                        output->append(buffer.get(), sz);
                        ouputfs.write(buffer.get(), sz);
                    }
                }
            }
        }

        {
            PeekNamedPipe(
              hErrorRead, buffer.get(), size - 1, &bread, &avail, NULL);

            if (bread) {
                errorfs << "[err] get PeekNamedPipe success " << avail << "\n";

                std::fill(buffer.get(), buffer.get() + size, '\0');
                if (avail > size - 1) {
                    while (bread >= size - 1) {
                        ReadFile(
                          hErrorRead, buffer.get(), size - 1, &bread, NULL);

                        if (bread > 0) {
                            unsigned long sz = std::min(
                              bread, static_cast<unsigned long>(size - 1));

                            error->append(buffer.get(), sz);
                            errorfs.write(buffer.get(), sz);
                        }

                        std::fill(buffer.get(), buffer.get() + size, '\0');
                    }
                } else {
                    ReadFile(hErrorRead, buffer.get(), size - 1, &bread, NULL);

                    if (bread > 0) {
                        unsigned long sz = std::min(
                          bread, static_cast<unsigned long>(size - 1));

                        error->append(buffer.get(), sz);
                        errorfs.write(buffer.get(), sz);
                    }
                }
            }
        }

        return true;
    }

    bool start(const Path& exe,
               const Path& workingdir,
               const std::vector<std::string>& args)
    {
        m_start = true;
        m_finish = false;

        HANDLE hOutputReadTmp = INVALID_HANDLE_VALUE;
        HANDLE hErrorReadTmp = INVALID_HANDLE_VALUE;
        HANDLE hOutputWrite = INVALID_HANDLE_VALUE;
        HANDLE hErrorWrite = INVALID_HANDLE_VALUE;
        STARTUPINFOW startupinfo;
        SECURITY_ATTRIBUTES securityatt;
        SECURITY_DESCRIPTOR securitydescriptor;

        InitializeSecurityDescriptor(&securitydescriptor,
                                     SECURITY_DESCRIPTOR_REVISION);

        SetSecurityDescriptorDacl(&securitydescriptor, TRUE, NULL, FALSE);
        securityatt.lpSecurityDescriptor = &securitydescriptor;
        securityatt.nLength = sizeof(SECURITY_ATTRIBUTES);
        securityatt.bInheritHandle = TRUE;

        if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &securityatt, 0) ||
            !DuplicateHandle(GetCurrentProcess(),
                             hOutputReadTmp,
                             GetCurrentProcess(),
                             &hOutputRead,
                             0,
                             FALSE,
                             DUPLICATE_SAME_ACCESS))
            return false;

        if (!CreatePipe(&hErrorReadTmp, &hErrorWrite, &securityatt, 0) ||
            !DuplicateHandle(GetCurrentProcess(),
                             hErrorReadTmp,
                             GetCurrentProcess(),
                             &hErrorRead,
                             0,
                             TRUE,
                             DUPLICATE_SAME_ACCESS)) {
            CloseHandle(hOutputReadTmp);
            CloseHandle(hOutputRead);
            CloseHandle(hOutputWrite);
            return false;
        }

        CloseHandle(hOutputReadTmp);
        CloseHandle(hErrorReadTmp);

        GetStartupInfoW(&startupinfo);
        startupinfo.cb = sizeof(STARTUPINFO);
        startupinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        startupinfo.hStdOutput = hOutputWrite;
        startupinfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        startupinfo.hStdError = hErrorWrite;
        startupinfo.wShowWindow = SW_SHOWDEFAULT;

        std::wstring cmdline = build_command_line(exe, args);
        // std::wstring envp = w_prepare_environment_variable(m_context);

        if (cmdline.empty())
            goto create_process_failure;

        ouputfs << "Command: [" << from_wide_to_utf8(cmdline) << "]\n";

        ZeroMemory(&m_pi, sizeof(PROCESS_INFORMATION));

        if (!(CreateProcessW(NULL,
                             &cmdline[0],
                             NULL,
                             NULL,
                             TRUE,
                             CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT,
                             NULL,
                             workingdir.wstring().c_str(),
                             &startupinfo,
                             &m_pi))) {

            auto msg = format_message(GetLastError());
            show_message_box(msg);

            goto create_process_failure;
        }

        CloseHandle(hOutputWrite);
        CloseHandle(hErrorWrite);

        Sleep(25);

        ouputfs << "CreateProcess success\n";
        return true;

    create_process_failure:
        CloseHandle(hErrorRead);
        CloseHandle(hErrorWrite);
        CloseHandle(hOutputRead);
        CloseHandle(hOutputWrite);
        return false;
    }

    void format(const char* function, DWORD error)
    {
        m_context->error(
          "%s failed: %s\n", function, format_message(error).c_str());
    }

    bool wait()
    {
        assert(m_start);

        if (WaitForSingleObject(m_pi.hProcess, m_waitchildtimeout.count())) {
            if (GetExitCodeProcess(m_pi.hProcess, &m_status)) {
                m_start = false;
                m_finish = true;

                CloseHandle(hErrorRead);
                CloseHandle(hOutputRead);
                CloseHandle(m_pi.hThread);
                CloseHandle(m_pi.hProcess);
            }
        }

        return true;
    }

    void kill()
    {
        assert(m_start);

        if (m_finish)
            return;

        if (TerminateProcess(m_pi.hProcess, 0) == 0)
            format("TerminateProcess", GetLastError());
    }

    bool isfinish()
    {
        assert(m_start);

        return m_finish;
    }

    bool isstart()
    {
        return m_start;
    }

    bool status(std::string* msg, bool* success)
    {
        assert(m_start);
        assert(m_finish);

        *msg = m_msg;

        if (m_status == 0) {
            *success = true;
        } else {
            *success = false;
        }

        return true;
    }
};

Spawn::Spawn(ContextPtr ctx)
  : m_pimpl(
      std::make_unique<Spawn::Pimpl>(ctx, std::chrono::milliseconds{ 5 }))
{}

Spawn::~Spawn() = default;

bool
Spawn::start(const Path& exe,
             const Path& workingdir,
             const std::vector<std::string>& args,
             std::chrono::milliseconds waitchildtimeout)
{
    m_pimpl->init(waitchildtimeout);

    m_pimpl->m_context->debug(_("Spawn: command: `%s' chdir: `%s'\n"),
                              exe.string().c_str(),
                              workingdir.string().c_str());

    for (const auto& elem : args) {
        m_pimpl->m_context->debug(_("[%s]\n"), elem.c_str());
    }

    return m_pimpl->start(exe, workingdir, args);
}

bool
Spawn::wait()
{
    return m_pimpl->wait();
}

void
Spawn::kill()
{
    m_pimpl->kill();
}

bool
Spawn::isstart()
{
    return m_pimpl->isstart();
}

bool
Spawn::isfinish()
{
    return m_pimpl->isfinish();
}

bool
Spawn::get(std::string* output, std::string* error)
{
    return m_pimpl->get(output, error);
}

bool
Spawn::status(std::string* msg, bool* success)
{
    return m_pimpl->status(msg, success);
}

std::vector<std::string>
Spawn::splitCommandLine(const std::string& command)
{
    std::vector<std::string> argv;
    details::ShellUtils su;
    int argcp;
    su.g_shell_parse_argv(command, argcp, argv);

    if (argv.empty())
        throw utils::ArgError(
          _("Package command line: error, empty command `%s'"),
          command.c_str());

    argv.front() = m_pimpl->m_context->findProgram(argv.front()).string();

    return argv;
}
}
}
