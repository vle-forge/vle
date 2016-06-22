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
#include <vle/utils/i18n.hpp>
#include <vle/utils/details/ShellUtils.hpp>
#include <algorithm>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef __APPLE__
 #include <crt_externs.h>
 extern "C" char ** environ = *_NSGetEnviron();
#endif

namespace vle { namespace utils {

const unsigned long int Spawn::default_buffer_size = BUFSIZ;

struct strdup_functor
    : std::unary_function < std::string, char* >
{
    char * operator()(const std::string& str) const
    {
        return strdup(str.c_str());
    }
};

void free_str_array(char **args)
{
    char **tmp;

    for (tmp = args; *tmp; ++tmp) {
        free(*tmp);
    }

    delete[] args;
}

char ** convert_string_str_array(const std::vector < std::string >& args)
{
    char **result = nullptr;

    result = new char*[args.size() + 1];

    std::transform(args.begin(),
                   args.end(),
                   result,
                   strdup_functor());

    result[args.size()] = nullptr;

    return result;
}

static char ** prepare_environment_variable(void)
{
    char **it = ::environ;
    char **jt = nullptr;
    char **result = nullptr;
    int size = 0;

    while (*it++) {
        size++;
    }

    result = new char*[size + 1];

    it = ::environ;
    jt = result;
    while (*it) {
        *jt++ = strdup(*it++);
    }

    result[size] = nullptr;

    return result;
}

/**
 * @e input_timeout function blocks the calling process until input is
 * available on the file descriptor, or until the timeout period
 * expires.
 *
 * @param fd File descriptor.
 * @param microseconds timeout.
 *
 * @return returns 0 if timeout, 1 if input available, -1 if error.
 */
static int input_timeout(int fd, unsigned int microseconds)
{
    fd_set set;
    struct timeval timeout;
    long int result;

    FD_ZERO (&set);
    FD_SET (fd, &set);

    timeout.tv_sec = 0;
    timeout.tv_usec = microseconds;

    do {
        result = select (FD_SETSIZE, &set, nullptr, nullptr, &timeout);
    } while (result == -1L && errno == EINTR);

    return result;
}

class Spawn::Pimpl
{
public:
    ContextPtr m_context;
    pid_t m_pid;
    unsigned int m_waitchildtimeout;
    int m_pipeout[2];
    int m_pipeerr[2];
    int m_status;
    bool m_finish;
    std::string m_msg;
    std::string m_command;

    Pimpl(ContextPtr ctx, unsigned int waitchildtimeout)
        : m_context(ctx)
        , m_pid(-1)
        , m_waitchildtimeout(waitchildtimeout / 1000.0)
        , m_status(0)
        , m_finish(false)
    {
    }

    void init(unsigned int waitchildtimeout)
    {
        m_pid = -1;
        m_waitchildtimeout = waitchildtimeout / 1000.0;
        m_status = 0;
        m_finish = false;
    }

    ~Pimpl()
    {
        if (not m_finish) {
            wait();
        }
        ::close(m_pipeout[1]);
        ::close(m_pipeerr[1]);
        ::close(m_pipeout[0]);
        ::close(m_pipeerr[0]);
    }

    bool is_running()
    {
        assert(not m_finish);

        /* waitpid(): on success, returns the process ID of the child
           whose state has changed; if WNOHANG was specified and one
           or more child(ren) specified by pid exist, but have not yet
           changed state, then 0 is returned.  On error, -1 is
           returned. */
        switch (waitpid(m_pid, &m_status, WNOHANG)) {
        case 0:
            m_finish = false;
            return true;
        case -1:
        default:
            m_finish = true;
            return false;
        }
    }

    bool get(std::string *output, std::string *error)
    {
        if (m_finish) {
            return false;
        }

        is_running();

        std::vector < char > buffer;
        buffer.reserve(BUFSIZ);

        int result;

        if ((result = input_timeout(m_pipeout[0], m_waitchildtimeout)) == -1)
            return false;

        if (result) {
            int size = read(m_pipeout[0], &buffer[0], BUFSIZ);
            if (size > 0 and output) {
                output->append(&buffer[0], size);
            }
        }

        if ((result = input_timeout(m_pipeerr[0], m_waitchildtimeout)) == -1)
            return false;

        if (result) {
            int size = read(m_pipeerr[0], &buffer[0], BUFSIZ);
            if (size > 0 and error) {
                error->append(&buffer[0], size);
            }
        }

        return true;
    }

