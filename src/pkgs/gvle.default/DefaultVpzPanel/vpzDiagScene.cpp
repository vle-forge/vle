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

#include <QGraphicsSceneMouseEvent>
#include <QtGui>
#include <qpainterpath.h>

#include "vpzDiagScene.h"
#include <QComboBox>
#include <QGraphicsView>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QtDebug>

namespace vle {
namespace gvle {

VpzConnectionLineItem::VpzConnectionLineItem(QDomNode node,
                                             vleVpz* vpz,
                                             QLineF l,
                                             QGraphicsItem* parent,
                                             VpzDiagScene* scene)
  : QGraphicsItem(parent)
  , mVpz(vpz)
  , mnode(node)
{
    QGraphicsLineItem* it = new QGraphicsLineItem(l, this);
    it->setPen(scene->getPenConnection());
    update();
    it->update();
    it->show();

    // build tool tip
    QDomNode origin = mnode.firstChildElement("origin");
    QDomNode dest = mnode.firstChildElement("destination");
    QString toolTipMessage =
      QString("%1:%2 -> %3:%4")
        .arg(DomFunctions::attributeValue(origin, "model"))
        .arg(DomFunctions::attributeValue(origin, "port"))
        .arg(DomFunctions::attributeValue(dest, "model"))
        .arg(DomFunctions::attributeValue(dest, "port"));
    setToolTip(toolTipMessage);
}

void
VpzConnectionLineItem::paint(QPainter* /*painter*/,
                             const QStyleOptionGraphicsItem* /*option*/,
                             QWidget* /*widget*/)
{
}

QRectF
VpzConnectionLineItem::boundingRect() const
{
    return childrenBoundingRect();
}

void
VpzConnectionLineItem::update(const QRectF& rect)
{
    childItems().at(0)->update(rect);
}

int
VpzConnectionLineItem::type() const
{
    return VpzConnectionLineType;
}

/*******************************************************************************/

VpzPortItem::VpzPortItem(QDomNode node,
                         vleVpz* vpz,
                         bool input,
                         QGraphicsItem* parent,
                         VpzDiagScene* scene)
  : QGraphicsObject(parent)
  , mVpz(vpz)
  , mnode(node)
  , mInput(input)
{
    this->setAcceptHoverEvents(true);

    QPixmap pix(":/icon/resources/icon/vpz_inport.png");
    QGraphicsTextItem* textItem = new QGraphicsTextItem(getPortName(), this);
    textItem->setFont(scene->getFontPort());
    //    textItem->setDefaultTextColor(Qt::black);
    // textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    // textItem->setTextInteractionFlags(Qt::TextEditable);
    textItem->setTextInteractionFlags(Qt::NoTextInteraction);

    new QGraphicsPixmapItem(pix, this);
    update();
}

QRectF
VpzPortItem::boundingRect() const
{
    return QRectF(0,
                  0,
                  getTextItem()->boundingRect().width() +
                    getPixItem()->boundingRect().width(),
                  getTextItem()->boundingRect().height());
}

void
VpzPortItem::update(const QRectF& /*rect*/)
{
    QGraphicsPixmapItem* pixItem = getPixItem();
    QGraphicsTextItem* textItem = getTextItem();
    if (mInput) {
        pixItem->setPos(0,
                        textItem->boundingRect().height() / 2 -
                          pixItem->boundingRect().height() / 2.0);
        textItem->setPos(pixItem->boundingRect().width(), 0);
    } else {

        pixItem->setPos(textItem->boundingRect().width(),
                        textItem->boundingRect().height() / 2 -
                          pixItem->boundingRect().height() / 2.0);
    }
}

void
VpzPortItem::paint(QPainter* /*painter*/,
                   const QStyleOptionGraphicsItem* /*option*/,
                   QWidget* /*widget*/)
{

    // for debug
    // painter->drawRect(boundingRect());
}

QPointF
VpzPortItem::getConnectionPoint()
{
    bool input = VpzDiagScene::isVpzInputPort(this);
    bool main = VpzDiagScene::isVpzMainModel(parentItem());
    QRectF br = boundingRect();
    if ((not input and not main) or (input and main)) {
        return scenePos() + QPointF(br.topRight().x(),
                                    br.topRight().y() + br.height() / 2.0);
    } else {
        return scenePos() +
               QPointF(br.topLeft().x(), br.topLeft().y() + br.height() / 2.0);
    }
}

QString
VpzPortItem::getPortName()
{
    return DomFunctions::attributeValue(mnode, "name");
}

QGraphicsTextItem*
VpzPortItem::getTextItem() const
{
    QList<QGraphicsItem*> chs = QGraphicsItem::childItems();
    for (int i = 0; i < chs.length(); i++) {
        if (chs.at(i)->type() == 8) { // 8 is for QGraphicsTextItem
            return static_cast<QGraphicsTextItem*>(chs.at(i));
        }
    }
    return 0;
}

QGraphicsPixmapItem*
VpzPortItem::getPixItem() const
{
    QList<QGraphicsItem*> chs = QGraphicsItem::childItems();
    for (int i = 0; i < chs.length(); i++) {
        if (chs.at(i)->type() == 7) { // 7 is for QGraphicsPixmapItem
            return static_cast<QGraphicsPixmapItem*>(chs.at(i));
        }
    }
    return 0;
}

void
VpzPortItem::setNameEdition(bool val)
{
    QGraphicsTextItem* textItem = getTextItem();
    if (val and textItem->textInteractionFlags() == Qt::NoTextInteraction) {
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setFocus(
          Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true);    // this ensures that itemChange() gets
                                        // called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();

        if (getPortName() != textItem->toPlainText()) {
            mVpz->renameModelPort(mnode, getTextItem()->toPlainText());
            update();
        }
    }
}

int
VpzPortItem::type() const
{
    if (mInput) {
        return VpzPortTypeIn;
    } else {
        return VpzPortTypeOut;
    }
}

void
VpzPortItem::hoverEnterEvent(QGraphicsSceneHoverEvent* /*evt*/)
{
    setCursor(Qt::PointingHandCursor);
}

/*******************************************************************************/

VpzModelItem::VpzModelItem(QDomNode node,
                           vleVpz* vpz,
                           QGraphicsItem* parent,
                           QGraphicsScene* /*scene*/)
  : QGraphicsItem(parent)
  , mVpz(vpz)
  , mnode(node)
  , margin(0.0)
  , rectWidth(0)
  , rectHeight(0)
{
}

QGraphicsTextItem*
VpzModelItem::getTitle() const
{
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (children.at(i)->type() == 8) { // 8 for QGraphicsTextItem
            return static_cast<QGraphicsTextItem*>(children.at(i));
        }
    }
    return 0;
}

QGraphicsRectItem*
VpzModelItem::getRectangle() const
{
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (children.at(i)->type() == 3) { // 3 for QGraphicsRectItem
            return static_cast<QGraphicsRectItem*>(children.at(i));
        }
    }
    return 0;
}

QList<VpzPortItem*>
VpzModelItem::getInPorts()
{
    QList<VpzPortItem*> res;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (children.at(i)->type() == VpzPortTypeIn) {
            res.append(static_cast<VpzPortItem*>(children.at(i)));
        }
    }
    return res;
}

QList<VpzPortItem*>
VpzModelItem::getOutPorts()
{
    QList<VpzPortItem*> res;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (children.at(i)->type() == VpzPortTypeOut) {
            res.append(static_cast<VpzPortItem*>(children.at(i)));
        }
    }
    return res;
}
VpzPortItem*
VpzModelItem::getInPort(const QString& name)
{
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (children.at(i)->type() == VpzPortTypeIn) {
            if (static_cast<VpzPortItem*>(children.at(i))->getPortName() ==
                name) {
                return static_cast<VpzPortItem*>(children.at(i));
            }
        }
    }
    return 0;
}
VpzPortItem*
VpzModelItem::getOutPort(const QString& name)
{
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (children.at(i)->type() == VpzPortTypeOut) {
            if (static_cast<VpzPortItem*>(children.at(i))->getPortName() ==
                name) {
                return static_cast<VpzPortItem*>(children.at(i));
            }
        }
    }
    return 0;
}

