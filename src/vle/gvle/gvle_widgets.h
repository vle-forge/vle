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

#ifndef gvle_WIDGETS_H
#define gvle_WIDGETS_H

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <vle/value/Value.hpp>
//#include <QtWidgets/QStackedWidget>
#include <QStackedWidget>

#include <iostream>

namespace vle {
namespace gvle {



/**
 * A Null widget allows to get the selected signal
 */
class VleNullWidget : public QWidget
{
    Q_OBJECT
public:
    VleNullWidget(QWidget* parent, const QString& idStr="");
    ~VleNullWidget();
    void focusInEvent(QFocusEvent* e);

    QString id;
signals:
    void selected(const QString& id);
};


/**
 * A Bool Editor
 */
class VleBooleanEdit : public QCheckBox
{
    Q_OBJECT
public:
    VleBooleanEdit(QWidget* parent, bool val, const QString& idStr="");
    ~VleBooleanEdit();
    void focusInEvent(QFocusEvent* e);
    void setValue(bool val);

    QString id;
public slots:
    void onValueChanged(bool checked);
signals:
    void valUpdated(const QString& id, bool v);
    void selected(const QString& id);
};

/**
 * A double Editor
 */
class VleDoubleEdit : public QLineEdit
{
    Q_OBJECT
public:

    VleDoubleEdit(QWidget* parent, double val, const QString& idStr="",
            bool withDefaultMenu = true);
    ~VleDoubleEdit();
    void focusInEvent(QFocusEvent* e);
    bool eventFilter(QObject *target, QEvent *event);
    void setValue(double val);

    QString id;
    double backup;
public slots:
    void onValueChanged();
signals:
    void valUpdated(const QString& id, double v);
    void selected(const QString& id);
};

/**
 * A push button with an id
 */
class VlePushButton : public QPushButton
{
    Q_OBJECT
public:
    VlePushButton(QWidget *parent, const QString& text,
            const QString& idStr="");
    ~VlePushButton();
    QString id;
public slots:
    void onClicked(bool b);
signals:
    void clicked(const QString& id, bool i);
};

/**
 * Required to avoid wheelEvent if for example embedded into a ScrollArea
 * reimplement wheelEvent(QWheelEvent *event)
 */
class VleSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    VleSpinBox(QWidget *parent, int val, const QString& idStr = "",
	       int min = -10000000, int max = 10000000);
    ~VleSpinBox();
    void setMin(int min);
    void wheelEvent(QWheelEvent *event);
    void focusInEvent(QFocusEvent* e);
    QString id;
public slots:
    void onValueChanged(int i);
    void onEditingFinished()
    {
    }
signals:
    void valUpdated(const QString& id, int i);
    void selected(const QString& id);
};

/**
 * Required to avoid wheelEvent if for example embedded into a ScrollArea
 * reimplement wheelEvent(QWheelEvent *event)
 */
class VleDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    VleDoubleSpinBox(QWidget* parent, double val, const QString& idStr = "",
	 int min = -10000000, int max = 10000000);
    ~VleDoubleSpinBox();
    void wheelEvent(QWheelEvent *event);
    void focusInEvent(QFocusEvent* e);

    QString id;
public slots:
    void onValueChanged(double v);
signals:
    void valUpdated(const QString& id, double v);
    void selected(const QString& id);
};

/**
 * Required to identify which combo is pressed (id is the name of the dynamic)
 * reimplement mousePressEvent(QMouseEvent * e)
 * send
 */
class VleCombo : public QComboBox
{
    Q_OBJECT
public:
    VleCombo(QWidget *parent, const QString& idStr="");
    ~VleCombo();
    void wheelEvent(QWheelEvent *event);
    static VleCombo* buildVleBoolCombo(QWidget* parent, const QString& idStr,
            bool val);
    void focusInEvent(QFocusEvent* e);
    QString id;
public slots:
    void onValueChanged(const QString& v);
signals:
    void valUpdated(const QString& id, const QString& v);
    void selected(const QString& id);
};


/**
 * A line Editor (TODO validator for c++ names)
 */
class VleLineEdit : public QLineEdit
{
    Q_OBJECT
public:

    VleLineEdit(QWidget* parent, const QString& val, const QString& idStr="",
            bool withDefaultMenu = true);
    ~VleLineEdit();
    void focusInEvent(QFocusEvent* e);
    bool eventFilter(QObject *target, QEvent *event);
    void setValue(const QString& val);

    QString id;
    QString backup;
public slots:
    void onValueChanged();
signals:
    void textUpdated(const QString& id, const QString& old, const QString& neW);
    void selected(const QString& id);
};

