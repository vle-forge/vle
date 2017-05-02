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

#include <algorithm>
#include <float.h>
#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QTableWidget>
#include <QtDebug>
////#include <QtWidgets/QStackedWidget>
//#include <QStackedWidget>

#include "gvle_widgets.h"
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
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>

namespace vle {
namespace gvle {

VleNullWidget::VleNullWidget(QWidget* parent, const QString& idStr)
  : QWidget(parent)
  , id(idStr)
{
    setFocusPolicy(Qt::StrongFocus);
}

VleNullWidget::~VleNullWidget()
{
}

void
VleNullWidget::focusInEvent(QFocusEvent* /*e*/)
{
    emit selected(id);
}

/******************* Bool ********************************/

VleBooleanEdit::VleBooleanEdit(QWidget* parent, bool val, const QString& idStr)
  : QCheckBox(parent)
  , id(idStr)
{

    if (val) {
        setCheckState(Qt::Checked);
    } else {
        setCheckState(Qt::Unchecked);
    }

    QObject::connect(
      this, SIGNAL(clicked(bool)), this, SLOT(onValueChanged(bool)));

    installEventFilter(this);
}

VleBooleanEdit::~VleBooleanEdit()
{
}

void
VleBooleanEdit::setValue(bool val)
{
    if (val) {
        setCheckState(Qt::Checked);
    } else {
        setCheckState(Qt::Unchecked);
    }
}

void
VleBooleanEdit::focusInEvent(QFocusEvent* e)
{
    QCheckBox::focusInEvent(e);
    emit selected(id);
}

void
VleBooleanEdit::onValueChanged(bool /*checked*/)
{
    if (checkState() == Qt::Checked) {
        emit valUpdated(id, true);
    } else {
        emit valUpdated(id, false);
    }
}

VleDoubleEdit::VleDoubleEdit(QWidget* parent,
                             double val,
                             const QString& idStr,
                             bool withDefaultMenu)
  : QLineEdit(parent)
  , id(idStr)
  , backup(val)
{
    if (not withDefaultMenu) {
        setContextMenuPolicy(Qt::NoContextMenu);
    }

    QDoubleValidator* validator = new QDoubleValidator(
      -DBL_MAX, DBL_MAX, std::numeric_limits<double>::digits10, this);
    validator->setNotation(QDoubleValidator::ScientificNotation);
    setValidator(validator);

    setValue(val);

    QObject::connect(
      this, SIGNAL(editingFinished()), this, SLOT(onValueChanged()));

    installEventFilter(this);
}

VleDoubleEdit::~VleDoubleEdit()
{
}

void
VleDoubleEdit::setValue(double val)
{
    setText(
      QLocale().toString(val, 'g', std::numeric_limits<double>::digits10));
}

bool
VleDoubleEdit::eventFilter(QObject* target, QEvent* event)
{
    if (target == this) {
        switch (event->type()) {
        case QEvent::FocusOut:
            if (text().isEmpty()) {
                setValue(backup);
            }
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(target, event);
}

void
VleDoubleEdit::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);
    emit selected(id);
}

void
VleDoubleEdit::onValueChanged()
{
    bool ok;
    double v = QLocale().toDouble(text(), &ok);
    if (ok and v != backup) {
        backup = v;
        emit valUpdated(id, v);
    }
}

VlePushButton::VlePushButton(QWidget* parent,
                             const QString& text,
                             const QString& idStr)
  : QPushButton(text, parent)
  , id(idStr)
{
    QObject::connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
}

VlePushButton::~VlePushButton()
{
}

void
VlePushButton::onClicked(bool b)
{
    emit clicked(id, b);
}

VleSpinBox::VleSpinBox(QWidget* parent,
                       int val,
                       const QString& idStr,
                       int min,
                       int max)
  : QSpinBox(parent)
  , id(idStr)
{
    setMaximum(max);
    setMinimum(min);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::NoContextMenu);
    setValue(val);

    QObject::connect(
      this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));

    QObject::connect(
      this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

VleSpinBox::~VleSpinBox()
{
}

void
VleSpinBox::setMin(int min)
{
    setMinimum(min);
}

void
VleSpinBox::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}

void
VleSpinBox::focusInEvent(QFocusEvent* e)
{
    QSpinBox::focusInEvent(e);
    emit selected(id);
}

void
VleSpinBox::onValueChanged(int i)
{
    emit valUpdated(id, i);
}

VleDoubleSpinBox::VleDoubleSpinBox(QWidget* parent,
                                   double val,
                                   const QString& idStr,
                                   int min,
                                   int max)
  : QDoubleSpinBox(parent)
  , id(idStr)
{
    setMaximum(max);
    setMinimum(min);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::NoContextMenu);
    setDecimals(6);
    setValue(val);

    QObject::connect(
      this, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
}
VleDoubleSpinBox::~VleDoubleSpinBox()
{
}

void
VleDoubleSpinBox::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}

void
VleDoubleSpinBox::focusInEvent(QFocusEvent* e)
{
    QDoubleSpinBox::focusInEvent(e);
    emit selected(id);
}

void
VleDoubleSpinBox::onValueChanged(double v)
{
    emit valUpdated(id, v);
}

VleCombo::VleCombo(QWidget* parent, const QString& idStr)
  : QComboBox(parent)
  , id(idStr)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(this,
                     SIGNAL(currentIndexChanged(const QString&)),
                     this,
                     SLOT(onValueChanged(const QString&)));
}
VleCombo::~VleCombo()
{
}

void
VleCombo::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}

VleCombo*
VleCombo::buildVleBoolCombo(QWidget* parent, const QString& idStr, bool val)
{
    VleCombo* w = new VleCombo(parent, idStr);
    QList<QString> l;
    l.append("true");
    l.append("false");
    QString currVal = "false";
    if (val) {
        currVal = "true";
    }
    w->addItems(l);
    w->setCurrentIndex(w->findText(currVal));
    return w;
}