void
VpzModelItem::clearPorts()
{
    QList<VpzPortItem*> inPorts = getInPorts();
    for (int i = 0; i < inPorts.length(); i++) {
        scene()->removeItem(inPorts[i]);
        inPorts[i]->deleteLater();
    }
    QList<VpzPortItem*> outPorts = getOutPorts();
    for (int i = 0; i < outPorts.length(); i++) {
        scene()->removeItem(outPorts[i]);
        outPorts[i]->deleteLater();
    }
}

void
VpzModelItem::clearTitle()
{
    delete getTitle();
}

QString
VpzModelItem::getModelName()
{
    return DomFunctions::attributeValue(mnode, "name");
}

double
VpzModelItem::widthInputPorts()
{
    double maxv = 0;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        QGraphicsItem* it = children.at(i);
        if (VpzDiagScene::isVpzInputPort(it)) {
            maxv = std::max(maxv, it->boundingRect().width());
        }
    }
    return maxv;
}

double
VpzModelItem::widthOutputPorts()
{
    double maxv = 0;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        QGraphicsItem* it = children.at(i);
        if (VpzDiagScene::isVpzOutputPort(it)) {
            maxv = std::max(maxv, it->boundingRect().width());
        }
    }
    return maxv;
}

double
VpzModelItem::heightPort()
{
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        QGraphicsItem* it = children.at(i);
        if (VpzDiagScene::isVpzPort(it)) {
            return it->boundingRect().height();
        }
    }
    return 0;
}

void
VpzModelItem::setNameEdition(bool val)
{

    QGraphicsTextItem* textItem = getTitle();
    if (val and textItem->textInteractionFlags() == Qt::NoTextInteraction) {
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setFocus(
          Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true);    // this ensures that itemChange() gets
                                        // called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();
        if (getModelName() != textItem->toPlainText()) {
            if (mVpz->existSiblingModel(mnode, textItem->toPlainText())) {
                bool oldBlock = textItem->blockSignals(true);
                textItem->setPlainText(getModelName());
                textItem->blockSignals(oldBlock);
            } else {
                mVpz->renameModel(mnode, textItem->toPlainText());
            }
            update();
        }
    }
}

/*******************************************************************************/

VpzSubModelItem::VpzSubModelItem(QDomNode node,
                                 vleVpz* vpz,
                                 QGraphicsItem* parent,
                                 QGraphicsScene* scene)
  : VpzModelItem(node, vpz, parent, scene)
{
    this->setAcceptHoverEvents(true);
    initializeFromDom();
    update();
}

VpzSubModelItem::~VpzSubModelItem()
{
    // clear();
}

QRectF
VpzSubModelItem::boundingRect() const
{
    return QRectF(
      0, 0, rectWidth, rectHeight + getTitle()->boundingRect().height());
}

void
VpzSubModelItem::paint(QPainter* /*painter*/,
                       const QStyleOptionGraphicsItem* /*option*/,
                       QWidget* /*widget*/)
{
}

void
VpzSubModelItem::initializeFromDom()
{
    clearPorts();
    clearTitle();

    VpzDiagScene* vpzscene = static_cast<VpzDiagScene*>(scene());
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, true);
    QGraphicsTextItem* it = new QGraphicsTextItem(getModelName(), this);
    it->setFont(vpzscene->getFontModel());

    QGraphicsRectItem* rec = new QGraphicsRectItem(QRectF(), this);
    if (DomFunctions::attributeValue(mnode, "type") == "atomic") {
        rec->setBrush(vpzscene->getBrushAtomicModel());
    } else {
        rec->setBrush(vpzscene->getBrushCoupledModel());
    }
    rec->setPen(vpzscene->getPenNotSelectedModel());

    QDomNodeList chList = mnode.childNodes();
    // build port items and get max widths of items
    for (int j = 0; j < chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "in") {
            QDomNodeList nChildIns =
              nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsIn = nChildIns.length();
            for (unsigned int k = 0; k < nbPortsIn; k++) {
                QDomNode nChildIn = nChildIns.at(k);
                new VpzPortItem(nChildIn, mVpz, true, this, vpzscene);
            }
        }
    }
    for (int j = 0; j < chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "out") {
            QDomNodeList nChildOuts =
              nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsOut = nChildOuts.length();
            for (unsigned int k = 0; k < nbPortsOut; k++) {
                QDomNode nChildOut = nChildOuts.at(k);
                new VpzPortItem(nChildOut, mVpz, false, this, vpzscene);
            }
        }
    }
}

void
VpzSubModelItem::update(const QRectF& /*rect*/)
{

    // compute rect width and height
    //->initialize with minimal sizes constrained by ports and names

    VpzDiagScene* vpzscene = static_cast<VpzDiagScene*>(scene());

    double w = DomFunctions::attributeValue(mnode, "width").toDouble();
    double h = DomFunctions::attributeValue(mnode, "height").toDouble();
    rectWidth = std::max(
      getTitle()->boundingRect().width(),
      std::max(2 * margin + widthInputPorts() + widthOutputPorts(), w));
    double nbPortMax = std::max(getInPorts().size(), getOutPorts().size());

    rectHeight =
      std::max((nbPortMax + 1) * margin + nbPortMax * heightPort(), h);
    // set localization of ports
    for (int i = 0; i < getInPorts().length(); i++) {
        getInPorts().at(i)->setPos(
          QPointF(0, margin + ((double)i) * (margin + heightPort())));
    }
    for (int i = 0; i < getOutPorts().length(); i++) {
        getOutPorts().at(i)->setPos(
          QPointF(rectWidth - getOutPorts()[i]->boundingRect().width(),
                  margin + ((double)i) * (margin + heightPort())));
    }

    getTitle()->setPos(0, rectHeight);
    QGraphicsRectItem* rect = getRectangle();
    rect->setRect(0, 0, rectWidth, rectHeight);
    if (isSelected()) {
        rect->setPen(vpzscene->getPenSelectedModel());
        getTitle()->setDefaultTextColor(vpzscene->getColorModelNameSelected());
    } else {
        rect->setPen(vpzscene->getPenNotSelectedModel());
        getTitle()->setDefaultTextColor(
          vpzscene->getColorModelNameNotSelected());
    }
}

void
VpzSubModelItem::hoverMoveEvent(QGraphicsSceneHoverEvent* evt)
{
    switch (getSelType(mapToParent(evt->pos()))) {
    case RIGHT:
        setCursor(Qt::SizeHorCursor);
        break;
    case BOTTOM:
        setCursor(Qt::SizeVerCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
        break;
    }
}

void
VpzSubModelItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsItem::mouseDoubleClickEvent(mouseEvent);
}
void
VpzSubModelItem::mouseMoveEvent(QGraphicsSceneMouseEvent* /*mouseEvent*/)
{
    //    QGraphicsItem::mouseMoveEvent(mouseEvent);
    //    double wip = 0;
    //    wip =
    //    static_cast<VpzMainModelItem*>(parentItem())->widthInputPorts();
    //    setX(std::max(x(),wip +margin));
    //    setY(std::max(y(), margin));
    //    this->parentItem()->update();
}
void
VpzSubModelItem::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

    QGraphicsItem::mousePressEvent(mouseEvent);
}
void
VpzSubModelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsItem::mouseReleaseEvent(mouseEvent);
}

SEL_TYPE
VpzSubModelItem::getSelType(QPointF selPoint)
{
    double m = 5; // margin for the selection of borders
    QPointF p = mapFromParent(selPoint);
    if (p.x() < m and p.y() < rectHeight + m) {
        return LEFT;
    }
    if (p.y() < m) {
        return TOP;
    }
    if (p.x() > rectWidth - m and p.y() < rectHeight + m) {
        return RIGHT;
    }
    if (p.y() > rectHeight - m and p.y() < rectHeight + m) {
        return BOTTOM;
    }
    return MIDDLE;
}

QString
VpzSubModelItem::getModelName()
{
    return DomFunctions::attributeValue(mnode, "name");
}

int
VpzSubModelItem::type() const
{
    return VpzSubModelType;
}

