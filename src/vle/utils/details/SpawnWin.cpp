/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/details/ShellUtils.hpp>
#include <vle/utils/details/UtilsWin.hpp>
#include <vle/utils/i18n.hpp>
#include <windows.h>
#include <strsafe.h>

namespace vle {
namespace utils {

// Max default size of command line buffer. See the CreateProcess
// function in msdn.
const unsigned long int Spawn::default_buffer_size = 32768;

typedef std::vector<std::pair<std::string, std::string>> Envp;

/**
 * A specific \b Win32 function to build a new environment
 * variable.
 *
 * @param[out] envp
 * @param variable
 * @param value
 * @param append either with in places values or in env vars
 *
 */
static void
replaceEnvironmentVariable(Envp& envp,
                           const std::string& variable,
                           const std::string& value,
                           bool append)
{
    // check if in envp
    Envp::iterator itb = envp.begin();
    Envp::iterator ite = envp.end();
    Envp::iterator itf = ite;
    for (; (itb != ite) and (itf == ite); itb++) {
        if (itb->first == variable) {
            itf = itb;
        }
    }

    if (itf != ite) { // found in envp
        Envp::value_type& result = *itf;
        if (append) {
            result.second = value + ";" + result.second;
        } else {
            result.second = value;
        }
    } else { // not in envp
        Envp::value_type result;
        result.first = variable;
        result.second = value;
        if (append) {
            char* env = std::getenv(variable.c_str());
            if (env != NULL) {
                std::string old(env, std::strlen(env));
                result.second += ";";
                result.second += old;
            }
        }
        envp.push_back(result);
    }
}

/**
 * Build list of string which represents the 'VARIABLE=VALUE'
 * environment variable.
 *
 * @attention @b Win32: the PKG_CONFIG_FILE is path is update with the
 * vle's pkgconfig directory. The BOOST_ROOT, BOOST_INCLUDEDIR and
 * BOOST_LIBRARYDIR are assigned with the vle's patch to avoid
 * conflict with different boost version.
 *
 * @return A list of string.
 */
static Envp
prepareEnvironmentVariable(vle::utils::ContextPtr ctx)
{
    Envp envp;
    char* env_char = ::GetEnvironmentStrings();
    if (env_char == NULL) {
        throw vle::utils::InternalError(
          "[SpawnWin] error reading environmental variables");
    }
    unsigned int prev = 0;
    std::string env_string;
    for (unsigned int i = 0;; i++) {
        if (env_char[i] == '\0') {
            env_string.assign(std::string(env_char + prev, env_char + i));
            std::vector<std::string> splitvec;
            boost::algorithm::split(splitvec,
                                    env_string,
                                    boost::is_any_of("="),
                                    boost::algorithm::token_compress_on);
            if ((splitvec.size() == 2) and (not splitvec[0].empty()) and
                (splitvec[0].size() > 1)) {
                replaceEnvironmentVariable(
                  envp,
                  splitvec[0].substr(1), // don't know why we
                  // have to remove a blank
                  splitvec[1],
                  false);
            }
            prev = i;
            if (env_char[i + 1] == '\0') {
                break;
            }
        }
    }
    ::FreeEnvironmentStrings(env_char);

    Path prefix = UtilsWin::convertPathTo83Path(ctx->getPrefixDir());

    {
        Path bin = prefix;
        bin /= "bin";

        replaceEnvironmentVariable(envp, "PATH", bin.string(), true);
        replaceEnvironmentVariable(envp, "Path", bin.string(), true);
    }

    {
        Path lib = prefix;
        lib /= "lib";
        lib /= "pkgconfig";

        replaceEnvironmentVariable(
          envp, "PKG_CONFIG_PATH", lib.string(), false);
    }

    {
        Path inc = prefix;
        inc /= "include";

        replaceEnvironmentVariable(
          envp, "BOOST_INCLUDEDIR", inc.string(), false);
    }

    {
        Path lib = prefix;
        lib /= "lib";

        replaceEnvironmentVariable(
          envp, "BOOST_LIBRARYDIR", lib.string(), false);
    }

    replaceEnvironmentVariable(envp, "BOOST_ROOT", prefix.string(), false);

    return envp;
}

static std::string
win32_quote(const std::string& arg)
{
    std::string result;

    if (not arg.empty() and arg.find_first_of(" \t\n\v\"") == arg.npos) {
        result.append(arg);
    } else {
        result.push_back('"');

        for (std::string::const_iterator it = arg.begin();; ++it) {
            unsigned NumberBackslashes = 0;

            while (it != arg.end() and *it == '\\') {
                ++it;
                ++NumberBackslashes;
            }

            if (it == arg.end()) {
                result.append(NumberBackslashes * 2, '\\');
                break;
            } else if (*it == '"') {
                result.append(NumberBackslashes * 2 + 1, '\\');
                result.push_back(*it);
            } else {
                result.append(NumberBackslashes, '\\');
                result.push_back(*it);
            }
        }

        result.push_back('"');
    }

    return result;
}

struct win32_argv_quote : public std::unary_function<std::string, void>
{
    std::string* cmd;
    char separator;

