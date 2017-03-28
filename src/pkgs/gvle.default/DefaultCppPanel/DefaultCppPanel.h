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

#ifndef GVLE_DEFAULT_CPP_PANEL_H
#define GVLE_DEFAULT_CPP_PANEL_H

#include <QWidget>
#include <QObject>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <vle/gvle/plugin_mainpanel.h>
#include <vle/gvle/gvle_file.h>

namespace vle {
namespace gvle {

class CodeEditor;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

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

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *codeEditor;
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    Highlighter* m_highlighter;
    QWidget *m_lineNumberArea;
};

class DefaultCppPanel : public PluginMainPanel
{
    Q_OBJECT;
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginMainPanel");
    Q_INTERFACES(vle::gvle::PluginMainPanel);

public:
    DefaultCppPanel();
    virtual ~DefaultCppPanel();

    QString  getname() override;
    QWidget* leftWidget() override;
    QWidget* rightWidget() override;
    void undo() override;
    void redo() override;
    void init(const gvle_file& file, utils::Package* pkg, Logger*,
            gvle_plugins* plugs, const utils::ContextPtr& ctx) override;
    QString canBeClosed() override;
    void save() override;
    void discard() override {}
    PluginMainPanel* newInstance() override {return new DefaultCppPanel();}

public slots:
    void onUndoAvailable(bool);
public:
    CodeEditor* m_edit;
    QString m_file;

private:
    void initCpp(QString pkg, QString classname, QString filePath);
};

}} //namespaces


#endif