void
VpzSubModelItem::removeNameEditionMode()
{

    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (VpzDiagScene::isVpzPort(children.at(i))) {
            static_cast<VpzPortItem*>(children.at(i))->setNameEdition(false);
        }
    }
    setNameEdition(false);
}

/****************************************************************************/
VpzMainModelItem::VpzMainModelItem(QDomNode node,
                                   vleVpz* vpz,
                                   QGraphicsItem* parent,
                                   QGraphicsScene* scene)
  : VpzModelItem(node, vpz, parent, scene)
{
    scene->addItem(this);

    initializeFromDom();
    update();
}

QRectF
VpzMainModelItem::boundingRect() const
{
    return QRectF(
      0, 0, rectWidth, rectHeight + getTitle()->boundingRect().height());
}

void
VpzMainModelItem::initializeFromDom()
{
    clearPorts();
    clearTitle();
    clearLines();
    clearSubModels();

    VpzDiagScene* vpzscene = static_cast<VpzDiagScene*>(scene());

    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, false);

    QGraphicsRectItem* rec = new QGraphicsRectItem(QRectF(), this);
    rec->setPen(vpzscene->getPenNotSelectedModel());
    if (DomFunctions::attributeValue(mnode, "type") == "atomic") {
        rec->setBrush(vpzscene->getBrushAtomicModel());
    } else {
        rec->setBrush(vpzscene->getBrushCoupledModel());
    }
    rec->setFlag(QGraphicsItem::ItemIsMovable, false);
    rec->setFlag(QGraphicsItem::ItemIsSelectable, false);

    // add port items
    QDomNodeList chList = mnode.childNodes();
    for (int j = 0; j < chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "in") {
            QDomNodeList nChildIns =
              nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsIn = nChildIns.length();
            for (unsigned int k = 0; k < nbPortsIn; k++) {
                QDomNode nChildIn = nChildIns.at(k);
                new VpzPortItem(nChildIn, mVpz, true, this, vpzscene);
            }
        }
    }
    for (int j = 0; j < chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "out") {
            QDomNodeList nChildOuts =
              nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsOut = nChildOuts.length();
            for (unsigned int k = 0; k < nbPortsOut; k++) {
                QDomNode nChildOut = nChildOuts.at(k);
                new VpzPortItem(nChildOut, mVpz, false, this, vpzscene);
            }
        }
    }
    // add submodels
    double wports = widthInputPorts();
    QList<QDomNode> childList = mVpz->submodelsFromModel(mnode);
    for (int i = 0; i < childList.size(); i++) {
        QDomNode& n = childList[i];
        double x, y;
        x = QVariant(DomFunctions::attributeValue(n, "x")).toDouble();
        y = QVariant(DomFunctions::attributeValue(n, "y")).toDouble();
        VpzSubModelItem* mod = new VpzSubModelItem(n, mVpz, this, vpzscene);
        mod->setPos(QPointF(std::max(wports + margin, x), std::max(0.0, y)));
    }
    QGraphicsTextItem* it = new QGraphicsTextItem(getModelName(), this);
    it->setFont(vpzscene->getFontModel());
    it->setDefaultTextColor(vpzscene->getColorModelNameNotSelected());
    update();
    addConnLines();
}

void
VpzMainModelItem::clearLines(const QList<VpzSubModelItem*>& sels)
{
    QList<QString> selmodel_names;
    for (int i = 0; i < sels.size(); i++) {
        selmodel_names.append(sels.at(i)->getModelName());
    }

    QList<VpzConnectionLineItem*> lines = getConnLines();
    for (int i = 0; i < lines.length(); i++) {
        VpzConnectionLineItem* con_item = lines[i];
        if (sels.empty() or
            mVpz->isConnectionAssociatedTo(con_item->mnode, selmodel_names)) {
            scene()->removeItem(con_item);
            delete con_item;
        }
    }
}

void
VpzMainModelItem::clearSubModels()
{
    QList<VpzSubModelItem*> subs = getSubModels();
    for (int i = 0; i < subs.length(); i++) {
        scene()->removeItem(subs[i]);
        delete subs[i];
    }
}

void
VpzMainModelItem::addConnLines(const QList<VpzSubModelItem*>& sels)
{
    QList<QString> selmodel_names;
    for (int i = 0; i < sels.size(); i++) {
        selmodel_names.append(sels.at(i)->getModelName());
    }
    // Add connections
    QList<QDomNode> childList =
      mVpz->connectionListFromModel(mnode, selmodel_names);

    VpzDiagScene* vpzscene = static_cast<VpzDiagScene*>(this->scene());

    double SPACE = 5;
    double width_input = widthInputPorts();
    double x_output =
      boundingRect().x() + boundingRect().width() - widthOutputPorts();
    std::map<std::string, int> deltas_out;
    std::map<std::string, int> deltas_in;

    VpzPortItem* ap = 0;
    VpzPortItem* bp = 0;

    VpzSubModelItem* amod = 0;
    VpzSubModelItem* bmod = 0;

    QDomNode orig;
    QDomNode dest;

    QString model_a;
    QString model_b;
    QString mod_port_b; // mod:port for destination

    QString port_a;
    QString port_b;

    int model_a_y_up = 0;
    int model_a_y_down = 0;

    QString conn_type;

    for (int i = 0; i < childList.size(); i++) {
        QDomNode conn = childList[i];
        if (conn.nodeName() == "connection") {
            ap = 0;
            bp = 0;
            amod = 0;
            bmod = 0;

            orig = conn.toElement().elementsByTagName("origin").at(0);
            dest = conn.toElement().elementsByTagName("destination").at(0);
            model_a = DomFunctions::attributeValue(orig, "model");
            model_b = DomFunctions::attributeValue(dest, "model");
            port_a = DomFunctions::attributeValue(orig, "port");
            port_b = DomFunctions::attributeValue(dest, "port");

            conn_type = DomFunctions::attributeValue(conn, "type");

            if (conn_type == "output") {
                amod = getSubModel(model_a);
                ap = amod->getOutPort(port_a);
                bp = getOutPort(port_b);
                model_b = QString("_%1").arg(model_b);
            } else if (conn_type == "internal") {
                amod = getSubModel(model_a);
                bmod = getSubModel(model_b);
                model_a_y_up = amod->pos().y();
                model_a_y_down =
                  amod->pos().y() + amod->boundingRect().height();
                ap = amod->getOutPort(port_a);
                bp = bmod->getInPort(port_b);
            } else if (conn_type == "input") {
                bmod = getSubModel(model_b);
                ap = getInPort(port_a);
                bp = bmod->getInPort(port_b);
                model_a = QString("_%1").arg(model_a);
            }
            deltas_out[model_a.toStdString()]++;
            QString mod_port_b = QString("%1:%2").arg(model_b).arg(port_b);
            deltas_in[mod_port_b.toStdString()]++;

            int ind_din = deltas_in[mod_port_b.toStdString()];
            double dout = deltas_out[model_a.toStdString()] * SPACE;
            double din = ind_din / (int)2;
            din = din * SPACE;
            if (ind_din % 2 == 0) {
                din = -din;
            }

            if (ap and bp) { // paint connections
                QPointF a = ap->getConnectionPoint();
                QPointF b = bp->getConnectionPoint();
                if (a.x() + 3 * SPACE >= b.x()) {
                    QPointF trans1(a.x() + dout, a.y());
                    QPointF trans2(a.x() + dout, model_a_y_up - SPACE);
                    QPointF trans3(b.x() - 3 * SPACE, model_a_y_up - SPACE);
                    if (a.y() < b.y()) {
                        trans2.setY(model_a_y_down + SPACE);
                        trans3.setY(model_a_y_down + SPACE);
                    }
                    QPointF trans4(b.x() - 3 * SPACE, b.y() + din);
                    QPointF trans5(b.x() - 2 * SPACE, b.y() + din);

                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(a, trans1), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans1, trans2), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans2, trans3), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans3, trans4), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans4, trans5), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans5, b), this, vpzscene);
                } else {
                    QPointF trans1(a.x() + dout, a.y());
                    QPointF trans2(a.x() + dout, b.y() + din);
                    QPointF trans3(b.x() - 2 * SPACE, b.y() + din);
                    if (conn_type == "input") {
                        trans1.setX(std::min(width_input + dout,
                                             width_input + 10 * SPACE));
                        trans2.setX(std::min(width_input + dout,
                                             width_input + 10 * SPACE));
                    } else if (conn_type == "output") {
                        trans1.setX(std::min(a.x() + dout, x_output - SPACE));
                        trans2.setX(std::min(a.x() + dout, x_output - SPACE));
                    }

                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(a, trans1), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans1, trans2), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans2, trans3), this, vpzscene);
                    new VpzConnectionLineItem(
                      conn, mVpz, QLineF(trans3, b), this, vpzscene);
                }
            }
        }
    }
}
void
VpzMainModelItem::update(const QRectF& /*rect*/)
{
    setSelected(false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, false);
    double SPACE = 5;
    // compute rect width and height
    //->initialize with minimal sizes constrained by ports and names
    QRectF subModsRect = subModelsBoundingRect(false);
    rectWidth =
      std::max(std::max(std::max(200.0, getTitle()->boundingRect().width()),
                        subModsRect.x() + subModsRect.width() +
                          widthOutputPorts() + 10 * SPACE),
               widthInputPorts() + widthOutputPorts() + 10 * SPACE);
    double nbPortMax = std::max(getInPorts().size(), getOutPorts().size());

    rectHeight = std::max(
      std::max(200.0, (nbPortMax + 1) * SPACE + nbPortMax * heightPort()),
      subModsRect.y() + subModsRect.height() + 2 * SPACE);

    // set localization of ports
    for (int i = 0; i < getInPorts().length(); i++) {
        getInPorts().at(i)->setPos(
          QPointF(0, SPACE + ((double)i) * (SPACE + heightPort())));
    }
    for (int i = 0; i < getOutPorts().length(); i++) {
        getOutPorts().at(i)->setPos(
          QPointF(rectWidth - getOutPorts()[i]->boundingRect().width(),
                  SPACE + ((double)i) * (SPACE + heightPort())));
    }

    // move to x,y
    QList<VpzSubModelItem*> sub = getSubModels();
    for (int i = 0; i < sub.length(); i++) {
        sub[i]->setPos(QPointF(
          std::max(
            widthInputPorts() + SPACE,
            DomFunctions::attributeValue(sub[i]->mnode, "x").toDouble()),
          std::max(
            0.0,
            DomFunctions::attributeValue(sub[i]->mnode, "y").toDouble())));
        sub[i]->update();
    }

    getTitle()->setPos(0, rectHeight);
    getRectangle()->setRect(0, 0, rectWidth, rectHeight);
    clearLines();
    addConnLines();
}