void
VleCombo::focusInEvent(QFocusEvent* e)
{
    QComboBox::focusInEvent(e);
    emit selected(id);
}

void
VleCombo::onValueChanged(const QString& v)
{
    emit valUpdated(id, v);
}

/************************** VleLineEdit ******************************/

VleLineEdit::VleLineEdit(QWidget* parent,
                         const QString& val,
                         const QString& idStr,
                         bool withDefaultMenu)
  : QLineEdit(parent)
  , id(idStr)
  , backup(val)
{
    if (not withDefaultMenu) {
        setContextMenuPolicy(Qt::NoContextMenu);
    }
    setAlignment(Qt::AlignLeft);
    setValue(val);

    QObject::connect(
      this, SIGNAL(editingFinished()), this, SLOT(onValueChanged()));

    installEventFilter(this);
}

VleLineEdit::~VleLineEdit()
{
}

void
VleLineEdit::setValue(const QString& val)
{
    if (text() != val) {
        backup = val;
        setText(val);
    }
    home(false);
}

bool
VleLineEdit::eventFilter(QObject* target, QEvent* event)
{
    if (target == this) {
        switch (event->type()) {
        case QEvent::FocusOut:
            if (text().isEmpty()) {
                setValue(backup);
            }
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(target, event);
}

void
VleLineEdit::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);
    emit selected(id);
}

void
VleLineEdit::onValueChanged()
{
    if (text() != backup) {
        emit textUpdated(id, backup, text());
        backup = text();
    }
}

/************************** VleTextEdit ******************************/
VleTextEdit::VleTextEdit(QWidget* parent,
                         const QString& text,
                         const QString& idstr,
                         bool editOnDbleClick)
  : QPlainTextEdit(parent)
  , saved_value("")
  , id(idstr)
  , edit_on_dble_click(editOnDbleClick)
{
    this->setLineWrapMode(QPlainTextEdit::NoWrap);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setTextInteractionFlags(Qt::NoTextInteraction);
    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setFocusPolicy(Qt::ClickFocus);

    setText(text);
    this->resize(document()->size().toSize());
}
VleTextEdit::~VleTextEdit()
{
}

void
VleTextEdit::setTextEdition(bool val)
{

    if (val and this->textInteractionFlags() == Qt::NoTextInteraction) {
        saved_value = document()->toPlainText();
        this->setFocus(
          Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        this->setTextInteractionFlags(Qt::TextEditorInteraction);
    } else {
        this->setTextInteractionFlags(Qt::NoTextInteraction);
        this->clearFocus();
        if (document()->toPlainText() != saved_value) {
            emit textUpdated(id, saved_value, document()->toPlainText());
        }
    }
}

QString
VleTextEdit::getSavedText()
{
    return saved_value;
}
QString
VleTextEdit::getCurrentText()
{
    return document()->toPlainText();
}

void
VleTextEdit::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}

void
VleTextEdit::setText(const QString& text)
{
    this->document()->setPlainText(text);
    saved_value = text;
}

void
VleTextEdit::focusOutEvent(QFocusEvent* e)
{
    setPalette(QApplication::palette());
    setTextEdition(false);
    update();
    QPlainTextEdit::focusOutEvent(e);
}

void
VleTextEdit::focusInEvent(QFocusEvent* e)
{
    if (e->reason() == Qt::MouseFocusReason) {
        QPalette p = this->palette();
        p.setBrush(QPalette::Base,
                   QBrush(QColor(255, 127, 80, 200), Qt::SolidPattern));
        setPalette(p);
        QPlainTextEdit::focusInEvent(e);
        emit selected(id);
    }
}

void
VleTextEdit::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (edit_on_dble_click) {
        setTextEdition(true);
    } else {
        QPlainTextEdit::mouseDoubleClickEvent(e);
    }
}

/************************** VleValueWIdget ******************************/
QSize
VleStackedWidget::sizeHint() const
{
    return currentWidget()->sizeHint();
}

QSize
VleStackedWidget::minimumSizeHint() const
{
    return currentWidget()->minimumSizeHint();
}

/************************** VleValueWIdget ******************************/

QString
VleValueWidget::getValueDisplay(const vle::value::Value& v,
                                ValueDisplayType displayType)
{

    QString toStd;
    QString type;

    switch (v.getType()) {
    case vle::value::Value::BOOLEAN: {
        type = "boolean";
        toStd = QString("%1").arg(v.toBoolean().value());
        break;
    }
    case vle::value::Value::INTEGER: {
        type = "integer";
        toStd = QString("%1").arg(v.toInteger().value());
        break;
    }
    case vle::value::Value::DOUBLE: {
        type = "double";
        toStd = QString("%1").arg(v.toDouble().value());
        break;
    }
    case vle::value::Value::STRING: {
        type = "string";
        toStd = v.toString().value().c_str();
        break;
    }
    case vle::value::Value::SET: {
        type = QString("set %1").arg(v.toSet().size());
        toStd = "";
        break;
    }
    case vle::value::Value::MAP: {
        type = QString("map %1").arg(v.toMap().size());
        toStd = "";
        break;
    }
    case vle::value::Value::TUPLE: {
        type = QString("tuple %1").arg(v.toTuple().size());
        toStd = "";
        break;
    }
    case vle::value::Value::TABLE: {
        type = QString("table %1x%2")
                 .arg(v.toTable().height())
                 .arg(v.toTable().width());
        toStd = "";
        break;
    }
    case vle::value::Value::XMLTYPE: {
        type = "xml";
        toStd = "";
        if (displayType == TypeOnly) {
            return "xml";
        }
        break;
    }
    case vle::value::Value::NIL: {
        type = "null";
        toStd = "nil";
        break;
    }
    case vle::value::Value::MATRIX: {
        type = QString("matrix %1x%2")
                 .arg(v.toMatrix().rows())
                 .arg(v.toMatrix().columns());
        toStd = "";
        break;
    }
    case vle::value::Value::USER: {
        type = "user";
        toStd = "nil";
        break;
    }
    }

    switch (displayType) {
    case TypeOnly:
        return type;
        break;
    case Insight:
        return QString("%1 (%2)").arg(toStd).arg(type);
        break;
    case ValueOnly:
        return toStd;
        break;
    }
    return "error";
}

