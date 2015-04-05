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

#ifndef VLEVPZ_H
#define VLEVPZ_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QStaticText>
#include <QStyleOption>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QXmlDefaultHandler>
#include <QDateTimeEdit>
#include "vlepackage.h"
#include "vlevpzconn.h"
#include "vlevpzdynamic.h"
#include "vlevpzport.h"
#include <vle/value/Value.hpp>
#include "vle/gvle2/ui_filevpztooltip.h"

#undef USE_GRAPHICVIEW

class vleVpzModel;
class vlePackage;

namespace Ui {
class fileVpzTooltip;
}

class vleVpz : public QObject /*, public QXmlDefaultHandler */
{
    Q_OBJECT
public:
    vleVpz();
    vleVpz(const QString &filename);
    QString        getFilename();
    const QDomDocument& getDomDoc() const;
    QDomDocument& getDomDoc();
    QString        getBasePath();

    QString        getAuthor() const;
    void           setAuthor(const QString author);
    QString        getDate() const;
    void           setDate(const QString date);
    QString        getVersion() const;
    void           setVersion(const QString version);
    QString        getExpName() const;
    void           setExpName(const QString name);
    QString        getExpDuration() const;
    void           setExpDuration(const QString duration);
    QString        getExpBegin() const;
    void           setExpBegin(const QString begin);
    /**
     * @brief get <views> tag from Vpz doc
     */
    QDomNode viewsFromDoc() const;
    /**
     * @brief get <conditions> tag from Vpz doc
     */
    QDomNode condsFromDoc() const;
    /**
     * @brief build an empty node corresponding to the value type from Vpz doc
     */
    QDomElement buildEmptyValueFromDoc(vle::value::Value::type vleType);
    /**
     * @brief tells if a condition exists
     */
    bool existCondFromDoc(const QString& condName) const;
    /**
     * @brief tells if the port portName exists in condition condName
     */
    bool existPortFromDoc(const QString& condName,
            const QString& portName) const;
    /**
     * @brief set the 'name' attribute of tag <condition> to a new value
     */
    void renameConditionToDoc(const QString& oldName, const QString& newName);
    /**
     * @brief set the 'name' attribute of tag <port> to a new value
     */
    void renameCondPortToDoc(const QString& condName, const QString& oldName,
            const QString& newName);
    /**
     * @brief get a new condition name not already in tag <conditions>
     * from the Vpz doc
     */
     QString newCondNameToDoc() const;
     /**
      * @brief get a new port name for condition condName not already in tag
      * <condition> from the Vpz doc
      */
     QString newCondPortNameToDoc(const QString& condName) const;
    /**
     * @brief get list of node with tag <port> tag from Vpz doc,
     * for condition of name 'condName'
     */
    QDomNodeList portsListFromDoc(const QString& condName) const;
    /**
     * @brief get <port> tag from Vpz Doc into condition condName
     * which attribute 'name' is portName
     */
    QDomNode portFromDoc(const QString& condName, const QString& portName) const;
    /**
     * @brief add a <condition> tag to a Vpz Doc
     * whith attribute 'name'  condName
     */
    QDomNode addConditionToDoc(const QString& condName);
    /**
     * @brief add a <port> tag to a Vpz Doc
     * whith attribute 'name'  portName for condition condName
     */
    QDomNode addCondPortToDoc(const QString& condName, const QString& portName);
    /**
     * @brief remove a <condition> tag to a Vpz Doc
     * whith attribute 'name'  condName
     */
    void rmConditionToDoc(const QString& condName);
    /**
     * @brief remove <port> tag from a condition to a Vpz doc
     * which attribute 'name' is portName
     */
    void rmCondPortToDoc(const QString& condName, const QString& portName);
    /**
     * @brief add a value to a tag <port> tag from a condition into a Vpz doc
     */
    void addValuePortCondToDoc(const QString& condName, const QString& portName,
            const vle::value::Value& val);
    /**
     * @brief rm a value from a tag <port> tag from a condition into a Vpz doc
     * 'index' is the index of value in port
     */
    void rmValuePortCondToDoc(const QString& condName, const QString& portName,
            int index);
    /**
     * @brief get atomic model tag <model> from a tag <model>,
     * which name is atom
     */
    QDomNode atomicModelFromModel(const QDomNode& node,
            const QString& atom) const;

