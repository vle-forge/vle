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

#ifndef gvle_PLUGINMODELERVIEW_H
#define gvle_PLUGINMODELERVIEW_H

#include <QPluginLoader>
#include <QTextStream>
#include <QWidget>
#include "vlepackage.h"
#include "plugin_modeler.h"
#include "sourcecpp.h"

namespace Ui {
class pluginModelerView;
}

namespace vle {
namespace gvle {


class pluginModelerView : public QWidget
{
    Q_OBJECT

public:
    explicit pluginModelerView(QWidget *parent = 0);
    ~pluginModelerView();
    void setPlugin(QString filename);
    void setPackage(vlePackage *package);
    void addClass(QString name, QString filename);
    void showSummary();
    void addEditTab(QWidget *widget, QString name);
    QString getName();
    QString getFilename();

public slots:
    void addNewTab();
    void onTabRename(QWidget *tab, QString name);
    void onSaveClass(QString name);
    void onOpenTab  (QString filename);
    void onCloseTab (int index);
    void onDuplicateRequest(QString className);
    void onDuplicateClass(QString srcFile, QString dst);

signals:
    void refreshFiles();

protected:
    void setMainWidget();

private:
    Ui::pluginModelerView *ui;
    QString                mPluginFile;
    QPluginLoader         *mLoader;
    PluginModeler         *mModeler;
    vlePackage            *mPackage;
    QList<sourceCpp *>     mListSources;
};

}}//namepsaces

#endif // PLUGINMODELERVIEW_H
