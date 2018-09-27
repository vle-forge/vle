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

#include "GvleTextEdit.h"

namespace vle {
namespace gvle {

/************************** GvleTextEdit ******************************/
GvleTextEdit::GvleTextEdit(QWidget* parent,
                           const QString& text,
                           const QString& idstr,
                           bool allowExternalUndo)
  : QPlainTextEdit(parent)
  , stacked_value("")
  , id(idstr)
  , mtimer()
{
    this->setLineWrapMode(QPlainTextEdit::NoWrap);
    this->setTextInteractionFlags(Qt::TextEditorInteraction);
    // this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // this->setContextMenuPolicy(Qt::NoContextMenu);

    if (not allowExternalUndo) {
        setUndoRedoEnabled(false);
    }
    setText(text);
    installEventFilter(this);
    // this->resize(document()->size().toSize());
    mtimer.setInterval(500);
    connect(&mtimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

GvleTextEdit::~GvleTextEdit()
{}

void
GvleTextEdit::setText(const QString& text)
{
    this->document()->setPlainText(text);
    stacked_value = text;
}

void
GvleTextEdit::focusInEvent(QFocusEvent* e)
{
    mtimer.start();
    emit selected(id);
    QPlainTextEdit::focusInEvent(e);
}

void
GvleTextEdit::focusOutEvent(QFocusEvent* e)
{
    mtimer.stop();
    if (document()->toPlainText() != stacked_value) {
        emit textUpdated(id, stacked_value, document()->toPlainText());
        stacked_value = document()->toPlainText();
    }
    QPlainTextEdit::focusOutEvent(e);
}

bool
GvleTextEdit::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::ShortcutOverride) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->matches(QKeySequence::Undo)) {
            event->ignore();
            return true;
        }
        if (keyEvent->matches(QKeySequence::Redo)) {
            event->ignore();
            return true;
        }
    }
    return QPlainTextEdit::eventFilter(watched, event);
}

void
GvleTextEdit::onTimeout()
{
    if (document()->toPlainText() != stacked_value) {
        emit textUpdated(id, stacked_value, document()->toPlainText());
        stacked_value = document()->toPlainText();
    }
}
}
} // namespaces