    /**
     * @brief get list of node with tag <condition> tag from tag <conditions>
     */
    QDomNodeList condsListFromConds(const QDomNode& node) const;

    /**
     * @brief get list of node with tag <port> tag from tag <condition>
     */
    QDomNodeList portsListFromCond(const QDomNode& node) const;

    /**
     * @brief get <condition> tag from  tag <conditions>
     * which attribute 'name' is condName
     */
    QDomNode condFromConds(const QDomNode& node, const QString& condName) const;
    /**
     * @brief get <port> tag from  tag <condition>
     * which attribute 'name' is portName
     */
    QDomNode portFromCond(const QDomNode& node, const QString& portName) const;
    /**
     * @brief remove <condition> tag from <conditions>
     * which attribute 'name' is condName
     */
    void rmCondFromConds(QDomNode node, const QString& condName);
    /**
     * @brief remove <port> tag from <condition>
     * which attribute 'name' is portName
     */
    void rmPortFromCond(QDomNode node, const QString& portName);
    /**
     * @brief add a <condition> tag to <conditions>
     * whith attribute 'name'  condName
     */
    QDomNode addCondition(QDomNode node, const QString& condName);
    /**
     * @brief attach a condition to an anatomic model
     * @param atom, atomic model name
     * @param condName, condition name
     *
     * TODO: atomic model is given without the full name, this is due to
     * vleVpzModel API that does not provide this intel => to fix
     */
    void attachCondToAtomicModel(const QString& atom, const QString& condName);
    /**
     * @brief detach a condition to an anatomic model
     * @param atom, atomic model name
     * @param condName, condition name
     *
     * TODO: atomic model is given without the full name, this is due to
     * vleVpzModel API that does not provide this intel => to fix
     */
    void detachCondToAtomicModel(const QString& atom, const QString& condName);

    /**
     * @brief add a <port> tag to <condition>
     * whith attribute 'name'  portName
     */
    QDomNode addPort(QDomNode node, const QString& portName);
    /**
     * @brief get attribute value of a node which name is attrName
     */
    QString attributeValue(const QDomNode& node, const QString& attrName) const;
    /**
     * @brief set attribute value of a node which name is attrName
     * and value is val
     */
    void setAttributeValue(QDomElement node, const QString& attrName,
            const QString& val);

    /**
     * @brief get <outputs> tag from <views> tag
     */
    QDomNode outputsFromViews(QDomNode node);
    /**
     * @brief get <output> tag from <outputs> tag
     * which name is outputName
     */
    QDomNode outputFromOutputs(QDomNode node, const QString& outputName);
    /**
     * @brief get the list of attribute values of 'name' of different <output>
     * in <experiment>/<views>/<outputs>
     * @param the values to fill
     */
    void viewOutputNames(std::vector<std::string>& v) const;
    /**
     * @brief get the list of attribute values of 'name' of different <view>
     * in <experiment>/<views>
     * @param the values to fill
     */
    void viewNames(std::vector<std::string>& v) const;
    /**
     * @brief get <view> tag from <views> tag
     * which name is viewName
     */
    QDomNode viewFromViews(QDomNode node, const QString& viewName) const;
    /**
     * @brief get attribute 'type' from a tag <view>
     */
     QString viewTypeFromView(QDomNode node) const;
     /**
      * @brief set attribute 'type' to a tag <view>
      */
     void setViewTypeFromView(QDomNode node, const QString& viewType);
     /**
      * @brief get attribute 'timestep' from a tag <view>
      */
     double timeStepFromView(QDomNode node) const;
     /**
      * @brief set attribute 'timestep' to a tag <view>
      */
     void setTimeStepFromView(QDomNode node, double ts);
     /**
      * @brief set output plugin to a tag <output>
      * plugin name has to be of the form :
      *   package/plugin
      */
     bool setOutputPlugin(QDomNode node, const QString& outputPlugin);
     /**
     * @brief get output plugin from a tag <output>
     * plugin name has the form :
     *   package/plugin
     */
    QString getOutputPlugin(QDomNode node);

    /**
     * @brief get map from tag <output> (configuration of output)
     */
    QDomNode mapFromOutput(QDomNode node);