VleValueWidget::value_stack::value_stack()
  : startValue(nullptr)
  , stack()
{
}
VleValueWidget::value_stack::~value_stack()
{
    setStartValue(std::unique_ptr<value::Value>(nullptr));
}

void
VleValueWidget::value_stack::setStartValue(std::unique_ptr<value::Value> val)
{
    startValue = std::move(val);
    stack.clear();
}

void
VleValueWidget::value_stack::push(const std::string& key)
{
    stack.push_back(value::String::create(key));
}
void
VleValueWidget::value_stack::push(int index)
{
    stack.push_back(value::Integer::create(index));
}

void
VleValueWidget::value_stack::push(int row, int col)
{
    std::unique_ptr<vle::value::Tuple> val(new vle::value::Tuple(0));
    val->add((double)row);
    val->add((double)col);
    stack.push_back(std::move(val));
}

value::Value&
VleValueWidget::value_stack::editingValue()
{
    value::Value* res = startValue.get();
    cont::const_iterator itb = stack.begin();
    cont::const_iterator ite = stack.end();
    for (; itb != ite; itb++) {
        switch ((*itb)->getType()) {
        case vle::value::Value::INTEGER: { // for id a Set elem
            res = res->toSet().value().at((*itb)->toInteger().value()).get();
            break;
        }
        case vle::value::Value::STRING: { // for id a Map elem
            res = res->toMap().value().at((*itb)->toString().value()).get();
            break;
        }
        case vle::value::Value::TUPLE: { // for id a Matrix elem
            const vle::value::Tuple& tuple = (*itb)->toTuple();
            res =
              res->toMatrix().get((int)tuple.at(1), (int)tuple.at(0)).get();
            // WARNING inversed above
            break;
        }
        default: {
            qDebug() << " value_stack::editingValue not yet implemented";
            break;
        }
        }
    }
    return *res;
}

std::vector<std::string>
VleValueWidget::value_stack::toString()
{
    std::vector<std::string> res;

    cont::const_iterator itb = stack.begin();
    cont::const_iterator ite = stack.end();
    for (; itb != ite; itb++) {
        switch ((*itb)->getType()) {
        case vle::value::Value::INTEGER: {
            res.push_back(
              QVariant((*itb)->toInteger().value()).toString().toStdString());
            break;
        }
        case vle::value::Value::STRING: {
            res.push_back((*itb)->toString().value());
            break;
        }
        case vle::value::Value::TUPLE: {
            const vle::value::Tuple& tuple = (*itb)->toTuple();
            std::string toadd = "(";
            toadd += QVariant((int)tuple.at(1)).toString().toStdString();
            toadd += ",";
            toadd += QVariant((int)tuple.at(0)).toString().toStdString();
            toadd += ")";
            res.push_back(toadd);
            // WARNING inversed above
            break;
        }
        default: {
            qDebug() << " value_stack::toString not yet implemented";
            break;
        }
        }
    }
    return res;
}

VleValueWidget::VleValueWidget(QWidget* parent,
                               bool limited,
                               const QString& header)
  : QWidget(parent)
  , mValueStack()
  , stack(0)
  , value_bool(0)
  , value_int(0)
  , value_double(0)
  , value_string(0)
  , value_complex(0)
  , stack_buttons(0)
  , table(0)
  , resize_place(0)
  , resize_row(0)
  , resize_col(0)
  , resize(0)
  , mId("")
  , mLimited(limited)
{
    setObjectName("VleValueWidget");
    setLayout(new QVBoxLayout());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    stack = new VleStackedWidget();
    stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (not header.isEmpty()) {
        layout()->addWidget(new QLabel(header));
    }

    value_bool = new VleCombo(0, "");
    QObject::connect(value_bool,
                     SIGNAL(valUpdated(const QString&, const QString&)),
                     this,
                     SLOT(onValUpdated(const QString&, const QString&)));

    stack->layout()->addWidget(value_bool);

    value_int = new VleSpinBox(0, 0, "");
    stack->layout()->addWidget(value_int);

    value_double = new VleDoubleEdit(0, 0.0, "");
    value_double->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QObject::connect(value_double,
                     SIGNAL(valUpdated(const QString&, double)),
                     this,
                     SLOT(onValUpdated(const QString&, double)));

    stack->layout()->addWidget(value_double);

    value_string = new VleTextEdit(0, "");
    QObject::connect(
      value_string,
      SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
      this,
      SLOT(onTextUpdated(const QString&, const QString&, const QString&)));

    stack->layout()->addWidget(value_string);

    value_complex = new QWidget();
    value_complex->setLayout(new QVBoxLayout());
    value_complex->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Expanding);

    // stack buttons place
    stack_buttons = new QWidget();
    stack_buttons->setLayout(new QHBoxLayout());
    QPushButton* root = new QPushButton("/");
    stack_buttons->layout()->addWidget(root);
    stack_buttons->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    value_complex->layout()->addWidget(stack_buttons);
    // views
    table = new QTableWidget();
    if (not mLimited) {
        table->setContextMenuPolicy(Qt::CustomContextMenu);
    }
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    value_complex->layout()->addWidget(table);
    // resize place
    resize_place = new QWidget();
    resize_place->setLayout(new QHBoxLayout());

    resize_place->layout()->addWidget(new QLabel("rows", resize_place));
    resize_row = new VleSpinBox(resize_place, 0, "resizeRow");
    resize_col = new VleSpinBox(resize_place, 0, "resizeCol");
    ;
    resize_place->layout()->addWidget(resize_row);
    resize_place->layout()->addWidget(new QLabel("columns", resize_place));
    resize_place->layout()->addWidget(resize_col);
    resize = new VlePushButton(resize_place, "resize");
    resize_place->layout()->addWidget(resize);
    value_complex->layout()->addWidget(resize_place);

    QObject::connect(table,
                     SIGNAL(cellDoubleClicked(int, int)),
                     this,
                     SLOT(setDoubleClicked(int, int)));

    QObject::connect(table,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,
                     SLOT(onMenuSetView(const QPoint&)));
    QObject::connect(
      resize, SIGNAL(clicked(bool)), this, SLOT(onResize(bool)));

    if (mLimited) {
        stack_buttons->setEnabled(false);

        resize_row->setEnabled(false);
        resize_col->setEnabled(false);
        resize->setEnabled(false);
    }

    stack->layout()->addWidget(value_complex);
    stack->setCurrentIndex(0);
    //    layout()->addWidget(e);
    //    layout()->addWidget(e2);
    layout()->addWidget(stack);
}