void
VpzMainModelItem::paint(QPainter* /*painter*/,
                        const QStyleOptionGraphicsItem* /*option*/,
                        QWidget* /*widget*/)
{
}

void
VpzMainModelItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

    QGraphicsItem::mouseDoubleClickEvent(mouseEvent);
}
void
VpzMainModelItem::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsItem::mouseMoveEvent(mouseEvent);
}
void
VpzMainModelItem::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsItem::mousePressEvent(mouseEvent);
}
void
VpzMainModelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsItem::mouseReleaseEvent(mouseEvent);
}

void
VpzMainModelItem::dragEnterEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
}

void
VpzMainModelItem::dragLeaveEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
}

void
VpzMainModelItem::dragMoveEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
}

QRectF
VpzMainModelItem::subModelsBoundingRect(bool onlySelected)
{
    QRectF res(0, 0, 0, 0);
    bool first = true;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        QGraphicsItem* it = children.at(i);
        if (VpzDiagScene::isVpzSubModel(it)) {
            if (not onlySelected or (onlySelected and it->isSelected())) {
                if (first) {
                    res.setX(it->x());
                    res.setY(it->y());
                    res.setWidth(it->boundingRect().width());
                    res.setHeight(it->boundingRect().height());
                    first = false;
                } else {
                    res.setX(std::min(res.x(), it->x()));
                    res.setY(std::min(res.y(), it->y()));
                    res.setWidth(
                      std::max(it->x() - res.x() + it->boundingRect().width(),
                               res.width()));
                    res.setHeight(
                      std::max(it->y() - res.y() + it->boundingRect().height(),
                               res.height()));
                }
            }
        }
    }
    return res;
}

VpzSubModelItem*
VpzMainModelItem::getSubModel(const QString& subMod)
{
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (VpzDiagScene::isVpzSubModel(children.at(i))) {
            VpzSubModelItem* modItem =
              static_cast<VpzSubModelItem*>(children.at(i));
            if (modItem->getModelName() == subMod) {
                return modItem;
            }
        }
    }
    return 0;
}

QList<VpzSubModelItem*>
VpzMainModelItem::getSubModels()
{
    QList<VpzSubModelItem*> res;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (VpzDiagScene::isVpzSubModel(children.at(i))) {
            res.append(static_cast<VpzSubModelItem*>(children.at(i)));
        }
    }
    return res;
}

QList<VpzSubModelItem*>
VpzMainModelItem::getSelectedSubModels()
{
    QList<VpzSubModelItem*> res;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        QGraphicsItem* ch = children.at(i);
        if (VpzDiagScene::isVpzSubModel(ch) and ch->isSelected()) {
            res.append(static_cast<VpzSubModelItem*>(ch));
        }
    }
    return res;
}

QList<QDomNode>
VpzMainModelItem::getSelectedSubModelsNode()
{
    QList<QDomNode> res;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        QGraphicsItem* ch = children.at(i);
        if (VpzDiagScene::isVpzSubModel(ch) and ch->isSelected()) {
            res.append(static_cast<VpzSubModelItem*>(ch)->mnode);
        }
    }
    return res;
}

QList<VpzConnectionLineItem*>
VpzMainModelItem::getConnLines()
{
    QList<VpzConnectionLineItem*> res;
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (VpzDiagScene::isVpzConnectionLine(children.at(i))) {
            res.append(static_cast<VpzConnectionLineItem*>(children.at(i)));
        }
    }
    return res;
}

void
VpzMainModelItem::removeNameEditionMode()
{

    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.length(); i++) {
        if (children.at(i)) {
            if (VpzDiagScene::isVpzPort(children.at(i))) {
                static_cast<VpzPortItem*>(children.at(i))
                  ->setNameEdition(false);
            } else if (VpzDiagScene::isVpzSubModel(children.at(i))) {
                static_cast<VpzSubModelItem*>(children.at(i))
                  ->removeNameEditionMode();
            }
        }
    }
    setNameEdition(false);
}

bool
VpzMainModelItem::isAtomic()
{
    return (DomFunctions::attributeValue(mnode, "type") == "atomic");
}

int
VpzMainModelItem::type() const
{
    return VpzMainModelType;
}

/*********************************************************************************/

VpzDiagScene::VpzDiagScene(Logger* log, const utils::ContextPtr& ctx)
  : QGraphicsScene()
  , mVpz(0)
  , mClass("")
  , mCoupled(0)
  , mPortSel1(0)
  , mPortSel2(0)
  , mConnection(0)
  , mDragStartPoint()
  , mDragCurrPoint()
  , mModelSelType(MIDDLE)
  , mIsEnteringCoupled(false)
  , mLog(log)
  , mCtx(ctx)
{
    setBackgroundBrush(getBrushBackground());
}

void
VpzDiagScene::init(vleVpz* vpz, const QString& className)
{
    clear();
    mVpz = vpz;
    mClass = className;
    if (mClass == "" or mVpz->existClassFromDoc(className)) {
        QDomNode selModelNode = mVpz->classModelFromDoc(className);
        setFocus(selModelNode);
    }

    emit initializationDone(this);
}

QString
VpzDiagScene::getClass()
{
    return mClass;
}