    /**
     * @brief Build a value from given index  of <port> portName of
     * <condtion> condName
     * @note: result is a new allocated vle value.
     */
    vle::value::Value* buildValue(const QString& condName,
            const QString& portName, int index) const;
    /**
     * @brief build a vle value from either tag
     * <integer>, <string>, <map> etc..
     * @note: result is a new allocated vle value.
     */
    vle::value::Value* buildValue(const QDomNode& node) const;
    /**
     * @brief Fill a value at index of <port> portName of <condtion> condName
     * @note: the map is first cleared
     */
    bool fillWithValue(const QString& condName, const QString& portName,
            int index, const vle::value::Value& val);
    /**
     * @brief Fill a Node from a value
     * @note: the main tag should corresponds to the value type ie:
     * <integer>, <string>, <map> etc..
     * @note: the map is first cleared
     */
    bool fillWithValue(QDomNode node, const vle::value::Value& val);

    /**
     * @brief tells if a node is an integer from undefined tag
     */
    bool isInteger(QDomNode node);
    /**
     * @brief get an integer vle value from tag <integer>
     */
    int getInteger(QDomNode node);
    /**
     * @brief get the integer vle value from tag <map> at a specified key
     */
    int getIntegerFromMap(QDomNode node, QString key);
    /**
     * @brief tells the key name has an integer from tag <map>
     */
    bool existIntegerKeyInMap(QDomNode node, QString key);
    /**
     * @brief add an integer key in a <map>
     */
    void addIntegerKeyInMap(QDomNode* node, const QString& key,
            int val);
    /**
     * @brief Remove all childs from a QDomNode (keep attributes)
     */
    void removeAllChilds(QDomNode node);
    /**
     * @brief get <output>  from a tag <views>
     * @note : combine outputsFromViews, outputFromOutput
     */
    QDomNode getOutputFromViews(QDomNode node,
            const QString& outputName);
    /**
     * @brief get output plugin from a tag <views>
     * plugin name has the form :
     *   package/plugin
     */
    QString getOutputPluginFromViews(QDomNode node,
            const QString& outputName);
    /**
     * @brief get a QString representation of a QDomNode
     */
    QString toQString(const QDomNode& node) const;


    void           setBasePath(const QString path);
    vlePackage    *getPackage();
    void           setPackage(vlePackage *package);
    bool           isAltered();
    void           save();
    vleVpzDynamic *getDynamic(QString name);
    void           addDynamic(vleVpzDynamic *dynamic);
    void           removeDynamic(vleVpzDynamic *dynamic);
    QList <vleVpzDynamic *> *getDynamicsList()
            {
        return &mDynamics;
            }

    public slots:
    void focusChange(vleVpzModel *model);
    void enterModel(vleVpzModel *model);
    void addModeler(QString name);
    void addModelerDynamic(vleVpzModel *model, QString lib);
    void openModeler();

    signals:
    void sigFocus(vleVpzModel *model);
    void sigEnterModel(vleVpzModel *model);
    void sigConditionsChanged();
    void sigDynamicsChanged();
    void sigOpenModeler(vleVpzModel *model);
    void sigChanged(QString filename);

    protected:
    bool startElement(const QString &namespaceURI,
            const QString &localName,
            const QString &qName,
            const QXmlAttributes &attributes);
public:
    QByteArray xGetXml();
private:
    void xReadDom();
    void xReadDomStructures(const QDomNode &baseNode);
    void xReadDomDynamics(const QDomNode &baseNode);
    void xReadDomExperiments(const QDomNode &baseNode);
    void xSaveDom(QDomDocument *doc);
    bool xSaveStructures(QDomDocument *doc, QDomNode *baseNode);
    bool xSaveModel(QDomDocument *doc, QDomElement *baseNode, vleVpzModel *model);
    bool xSaveDynamics(QDomDocument *doc, QDomNode *baseNode);
    bool xSaveExperiments(QDomDocument *doc, QDomElement *baseNode);
    int  removeModelDynamic(vleVpzModel *model, vleVpzDynamic *dynamic, bool recurse = false);

private:
    QString mFilename;
    QString mPath;

    QFile mFile;
    QDomDocument mDoc;

    QString mExpCombination;
    vlePackage *mPackage;
    int                     mConditionIteratorIndex;
    QList <vleVpzDynamic*> mDynamics;
    QList <void*>           mViews;
    QDomNode*              mClassesRaw;
    public:
    vleVpzModel*           mRootModel;
};