VleValueWidget::~VleValueWidget()
{
}

void
VleValueWidget::showCurrentValueDetail()
{
    { // update buttons to stack
        QLayoutItem* child;
        while ((child = stack_buttons->layout()->takeAt(0)) != 0) {
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }
        int stackVal = 0;
        VlePushButton* stackBut =
          new VlePushButton(this, "/", QString(stackVal));
        QObject::connect(stackBut,
                         SIGNAL(clicked(const QString&, bool)),
                         this,
                         SLOT(onStackButtonClicked(const QString&, bool)));
        stack_buttons->layout()->addWidget(stackBut);
        std::vector<std::string> stack_str = mValueStack.toString();
        std::vector<std::string>::const_iterator itb = stack_str.begin();
        std::vector<std::string>::const_iterator ite = stack_str.end();
        for (; itb != ite; itb++) {
            stackVal++;
            VlePushButton* stackBut =
              new VlePushButton(this, itb->c_str(), QString::number(stackVal));
            QObject::connect(stackBut,
                             SIGNAL(clicked(const QString&, bool)),
                             this,
                             SLOT(onStackButtonClicked(const QString&, bool)));
            stack_buttons->layout()->addWidget(stackBut);
        }
    }

    const value::Value& editingValue = mValueStack.editingValue();
    value::Value::type editType = editingValue.getType();
    switch (editType) {
    case value::Value::BOOLEAN: {
        setBoolWidget(QString("/"), editingValue, -1, -1); // TODO
        stack->setCurrentIndex(0);
        break;
    }
    case value::Value::INTEGER: {
        setIntWidget(QString("/"), editingValue, -1, -1); // TODO
        break;
    }
    case value::Value::DOUBLE: {
        setDoubleWidget(QString("/"), editingValue, -1, -1); // TODO
        break;
    }
    case value::Value::STRING: {
        setStringWidget(QString("/"), editingValue, -1, -1); // TODO
        break;
    }
    case value::Value::SET: {
        table->clear();
        table->setRowCount(editingValue.toSet().size());
        table->setColumnCount(1);
        table->setHorizontalHeaderItem(0, new QTableWidgetItem("set values"));
        value::Set::const_iterator itb = editingValue.toSet().begin();
        value::Set::const_iterator ite = editingValue.toSet().end();
        int nbEl = 0;
        for (; itb != ite; itb++) {
            QString id = QString("%1").arg(nbEl);
            switch ((*itb)->getType()) {
            case value::Value::BOOLEAN: {
                setBoolWidget(id, *(*itb), nbEl, 0);
                break;
            }
            case value::Value::INTEGER: {
                setIntWidget(id, *(*itb), nbEl, 0);
                break;
            }
            case value::Value::DOUBLE: {
                setDoubleWidget(id, *(*itb), nbEl, 0);
                break;
            }
            case value::Value::STRING: {
                setStringWidget(id, *(*itb), nbEl, 0);
                break;
            }
            default: {
                setComplexWidget(id, *(*itb), nbEl, 0);
                break;
            }
            }
            nbEl++;
        }
        break;
    }
    case value::Value::MAP: {
        const value::Map& map = editingValue.toMap();
        table->clear();
        table->setRowCount(map.size());
        table->setColumnCount(2);
        table->setHorizontalHeaderItem(0, new QTableWidgetItem("map keys"));
        table->setHorizontalHeaderItem(1, new QTableWidgetItem("map values"));

        std::vector<std::string> keys;
        for (auto& i : map) {
            keys.push_back(i.first);
        }
        std::sort(keys.begin(), keys.end());
        int nbEl = 0;
        for (auto& k : keys) {
            QString keyValue = k.c_str();
            QString id = QString("key:%1").arg(keyValue);
            setStringWidget(id, keyValue, nbEl, 0);
            switch (map[k]->getType()) {
            case value::Value::BOOLEAN: {
                setBoolWidget(keyValue, *map[k], nbEl, 1);
                break;
            }
            case value::Value::INTEGER: {
                setIntWidget(keyValue, *map[k], nbEl, 1);
                break;
            }
            case value::Value::DOUBLE: {
                setDoubleWidget(keyValue, *map[k], nbEl, 1);
                break;
            }
            case value::Value::STRING: {
                setStringWidget(keyValue, *map[k], nbEl, 1);
                break;
            }
            default: {
                setComplexWidget(keyValue, *map[k], nbEl, 1);
                break;
            }
            }
            nbEl++;
        }
        break;
    }
    case value::Value::TUPLE: {
        table->clear();
        table->setRowCount(editingValue.toTuple().size());
        table->setColumnCount(1);
        table->setHorizontalHeaderItem(0,
                                       new QTableWidgetItem("tuple values"));
        const value::Tuple& tuple = editingValue.toTuple();
        for (unsigned int nbEl = 0; nbEl < tuple.size(); nbEl++) {
            setDoubleWidget(QString("%2").arg(nbEl), tuple.at(nbEl), nbEl, 0);
        }
        break;
    }
    case value::Value::TABLE: {
        const value::Table& tableVal = editingValue.toTable();
        table->clear();
        table->setRowCount(tableVal.height());
        table->setColumnCount(tableVal.width());
        for (unsigned int r = 0; r < tableVal.height(); r++) {
            for (unsigned int c = 0; c < tableVal.width(); c++) {
                setDoubleWidget(QString("(%1,%2)").arg(r).arg(c),
                                tableVal.get(c, r),
                                r,
                                c); // inverted
            }
        }
        break;
    }
    case value::Value::MATRIX: {
        const value::Matrix& matrixVal = editingValue.toMatrix();
        table->clear();
        table->setRowCount(matrixVal.rows());
        table->setColumnCount(matrixVal.columns());
        for (unsigned int i = 0; i < matrixVal.rows(); i++) {
            for (unsigned int j = 0; j < matrixVal.columns(); j++) {
                QString id = QString("(%1,%2)").arg(i).arg(j);
                const auto& valij = matrixVal.get(j, i);
                switch (valij->getType()) {
                case value::Value::BOOLEAN: {
                    setBoolWidget(id, *valij, i, j);
                    break;
                }
                case value::Value::INTEGER: {
                    setIntWidget(id, *valij, i, j);
                    break;
                }
                case value::Value::DOUBLE: {
                    setDoubleWidget(id, *valij, i, j);
                    break;
                }
                case value::Value::STRING: {
                    setStringWidget(id, *valij, i, j);
                    break;
                }
                default: {
                    setComplexWidget(id, *valij, i, j);
                    break;
                }
                }
            }
        }
        break;
    }
    default: {
        break;
    }
    }

    int nbRows = (editType == value::Value::TABLE)
                   ? editingValue.toTable().height()
                   : (editType == value::Value::MATRIX)
                       ? editingValue.toMatrix().rows()
                       : (editType == value::Value::SET)
                           ? editingValue.toSet().size()
                           : (editType == value::Value::MAP)
                               ? editingValue.toMap().size()
                               : (editType == value::Value::TUPLE)
                                   ? editingValue.toTuple().size()
                                   : 0;
    int nbCols = (editType == value::Value::TABLE)
                   ? editingValue.toTable().width()
                   : (editType == value::Value::MATRIX)
                       ? editingValue.toMatrix().columns()
                       : 0;
    resize_row->setValue(nbRows);
    resize_col->setValue(nbCols);
    resize_row->setEnabled((editType == value::Value::TABLE) or
                           (editType == value::Value::MATRIX) or
                           (editType == value::Value::TUPLE) or
                           (editType == value::Value::SET));
    resize_col->setEnabled((editType == value::Value::TABLE) or
                           (editType == value::Value::MATRIX));
    resize->setEnabled(resize_row->isEnabled());
    if (editType == value::Value::TABLE) {
        resize_col->setMin(1);
        resize_row->setMin(1);
    } else {
        resize_col->setMin(0);
        resize_row->setMin(0);
    }
    if (mLimited) {
        stack_buttons->setEnabled(false);

        resize_row->setEnabled(false);
        resize_col->setEnabled(false);
        resize->setEnabled(false);
    }

    int widgetToSet = -1;
    switch (editType) {
    case value::Value::BOOLEAN: {
        widgetToSet = 0;
        break;
    }
    case value::Value::INTEGER: {
        widgetToSet = 1;
        break;
    }
    case value::Value::DOUBLE: {
        widgetToSet = 2;
        break;
    }
    case value::Value::STRING: {
        widgetToSet = 3;
        break;
    }
    default: {
        widgetToSet = 4;
        break;
    }
    }
    if (stack->currentIndex() != widgetToSet) {
        stack->setCurrentIndex(widgetToSet);
    }
}