    win32_argv_quote(std::string* cmd, char separator)
      : cmd(cmd)
      , separator(separator)
    {
    }

    ~win32_argv_quote()
    {
    }

    void operator()(const std::string& arg)
    {
        cmd->append(win32_quote(arg));
        cmd->push_back(separator);
    }
};

struct win32_envp_quote : public std::unary_function<Envp::value_type, void>
{
    std::string* cmd;

    win32_envp_quote(std::string* cmd)
      : cmd(cmd)
    {
    }

    ~win32_envp_quote()
    {
    }

    void operator()(const Envp::value_type& arg)
    {
        if (not arg.first.empty()) {
            std::vector<std::string> tokens;

            boost::algorithm::split(tokens,
                                    arg.second,
                                    boost::algorithm::is_any_of(";"),
                                    boost::algorithm::token_compress_on);

            cmd->append(arg.first);
            cmd->push_back('=');

            std::for_each(
              tokens.begin(), tokens.end(), win32_argv_quote(cmd, ';'));

            cmd->operator[](cmd->size() - 1) =
              '\0'; /**< remove the last `;' to
                     * avoid bad environment.*/
        }
    }
};

static char*
build_command_line(const std::string& exe,
                   const std::vector<std::string>& args)
{
    std::string cmd;
    char* buf;

    cmd.reserve(Spawn::default_buffer_size);
    cmd.append(win32_quote(exe));
    cmd.push_back(' ');

    std::for_each(args.begin(), args.end(), win32_argv_quote(&cmd, ' '));

    buf = (char*)malloc(cmd.size() + 1);
    if (buf) {
        strncpy(buf, cmd.c_str(), cmd.size());
    }
    buf[cmd.size()] = '\0';

    return buf;
}

static char*
prepare_environment_variable(vle::utils::ContextPtr ctx)
{
    Envp envp = prepareEnvironmentVariable(ctx);

    std::string cmd;
    cmd.reserve(Spawn::default_buffer_size);

    std::for_each(envp.begin(), envp.end(), win32_envp_quote(&cmd));
    cmd.push_back('\0');

    char* result = (char*)malloc(sizeof(char) * (cmd.size() + 1));

    std::copy(cmd.begin(), cmd.end(), result);

    return result;
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

        std::vector<char> buffer(4096, '\0');
        unsigned long bread;
        unsigned long avail;

        {
            PeekNamedPipe(hOutputRead,
                          &buffer[0],
                          buffer.size() - 1,
                          &bread,
                          &avail,
                          NULL);

            if (bread) {
                ouputfs << "get PeekNamedPipe success " << bread << " "
                        << avail << "\n";

                std::fill(buffer.begin(), buffer.end(), '\0');
                if (avail > buffer.size() - 1) {
                    ouputfs << "get PeekNamedPipe " << avail << "\n"
                            << buffer.size() - 1 << "\n";
                    while (bread >= buffer.size() - 1) {
                        ReadFile(hOutputRead,
                                 &buffer[0],
                                 buffer.size() - 1,
                                 &bread,
                                 NULL);

                        ouputfs << "get: " << bread << "\n";

                        if (bread > 0) {
                            unsigned long sz = std::min(
                              bread,
                              static_cast<unsigned long>(buffer.size() - 1));

                            output->append(&buffer[0], sz);
                            ouputfs.write(&buffer[0], sz);
                        }

                        std::fill(buffer.begin(), buffer.end(), '\0');
                    }
                } else {
                    ouputfs << "get PeekNamedPipe success (else) " << bread
                            << " " << avail << "\n";

                    ReadFile(hOutputRead,
                             &buffer[0],
                             buffer.size() - 1,
                             &bread,
                             NULL);

                    ouputfs << "get: " << bread << "\n";

                    if (bread > 0) {
                        unsigned long sz = std::min(
                          bread,
                          static_cast<unsigned long>(buffer.size() - 1));

                        output->append(&buffer[0], sz);
                        ouputfs.write(&buffer[0], sz);
                    }
                }
            }
        }

        {
            PeekNamedPipe(
              hErrorRead, &buffer[0], buffer.size() - 1, &bread, &avail, NULL);

            if (bread) {
                errorfs << "[err] get PeekNamedPipe success " << avail << "\n";

                std::fill(buffer.begin(), buffer.end(), '\0');
                if (avail > buffer.size() - 1) {
                    while (bread >= buffer.size() - 1) {
                        ReadFile(hErrorRead,
                                 &buffer[0],
                                 buffer.size() - 1,
                                 &bread,
                                 NULL);

                        if (bread > 0) {
                            unsigned long sz = std::min(
                              bread,
                              static_cast<unsigned long>(buffer.size() - 1));

                            error->append(&buffer[0], sz);
                            errorfs.write(&buffer[0], sz);
                        }

                        std::fill(buffer.begin(), buffer.end(), '\0');
                    }
                } else {
                    ReadFile(
                      hErrorRead, &buffer[0], buffer.size() - 1, &bread, NULL);

                    if (bread > 0) {
                        unsigned long sz = std::min(
                          bread,
                          static_cast<unsigned long>(buffer.size() - 1));

                        error->append(&buffer[0], sz);
                        errorfs.write(&buffer[0], sz);
                    }
                }
            }
        }

        return true;
    }