class vleVpzModel : public QWidget
{
    Q_OBJECT
public:
    vleVpzModel(vleVpz *parent = 0);
    ~vleVpzModel();
    QString getName();
    vleVpz *getVpz();
    void setName(QString name);
    void copyFromModel(vleVpzModel *model);
    void addSubmodel(vleVpzModel *model);
    void delSubmodel(vleVpzModel *model);
    int  countSubmodels();
    vleVpzModel *getSubmodelAt(int i)
    {
        return mSubmodels.at(i);
    }
    QList <vleVpzModel *> *getSubmodels()
            {
        return &mSubmodels;
            }
    QList <vleVpzPort  *> *getInPorts()
            {
        return &mInPorts;
            }
    QList <vleVpzPort  *> *getOutPorts()
            {
        return &mOutPorts;
            }
    QList <vleVpzConn  *> *getConnections()
            {
        return &mConnections;
            }
    vleVpzDynamic *getDynamic();
    void setDynamic(QString dynamicName);
    void removeDynamic();

    void addCondition(const QString& cond);
    void removeCondition(const QString& cond);
    QString getConditionStringList();
    bool hasCondition(const QString& condName);

    QString getObservables()
    {
        return mObservables;
    }
    //Todo a modeler does not have cond
//    bool hasModeler();
//    vpzExpCond *getModelerExpCond();

    bool isAltered();
    void fixWidgetSize(bool doResize = false);
    int getWidth()  { return mWidgetWidth; }
    int getHeight() { return mWidgetHeight; }
    int getRealWidth();
    int getX()      { return mWidgetX; }
    int getY()      { return mWidgetY; }
    int nextInPosY() { mWidInPortY += 16;  return mWidInPortY;  }
    int nextOutPosY(){ mWidOutPortY += 16; return mWidOutPortY; }
    void dispNormal();
    void dispMaximize();
    bool isMaximized();
    void select(bool setFocus = 0);
    void deselect();
    bool isSelected();
    void deselectSub();
    bool mouseMultipleSelect();
    vleVpzModel* getSubmodelByName(QString *name);
    vleVpzPort * getInPortByName(QString *name);
    vleVpzPort * getOutPortByName(QString *name);
protected:
    bool event(QEvent *event);
    void enterEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void hideEvent (QHideEvent * event);
    void showEvent (QShowEvent * event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *evt);
    void mouseDoubleClickEvent(QMouseEvent * event);

    signals:
    void sigFocus(vleVpzModel *model);
    void sigDblClick(vleVpzModel *model);
    void sigAddModeler(QString name);
    void sigOpenModeler();

    public slots:
    void contextMenu(const QPoint & pos);
    void contextMenu(const QPoint & pos, vleVpzPort *port);
    void portConnect(vleVpzPort *port);
    void portDisconnect(vleVpzPort *port);

    public:
    void xLoadNode(const QDomNode &baseNode);
    private:
    void xreadSubmodels(const QDomNode &baseNode);
    void xReadInputs(const QDomNode &baseNode);
    void xReadOutputs(const QDomNode &baseNode);
    void xReadConnections(const QDomNode &baseNode);
    bool xgetXml(QDomDocument *doc, QDomElement *base);
    private:
    QString mName;
    vleVpz *mVpz;
    bool    mIsAltered;
    int     mWidgetHeight;
    int     mWidgetWidth;
    int     mWidgetX;
    int     mWidgetY;
    int     mWidInPortY;
    int     mWidOutPortY;
    QList <vleVpzModel *> mSubmodels;
    QList <vleVpzPort  *> mInPorts;
    QList <vleVpzPort  *> mOutPorts;
    QList <vleVpzConn  *> mConnections;
    vleVpzDynamic *mDynamic;
    QList <QString>  mConditions;
    QString               mObservables;
    QLabel  mTitle;
    bool    mIsMaximized;
    QPoint  mOldPos, mStartPos;
    bool    mIsFocus;
    QWidget *mTooltip;
    Ui::fileVpzTooltip *uiTooltip;
    bool    mIsSelected;
    // Used for mouse selection into maximized model
    bool    mSelect;
    QPoint  mSelectStart;
    QPoint  mSelectCurrent;
    // Used for port selection/connection
    vleVpzPort *mPortInSel;
    vleVpzPort *mPortOutSel;
    private:
    QFont   mPainterFont;
    private:
    int     mSettingLine;
    int     mSettingCorner;
    int     mSettingFontSize;

    public:
    sourceCpp *getModelerClass();

    private:
    sourceCpp *mModelerClass;
};

#endif // VLEVPZ_H
