/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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

#include <vle/utils/Spawn.hpp>
#include <vle/utils/details/UtilsWin.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <string>
#include <algorithm>
#include <functional>
#include <fstream>
#include <windows.h>
#include <glib.h>


namespace vle { namespace utils {

// Max default size of command line buffer. See the CreateProcess
// function in msdn.
const unsigned long int Spawn::default_buffer_size = 32768;

typedef std::vector < std::pair < std::string, std::string > > Envp;

/**
 * A specific \b Win32 function to build a new environment
 * variable.
 *
 * @param variable
 * @param value
 * @param append
 *
 * @return
 */
static Envp::value_type replaceEnvironmentVariable(const std::string& variable,
                                                   const std::string& value,
                                                   bool append)
{
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

    return result;
}

/**
 * A specific \b Win32 function to build a new environment
 * variable. Three arguments are given to this function corresponding to
 * three paths to add
 *
 * @param variable
 * @param value1
 * @param value2
 * @param value3
 * @param append
 *
 * @return
 */
static Envp::value_type replaceEnvironmentVariable(const std::string& variable,
                                                   const std::string& value1,
                                                   const std::string& value2,
                                                   const std::string& value3,
                                                   bool append)
{
    Envp::value_type result;

    result.first = variable;
    result.second = value1;
    result.second += ";";
    result.second += value2;
    result.second += ";";
    result.second += value3;

    if (append) {
        char* env = std::getenv(variable.c_str());

        if (env != NULL) {
            std::string old(env, std::strlen(env));
            result.second += ";";
            result.second += old;
        }
    }

    return result;
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
static Envp prepareEnvironmentVariable()
{
    Envp envp;
    gchar** allenv = g_listenv();
    gchar** it = allenv;

    while (*it != NULL) {
        if (strcasecmp(*it, "PATH") and
            strcasecmp(*it, "PKG_CONFIG_PATH") and
            strcasecmp(*it, "BOOST_ROOT") and
            strcasecmp(*it, "BOOST_INCLUDEDIR") and
            strcasecmp(*it, "BOOST_LIBRARYDIR")) {

            const gchar* res = g_getenv(*it);
            if (res) {
                envp.push_back(Envp::value_type(*it, res));
            }
        }

        it++;
    }

    g_strfreev(allenv);

    envp.push_back(replaceEnvironmentVariable(
                       "PATH",
                       Path::buildFilename(
                           UtilsWin::convertPathTo83Path(
                                   Path::path().getPrefixDir()), "bin"),
                       Path::buildFilename(
                           UtilsWin::convertPathTo83Path(
                                   Path::path().getPrefixDir()), "MinGW","bin"),
                       Path::buildFilename(
                           UtilsWin::convertPathTo83Path(
                                   Path::path().getPrefixDir()), "CMake","bin"),
                       true));

    envp.push_back(replaceEnvironmentVariable(
                       "PKG_CONFIG_PATH",
                       Path::buildFilename(
                           UtilsWin::convertPathTo83Path(
                                   Path::path().getPrefixDir()), "lib",
                                   "pkgconfig"),
                       false));

    envp.push_back(replaceEnvironmentVariable(
                       "BOOST_INCLUDEDIR",
                       Path::buildFilename(
                               UtilsWin::convertPathTo83Path(
                                       Path::path().getPrefixDir()), "include"),
                       false));

    envp.push_back(replaceEnvironmentVariable(
                       "BOOST_LIBRARYDIR",
                       Path::buildFilename(
                               UtilsWin::convertPathTo83Path(
                                       Path::path().getPrefixDir()), "lib"),
                       false));

    envp.push_back(replaceEnvironmentVariable(
                       "BOOST_ROOT",
                       UtilsWin::convertPathTo83Path(
                               Path::path().getPrefixDir()), false));
    return envp;
}

static std::string win32_quote(const std::string& arg)
{
    std::string result;

    if (not arg.empty() and arg.find_first_of(" \t\n\v\"") == arg.npos) {
        result.append(arg);
    } else {
        result.push_back('"');

        for (std::string::const_iterator it = arg.begin() ; ; ++it) {
            unsigned NumberBackslashes = 0;

            while (it != arg.end () and *it == '\\') {
                ++it;
                ++NumberBackslashes;
            }

            if (it == arg.end ()) {
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

struct win32_argv_quote
    : public std::unary_function < std::string, void >
{
    std::string *cmd;
    char separator;

    win32_argv_quote(std::string *cmd, char separator)
        : cmd(cmd), separator(separator)
    {}

    ~win32_argv_quote()
    {}

    void operator()(const std::string& arg)
    {
        cmd->append(win32_quote(arg));
        cmd->push_back(separator);
    }
};

struct win32_envp_quote
    : public std::unary_function < Envp::value_type, void >
{
    std::string *cmd;

    win32_envp_quote(std::string *cmd)
        : cmd(cmd)
    {}

    ~win32_envp_quote()
    {}

    void operator()(const Envp::value_type& arg)
    {
        if (not arg.first.empty()) {
            std::vector < std::string > tokens;

            boost::algorithm::split(tokens, arg.second,
                                    boost::algorithm::is_any_of(";"),
                                    boost::algorithm::token_compress_on);

            cmd->append(arg.first);
            cmd->push_back('=');

            std::for_each(tokens.begin(), tokens.end(),
                          win32_argv_quote(cmd, ';'));

            cmd->operator[](cmd->size() - 1) = '\0'; /**< remove the
                                                      * last `;' to
                                                      * avoid bad
                                                      * environment. */
        }
    }
};

static char* build_command_line(const std::string& exe,
                                const std::vector < std::string >&args)
{
    std::string cmd;
    char *buf;

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

static char * prepare_environment_variable(void)
{
    Envp envp = prepareEnvironmentVariable();

    std::string cmd;
    cmd.reserve(Spawn::default_buffer_size);

    std::for_each(envp.begin(), envp.end(), win32_envp_quote(&cmd));
    cmd.push_back('\0');

    char *result = (char*)malloc(sizeof(char) * (cmd.size() + 1));

    std::copy(cmd.begin(), cmd.end(), result);

    return result;
}

struct Spawn::Pimpl
{
    HANDLE hOutputRead;
    HANDLE hErrorRead;

    PROCESS_INFORMATION m_pi;
    DWORD               m_status;
    std::string         m_msg;
    unsigned int        m_waitchildtimeout;
    bool                m_finish;

    std::ofstream out__, err__;


    Pimpl(unsigned int waitchildtimeout)
        : hOutputRead(INVALID_HANDLE_VALUE),
          hErrorRead(INVALID_HANDLE_VALUE),
          m_status(0), m_waitchildtimeout(waitchildtimeout),
          m_finish(false), out__("c:/out.txt"), err__("c:/err.txt")
    {
    }

    ~Pimpl()
    {
        if (not m_finish) {
            wait();
        }
    }

    bool is_running()
    {
        assert(not m_finish);

        if (GetExitCodeProcess(m_pi.hProcess, &m_status)) {
            if (m_status == STILL_ACTIVE) {
                m_finish = false;

                return true;
            }
        }

        m_finish = true;
        return false;
    }

    bool get(std::string *output, std::string *error)
    {
        if (m_finish) {
            return false;
        }

        is_running();

        std::vector < char > buffer(4096, '\0');
        unsigned long bread;
        unsigned long avail;

        {
            PeekNamedPipe(hOutputRead, &buffer[0], buffer.size() - 1,
                          &bread, &avail, NULL);

            if (bread) {
                out__ << "get PeekNamedPipe success " << bread << " " <<  avail << "\n";

                std::fill(buffer.begin(), buffer.end(), '\0');
                if (avail > buffer.size() - 1) {
                    out__ << "get PeekNamedPipe " << avail << "\n"
                           << buffer.size() - 1 << "\n";
                    while (bread >= buffer.size() - 1) {
                        ReadFile(hOutputRead, &buffer[0],
                                 buffer.size() - 1, &bread, NULL);

                    out__ << "get: " << bread << "\n";

                        if (bread > 0) {
                            unsigned long sz = std::min(
                                bread,
                                static_cast < unsigned long >(
                                    buffer.size() - 1));

                            output->append(&buffer[0], sz);
                            out__.write(&buffer[0], sz);
                        }

                        std::fill(buffer.begin(), buffer.end(), '\0');
                    }
                } else {
                out__ << "get PeekNamedPipe success (else) " << bread << " " <<  avail << "\n";

                    ReadFile(hOutputRead, &buffer[0],
                             buffer.size() - 1, &bread, NULL);

                    out__ << "get: " << bread << "\n";

                    if (bread > 0) {
                        unsigned long sz = std::min(
                            bread,
                            static_cast < unsigned long >(
                                buffer.size() - 1));

                        output->append(&buffer[0], sz);
                        out__.write(&buffer[0], sz);
                    }
                }
            }
        }

        {
            PeekNamedPipe(hErrorRead, &buffer[0], buffer.size() - 1,
                          &bread, &avail, NULL);

            if (bread) {
                err__ << "[err] get PeekNamedPipe success " <<  avail << "\n";

                std::fill(buffer.begin(), buffer.end(), '\0');
                if (avail > buffer.size() - 1) {
                    while (bread >= buffer.size() - 1) {
                        ReadFile(hErrorRead, &buffer[0],
                                 buffer.size() - 1, &bread, NULL);

                        if (bread > 0) {
                            unsigned long sz = std::min(
                                bread,
                                static_cast < unsigned long >(
                                    buffer.size() - 1));

                            error->append(&buffer[0], sz);
                            err__.write(&buffer[0], sz);
                        }

                        std::fill(buffer.begin(), buffer.end(), '\0');
                    }
                } else {
                    ReadFile(hErrorRead, &buffer[0],
                             buffer.size() - 1, &bread, NULL);

                    if (bread > 0) {
                        unsigned long sz = std::min(
                            bread,
                            static_cast < unsigned long >(
                                buffer.size() - 1));

                        error->append(&buffer[0], sz);
                        err__.write(&buffer[0], sz);
                    }
                }
            }
        }

        return true;
    }

    bool start(const std::string& exe,
               const std::string& workingdir,
               const std::vector < std::string > &args)
    {
        HANDLE hOutputReadTmp = INVALID_HANDLE_VALUE;
        HANDLE hErrorReadTmp = INVALID_HANDLE_VALUE;
        HANDLE hOutputWrite = INVALID_HANDLE_VALUE;
        HANDLE hErrorWrite = INVALID_HANDLE_VALUE;
        STARTUPINFO startupinfo;
        SECURITY_ATTRIBUTES securityatt;
        SECURITY_DESCRIPTOR securitydescriptor;
        char *cmdline = NULL;
        LPVOID envp;

        InitializeSecurityDescriptor(&securitydescriptor,
                                     SECURITY_DESCRIPTOR_REVISION);

        SetSecurityDescriptorDacl(&securitydescriptor, TRUE, NULL, FALSE);
        securityatt.lpSecurityDescriptor = &securitydescriptor;
        securityatt.nLength = sizeof(SECURITY_ATTRIBUTES);
        securityatt.bInheritHandle = TRUE;

        if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &securityatt, 0) ||
            !DuplicateHandle(GetCurrentProcess(), hOutputReadTmp,
                             GetCurrentProcess(), &hOutputRead, 0,
                             FALSE, DUPLICATE_SAME_ACCESS))
            goto pipe_out_failure;

        if (!CreatePipe(&hErrorReadTmp, &hErrorWrite, &securityatt, 0) ||
            !DuplicateHandle(GetCurrentProcess(), hErrorReadTmp,
                             GetCurrentProcess(), &hErrorRead, 0,
                             TRUE, DUPLICATE_SAME_ACCESS))
            goto pipe_err_failure;

        CloseHandle(hOutputReadTmp);
        CloseHandle(hErrorReadTmp);

        GetStartupInfo(&startupinfo);
        startupinfo.cb = sizeof(STARTUPINFO);
        startupinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        startupinfo.hStdOutput = hOutputWrite;
        startupinfo.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
        startupinfo.hStdError = hErrorWrite;
        startupinfo.wShowWindow = SW_SHOWDEFAULT;

        cmdline = build_command_line(exe, args);
        if (!cmdline)
            goto malloc_failure;

        envp = prepare_environment_variable();
        if (!envp)
            goto malloc_failure;

        out__ << "Cmd: [" << cmdline << "]\n";
        err__ << "Cmd: [" << cmdline << "]\n";

        ZeroMemory(&m_pi, sizeof(PROCESS_INFORMATION));
        if (!(CreateProcess(NULL, cmdline, NULL, NULL, TRUE,
                            CREATE_NO_WINDOW,
                            envp,
                            workingdir.c_str(), &startupinfo, &m_pi)))
            goto create_process_failure;

        free(cmdline);

        CloseHandle(hOutputWrite);
        CloseHandle(hErrorWrite);

        Sleep(25);

        out__ << "CreateProcess success\n";
        err__ << "CreateProcess success\n";

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

    bool wait()
    {
        if (m_finish)
            return true;

        if (WaitForSingleObject(m_pi.hProcess, 50000)) {
            if (GetExitCodeProcess(m_pi.hProcess, &m_status)) {
                m_finish = true;

                CloseHandle(hErrorRead);
                CloseHandle(hOutputRead);

                CloseHandle(m_pi.hThread);
                CloseHandle(m_pi.hProcess);
            }
        }

        return true;
    }

    bool status(std::string *msg, bool *success)
    {
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

Spawn::Spawn()
    : m_pimpl(0)
{
}

Spawn::~Spawn()
{
    delete m_pimpl;
}

bool Spawn::start(const std::string& exe,
                  const std::string& workingdir,
                  const std::vector < std::string > &args,
                  unsigned int waitchildtimeout)
{
    if (m_pimpl) {
        delete m_pimpl;
    }

    m_pimpl = new Spawn::Pimpl(waitchildtimeout);

    return m_pimpl->start(exe, workingdir, args);
}

bool Spawn::wait()
{
    if (not m_pimpl)
        return false;

    return m_pimpl->wait();
}

bool Spawn::isstart()
{
    if (not m_pimpl)
        return false;

    return not m_pimpl->m_finish;
}

bool Spawn::isfinish()
{
    if (not m_pimpl)
        return false;

    return m_pimpl->m_finish;
}

bool Spawn::get(std::string *output, std::string *error)
{
    if (not m_pimpl and not isfinish())
        return false;

    return m_pimpl->get(output, error);
}

bool Spawn::status(std::string *msg, bool *success)
{
    if (not m_pimpl or not m_pimpl->m_finish)
        return false;

    return m_pimpl->status(msg, success);
}

}}