    bool start(const std::string& exe,
               const std::string& workingdir,
               const std::vector<std::string>& args)
    {
        m_start = true;
        m_finish = false;

        HANDLE hOutputReadTmp = INVALID_HANDLE_VALUE;
        HANDLE hErrorReadTmp = INVALID_HANDLE_VALUE;
        HANDLE hOutputWrite = INVALID_HANDLE_VALUE;
        HANDLE hErrorWrite = INVALID_HANDLE_VALUE;
        STARTUPINFO startupinfo;
        SECURITY_ATTRIBUTES securityatt;
        SECURITY_DESCRIPTOR securitydescriptor;
        char* cmdline = NULL;
        LPVOID envp;

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
            goto pipe_out_failure;

        if (!CreatePipe(&hErrorReadTmp, &hErrorWrite, &securityatt, 0) ||
            !DuplicateHandle(GetCurrentProcess(),
                             hErrorReadTmp,
                             GetCurrentProcess(),
                             &hErrorRead,
                             0,
                             TRUE,
                             DUPLICATE_SAME_ACCESS))
            goto pipe_err_failure;

        CloseHandle(hOutputReadTmp);
        CloseHandle(hErrorReadTmp);

        GetStartupInfo(&startupinfo);
        startupinfo.cb = sizeof(STARTUPINFO);
        startupinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        startupinfo.hStdOutput = hOutputWrite;
        startupinfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        startupinfo.hStdError = hErrorWrite;
        startupinfo.wShowWindow = SW_SHOWDEFAULT;

        cmdline = build_command_line(exe, args);
        if (!cmdline)
            goto malloc_failure;

        envp = prepare_environment_variable(m_context);
        if (!envp)
            goto malloc_failure;

        ouputfs << "Cmd: [" << cmdline << "]\n";
        errorfs << "Cmd: [" << cmdline << "]\n";

        ZeroMemory(&m_pi, sizeof(PROCESS_INFORMATION));

        if (!(CreateProcess(NULL,
                            cmdline,
                            NULL,
                            NULL,
                            TRUE,
                            CREATE_NO_WINDOW,
                            envp,
                            workingdir.c_str(),
                            &startupinfo,
                            &m_pi)))
            goto create_process_failure;

        free(cmdline);

        CloseHandle(hOutputWrite);
        CloseHandle(hErrorWrite);

        Sleep(25);

        ouputfs << "CreateProcess success\n";
        errorfs << "CreateProcess success\n";

        return true;

    create_process_failure:
        free(cmdline);

    malloc_failure:
        CloseHandle(hErrorReadTmp);
        CloseHandle(hErrorRead);
        CloseHandle(hErrorWrite);

    pipe_err_failure:
        CloseHandle(hOutputReadTmp);
        CloseHandle(hOutputRead);
        CloseHandle(hOutputWrite);

    pipe_out_failure:
        return false;
    }

    void format(const char* function, DWORD error)
    {
        LPVOID buffer;
        LPVOID displaybuffer;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&buffer,
                      0,
                      NULL);

        displaybuffer = (LPVOID)LocalAlloc(
          LMEM_ZEROINIT,
          (lstrlen((LPCTSTR)buffer) + lstrlen((LPCTSTR)function) + 40) *
            sizeof(TCHAR));

        StringCchPrintf((LPTSTR)displaybuffer,
                        LocalSize(displaybuffer) / sizeof(TCHAR),
                        TEXT("%s failed with error %d: %s"),
                        function,
                        error,
                        buffer);

        // MessageBox(NULL, (LPCTSTR)displaybuffer, TEXT("Error"), MB_OK);

        vDbg(m_context, "%s\n", (char*)displaybuffer);

        LocalFree(buffer);
        LocalFree(displaybuffer);
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
{
}

Spawn::~Spawn() = default;

bool
Spawn::start(const std::string& exe,
             const std::string& workingdir,
             const std::vector<std::string>& args,
             std::chrono::milliseconds waitchildtimeout)
{
    m_pimpl->init(waitchildtimeout);

    vDbg(m_pimpl->m_context,
         _("Spawn: command: `%s' chdir: `%s'\n"),
         exe.c_str(),
         workingdir.c_str());

    for (const auto& elem : args) {
        vDbg(m_pimpl->m_context, _("[%s]\n"), elem.c_str());
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
          (fmt(_("Package command line: error, empty command `%1%'")) %
           command)
            .str());

    argv.front() = m_pimpl->m_context->findProgram(argv.front()).string();

    return argv;
}
}
}
