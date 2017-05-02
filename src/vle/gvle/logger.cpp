/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
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
}
} // namepsaces
