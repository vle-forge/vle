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

#ifndef gvle_LOGGER_H
#define gvle_LOGGER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QTime>

#include <vle/utils/Context.hpp>

namespace vle {
namespace gvle {

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger();
    void setWidget(QPlainTextEdit* widget);
    void log(QString message);
    void logExt(QString message, bool isError = false);
    /**
     * forward a log from Context, typically coming from another
     * thread than the GUI, by using QT signal/slots system
     **/
    void forwardFromCtx(QString msg);

signals:
    void logFromCtx(QString msg);

private:
    QPlainTextEdit* mWidget;
};

/*******************************************/
struct Logger_ctx : public vle::utils::Context::LogFunctor
{
public:
    Logger_ctx(Logger* mainWin);

    ~Logger_ctx() override = default;

    void write(const vle::utils::Context& /*ctx*/,
               int priority,
               const std::string& str) noexcept override;

    void write(const vle::utils::Context& /*ctx*/,
               int priority,
               const char* format,
               va_list args) noexcept override;

    Logger* logger_fwd;
};
}
} // namespaces

#endif // LOGGER_H
