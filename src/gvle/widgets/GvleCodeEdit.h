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

#ifndef GVLE_WIDGETS_GVLECODEEDIT_H
#define GVLE_WIDGETS_GVLECODEEDIT_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTimer>

#include <vle/gvle/export.hpp>

namespace vle {
namespace gvle {

class GvleCodeEdit;

/****
 * Syntax highlighter
 */
class GVLE_API GvleCodeHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    GvleCodeHighlighter(QTextDocument* parent = 0);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

/****
 * Line number
 */
class GVLE_API GvleCodeLineNumberArea : public QWidget
{
public:
    GvleCodeLineNumberArea(GvleCodeEdit* editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    GvleCodeEdit* codeEditor;
};

/****
 * Main class
 */
class GVLE_API GvleCodeEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    GvleCodeEdit(QWidget* parent = 0);
    GvleCodeEdit(QWidget* parent,
                 const QString& text,
                 const QString& id = "",
                 bool allowExternalUndo = false);
    ~GvleCodeEdit() override;

    void setText(const QString& text);
    void GvleCodelineNumberAreaPaintEvent(QPaintEvent* event);
    int GvleCodelineNumberAreaWidth();
signals:
    void textUpdated(const QString& id,
                     const QString& old,
                     const QString& newVal);
    void selected(const QString& id);

protected:
    void focusOutEvent(QFocusEvent* e) override;
    void focusInEvent(QFocusEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* event);
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onTimeout();
    void updateVleCodeLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateVleCodeLineNumberArea(const QRect&, int);

private:
    QString stacked_value;
    QString id;
    QTimer mtimer;
    GvleCodeHighlighter* m_highlighter;
    QWidget* m_lineNumberArea;
};
}
} // namespaces

#endif
