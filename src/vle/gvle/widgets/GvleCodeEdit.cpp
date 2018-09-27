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

#include <algorithm>
#include <cfloat>
#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QTableWidget>
#include <QtDebug>

#include "GvleCodeEdit.h"

#include <vle/utils/Tools.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/User.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>

namespace vle {
namespace gvle {

GvleCodeHighlighter::GvleCodeHighlighter(QTextDocument* parent)
  : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b"
                    << "\\bclass\\b"
                    << "\\bconst\\b"
                    << "\\bdouble\\b"
                    << "\\benum\\b"
                    << "\\bexplicit\\b"
                    << "\\bfriend\\b"
                    << "\\binline\\b"
                    << "\\bint\\b"
                    << "\\blong\\b"
                    << "\\bnamespace\\b"
                    << "\\boperator\\b"
                    << "\\bprivate\\b"
                    << "\\bprotected\\b"
                    << "\\bpublic\\b"
                    << "\\bshort\\b"
                    << "\\bsignals\\b"
                    << "\\bsigned\\b"
                    << "\\bslots\\b"
                    << "\\bstatic\\b"
                    << "\\bstruct\\b"
                    << "\\btemplate\\b"
                    << "\\btypedef\\b"
                    << "\\btypename\\b"
                    << "\\bunion\\b"
                    << "\\bunsigned\\b"
                    << "\\bvirtual\\b"
                    << "\\bvoid\\b"
                    << "\\bvolatile\\b";
    foreach (const QString& pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void
GvleCodeHighlighter::highlightBlock(const QString& text)
{
    foreach (const HighlightingRule& rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength =
              endIndex - startIndex + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex =
          commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

GvleCodeLineNumberArea::GvleCodeLineNumberArea(GvleCodeEdit* editor)
  : QWidget(editor)
  , codeEditor(editor)
{}

///////////////////////////////////////////////

QSize
GvleCodeLineNumberArea::sizeHint() const
{
    return QSize(codeEditor->GvleCodelineNumberAreaWidth(), 0);
}

void
GvleCodeLineNumberArea::paintEvent(QPaintEvent* event)
{
    codeEditor->GvleCodelineNumberAreaPaintEvent(event);
}

///////////////////////////////////////////////

GvleCodeEdit::GvleCodeEdit(QWidget* parent)
  : QPlainTextEdit(parent)
{
    // handle highlighting
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(9);
    setFont(font);

    m_highlighter = new GvleCodeHighlighter(document());
    m_lineNumberArea = new GvleCodeLineNumberArea(this);

    connect(this,
            SIGNAL(blockCountChanged(int)),
            this,
            SLOT(updateVleCodeLineNumberAreaWidth(int)));
    connect(this,
            SIGNAL(updateRequest(QRect, int)),
            this,
            SLOT(updateVleCodeLineNumberArea(QRect, int)));
    connect(this,
            SIGNAL(cursorPositionChanged()),
            this,
            SLOT(highlightCurrentLine()));

    updateVleCodeLineNumberAreaWidth(0);
    highlightCurrentLine();

    // setUndoRedoEnabled(false);
    installEventFilter(this);
    mtimer.setInterval(500);
    connect(&mtimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

GvleCodeEdit::GvleCodeEdit(QWidget* parent,
                           const QString& text,
                           const QString& idstr,
                           bool allowExternalUndo)
  : QPlainTextEdit(parent)
  , stacked_value("")
  , id(idstr)
{
    // handle highlighting
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(9);
    setFont(font);

    m_highlighter = new GvleCodeHighlighter(document());
    m_lineNumberArea = new GvleCodeLineNumberArea(this);

    connect(this,
            SIGNAL(blockCountChanged(int)),
            this,
            SLOT(updateVleCodeLineNumberAreaWidth(int)));
    connect(this,
            SIGNAL(updateRequest(QRect, int)),
            this,
            SLOT(updateVleCodeLineNumberArea(QRect, int)));
    connect(this,
            SIGNAL(cursorPositionChanged()),
            this,
            SLOT(highlightCurrentLine()));

    updateVleCodeLineNumberAreaWidth(0);
    highlightCurrentLine();

    this->setLineWrapMode(QPlainTextEdit::NoWrap);
    // this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // this->setTextInteractionFlags(Qt::NoTextInteraction);
    // this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setFocusPolicy(Qt::ClickFocus);
    const int tabStop = 4; // 4 characters

    QFontMetrics metrics(font);
    this->setTabStopWidth(tabStop * metrics.width(' '));

    setText(text);
    this->resize(document()->size().toSize());

    if (not allowExternalUndo) {
        setUndoRedoEnabled(false);
    }
    installEventFilter(this);
    mtimer.setInterval(500);
    connect(&mtimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

GvleCodeEdit::~GvleCodeEdit()
{
    delete m_highlighter;
    delete m_lineNumberArea;
}

void
GvleCodeEdit::setText(const QString& text)
{
    this->document()->setPlainText(text);
    stacked_value = text;
}

void
GvleCodeEdit::GvleCodelineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top =
      (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height();
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0,
                             top,
                             m_lineNumberArea->width(),
                             fontMetrics().height(),
                             Qt::AlignRight,
                             number);
        }
        block = block.next();
        top = bottom;
        bottom = top + (int)blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int
GvleCodeEdit::GvleCodelineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

void
GvleCodeEdit::focusInEvent(QFocusEvent* e)
{
    mtimer.start();
    emit selected(id);
    QPlainTextEdit::focusInEvent(e);
}

void
GvleCodeEdit::focusOutEvent(QFocusEvent* e)
{
    mtimer.stop();
    if (document()->toPlainText() != stacked_value) {
        emit textUpdated(id, stacked_value, document()->toPlainText());
        stacked_value = document()->toPlainText();
    }
    QPlainTextEdit::focusOutEvent(e);
}

bool
GvleCodeEdit::eventFilter(QObject* watched, QEvent* event)
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
GvleCodeEdit::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(
      QRect(cr.left(), cr.top(), GvleCodelineNumberAreaWidth(), cr.height()));
}

void
GvleCodeEdit::onTimeout()
{
    if (document()->toPlainText() != stacked_value) {
        emit textUpdated(id, stacked_value, document()->toPlainText());
        stacked_value = document()->toPlainText();
    }
}

void
GvleCodeEdit::updateVleCodeLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(GvleCodelineNumberAreaWidth(), 0, 0, 0);
}

void
GvleCodeEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void
GvleCodeEdit::updateVleCodeLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(
          0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateVleCodeLineNumberAreaWidth(0);
}
}
} // namespaces