void
VpzDiagScene::setFocus(QDomNode selModelNode, bool refreshAll)
{
    if (mCoupled) {
        QString oldCoupled = mVpz->vdo()->getXQuery(mCoupled->mnode);
        QString newCoupled = mVpz->vdo()->getXQuery(selModelNode);
        if ((oldCoupled == newCoupled) and not refreshAll) {
            mCoupled->update();
            return;
        }
        bool oldBlock = this->blockSignals(true);
        delete mCoupled;
        this->blockSignals(oldBlock);
    }
    mCoupled = new VpzMainModelItem(selModelNode, mVpz, 0, this);
    // addItem(mCoupled);
    mCoupled->setPos(QPointF(0, 0));
    update();
    unselectAllSubModels();
    emit enterCoupledModel(mCoupled->mnode);
}

void
VpzDiagScene::clear()
{
    bool oldBlock = this->blockSignals(true);
    mVpz = 0;
    delete mCoupled;
    mCoupled = 0;
    mPortSel1 = 0;
    mPortSel2 = 0;
    mConnection = 0;
    mDragStartPoint = QPointF();
    mDragCurrPoint = QPointF();
    mModelSelType = MIDDLE;
    mIsEnteringCoupled = false;
    this->blockSignals(oldBlock);
}

void
VpzDiagScene::update(const QRectF& /*rect*/)
{
    if (not mCoupled) {
        return;
    }
    if (views().size() > 0) {
        views()[0]->verticalScrollBar()->setValue(
          views()[0]->verticalScrollBar()->minimum());
        views()[0]->horizontalScrollBar()->setValue(
          views()[0]->horizontalScrollBar()->minimum());
        mCoupled->update();
    }
}

void
VpzDiagScene::setScale(const qreal z)
{
    foreach (QGraphicsView* view, views()) {
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        view->scale(z, z);
    }
}

void
VpzDiagScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QList<QGraphicsItem*> items = this->items(mouseEvent->scenePos());
    // if a port is selected do nothing
    for (int i = 0; i < items.length(); i++) {
        if (VpzDiagScene::isVpzPort(items[i])) {
            return;
        }
    }
    // try enter a coupled submodel
    for (int i = 0; i < items.length(); i++) {
        if (VpzDiagScene::isVpzSubModel(items[i])) {
            VpzSubModelItem* subMod = static_cast<VpzSubModelItem*>(items[i]);
            SEL_TYPE seType = subMod->getSelType(mouseEvent->scenePos());
            if ((seType == MIDDLE) and
                (DomFunctions::attributeValue(subMod->mnode, "type") ==
                 "coupled")) {
                setFocus(subMod->mnode);
                mIsEnteringCoupled = true;
                return;
            }
        }
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void
VpzDiagScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (not mCoupled) {
        mouseEvent->ignore();
        return;
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
void
VpzDiagScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (not mCoupled) {
        mouseEvent->ignore();
        return;
    }
    mPortSel1 = 0;
    if (mouseEvent->button() == Qt::LeftButton) {
        QList<QGraphicsItem*> items = this->items(mouseEvent->scenePos());
        // Look for a drag for connection from a VpzPortItem
        for (int i = 0; i < items.length(); i++) {
            if (VpzDiagScene::isVpzPort(items[i])) {
                unselectAllSubModels();
                VpzPortItem* itemPort = static_cast<VpzPortItem*>(items[i]);
                mPortSel1 = itemPort;
                QDrag* drag = new QDrag(mouseEvent->widget());
                drag->setMimeData(new QMimeData);
                QObject::connect(drag,
                                 SIGNAL(destroyed(QObject*)),
                                 this,
                                 SLOT(onDragDestroyed(QObject*)));
                drag->start();
                return;
            }
        }
        mCoupled->removeNameEditionMode();
        // look for VpzModel selection
        for (int i = 0; i < items.length(); i++) {
            if (VpzDiagScene::isVpzSubModel(items[i])) {
                VpzSubModelItem* mod = static_cast<VpzSubModelItem*>(items[i]);
                // look for incr selection of VpzSubModels with Ctrl touch
                if (mouseEvent->modifiers() == Qt::ControlModifier) {
                    mod->setSelected(not mod->isSelected());
                    return;
                } else {
                    if (not mod->isSelected()) {
                        unselectAllSubModels();
                        mod->setSelected(true);
                        mod->update();
                    }

                    // Start drag event
                    mModelSelType = mod->getSelType(mouseEvent->scenePos());
                    QDrag* drag = new QDrag(mouseEvent->widget());
                    mDragStartPoint = mouseEvent->scenePos();
                    mDragCurrPoint = mDragStartPoint;
                    drag->setMimeData(new QMimeData);
                    drag->start();
                    return;
                }
            }
        }
        // otherwise unselect all
        unselectAllSubModels();
        mCoupled->update();
    }
    // otherwise performs classical mousePressEvent, ie move item.
    // QGraphicsScene::mousePressEvent(mouseEvent);
}

void
VpzDiagScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (not mCoupled) {
        mouseEvent->ignore();
        return;
    }
    if (mIsEnteringCoupled) {
        mouseEvent->ignore();
        mIsEnteringCoupled = false;
        return;
    }

    mPortSel1 = 0;
    mPortSel2 = 0;
    delete mConnection;
    mConnection = 0;

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    mCoupled->update();
}

void
VpzDiagScene::dragEnterEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
    // QGraphicsScene::dragEnterEvent(event);
}
void
VpzDiagScene::dragLeaveEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
}
void
VpzDiagScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    double SPACE = 5;
    QPointF prevDragPoint = mDragCurrPoint;
    mDragCurrPoint = event->scenePos();

    if (mPortSel1) {
        if (mConnection) {
            mConnection->setLine(
              QLineF(mConnection->line().p1(), event->scenePos()));
            mConnection->update();
        } else {
            mConnection = new QGraphicsLineItem(
              QLineF(mPortSel1->getConnectionPoint(), event->scenePos()), 0);
        }
        return;
    }

    QList<VpzSubModelItem*> sels = mCoupled->getSelectedSubModels();
    if (sels.size() == 1) {
        // move VpzSubModel, and resize capabilities
        VpzSubModelItem* mod = sels.at(0);
        QPointF p = event->scenePos();
        double oldx = mod->pos().x();
        double oldy = mod->pos().y();
        switch (mModelSelType) {
        case RIGHT: {
            double oldwidth = mod->boundingRect().width();
            double ri = p.x() - (oldx + oldwidth);
            double newwidth = oldwidth + ri;
            mVpz->setWidthToModel(mod->mnode, newwidth);
            mod->update();
            mCoupled->update();
            break;
        }
        case BOTTOM: {
            double oldheight = mod->boundingRect().height();
            double bi = p.y() - (oldy + oldheight);
            double newheight = oldheight + bi;
            mVpz->setHeightToModel(mod->mnode, newheight);
            mod->update();
            mCoupled->update();
            break;
        }
        case MIDDLE: {
            double x = std::max(
              mCoupled->widthInputPorts() + mCoupled->margin + 10 * SPACE,
              mod->pos().x() + mDragCurrPoint.x() - prevDragPoint.x());
            double y = std::max(mCoupled->margin,
                                mod->pos().y() + mDragCurrPoint.y() -
                                  prevDragPoint.y());
            if ((mDragCurrPoint.x() != prevDragPoint.x()) or
                (mDragCurrPoint.y() != prevDragPoint.y())) {
                // avoid a modif on a doubleClicked
                mod->setPos(x, y);
                mVpz->setPositionToModel(mod->mnode, (int)x, (int)y);
                mCoupled->clearLines(sels);
                mCoupled->addConnLines(sels);
            }
            break;
        }
        default:
            break;
        }
    } else {

        QRectF r = mCoupled->subModelsBoundingRect(true);
        double deltaX =
          std::max((mCoupled->widthInputPorts() + mCoupled->margin) - r.x(),
                   mDragCurrPoint.x() - prevDragPoint.x());
        double deltaY = std::max(mCoupled->margin - r.y(),
                                 mDragCurrPoint.y() - prevDragPoint.y());

        // move VpzSubModel only
        QList<QDomNode> modsToMove;
        QList<int> newXs;
        QList<int> newYs;

        for (int i = 0; i < sels.length(); i++) {
            VpzSubModelItem* mod = sels.at(i);
            double x = mod->pos().x() + deltaX;
            double y = mod->pos().y() + deltaY;
            mod->setPos(x, y);
            modsToMove.append(mod->mnode);
            newXs.append((int)x);
            newYs.append((int)y);
            DomFunctions::setAttributeValue(
              mod->mnode, "x", QVariant(x).toString());
            DomFunctions::setAttributeValue(
              mod->mnode, "y", QVariant(y).toString());
        }
        mVpz->setPositionToModel(modsToMove, newXs, newYs);
        mCoupled->clearLines(sels);
        mCoupled->addConnLines(sels);
    }
}