void
VleValueWidget::setId(const QString& id)
{
    mId = id;
}

void
VleValueWidget::setValue(std::unique_ptr<vle::value::Value> val)
{
    mValueStack.setStartValue(std::move(val));
    showCurrentValueDetail();
}

std::unique_ptr<value::Value>
VleValueWidget::buildDefaultValue(eValueWidgetMenu type)
{
    switch (type) {
    case EMenuValueAddBoolean:
    case EMenuValueSetBoolean: {
        return value::Boolean::create();
        break;
    }
    case EMenuValueAddInteger:
    case EMenuValueSetInteger: {
        return value::Integer::create();
        break;
    }
    case EMenuValueAddDouble:
    case EMenuValueSetDouble: {
        return value::Double::create();
        break;
    }
    case EMenuValueAddString:
    case EMenuValueSetString: {
        return value::String::create();
        break;
    }
    case EMenuValueAddSet:
    case EMenuValueSetSet: {
        return value::Set::create();
        break;
    }
    case EMenuValueAddMap:
    case EMenuValueSetMap: {
        return value::Map::create();
        break;
    }
    case EMenuValueAddTuple:
    case EMenuValueSetTuple: {
        return value::Tuple::create();
        break;
    }
    case EMenuValueAddTable:
    case EMenuValueSetTable: {
        return value::Table::create();
        break;
    }
    case EMenuValueAddMatrix:
    case EMenuValueSetMatrix: {
        return std::unique_ptr<value::Value>(new value::Matrix());
        break;
    }
    default: {
        return std::unique_ptr<value::Value>();
    }
    }
}

