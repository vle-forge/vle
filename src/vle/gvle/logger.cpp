/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2014-2018 INRA http://www.inra.fr
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

#include "logger.h"

#include <iostream>
#include <vle/utils/Tools.hpp>

namespace vle {
namespace gvle {

Logger::Logger()
{
    mWidget = 0;
}

void
Logger::setWidget(QPlainTextEdit* widget)
{
    mWidget = widget;
}

void
Logger::log(QString message)
{
    if (mWidget == 0)
        return;

    QTime currentTime = QTime::currentTime();
    QString logTime = QString("[%1] ").arg(currentTime.toString("hh:mm:ss"));

    QString logLine = logTime + message;
    mWidget->appendPlainText(logLine);
    mWidget->moveCursor(QTextCursor::End);
}

void
Logger::logExt(QString message, bool isError)
{
    if (mWidget == 0)
        return;

    QString logLine;
    if (isError)
        logLine = "<font color=#FF0000><pre>" + message + "</pre></font>";
    else
        logLine = "<font color=#0000FF><pre>" + message + "</pre></font>";
    mWidget->appendHtml(logLine);
    mWidget->moveCursor(QTextCursor::End);
}

void
Logger::forwardFromCtx(QString msg)
{
    emit(logFromCtx(msg));
}

/*******************************************/

Logger_ctx::Logger_ctx(Logger* logger)
  : logger_fwd(logger)
{}

void
Logger_ctx::write(const vle::utils::Context& /*ctx*/,
                  int priority,
                  const std::string& str) noexcept
{
    QString msg = QString::fromStdString(str);
    if (priority <= 3)
        msg = QString("Error (level %1) :").arg(priority);
    else if (priority <= 5)
        msg = QString("Note: ");
    else if (priority > 6)
        msg = QString("[debug]: ");
    logger_fwd->forwardFromCtx(msg);
}

void
Logger_ctx::write(const vle::utils::Context& /*ctx*/,
                  int priority,
                  const char* format,
                  va_list args) noexcept
{
    QString msg;

    if (priority <= 3)
        msg = QString("Error (level %1) :").arg(priority);
    else if (priority <= 5)
        msg = QString("Note: ");
    else if (priority > 6)
        msg = QString("[debug]: ");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    msg += QString::vasprintf(format, args);
#else
    msg += QString::fromStdString(vle::utils::vformat(format, args));
#endif
    logger_fwd->forwardFromCtx(msg);
}
}
} // namepsaces
