/*
 * @file vle/utils/details/SpawnWin.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <windows.h>

namespace vle { namespace utils

// Max default size of command line buffer. See the CreateProcess
// function in msdn.
const unsigned long int Spawn::default_buffer_size = 32768;

struct append_arg_functor
    : std::unary_function < std::string, void >
{
    std::string *cmdline;

    append_arg_functor(std::string *cmdline)
        : cmdline(cmdline)
    {}

    ~append_arg_functor()
    {}

    void operator()(const std::string& arg)
    {
        cmdline->append(arg);
        cmdline->append(" ");
    }
}

static char* build_command_line(const std::string& exe,
                                const std::vector < std::string >&args)
{
    std::string cmd;
    char *buf;

    cmd.reserve(Spawn::default_buffer_size);
    cmd += exe;
    cmd += ' ';

    std::copy(args.begin(), args.end(), append_arg_functor(cmd));

    buf = (char*)malloc(cmd.size() + 1);
    if (buf) {
        strncpy(buf, cmd.c_str(), cmd.size());
    }
    buf[cmd.size()] = '\0';

    return buf;
}

// struct win32_spawn_t
// {
//     PROCESS_INFORMATION pi;
//     HANDLE              outread;
//     HANDLE              outwrite;
//     HANDLE              errread;
//     HANDLE              errwrite;
// };

// static int spawn_command(const char *command,
//                          const char *dirname,
//                          win32_spawn_t *spawn)
// {
//     SECURITY_ATTRIBUTES sa;
//     STARTUPINFO si;
//     char *cmdline = NULL;

//     sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//     sa.lpSecurityDescriptor = NULL;
//     sa.bInheritHandle = TRUE;

//     if (!(CreatePipe(&spawn->outread, &spawn->outwrite, &sa, 0)) &&
//         (SetHandleInformation(spawn->outread, HANDLE_FLAG_INHERIT, 0)))
//         goto pipe_out_failure;

//     if (!(CreatePipe(&spawn->errread, &spawn->errwrite, &sa, 0)) &&
//         (SetHandleInformation(spawn->errread, HANDLE_FLAG_INHERIT, 0)))
//         goto pipe_err_failure;

//     ZeroMemory(&si, sizeof(STARTUPINFO));
//     si.cb = sizeof(STARTUPINFO);
//     si.dwFlags |= STARTF_USESTHANDLES | STARTF_USESHOWWINDOW;
//     si.hStdOutput = spawn->outread;
//     si.hStdError = spawn->errread;
//     si.wShowWindow = SW_SHOWDEFAULT; /* must be SW_HIDE ? */
//     GetStartupInfo(&si);

//     cmdline = (char*)malloc(32768);
//     if (!cmdline)
//         goto malloc_failure;

//     strncpy(cmdline, 32768, command);
//     ZeroMemory(&spawn->pi, sizeof(PROCESS_INFORMATION));

//     if (!(CreateProcess(NULL, command, NULL, NULL, TRUE,
//                         0, NULL, dirname, &si, &spawn->pi)))
//         goto create_process_failure;

//     free(cmdline);

//     return 0;

// create_process_failure:
//     free(cmdline);

// malloc_failure:
//     CloseHandle(spawn->outwrite);
//     CloseHandle(spawn->outread);

// pipe_err_failure:
//     CloseHandle(spawn->errwrite);
//     CloseHandle(spawn->errread);

// pipe_out_failure:
//     return -1;
// }


// int main()
// {
//     win32_spawn_t spawn;
//     DWORD exitcode;

//     if (!spawn_command("cmake.exe", "c:\\", &spawn))
//         return -1;

//     WaitForSingleObject(spawn.pi.hProcess, INFINITE);
//     GetExitCodeProcess(pi.hProcess, &exitcode);

//     CloseHandle(spawn.pi.hThread);
//     CloseHandle(spawn.pi.hProcess);
//     CloseHandle(spawn.outwrite);
//     CloseHandle(spawn.outread);
//     CloseHandle(spawn.errwrite);
//     CloseHandle(spawn.errread);

//     return 0;
// }

#define PIPE_OUTPUT_READ 0
#define PIPE_OUTPUT_WRITE 1
#define PIPE_ERROR_READ 2
#define PIPE_ERROR_WIRTE 3

struct Spawn::Pimpl
{
    HANDLE              m_handle[4];
    PROCESS_INFORMATION m_pi;
    DWORD               m_status;
    bool                m_finish;
    std::string         m_msg;

