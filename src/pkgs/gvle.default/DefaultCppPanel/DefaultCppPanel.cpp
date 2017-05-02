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

#include "DefaultCppPanel.h"
#include <QFileInfo>
#include <QFontDatabase>
#include <QPainter>
#include <sstream>
#include <vle/utils/Template.hpp>

namespace vle {
namespace gvle {

Highlighter::Highlighter(QTextDocument* parent)
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
Highlighter::highlightBlock(const QString& text)
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

LineNumberArea::LineNumberArea(CodeEditor* editor)
  : QWidget(editor)
  , codeEditor(editor)
{
}

QSize
LineNumberArea::sizeHint() const
{
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void
LineNumberArea::paintEvent(QPaintEvent* event)
{
    codeEditor->lineNumberAreaPaintEvent(event);
}

CodeEditor::CodeEditor(QWidget* parent)
  : QPlainTextEdit(parent)
{
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(9);
    setFont(font);

    m_highlighter = new Highlighter(document());
    m_lineNumberArea = new LineNumberArea(this);

    connect(this,
            SIGNAL(blockCountChanged(int)),
            this,
            SLOT(updateLineNumberAreaWidth(int)));
    connect(this,
            SIGNAL(updateRequest(QRect, int)),
            this,
            SLOT(updateLineNumberArea(QRect, int)));
    connect(this,
            SIGNAL(cursorPositionChanged()),
            this,
            SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    //    QObject::connect(m_edit, SIGNAL(undoAvailable(bool)),
    //                     this, SLOT(onUndoAvailable(bool)));
}

int
CodeEditor::lineNumberAreaWidth()
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
CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void
CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(
          0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void
CodeEditor::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(
      QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void
CodeEditor::highlightCurrentLine()
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
CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
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

DefaultCppPanel::DefaultCppPanel()
  : PluginMainPanel()
{
    m_edit = new CodeEditor;
}

DefaultCppPanel::~DefaultCppPanel()
{
    delete m_edit;
}

void
DefaultCppPanel::initCpp(QString pkg, QString classname, QString filePath)
{
    std::string tpl =
      "/*\n"
      "* @@tagdynamic@@\n"
      "* @@tagdepends:@@endtagdepends\n"
      "*/\n"
      "#include <vle/value/Value.hpp>\n"
      "#include <vle/devs/Dynamics.hpp>\n"
      "namespace vd = vle::devs;\n"
      "namespace vv = vle::value;\n"
      "namespace {{pkg}} {\n"
      "class {{classname}} : public vd::Dynamics\n"
      "{\n"
      " public:\n"
      "  {{classname}}(const vd::DynamicsInit& init,\n"
      "                const vd::InitEventList& events)\n"
      "        : vd::Dynamics(init, events)\n"
      "  {\n"
      "  }\n"
      "  virtual ~{{classname}}()\n"
      "  {\n"
      "  }\n"
      "  virtual vd::Time init(vle::devs::Time /*time*/) override\n"
      "  {\n"
      "    return vd::infinity;\n"
      "  }\n"
      "  virtual void output(vle::devs::Time /*time*/,\n"
      "                      vd::ExternalEventList& /*output*/) const "
      "override\n"
      "  {\n"
      "  }\n"
      "  virtual vd::Time timeAdvance() const override\n"
      "  {\n"
      "    return vd::infinity;\n"
      "  }\n"
      "  virtual void internalTransition(vle::devs::Time /*time*/) override\n"
      "  {\n"
      "  }\n"
      "  virtual void externalTransition(\n"
      "            const vd::ExternalEventList& /*event*/,\n"
      "            vle::devs::Time /*time*/) override\n"
      "  {\n"
      "  }\n"
      "  virtual void confluentTransitions(\n"
      "            vle::devs::Time time,\n"
      "            const vd::ExternalEventList& events) override\n"
      "  {\n"
      "    internalTransition(time);\n"
      "    externalTransition(events, time);\n"
      "  }\n"
      "  virtual std::unique_ptr<vle::value::Value> observation(\n"
      "            const vd::ObservationEvent& /*event*/) const override\n"
      "  {\n"
      "    return 0;\n"
      "  }\n"
      "  virtual void finish()\n"
      "  {\n"
      "  }\n"
      " };\n"
      "} // namespace\n"
      "DECLARE_DYNAMICS({{pkg}}::{{classname}})\n";

    vle::utils::Template vleTpl(tpl);
    vleTpl.stringSymbol().append("classname", classname.toStdString());
    vleTpl.stringSymbol().append("pkg", pkg.toStdString());

    std::ostringstream out;
    vleTpl.process(out);

    QFile cppfile(filePath);
    cppfile.open(QIODevice::WriteOnly | QIODevice::Text);

    if (!cppfile.isOpen()) {
        qDebug() << "- Error, unable to open"
                 << "outputFilename"
                 << "for output";
    }
    QTextStream outStream(&cppfile);
    outStream << out.str().c_str();
    cppfile.close();
}

void
DefaultCppPanel::init(const gvle_file& gf,
                      utils::Package* pkg,
                      Logger* /*log*/,
                      gvle_plugins* /*plugs*/,
                      const utils::ContextPtr& /*ctx*/)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    m_file = gf.source_file;

    if (not QFile(m_file).exists()) {
        initCpp(
          QString(pkg->name().c_str()), QFileInfo(m_file).baseName(), m_file);
    }

    QFile cppFile(m_file);

    if (!cppFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " Error DefaultCppPanel::init ";
    }

    QTextStream in(&cppFile);
    m_edit->setPlainText(in.readAll());
}

QString
DefaultCppPanel::canBeClosed()
{
    return "";
}

void
DefaultCppPanel::save()
{
    if (m_file != "") {
        QFile file(m_file);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            file.write(m_edit->toPlainText().toStdString().c_str());
            file.flush();
            file.close();
        }
    }
    emit undoAvailable(false);
}

QString
DefaultCppPanel::getname()
{
    return "Default";
}

QWidget*
DefaultCppPanel::leftWidget()
{
    return m_edit;
}

QWidget*
DefaultCppPanel::rightWidget()
{
    return 0;
}

void
DefaultCppPanel::undo()
{
}

void
DefaultCppPanel::redo()
{
}

void
DefaultCppPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}
}
} // namespaces