/**
 * Required for setting a new value only on lost focus (not on all changes)
 * reimplement focusOutEvent
 * TODO should contain text rules no comma, no '<', etc..
 * send textUpdated on lost focus
 */
class VleTextEdit: public QPlainTextEdit
{
    Q_OBJECT
public:
    VleTextEdit(QWidget* parent, const QString& text, const QString& id="",
            bool editOnDbleClick = false);
    ~VleTextEdit();
    void wheelEvent(QWheelEvent *event);
    void setText(const QString& text);
    void setTextEdition(bool val);
    QString getSavedText();
    QString getCurrentText();

    void focusOutEvent(QFocusEvent* e);
    void focusInEvent(QFocusEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    QString saved_value;
    QString id;
    bool edit_on_dble_click;

signals:
    void textUpdated(const QString& id, const QString& old,
            const QString& newVal);
    void selected(const QString& id);

};


/**
 * @brief A resizeable QStackedWidget, size is adapted from the current widget
 * see: http://stackoverflow.com/questions/23511430/qt-qstackedwidget-resizing-issue
 */
class VleStackedWidget : public QStackedWidget
{
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;
};

/**
 * Wigdet to edit a vle::value recursively (no UI required)
 * Could be used for plugins ?
 */
class VleValueWidget : public QWidget
{
    Q_OBJECT

public:
    enum eValueWidgetMenu {
      EMenuValueAddBoolean, EMenuValueAddInteger, EMenuValueAddDouble,
      EMenuValueAddString, EMenuValueAddSet, EMenuValueAddMap,
      EMenuValueAddTuple, EMenuValueAddTable, EMenuValueAddMatrix,
      EMenuValueSetBoolean, EMenuValueSetInteger, EMenuValueSetDouble,
      EMenuValueSetString, EMenuValueSetSet, EMenuValueSetMap,
      EMenuValueSetTuple, EMenuValueSetTable, EMenuValueSetMatrix,
      EMenuValueRemove};
    enum ValueDisplayType {TypeOnly, Insight, ValueOnly};
    static QString getValueDisplay(const vle::value::Value& v,
            ValueDisplayType displayType);
    struct value_stack {
        value_stack();
        ~value_stack();
        std::unique_ptr<value::Value> startValue;
        typedef  std::vector<std::unique_ptr<value::Value>> cont;
        cont stack;
        void setStartValue(std::unique_ptr<vle::value::Value> val);
        /*
         * @brief push a vle::value::Map key
         */
        void push(const std::string& key);
        /*
         * @brief push a vle::value::Set index
         */
        void push(int index);
        /*
         * @brief push a vle::value::Matrix index (row column)
         */
        void push(int row, int col);

        value::Value& editingValue();
        std::vector<std::string> toString();
    };


    value_stack mValueStack;

    VleStackedWidget* stack;
    VleCombo*       value_bool;
    VleSpinBox*     value_int;
    VleDoubleEdit*  value_double;
    VleTextEdit*    value_string;
    QWidget*        value_complex;


    QWidget* stack_buttons;
    QTableWidget* table;
    QWidget* resize_place;
    VleSpinBox* resize_row;
    VleSpinBox* resize_col;
    VlePushButton* resize;
    QString mId;
    bool mLimited;

    /*
     * @brief limited provide a limited mode
     */
    explicit VleValueWidget(QWidget *parent = 0, bool limited = false,
            const QString& header ="");
    ~VleValueWidget();
    void showCurrentValueDetail();
    void setId(const QString& id);
    void setValue(std::unique_ptr<vle::value::Value> val);
    std::unique_ptr<value::Value> buildDefaultValue(eValueWidgetMenu type);

signals:
    void valUpdated(const vle::value::Value& newVal);
public slots:
    void setDoubleClicked(int, int);
    void onMenuSetView(const QPoint& pos);
    void onValUpdated(const QString& id, int newVal);
    void onValUpdated(const QString& id, double newVal);
    void onValUpdated(const QString& id, const QString& boolVal);
    void onTextUpdated(const QString& id, const QString& old,
            const QString& newVal);
    void onResize(bool b);
    void onStackButtonClicked(const QString& id, bool b);

private:
    void setBoolWidget(const QString& id, const vle::value::Value& val,
            int r, int c);
    void setIntWidget(const QString& id, const vle::value::Value& val,
            int r, int c);
    void setDoubleWidget(const QString& id, const vle::value::Value& val,
            int r, int c);
    void setDoubleWidget(const QString& id, double val, int r, int c);
    void setStringWidget(const QString& id, const vle::value::Value& val,
            int r, int c);
    void setStringWidget(const QString& id, const QString&  val,
            int r, int c);
    void setComplexWidget(const QString& id, const vle::value::Value& val,
            int r, int c);

};

}}//namespaces


#endif
