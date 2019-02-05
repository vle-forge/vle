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

#ifndef GVLE_WIDGETS_GVLETEXTEDIT_H
#define GVLE_WIDGETS_GVLETEXTEDIT_H

#include <QPlainTextEdit>
#include <QTimer>

#include <vle/gvle/export.hpp>

namespace vle {
namespace gvle {

/**
 * Text editor to be used with the global undo/rdo gvle system
 * TODO should contain text rules no comma, no '<', etc..
 */
class GVLE_API GvleTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    GvleTextEdit(QWidget* parent,
                 const QString& text,
                 const QString& id = "",
                 bool allowExternalUndo = false);
    ~GvleTextEdit() override;

    void setText(const QString& text);
signals:
    void textUpdated(const QString& id,
                     const QString& old,
                     const QString& newVal);
    void selected(const QString& id);

protected:
    void focusInEvent(QFocusEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* event);

private slots:
    void onTimeout();

private:
    QString stacked_value;
    QString id;
    QTimer mtimer;
};
}
} // namespaces

#endif