void
VleValueWidget::setDoubleClicked(int row, int col)
{
    value::Value& edit = mValueStack.editingValue();
    switch (edit.getType()) {
    case vle::value::Value::BOOLEAN:
    case vle::value::Value::INTEGER:
    case vle::value::Value::DOUBLE:
    case vle::value::Value::STRING:
    case vle::value::Value::TUPLE:
    case vle::value::Value::TABLE: {
        break;
    }
    case vle::value::Value::SET: {
        const auto& nextEdit = edit.toSet().get(row);
        switch (nextEdit->getType()) {
        case vle::value::Value::BOOLEAN:
        case vle::value::Value::INTEGER:
        case vle::value::Value::DOUBLE:
        case vle::value::Value::STRING: {
            break;
        }
        case vle::value::Value::SET:
        case vle::value::Value::TUPLE:
        case vle::value::Value::MAP:
        case vle::value::Value::TABLE:
        case vle::value::Value::MATRIX: {
            mValueStack.push(row);
            showCurrentValueDetail();
            break;
        }
        default: {
            break;
        }
        }
        break;
    }
    case vle::value::Value::MAP: {
        if (col == 1) {
            std::string key =
              static_cast<VleTextEdit*>(table->cellWidget(row, 0))
                ->getCurrentText()
                .toStdString();
            const auto& nextEdit = edit.toMap().get(key);
            switch (nextEdit->getType()) {
            case vle::value::Value::BOOLEAN:
            case vle::value::Value::INTEGER:
            case vle::value::Value::DOUBLE:
            case vle::value::Value::STRING: {
                break;
            }
            case vle::value::Value::SET:
            case vle::value::Value::TUPLE:
            case vle::value::Value::MAP:
            case vle::value::Value::TABLE:
            case vle::value::Value::MATRIX: {
                mValueStack.push(key);
                showCurrentValueDetail();
                break;
            }
            default: {
                break;
            }
            }
        }
        break;
    }
    case vle::value::Value::MATRIX: {
        if ((row >= 0 and col >= 0)) {
            const auto& nextEdit = edit.toMatrix().get(col, row);
            switch (nextEdit->getType()) {
            case vle::value::Value::BOOLEAN:
            case vle::value::Value::INTEGER:
            case vle::value::Value::DOUBLE:
            case vle::value::Value::STRING: {
                break;
            }
            case vle::value::Value::SET:
            case vle::value::Value::TUPLE:
            case vle::value::Value::MAP:
            case vle::value::Value::TABLE:
            case vle::value::Value::MATRIX: {
                mValueStack.push(row, col);
                showCurrentValueDetail();
                break;
            }
            default: {
                break;
            }
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void
VleValueWidget::onMenuSetView(const QPoint& pos)
{
    QPoint globalPos = table->mapToGlobal(pos);
    QModelIndex index = table->indexAt(pos);
    vle::value::Value& editVal = mValueStack.editingValue();
    vle::value::Value::type editType = editVal.getType();

    QAction* action;
    QMenu menu;
    QMenu* subMenu = menu.addMenu("Insert value");
    action = subMenu->addAction("boolean");
    action->setData(EMenuValueAddBoolean);
    action = subMenu->addAction("integer");
    action->setData(EMenuValueAddInteger);
    action = subMenu->addAction("double");
    action->setData(EMenuValueAddDouble);
    action = subMenu->addAction("string");
    action->setData(EMenuValueAddString);
    action = subMenu->addAction("set");
    action->setData(EMenuValueAddSet);
    action = subMenu->addAction("map");
    action->setData(EMenuValueAddMap);
    action = subMenu->addAction("tuple");
    action->setData(EMenuValueAddTuple);
    action = subMenu->addAction("table");
    action->setData(EMenuValueAddTable);
    action = subMenu->addAction("matrix");
    action->setData(EMenuValueAddMatrix);
    subMenu->setEnabled((editType == vle::value::Value::SET) or
                        (editType == vle::value::Value::MAP));
    subMenu = menu.addMenu("Replace by");
    action = subMenu->addAction("boolean");
    action->setData(EMenuValueSetBoolean);
    action = subMenu->addAction("integer");
    action->setData(EMenuValueSetInteger);
    action = subMenu->addAction("double");
    action->setData(EMenuValueSetDouble);
    action = subMenu->addAction("string");
    action->setData(EMenuValueSetString);
    action = subMenu->addAction("set");
    action->setData(EMenuValueSetSet);
    action = subMenu->addAction("map");
    action->setData(EMenuValueSetMap);
    action = subMenu->addAction("tuple");
    action->setData(EMenuValueSetTuple);
    action = subMenu->addAction("table");
    action->setData(EMenuValueSetTable);
    action = subMenu->addAction("matrix");
    action->setData(EMenuValueSetMatrix);
    subMenu->setEnabled((index.row() >= 0) and
                        ((editType == vle::value::Value::SET) or
                         (editType == vle::value::Value::MAP) or
                         (editType == vle::value::Value::MATRIX)));
    action = menu.addAction("Remove");
    action->setData(EMenuValueRemove);
    action->setEnabled((index.row() >= 0) and
                       ((editType == vle::value::Value::SET) or
                        (editType == vle::value::Value::MAP) or
                        (editType == vle::value::Value::TUPLE)));

    QAction* act = menu.exec(globalPos);
    if (act) {
        eValueWidgetMenu actCode = (eValueWidgetMenu)act->data().toInt();
        switch (actCode) {
        case EMenuValueRemove: {
            switch (editType) {
            case vle::value::Value::SET: {
                editVal.toSet().remove(index.row());
                break;
            }
            case vle::value::Value::TUPLE: {
                editVal.toTuple().remove(index.row());
                break;
            }
            case vle::value::Value::MAP: {
                editVal.toMap().value().erase(
                  static_cast<VleTextEdit*>(table->cellWidget(index.row(), 0))
                    ->getCurrentText()
                    .toStdString());
                break;
            }
            default: {
                break;
            }
            }
            showCurrentValueDetail();
            emit valUpdated(*mValueStack.startValue);
            break;
        }
        case EMenuValueAddBoolean:
        case EMenuValueAddInteger:
        case EMenuValueAddDouble:
        case EMenuValueAddString:
        case EMenuValueAddSet:
        case EMenuValueAddMap:
        case EMenuValueAddTuple:
        case EMenuValueAddTable:
        case EMenuValueAddMatrix: {
            auto v =
              std::unique_ptr<vle::value::Value>(buildDefaultValue(actCode));
            switch (editType) {
            case vle::value::Value::SET: {
                editVal.toSet().add(std::move(v));
                break;
            }
            case vle::value::Value::MAP: {
                editVal.toMap().addMultilpleValues(1, *v);
                break;
            }
            default: {
                break;
            }
            }
            showCurrentValueDetail();
            emit valUpdated(*mValueStack.startValue);
            break;
        }
        case EMenuValueSetBoolean:
        case EMenuValueSetInteger:
        case EMenuValueSetDouble:
        case EMenuValueSetString:
        case EMenuValueSetSet:
        case EMenuValueSetMap:
        case EMenuValueSetTuple:
        case EMenuValueSetTable:
        case EMenuValueSetMatrix: {
            auto v =
              std::unique_ptr<vle::value::Value>(buildDefaultValue(actCode));
            switch (editType) {
            case vle::value::Value::SET: {
                editVal.toSet().set(index.row(), std::move(v));
                break;
            }
            case vle::value::Value::MAP: {
                QString key =
                  static_cast<VleTextEdit*>(table->cellWidget(index.row(), 0))
                    ->id;
                key.replace("key:", "");
                editVal.toMap().set(key.toStdString(), std::move(v));
                break;
            }
            case vle::value::Value::MATRIX: {
                editVal.toMatrix().set(
                  index.column(), index.row(), std::move(v));
                break;
            }
            default: {
                break;
            }
            }
            showCurrentValueDetail();
            emit valUpdated(*mValueStack.startValue);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void
VleValueWidget::onValUpdated(const QString& id, int newVal)
{
    value::Value& editVal = mValueStack.editingValue();
    switch (editVal.getType()) {
    case vle::value::Value::INTEGER: {
        if (id != "") {
            qDebug() << " internal error VleValueWidget::onValUpdated 1 ";
        }
        editVal.toInteger().set(newVal);
        break;
    }
    case vle::value::Value::SET: {
        int ix = id.toInt();
        editVal.toSet().get(ix)->toInteger().set(newVal);
        break;
    }
    case vle::value::Value::MAP: {
        editVal.toMap().get(id.toStdString())->toInteger().set(newVal);
        break;
    }
    case vle::value::Value::MATRIX: {
        int rx = id.split(",").at(0).split("(").at(1).toInt();
        int cx = id.split(",").at(1).split(")").at(0).toInt();
        editVal.toMatrix().get(cx, rx)->toInteger().set(newVal);
        break;
    }
    default: {
        break;
    }
    }
    emit valUpdated(*mValueStack.startValue);
}

void
VleValueWidget::onValUpdated(const QString& id, double newVal)
{
    value::Value& editVal = mValueStack.editingValue();
    switch (editVal.getType()) {
    case vle::value::Value::DOUBLE: {
        if (id != "") {
            qDebug() << " internal error VleValueWidget::onValUpdated 1 ";
        }
        editVal.toDouble().set(newVal);
        break;
    }
    case vle::value::Value::SET: {
        int ix = id.toInt();
        editVal.toSet().get(ix)->toDouble().set(newVal);
        break;
    }
    case vle::value::Value::MAP: {
        editVal.toMap().get(id.toStdString())->toDouble().set(newVal);
        break;
    }
    case vle::value::Value::TUPLE: {
        int ix = id.toInt();
        editVal.toTuple()[ix] = newVal;
        break;
    }
    case vle::value::Value::TABLE: {
        int rx = id.split(",").at(0).split("(").at(1).toInt();
        int cx = id.split(",").at(1).split(")").at(0).toInt();
        editVal.toTable().get(cx, rx) = newVal;
        break;
    }
    case vle::value::Value::MATRIX: {
        int rx = id.split(",").at(0).split("(").at(1).toInt();
        int cx = id.split(",").at(1).split(")").at(0).toInt();
        editVal.toMatrix().get(cx, rx)->toDouble().set(newVal);
        break;
    }
    default: {
        break;
    }
    }
    emit valUpdated(*mValueStack.startValue);
}

void
VleValueWidget::onValUpdated(const QString& id, const QString& boolVal)
{
    bool val = (boolVal == "true");
    value::Value& editVal = mValueStack.editingValue();
    switch (editVal.getType()) {
    case vle::value::Value::BOOLEAN: {
        if (id != "") {
            qDebug() << " internal error VleValueWidget::onValUpdated 1 ";
        }
        editVal.toBoolean().set(val);
        break;
    }
    case vle::value::Value::SET: {
        int ix = id.toInt();
        editVal.toSet().get(ix)->toBoolean().set(val);
        break;
    }
    case vle::value::Value::MAP: {
        editVal.toMap().get(id.toStdString())->toBoolean().set(val);
        break;
    }
    case vle::value::Value::MATRIX: {
        int rx = id.split(",").at(0).split("(").at(1).toInt();
        int cx = id.split(",").at(1).split(")").at(0).toInt();
        editVal.toMatrix().get(cx, rx)->toBoolean().set(val);
        break;
    }
    default: {
        break;
    }
    }
    emit valUpdated(*mValueStack.startValue);
}

void
VleValueWidget::onTextUpdated(const QString& id,
                              const QString& /*old*/,
                              const QString& newVal)
{
    value::Value& editVal = mValueStack.editingValue();
    switch (editVal.getType()) {
    case vle::value::Value::STRING: {
        if (id != "") {
            qDebug() << " internal error VleValueWidget::onValUpdated 1 ";
        }
        editVal.toString().set(newVal.toStdString());
        break;
    }
    case vle::value::Value::SET: {
        int ix = id.toInt();
        editVal.toSet().get(ix)->toString().set(newVal.toStdString());
        break;
    }
    case vle::value::Value::MAP: {
        if (id.startsWith("key:")) {
            std::string oldKey = id.split("key:").at(1).toStdString();
            auto val = editVal.toMap().give(oldKey);
            editVal.toMap().value().erase(oldKey);
            editVal.toMap().set(newVal.toStdString(), std::move(val));
            showCurrentValueDetail();
        } else {
            editVal.toMap()
              .get(id.toStdString())
              ->toString()
              .set(newVal.toStdString());
        }
        break;
    }
    case vle::value::Value::MATRIX: {
        int rx = id.split(",").at(0).split("(").at(1).toInt();
        int cx = id.split(",").at(1).split(")").at(0).toInt();
        editVal.toMatrix().get(cx, rx)->toString().set(newVal.toStdString());
        break;
    }
    default: {
        break;
    }
    }
    emit valUpdated(*mValueStack.startValue);
}

void
VleValueWidget::onResize(bool /*b*/)
{
    value::Value& edit = mValueStack.editingValue();
    switch (edit.getType()) {
    case vle::value::Value::BOOLEAN:
    case vle::value::Value::INTEGER:
    case vle::value::Value::DOUBLE:
    case vle::value::Value::STRING: {
        break;
    }
    case vle::value::Value::SET: {
        edit.toSet().resize(resize_row->value(), vle::value::Boolean(true));
        break;
    }
    case vle::value::Value::TUPLE: {
        edit.toTuple().value().resize(resize_row->value());
        break;
    }
    case vle::value::Value::TABLE: {
        edit.toTable().resize(resize_col->value(), resize_row->value());
        break;
    }
    case vle::value::Value::MATRIX: {
        edit.toMatrix().resize(resize_col->value(),
                               resize_row->value(),
                               vle::value::Boolean::create(true));
        break;
    }
    default: {
        break;
    }
    }
    showCurrentValueDetail();
    emit valUpdated(*mValueStack.startValue);
}

void
VleValueWidget::onStackButtonClicked(const QString& id, bool /*b*/)
{
    int stackIx = id.toInt();
    while (mValueStack.stack.size() > (unsigned int)stackIx) {
        mValueStack.stack.pop_back();
    }
    showCurrentValueDetail();
}

void
VleValueWidget::setBoolWidget(const QString& id,
                              const vle::value::Value& val,
                              int r,
                              int c)
{
    if (r < 0 and c < 0) {
        bool oldBlock = value_bool->blockSignals(true);
        value_bool->setCurrentIndex(
          value_bool->findText(QString(val.toString().value().c_str())));
        if (stack->currentIndex() != 0) {
            stack->setCurrentIndex(0);
        }
        value_int->blockSignals(oldBlock);

    } else {
        VleCombo* w =
          VleCombo::buildVleBoolCombo(table, id, val.toBoolean().value());
        table->setCellWidget(r, c, w);
        table->setItem(r, c, new QTableWidgetItem);
        QObject::connect(w,
                         SIGNAL(valUpdated(const QString&, const QString&)),
                         this,
                         SLOT(onValUpdated(const QString&, const QString&)));
    }
}

void
VleValueWidget::setIntWidget(const QString& id,
                             const vle::value::Value& val,
                             int r,
                             int c)
{
    if (r < 0 and c < 0) {
        bool oldBlock = value_int->blockSignals(true);
        value_int->setValue(val.toInteger().value());
        if (stack->currentIndex() != 1) {
            stack->setCurrentIndex(1);
        }
        value_int->blockSignals(oldBlock);
    } else {
        VleSpinBox* w = new VleSpinBox(table, val.toInteger().value(), id);
        table->setCellWidget(r, c, w);
        table->setItem(r, c, new QTableWidgetItem); // used to find it
        QObject::connect(w,
                         SIGNAL(valUpdated(const QString&, int)),
                         this,
                         SLOT(onValUpdated(const QString&, int)));
    }
}

void
VleValueWidget::setDoubleWidget(const QString& id,
                                const vle::value::Value& val,
                                int r,
                                int c)
{
    setDoubleWidget(id, val.toDouble().value(), r, c);
}

void
VleValueWidget::setDoubleWidget(const QString& id, double val, int r, int c)
{
    if (r < 0 and c < 0) {
        bool oldBlock = value_double->blockSignals(true);
        value_double->setValue(val);
        if (stack->currentIndex() != 2) {
            stack->setCurrentIndex(2);
        }
        value_double->blockSignals(oldBlock);
    } else {
        VleDoubleEdit* w = new VleDoubleEdit(table, val, id, false);
        table->setCellWidget(r, c, w);
        table->setItem(r, c, new QTableWidgetItem);
        QObject::connect(w,
                         SIGNAL(valUpdated(const QString&, double)),
                         this,
                         SLOT(onValUpdated(const QString&, double)));
    }
}

void
VleValueWidget::setStringWidget(const QString& id,
                                const vle::value::Value& val,
                                int r,
                                int c)
{
    setStringWidget(id, QString(val.toString().value().c_str()), r, c);
}

void
VleValueWidget::setStringWidget(const QString& id,
                                const QString& val,
                                int r,
                                int c)
{
    if (r < 0 and c < 0) {
        bool oldBlock = value_string->blockSignals(true);
        value_string->setText(val);
        if (stack->currentIndex() != 3) {
            stack->setCurrentIndex(3);
        }
        value_string->blockSignals(oldBlock);
    } else {
        VleTextEdit* w = new VleTextEdit(table, val, id, true);
        table->setCellWidget(r, c, w);
        QTableWidgetItem* newItem = new QTableWidgetItem;
        table->setItem(r, c, newItem);
        newItem->setSizeHint(w->sizeHint());
        QObject::connect(
          w,
          SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
          this,
          SLOT(onTextUpdated(const QString&, const QString&, const QString&)));
    }
}

void
VleValueWidget::setComplexWidget(const QString& id,
                                 const vle::value::Value& val,
                                 int r,
                                 int c)
{

    VleTextEdit* w =
      new VleTextEdit(table, getValueDisplay(val, Insight), id, false);
    table->setCellWidget(r, c, w);
    table->setItem(r, c, new QTableWidgetItem);
}
}
} // namespaces
