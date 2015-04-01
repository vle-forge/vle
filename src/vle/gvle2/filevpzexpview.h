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
#ifndef FILEVPZEXPVIEW_H
#define FILEVPZEXPVIEW_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <vle/gvle2/vlevpz.h>
#include <vle/gvle2/plugin_output.h>

namespace Ui {
class FileVpzExpView;
}

class FileVpzExpView : public QWidget
{
    Q_OBJECT

public:
    explicit FileVpzExpView(QWidget *parent = 0);
    ~FileVpzExpView();
    void setVpz(vleVpz *vpz);
    void reload();

signals:
    void outputChange(QString viewName, QString outputName);

public slots:
    void onViewSelected(QListWidgetItem * item);
    void onOutputSelected(const QString& item);
    void onViewTypeSelected(const QString& item);
    void onTimeStepChanged(double v);

private:
    void reloadViews();
    void reloadOov();
    void outputHidePlugin();
    QString getSelectedViewName();
    QString getSelectedOutputPlugin();
    void updatePlugin(const QString& plug);

    Ui::FileVpzExpView *ui;
    vleVpz             *mVpz;
    PluginOutput       *mPlugin;
    QString currView;
    QString currOutput;

};

#endif // FILEVPZEXPVIEW_H