    Pimpl()
        : m_finish(false)
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
                m_finish = true;
                return true;
            }
        }

        m_finish = false;
        return false;
    }

    int get(std::string *output, std::string *error)
    {
        if (m_finish) {
            return false;
        }

        is_running();

        HANDLE signaled = NULL;

        DWORD result = WaitForMultipleObjects(4, m_handle, FALSE, INFINITE);
        if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + 4) {
            signaled = m_handle[result - WAIT_OBJECT_0];
        } else if (result == WAIT_FAILED) {
            *msg = Win32ErrorMessage(GetLastError());
            return false;
        } else {
            return false;
        }

        DWORD size;
        std::vector < char > buffer;
        buffer.resize(4096);
        if (!ReadFile(signaled, &buffer[0], buffer.size(), &size, NULL)) {
            *msg = Win32ErrorMessage(GetLastError());
            return false;
        } else {
            if (signaled == m_handle[PIPE_ERROR_READ]) {
                error->append(&buffer[0], size);
            } else {
                output->append(&buffer[0], size);
            }
        }

        return 0;
    }

    // int initchild(const std::string& exe,
    //               const std::string& workingdir,
    //               const std::vector < std::string > &args,
    //               const std::vector < std::string > &envp)
    // {
    //     ::dup2(m_pipeout[1], STDOUT_FILENO);
    //     ::dup2(m_pipeerr[1], STDERR_FILENO);

    //     ::close(m_pipeout[1]);
    //     ::close(m_pipeerr[1]);
    //     ::close(m_pipeout[0]);
    //     ::close(m_pipeerr[0]);

    //     ::chdir(workingdir.c_str());

    //     char **localenvp = convert_string_str_array(envp);
    //     char **localargv = convert_string_str_array(args);

    //     if (::execve(exe.c_str(), localargv, localenvp) == -1) {
    //         free_str_array(localargv);
    //         free_str_array(localenvp);
    //         exit(-1); /* Kill the child. */
    //     }

    //     return true;
    // }

    // int initparent(pid_t localpid)
    // {
    //     ::close(m_pipeout[1]);
    //     ::close(m_pipeerr[1]);
    //     m_pid = localpid;

    //     usleep(50000);

    //     return is_running();
    // }

    int start(const std::string& exe,
              const std::string& workingdir,
              const std::vector < std::string > &args,
              const std::vector < std::string > &envp)
    {
        SECURITY_ATTRIBUTES sa;
        STARTUPINFO si;

        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        if (!(CreatePipe(&m_handle[PIPE_OUTPUT_READ], &m_handle[PIPE_OUTPUT_WRITE], &sa, 0)) &&
            (SetHandleInformation(m_handle[PIPE_OUTPUT_READ], HANDLE_FLAG_INHERIT, 0)))
            goto pipe_out_failure;

        if (!(CreatePipe(&m_handle[PIPE_ERROR_READ], &m_handle[PIPE_ERROR_WRITE], &sa, 0)) &&
            (SetHandleInformation(m_handle[PIPE_ERROR_READ], HANDLE_FLAG_INHERIT, 0)))
            goto pipe_err_failure;

        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags |= STARTF_USESTHANDLES | STARTF_USESHOWWINDOW;
        si.hStdOutput = m_handle[PIPE_OUTPUT_READ];
        si.hStdError = m_handle[PIPE_ERROR_READ];
        si.wShowWindow = SW_SHOWDEFAULT; /* must be SW_HIDE ? */
        GetStartupInfo(&si);

        char *cmdline = build_command_line(exe, args);
        if (!cmdline)
            goto malloc_failure;

        ZeroMemory(&m_pi, sizeof(PROCESS_INFORMATION));

        if (!(CreateProcess(NULL, command, NULL, NULL, TRUE,
                            0, NULL, dirname, &si, &m_pi)))
            goto create_process_failure;




        free(cmdline);

        return 0;

    create_process_failure:
        free(cmdline);

    malloc_failure:
        CloseHandle(m_handle[PIPE_OUTPUT_WRITE]);
        CloseHandle(m_handle[PIPE_OUTPUT_READ]);

    pipe_err_failure:
        CloseHandle(m_handle[PIPE_ERROR_WRITE]);
        CloseHandle(m_handle[PIPE_ERROR_READ]);

    pipe_out_failure:
        return -1;
    }

    int wait()
    {
        if (m_finish)
            return -1;

        if (WaitForSingleObject(m_pi.hProcess, INFINITE)) {
            if (GetExitCodeProcess(m_pi.hProcess, &m_status)) {
                m_finish = true;
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

        // if (WIFEXITED(m_status)) {
        //     printf("[%s] (%d) exited, status=%d\n", m_command.c_str(),
        //            m_pid, WEXITSTATUS(m_status));
        //     *success = true;
        // } else if (WIFSIGNALED(m_status)) {
        //     printf("[%s] (%d) killed by signal %d\n", m_command.c_str(),
        //            m_pid, WTERMSIG(m_status));
        //     *success = false;
        // } else if (WIFSTOPPED(m_status)) {
        //     printf("[%s] (%d) stopped by signal %d\n", m_command.c_str(),
        //            m_pid, WSTOPSIG(m_status));
        //     *success = false;
        // } else if (WIFCONTINUED(m_status)) {
        //     printf("[%s] (%d) continued\n", m_command.c_str(), m_pid);
        //     *success = false;
        // }

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

int Spawn::start(const std::string& exe,
                 const std::string& workingdir,
                 const std::vector < std::string > &args,
                 const std::vector < std::string > &envp)
{
    if (m_pimpl) {
        delete m_pimpl;
    }

    m_pimpl = new Spawn::Pimpl;

    return m_pimpl->start(exe, workingdir, args, envp);
}

int Spawn::wait()
{
    if (not m_pimpl)
        return false;

    return m_pimpl->wait();
}

int Spawn::isfinish()
{
    if (not m_pimpl)
        return false;

    return m_pimpl->m_finish;
}

int Spawn::get(std::string *output, std::string *error)
{
    if (not m_pimpl and not isfinish())
        return -1;

    return m_pimpl->get(output, error);
}

bool Spawn::status(std::string *msg, bool *success)
{
    if (not m_pimpl and not m_pimpl->m_finish)
        return false;

    return m_pimpl->status(msg, success);
}

}}