void
VpzDiagScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if (mPortSel1) {
        QList<QGraphicsItem*> items = this->items(event->scenePos());
        // look for a second port for a connection
        bool found = false;
        for (int i = 0; i < items.length(); i++) {

            if ((not found) and (items[i] != mPortSel1) and
                (VpzDiagScene::isVpzPort(items[i]))) {
                VpzPortItem* itemPort = static_cast<VpzPortItem*>(items[i]);
                mPortSel2 = itemPort;
                mVpz->addModelConnection(mPortSel1->mnode, mPortSel2->mnode);
                found = true;
            }
        }
        delete mConnection;
        mConnection = 0;
        mPortSel1 = 0;
        mPortSel2 = 0;
        mCoupled->update();
    }
}

void
VpzDiagScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    mPortSel1 = 0;
    mPortSel2 = 0;

    QList<QGraphicsItem*> sels = items(event->scenePos());
    QGraphicsItem* sel = 0;

    for (int i = 0; i < sels.length(); i++) {
        if (isVpzPort(sels.at(i))) {
            sel = sels.at(i);
        } else if (isVpzConnectionLine(sels.at(i))) {
            sel = sels.at(i);
        } else if (isVpzSubModel(sels.at(i)) and not isVpzSubModel(sel) and
                   not isVpzPort(sel) and not isVpzConnectionLine(sel)) {
            sel = sels.at(i);
        } else if (isVpzMainModel(sels.at(i)) and not isVpzSubModel(sel) and
                   not isVpzPort(sel) and not isVpzConnectionLine(sel)) {
            sel = sels.at(i);
        }
    }

    bool debug_checked = false;
    if (sel and not isVpzMainModel(sel) and
        (static_cast<VpzMainModelItem*>(sel)->isAtomic())) {
        VpzSubModelItem* item_atomic = static_cast<VpzSubModelItem*>(sel);
        debug_checked = vleDomStatic::debuggingAtomic(item_atomic->mnode);
    }

    QList<VpzSubModelItem*> selMods = mCoupled->getSelectedSubModels();
    QMenu menu;
    QAction* action;
    QMenu* submenu;
    QMenu* datasubmenu;

    action = menu.addAction("Edit name");
    setActionType(action, VDMA_Edit_name);
    action->setEnabled(
      (sel and isVpzPort(sel)) or
      (sel and isVpzSubModel(sel) and (selMods.size() == 1) and
       (selMods.contains(static_cast<VpzSubModelItem*>(sel)))) or
      (sel and isVpzSubModel(sel) and (selMods.size() == 0)) or
      (sel and isVpzMainModel(sel)));
    action = menu.addAction("Remove");
    setActionType(action, VDMA_Remove);
    action->setEnabled(
      (sel and isVpzPort(sel)) or
      (sel and isVpzSubModel(sel) and (selMods.size() >= 1) and
       (selMods.contains(static_cast<VpzSubModelItem*>(sel)))) or
      (sel and isVpzSubModel(sel) and (selMods.size() == 0)) or
      (sel and isVpzConnectionLine(sel)));
    menu.addSeparator();
    action = menu.addAction("Add input port");
    setActionType(action, VDMA_Add_input_port);
    action->setEnabled(
      (sel and isVpzSubModel(sel) and (selMods.size() == 1) and
       (selMods.contains(static_cast<VpzSubModelItem*>(sel)))) or
      (sel and isVpzSubModel(sel) and (selMods.size() == 0)) or
      (sel and isVpzMainModel(sel)));
    action = menu.addAction("Add output port");
    menu.addSeparator();
    setActionType(action, VDMA_Add_output_port);
    action->setEnabled(
      (sel and isVpzSubModel(sel) and (selMods.size() == 1) and
       (selMods.contains(static_cast<VpzSubModelItem*>(sel)))) or
      (sel and isVpzSubModel(sel) and (selMods.size() == 0)) or
      (sel and isVpzMainModel(sel)));
    action = menu.addAction("Add atomic model");
    setActionType(action, VDMA_Add_atomic_model);
    action->setEnabled(sel and isVpzMainModel(sel) and
                       not(static_cast<VpzMainModelItem*>(sel)->isAtomic()));
    action = menu.addAction("Add coupled model");
    setActionType(action, VDMA_Add_coupled_model);
    action->setEnabled(sel and isVpzMainModel(sel) and
                       not(static_cast<VpzMainModelItem*>(sel)->isAtomic()));
    menu.addSeparator();
    action = menu.addAction("Zoom In");
    setActionType(action, VDMA_Zoom_In);
    action->setEnabled(true);
    action = menu.addAction("Zoom Out");
    setActionType(action, VDMA_Zoom_Out);
    action->setEnabled(true);
    menu.addSeparator();
    submenu = menu.addMenu("Configure");
    datasubmenu = menu.addMenu("Data configure");
    bool enabled = (sel and not isVpzMainModel(sel) and
                    (static_cast<VpzMainModelItem*>(sel)->isAtomic()) and
                    not isVpzSubModelConfigured(sel));

    submenu->setEnabled(enabled);
    datasubmenu->setEnabled(enabled);
    if (enabled) {
        populateConfigureMenu(submenu);
        populateDataConfigureMenu(datasubmenu);
    }
    action = menu.addAction("Unconfigure");
    setActionType(action, VDMA_Unconfigure_model);
    action->setEnabled(sel and not isVpzMainModel(sel) and
                       (static_cast<VpzMainModelItem*>(sel)->isAtomic()) and
                       isVpzSubModelConfigured(sel));
    action = menu.addAction("Unconfigure and clear");
    setActionType(action, VDMA_Clear_conf_model);
    action->setEnabled(sel and not isVpzMainModel(sel) and
                       (static_cast<VpzMainModelItem*>(sel)->isAtomic()) and
                       isVpzSubModelConfigured(sel));
    menu.addSeparator();
    submenu = menu.addMenu("Copy");
    submenu->setEnabled(
      (sel and isVpzSubModel(sel) and (selMods.size() >= 1) and
       (selMods.contains(static_cast<VpzSubModelItem*>(sel)))) or
      (sel and isVpzSubModel(sel) and (selMods.size() == 0)));
    action = submenu->addAction("Copy all");
    setActionType(action, VDMA_copy_models_track);
    action = submenu->addAction("Copy structure");
    setActionType(action, VDMA_copy_models_notrack);
    action = menu.addAction("Paste");
    setActionType(action, VDMA_paste_models);
    action->setEnabled(sel and isVpzMainModel(sel) and
                       not(static_cast<VpzMainModelItem*>(sel)->isAtomic()));
    menu.addSeparator();
    action = menu.addAction("Debug model");
    action->setCheckable(true);
    action->setChecked(debug_checked);
    setActionType(action, VDMA_Debug_atomic);
    action->setEnabled(sel and not isVpzMainModel(sel) and
                       (static_cast<VpzMainModelItem*>(sel)->isAtomic()));

    if (sel) {
        action = menu.exec(event->screenPos());
        if (action) {
            int actCode = getActionType(action);
            if (actCode == VDMA_copy_models_track) {
                QList<QDomNode> copy_mods;
                for (int i = 0; i < selMods.size(); i++) {
                    copy_mods.append(selMods.at(i)->mnode);
                }
                mVpz->exportToClipboard(copy_mods, true);

            } else if (actCode == VDMA_copy_models_notrack) {
                QList<QDomNode> copy_mods;
                for (int i = 0; i < selMods.size(); i++) {
                    copy_mods.append(selMods.at(i)->mnode);
                }
                mVpz->exportToClipboard(copy_mods, false);
            } else if (actCode == VDMA_paste_models) {
                mVpz->importFromClipboard(mCoupled->mnode);
                mCoupled->initializeFromDom();
            } else if (actCode == VDMA_Edit_name) {
                if (isVpzPort(sel)) {
                    VpzPortItem* it = static_cast<VpzPortItem*>(sel);
                    it->setNameEdition(true);
                } else if (isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    it->setNameEdition(true);
                } else if (isVpzMainModel(sel)) {
                    VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                    it->setNameEdition(true);
                }
            } else if (actCode == VDMA_Remove) {
                if (isVpzPort(sel)) {
                    VpzPortItem* it = static_cast<VpzPortItem*>(sel);
                    mVpz->rmModelPort(it->mnode);
                    removeItem(it);
                    delete it;
                    mCoupled->update();
                } else if (isVpzConnectionLine(sel)) {
                    VpzConnectionLineItem* it =
                      static_cast<VpzConnectionLineItem*>(sel);
                    mVpz->rmModelConnection(it->mnode);
                    mCoupled->update();
                } else if (isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    QList<QDomNode> torm =
                      mCoupled->getSelectedSubModelsNode();
                    if (torm.count() > 0) {
                        mVpz->rmModelsFromCoupled(torm);
                        bool oldBlock = this->blockSignals(true);
                        for (int i = 0; i < selMods.size(); i++) {
                            it = selMods.at(i);
                            // TODO not sure why a segfault occurs here
                            // see
                            // http://stackoverflow.com/questions/27188538/how-to-delete-qgraphicsitem-properly
                            // removeItem(it);
                            delete it;
                        }
                        this->blockSignals(oldBlock);
                        mCoupled->update();
                    }
                }
            } else if (actCode == VDMA_Add_input_port) {
                if (isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    mVpz->addModelInputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                } else if (isVpzMainModel(sel)) {
                    VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                    mVpz->addModelInputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                }
            } else if (actCode == VDMA_Add_output_port) {
                if (isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    mVpz->addModelOutputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                } else if (isVpzMainModel(sel)) {

                    VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                    mVpz->addModelOutputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                }
            } else if (actCode == VDMA_Add_atomic_model) {
                VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                mVpz->addModel(it->mnode, "atomic", event->scenePos());
                it->initializeFromDom();
                it->update();
            } else if (actCode == VDMA_Add_coupled_model) {
                VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                mVpz->addModel(it->mnode, "coupled", event->scenePos());
                it->initializeFromDom();
                it->update();
            } else if (actCode == VDMA_Zoom_In) {
                setScale(1.2);
                update();
            } else if (actCode == VDMA_Zoom_Out) {
                setScale(0.8);
                update();
            } else if (actCode == VDMA_Configure_model) {
                VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                QString metadata = action->data().toList()[1].toString();
                doConfigureMenu(it->mnode, metadata);
                it->initializeFromDom();
                it->update();
            } else if (actCode == VDMA_Unconfigure_model) {
                VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                mVpz->unConfigureModel(it->mnode);
                it->initializeFromDom();
                it->update();
            } else if (actCode == VDMA_Clear_conf_model) {
                VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                QSet<QString> sharedConds =
                  mVpz->sharedAttachedConds(it->mnode);
                QString sharedDyn = mVpz->sharedDynamic(it->mnode);
                QString sharedObs = mVpz->sharedObservable(it->mnode);
                if (sharedConds.count() > 0 or sharedDyn != "" or
                    sharedObs != "") {
                    QString text = "The configuation cannot be cleared since ";
                    text += "there are shared elements with other ";
                    text += "atomic models:\n";
                    text += "- conditions: ";
                    QSet<QString>::const_iterator itb = sharedConds.begin();
                    QSet<QString>::const_iterator ite = sharedConds.end();
                    for (; itb != ite; itb++) {
                        text += *itb + ", ";
                    }
                    text += "\n";
                    text += "- dynamic: " + sharedDyn + "\n";
                    text += "- observable: " + sharedObs + "\n";
                    QMessageBox msgBox;
                    msgBox.setText(text);
                    msgBox.exec();
                } else {
                    mVpz->clearConfModel(it->mnode);
                    it->initializeFromDom();
                    it->update();
                }
            } else if (actCode == VDMA_Debug_atomic) {
                VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                vleDomStatic::setDebuggingToAtomic(
                  it->mnode, not debug_checked, mVpz->getUndoStack());
            }
        }
    }
    // QGraphicsScene::contextMenuEvent(event);
}

