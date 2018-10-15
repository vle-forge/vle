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

#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/details/ShellUtils.hpp>
#include <vle/utils/i18n.hpp>

#include <algorithm>
#include <iostream>
#include <thread>
#include <utility>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/wait.h>
#include <unistd.h>

#ifdef __APPLE__
#include <crt_externs.h>
#include <signal.h>
extern "C" char** environ = *_NSGetEnviron();
#endif

namespace vle {
namespace utils {

const unsigned long int Spawn::default_buffer_size = BUFSIZ;

struct strdup_functor : std::unary_function<std::string, char*>
{
    char* operator()(const std::string& str) const
    {
        return strdup(str.c_str());
    }
};

void
free_str_array(char** args)
{
    char** tmp;

    for (tmp = args; *tmp; ++tmp) {
        free(*tmp);
    }

    delete[] args;
}

char**
convert_string_str_array(const std::vector<std::string>& args)
{
    char** result = nullptr;

    result = new char*[args.size() + 1];

    std::transform(args.begin(), args.end(), result, strdup_functor());

    result[args.size()] = nullptr;

    return result;
}

static char**
prepare_environment_variable()
{
    char** it = ::environ;
    char** jt = nullptr;
    char** result = nullptr;
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
static int
input_timeout(int fd, std::chrono::milliseconds wait)
{
    fd_set set;
    struct timeval timeout;
    long int result;

    FD_ZERO(&set);
    FD_SET(fd, &set);

    wait *= 1000;

    timeout.tv_sec = 0;
    timeout.tv_usec = wait.count();

    do {
        result = select(FD_SETSIZE, &set, nullptr, nullptr, &timeout);
    } while (result == -1L && errno == EINTR);

    return static_cast<int>(result);
}

class Spawn::Pimpl
{
public:
    ContextPtr m_context;
    std::chrono::milliseconds m_waitchildtimeout;
    pid_t m_pid;
    int m_pipeout[2];
    int m_pipeerr[2];
    int m_status;
    bool m_start;
    bool m_finish;
    std::string m_msg;
    std::string m_command;

    Pimpl(ContextPtr ctx, std::chrono::milliseconds waitchildtimeout)
      : m_context(std::move(ctx))
      , m_waitchildtimeout(waitchildtimeout)
      , m_pid(-1)
      , m_status(0)
      , m_start(false)
      , m_finish(true)
    {}

    void init(std::chrono::milliseconds waitchildtimeout)
    {
        if (m_start and not m_finish) {
            wait();
            ::close(m_pipeout[1]);
            ::close(m_pipeerr[1]);
            ::close(m_pipeout[0]);
            ::close(m_pipeerr[0]);
        }

        m_pid = -1;
        m_waitchildtimeout = waitchildtimeout;
        m_status = 0;
        m_finish = true;
        m_start = false;
    }

    ~Pimpl()
    {
        if (m_start) {
            if (not m_finish)
                wait();

            ::close(m_pipeout[1]);
            ::close(m_pipeerr[1]);
            ::close(m_pipeout[0]);
            ::close(m_pipeerr[0]);
        }
    }

    bool is_running()
    {
        if (m_start == false or m_finish == true)
            return false;

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
            m_context->log(VLE_LOG_ALERT,
                           _("Spawn: check running child fail: %s\n"),
                           strerror(errno));
            return false;
        default:
            m_finish = true;
            return true;
        }
    }

    bool get(std::string* output, std::string* error)
    {
        assert(m_start);

        is_running();

        auto buffer = std::make_unique<char[]>(BUFSIZ);
        int result;

        if ((result = input_timeout(m_pipeout[0], m_waitchildtimeout)) == -1)
            return false;

        if (result) {
            auto size = read(m_pipeout[0], buffer.get(), BUFSIZ);
            if (size > 0 and output) {
                output->append(buffer.get(), size);
            }
        }

        if ((result = input_timeout(m_pipeerr[0], m_waitchildtimeout)) == -1)
            return false;

        if (result) {
            auto size = read(m_pipeerr[0], buffer.get(), BUFSIZ);
            if (size > 0 and error) {
                error->append(buffer.get(), size);
            }
        }

        return true;
    }

    bool initchild(const Path& exe,
                   const Path& workingdir,
                   std::vector<std::string> args)
    {
        if (::chdir(workingdir.string().c_str())) {
            m_context->error(
              _("Spawn: child fails to change current directory:"
                " to `%s'\n"),
              workingdir.string().c_str());
            std::abort();
        }

        ::dup2(m_pipeout[1], STDOUT_FILENO);
        ::dup2(m_pipeerr[1], STDERR_FILENO);

        ::close(m_pipeout[1]);
        ::close(m_pipeerr[1]);
        ::close(m_pipeout[0]);
        ::close(m_pipeerr[0]);

        args.insert(args.begin(), exe.string());

        char** localenvp = prepare_environment_variable();
        char** localargv = convert_string_str_array(args);

        ::execve(exe.string().c_str(), localargv, localenvp);

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

        std::this_thread::sleep_for(m_waitchildtimeout);

        return is_running();
    }

    bool start(const Path& exe,
               const Path& workingdir,
               const std::vector<std::string>& args)
    {
        assert(m_finish);
        m_start = true;
        m_finish = false;

        m_command = exe.string();
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

        if (localpid == 0)
            return initchild(exe, workingdir, args);

        return initparent(localpid);

    fork_failed:
        ::close(m_pipeerr[0]);
        ::close(m_pipeerr[1]);
    m_pipeerr_failed:
        ::close(m_pipeout[0]);
        ::close(m_pipeout[1]);
    m_pipeout_failed:
        m_context->error(_("Spawn: failure `%s'\n"), strerror(err));
        m_msg.assign(strerror(err));
        return false;
    }

    bool wait()
    {
        assert(m_start);

        if (m_finish)
            return true;

        switch (waitpid(m_pid, &m_status, 0)) {
        case -1:
            std::cout.flush();
            return true;
        case 0:
            std::cout.flush();
            return false;
        default:
            std::cout.flush();
            m_finish = true;
            return false;
        }
    }

    void kill()
    {
        assert(m_start);

        if (m_finish)
            return;

        ::kill(m_pid, SIGTERM);
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

        if (WIFEXITED(m_status)) {
            m_msg += utils::format(_("[%s] (%d) exited, status=%d\n"),
                                   m_command.c_str(),
                                   static_cast<int>(m_pid),
                                   WEXITSTATUS(m_status));
            *success = !WEXITSTATUS(m_status);
        } else if (WIFSIGNALED(m_status)) {
            m_msg += utils::format(_("[%s] (%d) killed by signal %d\n"),
                                   m_command.c_str(),
                                   static_cast<int>(m_pid),
                                   WTERMSIG(m_status));
            *success = false;
        } else if (WIFSTOPPED(m_status)) {
            m_msg += utils::format(_("[%s] (%d) stopped by signal %d\n"),
                                   m_command.c_str(),
                                   static_cast<int>(m_pid),
                                   WSTOPSIG(m_status));

            *success = false;
        } else if (WIFCONTINUED(m_status)) {
            m_msg += utils::format(_("[%s] (%d) continued\n"),
                                   m_command.c_str(),
                                   static_cast<int>(m_pid));
            *success = false;
        }

        *msg = m_msg;

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
