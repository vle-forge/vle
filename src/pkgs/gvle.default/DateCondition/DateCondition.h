/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
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

#ifndef GVLE_CONDDATEPLUGIN_H
#define GVLE_CONDDATEPLUGIN_H

#include <QObject>
#include <QSettings>
#include <QDate>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QComboBox>
#include <vle/utils/Package.hpp>

#include <vle/gvle/plugin_cond.h>
#include <vle/gvle/logger.h>
#include <vle/gvle/vlevpz.hpp>
#include <vle/gvle/gvle_widgets.h>

//#include "tab.h"
//#include "toolbar.h"

namespace vle{
namespace gvle{



class DateCondition : public PluginExpCond
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginExpCond")
    Q_INTERFACES(vle::gvle::PluginExpCond)

public:
    DateCondition();
    ~DateCondition();
    QString getname() override;
    QWidget* getWidget() override;
    void setSettings(QSettings *s) override;
    void setLogger(Logger *logger) override;
    void init(vleVpz* vpz, const QString& cond) override;
    PluginExpCond* newInstance() override {return new DateCondition();}

public slots:
    void dateSelected(QDate date);
    void calendarDestroyed(QObject *obj);
    void onTextUpdated(const QString& id,
            const QString& old, const QString& neW);
    void onTypeChanged(const QString & type);

private:
    QSettings*  mSettings;
    Logger*     mLogger;
    //MainTab*    mWidgetTab;
    //widToolbar* mWidgetToolbar;

    QString     mExpCond;
    vleVpz*     mVpz;
    QWidget*    mWidget;
    VleLineEdit* mNamePortField;
    QComboBox*    mComboBox;
    QCalendarWidget* mcalendar;
};

}}//namespaces

#endif // EXPCOND_DUMMY_H