void
VpzDiagScene::populateConfigureMenu(QMenu* menu)
{
    QString pathgvlesm = "metadata/src";

    for (auto binPkgDir : mCtx->getBinaryPackagesDir()) {

        QString packagesDir = binPkgDir.string().c_str();
        QDirIterator it(packagesDir, QDir::AllDirs);
        QAction* action;

        while (it.hasNext()) {
            it.next();

            if (QDir(it.filePath() + "/" + pathgvlesm).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvlesm,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString metaDataFileName = itbis.next();
                    if (QFile(metaDataFileName).exists()) {
                        QFile file(metaDataFileName);
                        QDomDocument dom("vle_project_metadata");
                        QXmlInputSource source(&file);
                        QXmlSimpleReader reader;
                        dom.setContent(&source, &reader);
                        QDomElement docElem = dom.documentElement();
                        QDomNode srcPluginNode =
                          dom.elementsByTagName("srcPlugin").item(0);
                        QString className = srcPluginNode.attributes()
                                              .namedItem("class")
                                              .nodeValue();
                        QString packName = srcPluginNode.attributes()
                                             .namedItem("package")
                                             .nodeValue();
                        action = menu->addAction(packName + "/" + className);
                        setActionType(action, VDMA_Configure_model);
                        QVariantList dataList = action->data().toList();
                        dataList << metaDataFileName;
                        action->setData(dataList);
                    }
                }
            }
        }
    }
}

void
VpzDiagScene::populateDataConfigureMenu(QMenu* menu)
{
    QString pathgvledm = "metadata/data";

    for (auto binPkgDir : mCtx->getBinaryPackagesDir()) {

        QString packagesDir = binPkgDir.string().c_str();
        QDirIterator it(packagesDir, QDir::AllDirs);
        QAction* action;

        while (it.hasNext()) {
            it.next();
            if (QDir(it.filePath() + "/" + pathgvledm).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvledm,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString metaDataFileName = itbis.next();
                    if (QFile(metaDataFileName).exists()) {
                        QFile file(metaDataFileName);
                        QDomDocument dom("vle_project_metadata");
                        QXmlInputSource source(&file);
                        QXmlSimpleReader reader;
                        dom.setContent(&source, &reader);
                        QDomElement docElem = dom.documentElement();
                        QDomNode dataModelNode =
                          dom.elementsByTagName("dataModel").item(0);
                        QString confName = dataModelNode.attributes()
                                             .namedItem("conf")
                                             .nodeValue();
                        QString packName = dataModelNode.attributes()
                                             .namedItem("package")
                                             .nodeValue();
                        action = menu->addAction(packName + "/" + confName);
                        setActionType(action, VDMA_Configure_model);
                        QVariantList dataList = action->data().toList();
                        dataList << metaDataFileName;
                        action->setData(dataList);
                    }
                }
            }
        }
    }
}

void
VpzDiagScene::doConfigureMenu(QDomNode model, const QString& meta)
{
    QFile file(meta);
    QDomDocument dom("vle_project_metadata");
    QXmlInputSource source(&file);
    QXmlSimpleReader reader;
    dom.setContent(&source, &reader);
    QDomElement docElem = dom.documentElement();

    QDomNode dynamic = dom.elementsByTagName("dynamic").item(0);
    QDomNode observable = dom.elementsByTagName("observable").item(0);
    QDomNode condition = dom.elementsByTagName("condition").item(0);

    QString genericConditionName =
      condition.attributes().namedItem("name").nodeValue();
    QString specificConditionName =
      mVpz->newCondNameToDoc(genericConditionName);
    condition.attributes().namedItem("name").setNodeValue(
      specificConditionName);

    QDomNode in = dom.elementsByTagName("in").item(0);
    QDomNode out = dom.elementsByTagName("out").item(0);

    mVpz->configureModel(model, dynamic, observable, condition, in, out);
}