    bool initchild(const std::string& exe,
                   std::vector < std::string > args)
    {

        ::dup2(m_pipeout[1], STDOUT_FILENO);
        ::dup2(m_pipeerr[1], STDERR_FILENO);

        ::close(m_pipeout[1]);
        ::close(m_pipeerr[1]);
        ::close(m_pipeout[0]);
        ::close(m_pipeerr[0]);

        args.insert(args.begin(), exe);

        char **localenvp = prepare_environment_variable();
        char **localargv = convert_string_str_array(args);

        ::execve(exe.c_str(), localargv, localenvp);

        free_str_array(localargv);
        free_str_array(localenvp);
        std::abort();

        return false;
    }

    bool initparent(pid_t localpid)
    {
        ::close(m_pipeout[1]);
        ::close(m_pipeerr[1]);
        m_pid = localpid;

        usleep(m_waitchildtimeout);

        return is_running();
    }

    bool start(const std::string& exe,
               const std::string& workingdir,
               const std::vector < std::string > &args)
    {
        if (m_pid != -1)
            return false;

        m_command = exe;

        pid_t localpid;
        int err;

        if (::pipe(m_pipeout)) {
            err = errno;
            goto m_pipeout_failed;
        }

        if (::pipe(m_pipeerr)) {
            err = errno;
            goto m_pipeerr_failed;
        }

        if ((localpid = fork()) == -1) {
            err = errno;
            goto fork_failed;
        }

        if (::chdir(workingdir.c_str())) {
            err = errno;
            goto chdir_failed;
        }

        if (localpid == 0) {
            return initchild(exe, args);
        } else {
            return initparent(localpid);
        }

    fork_failed:
    chdir_failed:
        ::close(m_pipeerr[0]);
        ::close(m_pipeerr[1]);
    m_pipeerr_failed:
        ::close(m_pipeout[0]);
        ::close(m_pipeout[1]);
    m_pipeout_failed:
        m_msg.assign(strerror(err));
        return false;
    }

    bool wait()
    {
        if (m_finish) {
            return true;
        }
        switch (waitpid(m_pid, &m_status, 0)) {
        case -1:
            std::cout.flush();
            return true;
        case 0:
            std::cout.flush();
            assert(false);
        default:
            std::cout.flush();
            m_finish = true;
            return false;
        }
    }

    bool status(std::string *msg, bool *success)
    {
        assert(m_finish);

        if (WIFEXITED(m_status)) {
            m_msg += (fmt("[%1%] (%2%) exited, status=%3%\n") %
                      m_command % m_pid % WEXITSTATUS(m_status)).str();
            *success = !WEXITSTATUS(m_status);
        } else if (WIFSIGNALED(m_status)) {
            m_msg += (fmt("[%1%] (%2%) killed by signal %3%\n") %
                      m_command % m_pid % WTERMSIG(m_status)).str();
            *success = false;
        } else if (WIFSTOPPED(m_status)) {
            m_msg += (fmt("[%1%] (%2%) stopped by signal %3%\n") %
                      m_command % m_pid % WSTOPSIG(m_status)).str();
            *success = false;
        } else if (WIFCONTINUED(m_status)) {
            m_msg += (fmt("[%1%] (%2%) continued\n") %
                      m_command % m_pid).str();
            *success = false;
        }

        *msg = m_msg;

        return true;
    }
};

Spawn::Spawn(ContextPtr ctx)
    : m_pimpl(std::make_unique<Spawn::Pimpl>(ctx, 50000u))
{
}

Spawn::~Spawn() = default;

bool Spawn::start(const std::string& exe,
                  const std::string& workingdir,
                  const std::vector < std::string > &args,
                  unsigned int waitchildtimeout)
{
    if (not m_pimpl->m_finish) {
        m_pimpl->wait();
        ::close(m_pimpl->m_pipeout[1]);
        ::close(m_pimpl->m_pipeerr[1]);
        ::close(m_pimpl->m_pipeout[0]);
        ::close(m_pimpl->m_pipeerr[0]);
    }

    m_pimpl->init(waitchildtimeout);

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

    return m_pimpl->m_pid != -1 and not m_pimpl->m_finish;
}

bool Spawn::isfinish()
{
    if (not m_pimpl) {
        return false;
    }

    if (m_pimpl->m_finish) {
        return true;
    } else {
        m_pimpl->is_running();
    }
    return m_pimpl->m_finish;
}

bool Spawn::get(std::string *output, std::string *error)
{
    if (not m_pimpl or m_pimpl->m_finish)
        return false;

    return m_pimpl->get(output, error);
}

bool Spawn::status(std::string *msg, bool *success)
{
    if (not m_pimpl or not m_pimpl->m_finish)
        return false;

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
            (fmt(_("Package command line: error, empty command `%1%'"))
             % command).str());

    argv.front() = m_pimpl->m_context->findProgram(argv.front()).string();

    return argv;
}

}}