QBrush
VpzDiagScene::getBrushAtomicModel()
{
    return QBrush(QColor(245, 245, 245));
}

QBrush
VpzDiagScene::getBrushCoupledModel()
{
    return QBrush(Qt::white);
}

QPen
VpzDiagScene::getPenNotSelectedModel()
{
    return QPen(getColorModelNameNotSelected(), 3, Qt::SolidLine);
}
QPen
VpzDiagScene::getPenSelectedModel()
{
    return QPen(getColorModelNameSelected(), 3, Qt::SolidLine);
}

QPen
VpzDiagScene::getPenConnection()
{
    return QPen(QColor(127, 127, 127), 3, Qt::SolidLine);
}

QBrush
VpzDiagScene::getBrushBackground()
{
    return QBrush(QColor(240, 240, 240));
}

QFont
VpzDiagScene::getFontPort()
{
    return QFont("Cyrillic" /*"Greek"*/, 8, QFont::Normal, true);
}

QFont
VpzDiagScene::getFontModel()
{
    return QFont("Cyrillic", 9, QFont::DemiBold, false);
}

QColor
VpzDiagScene::getColorModelNameSelected()
{
    return QColor(255, 0, 0);
}

QColor
VpzDiagScene::getColorModelNameNotSelected()
{
    return QColor(0, 0, 255);
}

QString
VpzDiagScene::getXQueryOfTheSelectedModel()
{
    QList<VpzSubModelItem*> sels = mCoupled->getSelectedSubModels();
    if (sels.size() == 1) {
        QString res = mVpz->vdo()->getXQuery(sels.at(0)->mnode);
        return (res);
    } else {
        return "";
    }
}

VpzPortItem*
VpzDiagScene::getPort(const QString& portName, const QString& subMod)
{
    QList<QGraphicsItem*> children = mCoupled->childItems();
    if (subMod == "") {
        for (int i = 0; i < children.length(); i++) {
            if (VpzDiagScene::isVpzPort(children.at(i))) {
                VpzPortItem* portItem =
                  static_cast<VpzPortItem*>(children.at(i));
                if (portItem->getPortName() == portName) {
                    return portItem;
                }
            }
        }
    } else {
        for (int i = 0; i < children.length(); i++) {
            if (VpzDiagScene::isVpzSubModel(children.at(i))) {
                VpzSubModelItem* modItem =
                  static_cast<VpzSubModelItem*>(children.at(i));
                if (modItem->getModelName() == subMod) {
                    QList<QGraphicsItem*> childrenMod = modItem->childItems();
                    for (int j = 0; j < childrenMod.length(); j++) {
                        if (VpzDiagScene::isVpzPort(childrenMod.at(j))) {
                            VpzPortItem* portItem =
                              static_cast<VpzPortItem*>(childrenMod.at(j));
                            if (portItem->getPortName() == portName) {
                                return portItem;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

QStringList
VpzDiagScene::getSelectedModels()
{
    QStringList res;
    QList<VpzSubModelItem*> sels = mCoupled->getSelectedSubModels();
    for (int i = 0; i < sels.size(); i++) {
        res.append(mVpz->vdo()->getXQuery(sels.at(i)->mnode));
    }
    return res;
}
void
VpzDiagScene::unselectAllSubModels()
{
    QList<VpzSubModelItem*> sels = mCoupled->getSelectedSubModels();
    for (int j = 0; j < sels.length(); j++) {
        sels.at(j)->update();
        sels.at(j)->setSelected(false);
    }
    mCoupled->update();
}

void
VpzDiagScene::selectSubModel(const QString& nameSub)
{
    unselectAllSubModels();
    mCoupled->getSubModel(nameSub)->setSelected(true);
    mCoupled->getSubModel(nameSub)->update();
}

bool
VpzDiagScene::isVpzPort(QGraphicsItem* item)
{
    return item and ((item->type() == VpzPortTypeIn) or
                     (item->type() == VpzPortTypeOut));
}

bool
VpzDiagScene::isVpzInputPort(QGraphicsItem* item)
{
    return item and (item->type() == VpzPortTypeIn);
}

bool
VpzDiagScene::isVpzOutputPort(QGraphicsItem* item)
{
    return item and (item->type() == VpzPortTypeOut);
}

bool
VpzDiagScene::isVpzSubModel(QGraphicsItem* item)
{
    return item and (item->type() == VpzSubModelType);
}

bool
VpzDiagScene::isVpzConnectionLine(QGraphicsItem* item)
{
    return item and (item->type() == VpzConnectionLineType);
}

void
VpzDiagScene::setActionType(QAction* action, VPZ_DIAG_MENU_ACTION type)
{
    QVariantList dataList;
    dataList << type;
    action->setData(dataList);
}

VPZ_DIAG_MENU_ACTION
VpzDiagScene::getActionType(QAction* action)
{
    return static_cast<VPZ_DIAG_MENU_ACTION>(
      action->data().toList()[0].toInt());
}

bool
VpzDiagScene::isVpzSubModelConfigured(QGraphicsItem* item)
{
    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(item);
    return mVpz->isAtomicModelDynSet(it->mnode) ||
           mVpz->isAtomicModelCondsSet(it->mnode) ||
           mVpz->isAtomicModelObsSet(it->mnode) ||
           mVpz->hasModelInputPort(it->mnode) ||
           mVpz->hasModelOutputPort(it->mnode);
}

void
VpzDiagScene::onDragDestroyed(QObject* /*obj*/)
{
    delete mConnection;
    mConnection = 0;
}

void
VpzDiagScene::onUndoRedoVpz(QDomNode oldValVpz,
                            QDomNode newValVpz,
                            QDomNode /*oldValVpm*/,
                            QDomNode /*newValVpm*/)
{
    if (not mVpz) {
        return;
    }
    QString curr_query = mVpz->vdo()->getXQuery(mCoupled->mnode);
    // try to reinitialize with current node
    QDomNode cplNode =
      mVpz->vdo()->getNodeFromXQuery(mVpz->vdo()->getXQuery(mCoupled->mnode));
    if (cplNode.isNull()) {
        // try to reinitialize with merge newValVpz and current
        QString new_query = mVpz->vdo()->getXQuery(newValVpz);
        QString merged_query = mVpz->mergeQueries(new_query, curr_query);
        cplNode = mVpz->vdo()->getNodeFromXQuery(merged_query);
        if (cplNode.isNull()) {
            // try to reinitialize with missing model in oldValVpz
            QString old_query = mVpz->vdo()->getXQuery(oldValVpz);
            if (oldValVpz.nodeName() == "model") {
                old_query = old_query + "/submodels";
            }
            QDomNode root_oldValVpz =
              mVpz->vdo()->getNodeFromXQuery(old_query, oldValVpz);
            QList<QDomNode> oldMods =
              DomFunctions::childNodesWithoutText(root_oldValVpz, "model");
            QString subQuery = mVpz->subQuery(merged_query, 0, -1);
            QList<QDomNode> newMods = DomFunctions::childNodesWithoutText(
              mVpz->vdo()->getNodeFromXQuery(subQuery), "model");
            for (int i = 0; i < newMods.size(); i++) {
                if (not oldMods.contains(newMods.at(i))) {
                    cplNode = newMods.at(i);
                }
            }
            if (cplNode.isNull()) {
                // default got up to the first model
                bool stop = false;
                while (not stop) {
                    merged_query = mVpz->subQuery(merged_query, 0, -1);
                    if (merged_query == "") {
                        stop = true;
                    } else {
                        cplNode = mVpz->vdo()->getNodeFromXQuery(merged_query);
                        if (not cplNode.isNull() and
                            (cplNode.nodeName() == "model")) {
                            stop = true;
                        }
                    }
                }
            }
        }
    }
    if (cplNode.isNull()) {
        qDebug() << " Internal error in VpzDiagScene::onUndoRedoVpz";
    }
    setFocus(cplNode, true);
    emit initializationDone(this);
}

bool
VpzDiagScene::isVpzMainModel(QGraphicsItem* item)
{
    return item->type() == VpzMainModelType;
}
}
} // namespaces
