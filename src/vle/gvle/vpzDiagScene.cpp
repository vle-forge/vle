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

#include <QtGui>
#include <QGraphicsSceneMouseEvent>
#include <qpainterpath.h>

#include <QGraphicsView>
#include <QScrollBar>
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>
#include "vpzDiagScene.h"

namespace vle {
namespace gvle {

VpzConnectionLineItem::VpzConnectionLineItem(QDomNode node, vleVpm* vpm,
        QLineF l, QGraphicsItem* parent, VpzDiagScene* scene):
        QGraphicsItem(parent), mVpm(vpm), mnode(node)
{
    QGraphicsLineItem* it = new QGraphicsLineItem(l,this);
    it->setPen(scene->getPenConnection());
    update();
    it->update();
    it->show();
}

void
VpzConnectionLineItem::paint(QPainter * /*painter*/,
        const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
}

QRectF
VpzConnectionLineItem::boundingRect() const
{
    return childrenBoundingRect();
}

void
VpzConnectionLineItem::update(const QRectF & rect)
{
    childItems().at(0)->update(rect);
}

int
VpzConnectionLineItem::type() const
{
    return VpzConnectionLineType;
}

/*******************************************************************************/

VpzPortItem::VpzPortItem(QDomNode node, vleVpm* vpm, bool input,
        QGraphicsItem* parent, VpzDiagScene* scene):
        QGraphicsObject(parent), mVpm(vpm),
        mnode(node), mInput(input)
{
    this->setAcceptHoverEvents(true);

    QPixmap pix (":/icon/resources/vpz_inport.png");
    QGraphicsTextItem* textItem = new QGraphicsTextItem(getPortName(), this);
    textItem->setFont(scene->getFontPort());
//    textItem->setDefaultTextColor(Qt::black);
    //textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    //textItem->setTextInteractionFlags(Qt::TextEditable);
    textItem->setTextInteractionFlags(Qt::NoTextInteraction);

    new QGraphicsPixmapItem(pix, this);
    update();
}

QRectF
VpzPortItem::boundingRect() const
{
    return QRectF(0,0,
            getTextItem()->boundingRect().width() + getPixItem()->boundingRect().width(),
            getTextItem()->boundingRect().height());
}

void
VpzPortItem::update(const QRectF & /*rect*/)
{
    QGraphicsPixmapItem* pixItem = getPixItem();
    QGraphicsTextItem* textItem = getTextItem();
    if (mInput) {
        pixItem->setPos(0, textItem->boundingRect().height()/2
                - pixItem->boundingRect().height()/2.0);
        textItem->setPos(pixItem->boundingRect().width(), 0);
    } else {

        pixItem->setPos(textItem->boundingRect().width(),
                textItem->boundingRect().height()/2
                - pixItem->boundingRect().height()/2.0);
    }

}

void
VpzPortItem::paint(QPainter */*painter*/,
        const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{

    //for debug
    //painter->drawRect(boundingRect());
}

QPointF
VpzPortItem::getConnectionPoint()
{
    bool input = VpzDiagScene::isVpzInputPort(this);
    bool main = VpzDiagScene::isVpzMainModel(parentItem());
    QRectF br = boundingRect();
    if ((not input and not main) or (input and main)) {
        return scenePos() + QPointF(br.topRight().x() ,
                br.topRight().y() + br.height()/2.0);
    } else {
        return scenePos() + QPointF(br.topLeft().x(),
                br.topLeft().y() + br.height()/2.0);
    }
}

QString
VpzPortItem::getPortName()
{
    return mVpm->vdo()->attributeValue(mnode, "name");
}

QGraphicsTextItem*
VpzPortItem::getTextItem() const
{
    QList<QGraphicsItem*> chs = QGraphicsItem::childItems();
    for (int i =0; i<chs.length(); i++){
        if (chs.at(i)->type() == 8) {//8 is for QGraphicsTextItem
            return static_cast<QGraphicsTextItem*>(chs.at(i));
        }
    }
    return 0;
}


QGraphicsPixmapItem*
VpzPortItem::getPixItem() const
{
    QList<QGraphicsItem*> chs = QGraphicsItem::childItems();
    for (int i =0; i<chs.length(); i++){
        if (chs.at(i)->type() == 7) {//7 is for QGraphicsPixmapItem
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
        textItem->setFocus(Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true); // this ensures that itemChange() gets called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();

        if (getPortName() != textItem->toPlainText()) {
            mVpm->renameModelPort(mnode, getTextItem()->toPlainText());
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
VpzPortItem::hoverEnterEvent(QGraphicsSceneHoverEvent * /*evt*/)
{
    setCursor(Qt::PointingHandCursor);
}


/*******************************************************************************/

VpzModelItem::VpzModelItem(QDomNode node, vleVpm* vpm, QGraphicsItem* parent,
        QGraphicsScene* scene): QGraphicsItem(parent), mVpm(vpm),
                mnode(node), margin(5), rectWidth(0), rectHeight(0)
{

}

QGraphicsTextItem*
VpzModelItem::getTitle() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 8) {//8 for QGraphicsTextItem
            return static_cast<QGraphicsTextItem*>(children.at(i));
        }
    }
    return 0;
}


QGraphicsRectItem*
VpzModelItem::getRectangle() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 3) {//3 for QGraphicsRectItem
            return static_cast<QGraphicsRectItem*>(children.at(i));
        }
    }
    return 0;
}


QList<VpzPortItem*>
VpzModelItem::getInPorts()
{
    QList<VpzPortItem*> res;
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
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
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == VpzPortTypeOut) {
            res.append(static_cast<VpzPortItem*>(children.at(i)));
        }
    }
    return res;
}
VpzPortItem*
VpzModelItem::getInPort(const QString& name)
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == VpzPortTypeIn) {
            if (static_cast<VpzPortItem*>(children.at(i))->getPortName()
                    == name) {
                return static_cast<VpzPortItem*>(children.at(i));
            }
        }
    }
    return 0;
}
VpzPortItem*
VpzModelItem::getOutPort(const QString& name)
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == VpzPortTypeOut) {
            if (static_cast<VpzPortItem*>(children.at(i))->getPortName()
                    == name) {
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
     for (int i = 0; i<inPorts.length(); i++){
        scene()->removeItem(inPorts[i]);
        inPorts[i]->setParentItem(NULL);
        delete inPorts[i];

    }
    QList<VpzPortItem*> outPorts = getOutPorts();
     for (int i = 0; i<outPorts.length(); i++){
        scene()->removeItem(outPorts[i]);
        outPorts[i]->setParentItem(NULL);
        delete outPorts[i];
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
    return mVpm->vdo()->attributeValue(mnode, "name");
}

double
VpzModelItem::widthInputPorts()
{
    double maxv = 0;
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
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
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
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
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
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
        textItem->setFocus(Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true); // this ensures that itemChange() gets called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();
        if (getModelName() != textItem->toPlainText()) {
            mVpm->renameModel(mnode, textItem->toPlainText());
            update();
        }
    }
}


/*******************************************************************************/

VpzSubModelItem::VpzSubModelItem(QDomNode node, vleVpm* vpm, QGraphicsItem* parent,
        QGraphicsScene* scene): VpzModelItem(node, vpm, parent, scene)
{
    this->setAcceptHoverEvents(true);
    initializeFromDom();
    update();
}

VpzSubModelItem::~VpzSubModelItem()
{
    //clear();
}

QRectF
VpzSubModelItem::boundingRect() const
{
    return QRectF(0,0,rectWidth,rectHeight +
            getTitle()->boundingRect().height());
}

void
VpzSubModelItem::paint(QPainter */*painter*/, const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
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
    QGraphicsTextItem* it = new QGraphicsTextItem(getModelName(),
            this);
    it->setFont(vpzscene->getFontModel());

    QGraphicsRectItem * rec = new QGraphicsRectItem(QRectF(), this);
    if (mVpm->vdo()->attributeValue(mnode, "type") == "atomic") {
        rec->setBrush(vpzscene->getBrushAtomicModel());
    } else {
        rec->setBrush(vpzscene->getBrushCoupledModel());
    }
    rec->setPen(vpzscene->getPenNotSelectedModel());


    QDomNodeList chList = mnode.childNodes();
    //build port items and get max widths of items
    for (unsigned int j=0; j<chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "in") {
            QDomNodeList nChildIns = nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsIn = nChildIns.length();
            for (unsigned int k =0; k<nbPortsIn; k++) {
                QDomNode nChildIn = nChildIns.at(k);
                new VpzPortItem(nChildIn, mVpm, true, this, vpzscene);
            }
        }

    }
    for (unsigned int j=0; j<chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "out") {
            QDomNodeList nChildOuts = nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsOut = nChildOuts.length();
            for (unsigned int k =0; k<nbPortsOut; k++) {
                QDomNode nChildOut = nChildOuts.at(k);
                new VpzPortItem(nChildOut, mVpm, false, this, vpzscene);
            }
        }
    }

}

void
VpzSubModelItem::update(const QRectF & /*rect*/)
{

    //compute rect width and height
    //->initialize with minimal sizes constrained by ports and names

    VpzDiagScene* vpzscene = static_cast<VpzDiagScene*>(scene());

    double w = mVpm->vdo()->attributeValue(mnode, "width").toDouble();
    double h = mVpm->vdo()->attributeValue(mnode, "height").toDouble();
    rectWidth = std::max(getTitle()->boundingRect().width(),
                std::max(2*margin + widthInputPorts() + widthOutputPorts(),
                        w));
    double nbPortMax = std::max(getInPorts().size(),
                                getOutPorts().size());


    rectHeight = std::max((nbPortMax+1)*margin + nbPortMax*heightPort(),
                          h);
    //set localization of ports
    for (int i=0; i<getInPorts().length(); i++) {
        getInPorts().at(i)->setPos(QPointF(0,
                margin + ((double) i)*(margin + heightPort())));
    }
    for (int i=0; i<getOutPorts().length(); i++) {
        getOutPorts().at(i)->setPos(QPointF(
                rectWidth -getOutPorts()[i]->boundingRect().width(),
                margin + ((double) i)*(margin + heightPort())));
    }

    getTitle()->setPos(0,rectHeight);
    QGraphicsRectItem* rect = getRectangle();
    rect->setRect(0,0,rectWidth, rectHeight);
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
VpzSubModelItem::hoverMoveEvent(QGraphicsSceneHoverEvent * evt)
{
    switch(getSelType(mapToParent(evt->pos()))) {
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
VpzSubModelItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsItem::mouseDoubleClickEvent(mouseEvent);
}
void
VpzSubModelItem::mouseMoveEvent(QGraphicsSceneMouseEvent * /*mouseEvent*/)
{
//    QGraphicsItem::mouseMoveEvent(mouseEvent);
//    double wip = 0;
//    wip = static_cast<VpzMainModelItem*>(parentItem())->widthInputPorts();
//    setX(std::max(x(),wip +margin));
//    setY(std::max(y(), margin));
//    this->parentItem()->update();
}
void
VpzSubModelItem::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{

    QGraphicsItem::mousePressEvent(mouseEvent);
}
void
VpzSubModelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsItem::mouseReleaseEvent(mouseEvent);
}

SEL_TYPE
VpzSubModelItem::getSelType(QPointF selPoint)
{
    QPointF p = mapFromParent(selPoint);
    if (p.x() < margin and p.y() < rectHeight + margin) {
        return LEFT;
    }
    if (p.y() < margin) {
        return TOP;
    }
    if (p.x() > rectWidth - margin and p.y() < rectHeight + margin) {
        return RIGHT;
    }
    if (p.y() > rectHeight - margin and p.y() < rectHeight + margin) {
        return BOTTOM;
    }
    return MIDDLE;
}

QString
VpzSubModelItem::getModelName()
{
    return mVpm->vdo()->attributeValue(mnode, "name");
}

int
VpzSubModelItem::type() const
{
    return VpzSubModelType;

}

void
VpzSubModelItem::removeNameEditionMode()
{

    QList<QGraphicsItem*>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (VpzDiagScene::isVpzPort(children.at(i))) {
            static_cast<VpzPortItem*>(children.at(i))->setNameEdition(false);
        }
    }
    setNameEdition(false);
}

/****************************************************************************/
VpzMainModelItem::VpzMainModelItem(QDomNode node, vleVpm* vpm, QGraphicsItem* parent,
        QGraphicsScene* scene):  VpzModelItem(node, vpm, parent, scene)
{
    //this->set
    initializeFromDom();
    update();
}

QRectF
VpzMainModelItem::boundingRect() const
{
    return QRectF(0,0,rectWidth,
            rectHeight + getTitle()->boundingRect().height());
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

    QGraphicsRectItem * rec = new QGraphicsRectItem(QRectF(), this);
    rec->setPen(vpzscene->getPenNotSelectedModel());
    if (mVpm->vdo()->attributeValue(mnode, "type") == "atomic") {
        rec->setBrush(vpzscene->getBrushAtomicModel());
    } else {
        rec->setBrush(vpzscene->getBrushCoupledModel());
    }
    rec->setFlag(QGraphicsItem::ItemIsMovable, false);
    rec->setFlag(QGraphicsItem::ItemIsSelectable, false);

    //add port items
    QDomNodeList chList = mnode.childNodes();
    for (unsigned int j=0; j<chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "in") {
            QDomNodeList nChildIns = nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsIn = nChildIns.length();
            for (unsigned int k =0; k<nbPortsIn; k++) {
                QDomNode nChildIn = nChildIns.at(k);
                new VpzPortItem(nChildIn, mVpm, true, this, vpzscene);
            }
        }
    }
    for (unsigned int j=0; j<chList.length(); j++) {
        QDomNode nChild = chList.at(j);
        if (nChild.nodeName() == "out") {
            QDomNodeList nChildOuts = nChild.toElement().elementsByTagName("port");
            unsigned int nbPortsOut = nChildOuts.length();
            for (unsigned int k =0; k<nbPortsOut; k++) {
                QDomNode nChildOut = nChildOuts.at(k);
                new VpzPortItem(nChildOut, mVpm, false, this, vpzscene);
            }
        }
    }
    //add submodels
    double wports = widthInputPorts();
    QList<QDomNode> childList = mVpm->submodelsFromModel(mnode);
    for (int i =0; i<childList.size(); i++) {
        QDomNode& n = childList[i];
        double x,y;
        x = QVariant(mVpm->vdo()->attributeValue(n, "x")).toDouble();
        y = QVariant(mVpm->vdo()->attributeValue(n, "y")).toDouble();
        VpzSubModelItem* mod = new VpzSubModelItem(n, mVpm, this, vpzscene);
        mod->setPos(QPointF(std::max(wports+margin,x),
                std::max(0.0,y)));
    }
    QGraphicsTextItem* it = new QGraphicsTextItem(getModelName(), this);
    it->setFont(vpzscene->getFontModel());
    it->setDefaultTextColor(vpzscene->getColorModelNameNotSelected());
    update();
    addConnLines();
}

void
VpzMainModelItem::clearLines()
{
    QList<VpzConnectionLineItem *> lines = getConnLines();
    for (int i =0; i<lines.length() ; i++) {
        scene()->removeItem(lines[i]);
        lines[i]->setParentItem(NULL);
        delete lines[i];
    }
}

void
VpzMainModelItem::clearSubModels()
{
    QList<VpzSubModelItem *> subs = getSubModels();
    for (int i =0; i<subs.length() ; i++) {
        scene()->removeItem(subs[i]);
        subs[i]->setParentItem(NULL);
        delete subs[i];
    }
}

void
VpzMainModelItem::addConnLines()
{
    //Add connections
    QList<QDomNode> childList = mVpm->childNodesWithoutText(
            mVpm->connectionsFromModel(mnode));

    VpzDiagScene* vpzscene = static_cast<VpzDiagScene*>(this->scene());

    for (int i =0; i<childList.size(); i++) {
        QDomNode conn = childList[i];
        if (conn.nodeName() == "connection") {
            VpzPortItem* ap = 0;
            VpzPortItem* bp = 0;


            QDomNode orig = conn.toElement().elementsByTagName("origin").at(0);
            QDomNode dest =
                    conn.toElement().elementsByTagName("destination").at(0);
            if (mVpm->vdo()->attributeValue(conn, "type") == "output") {
                //            VpzSubModelItem* origMod = getSubModel(
                //                    mVpm->attributeValue(orig, "model"));
                ap = getSubModel(mVpm->vdo()->attributeValue(orig, "model"))
                            ->getOutPort(mVpm->vdo()->attributeValue(orig, "port"));
                bp = getOutPort(mVpm->vdo()->attributeValue(dest, "port"));
            } else if (mVpm->vdo()->attributeValue(conn, "type") == "internal") {
                ap = getSubModel(mVpm->vdo()->attributeValue(orig, "model"))
                            ->getOutPort(mVpm->vdo()->attributeValue(orig, "port"));
                bp = getSubModel(mVpm->vdo()->attributeValue(dest, "model"))
                            ->getInPort(mVpm->vdo()->attributeValue(dest, "port"));
            } else if (mVpm->vdo()->attributeValue(conn, "type") == "input") {
                ap = getInPort(mVpm->vdo()->attributeValue(orig, "port"));
                bp = getSubModel(mVpm->vdo()->attributeValue(dest, "model"))
                            ->getInPort(mVpm->vdo()->attributeValue(dest, "port"));
            }
            if (ap and bp) {//paint connections
                QPointF a = ap->getConnectionPoint();
                QPointF b = bp->getConnectionPoint();
                if (a.x() >= b.x()) {
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(a,
                                    QPointF(a.x()+2*margin,a.y())),
                                    this,vpzscene);
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(QPointF(a.x()+2*margin,a.y()),
                                    QPointF(a.x()+2*margin,(a.y()+b.y())/2.0)),
                                    this,vpzscene);
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(QPointF(a.x()+2*margin,(a.y()+b.y())/2.0),
                                    QPointF(b.x()-2*margin,(a.y()+b.y())/2.0)),
                                    this,vpzscene);
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(QPointF(b.x()-2*margin,(a.y()+b.y())/2.0),
                                    QPointF(b.x()-2*margin,b.y())),
                                    this,vpzscene);
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(QPointF(b.x()-2*margin,b.y()),
                                    b),
                                    this,vpzscene);

                } else {
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(a,
                                    QPointF((a.x()+b.x())/2.0,a.y())),
                                    this,vpzscene);
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(QPointF((a.x()+b.x())/2.0,a.y()),
                                    QPointF((a.x()+b.x())/2.0,b.y())),
                                    this,vpzscene);
                    new VpzConnectionLineItem(conn, mVpm,
                            QLineF(QPointF((a.x()+b.x())/2.0,b.y()),
                                    b),
                                    this,vpzscene);
                }
            }
        }
    }
}
void
VpzMainModelItem::update(const QRectF & /*rect*/)
{

    setSelected(false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, false);

    //compute rect width and height
    //->initialize with minimal sizes constrained by ports and names
    QRectF subModsRect = subModelsBoundingRect(false);
    rectWidth = std::max(std::max(std::max(
            200.0,
            getTitle()->boundingRect().width()),
            subModsRect.x()+subModsRect.width()+widthOutputPorts()+2*margin),
            widthInputPorts()+widthOutputPorts()+2*margin);
    double nbPortMax = std::max(getInPorts().size(),
                                getOutPorts().size());

    rectHeight = std::max(std::max(
            200.0,
            (nbPortMax+1)*margin + nbPortMax*heightPort()),
            subModsRect.y() + subModsRect.height()+2*margin);

    //set localization of ports
    for (int i=0; i<getInPorts().length(); i++) {
        getInPorts().at(i)->setPos(QPointF(0,
                margin + ((double) i)*(margin + heightPort())));
    }
    for (int i=0; i<getOutPorts().length(); i++) {
        getOutPorts().at(i)->setPos(QPointF(
                rectWidth-getOutPorts()[i]->boundingRect().width(),
                margin + ((double) i)*(margin + heightPort())));
    }

    //move to x,y
    QList<VpzSubModelItem *> sub = getSubModels();
    for (int i=0; i<sub.length(); i++) {
        sub[i]->setPos(QPointF(std::max(widthInputPorts()+margin,
                mVpm->vdo()->attributeValue(sub[i]->mnode, "x").toDouble()),
                std::max(0.0,
                        mVpm->vdo()->attributeValue(sub[i]->mnode, "y").toDouble())));
        sub[i]->update();
    }

    getTitle()->setPos(0,rectHeight);
    getRectangle()->setRect(0,0,rectWidth, rectHeight);
    clearLines();
    addConnLines();
}



void
VpzMainModelItem::paint(QPainter */*painter*/,
        const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{

}

void
VpzMainModelItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{

    QGraphicsItem::mouseDoubleClickEvent(mouseEvent);
}
void
VpzMainModelItem::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsItem::mouseMoveEvent(mouseEvent);
}
void
VpzMainModelItem::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsItem::mousePressEvent(mouseEvent);
}
void
VpzMainModelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsItem::mouseReleaseEvent(mouseEvent);
}


void
VpzMainModelItem::dragEnterEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}
void
VpzMainModelItem::dragLeaveEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}
void
VpzMainModelItem::dragMoveEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}


QRectF
VpzMainModelItem::subModelsBoundingRect(bool onlySelected)
{
    QRectF res(0,0,0,0);
    bool first = true;
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
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
                    res.setWidth(std::max(
                            it->x()-res.x()+it->boundingRect().width(),
                            res.width()));
                    res.setHeight(std::max(
                            it->y()-res.y()+it->boundingRect().height(),
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
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (VpzDiagScene::isVpzSubModel(children.at(i))) {
            VpzSubModelItem* modItem = static_cast<VpzSubModelItem*>(children.at(i));
            if (modItem->getModelName() == subMod) {
                return modItem;
            }
        }
    }
    return 0;
}

QList<VpzSubModelItem *>
VpzMainModelItem::getSubModels()
{
    QList<VpzSubModelItem*> res;
    QList<QGraphicsItem*>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (VpzDiagScene::isVpzSubModel(children.at(i))) {
            res.append(static_cast<VpzSubModelItem*>(children.at(i)));
        }
    }
    return res;
}


QList<VpzSubModelItem *>
VpzMainModelItem::getSelectedSubModels()
{
    QList<VpzSubModelItem*> res;
    QList<QGraphicsItem*>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        QGraphicsItem* ch = children.at(i);
        if (VpzDiagScene::isVpzSubModel(ch) and ch->isSelected()) {
            res.append(static_cast<VpzSubModelItem*>(ch));
        }
    }
    return res;
}

QList<VpzConnectionLineItem *>
VpzMainModelItem::getConnLines()
{
    QList<VpzConnectionLineItem*> res;
    QList<QGraphicsItem*>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (VpzDiagScene::isVpzConnectionLine(children.at(i))) {
            res.append(static_cast<VpzConnectionLineItem*>(children.at(i)));
        }
    }
    return res;
}

void
VpzMainModelItem::removeNameEditionMode()
{

    QList<QGraphicsItem*>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)) {
            if (VpzDiagScene::isVpzPort(children.at(i))) {
                static_cast<VpzPortItem*>(children.at(i))->setNameEdition(false);
            } else if (VpzDiagScene::isVpzSubModel(children.at(i))) {
                static_cast<VpzSubModelItem*>(children.at(i))->removeNameEditionMode();
            }
        }
    }
    setNameEdition(false);
}

bool
VpzMainModelItem::isAtomic()
{
    return (mVpm->vdo()->attributeValue(mnode,"type") == "atomic");
}

int
VpzMainModelItem::type() const
{
    return VpzMainModelType;

}

/*********************************************************************************/

VpzDiagScene::VpzDiagScene() :
        QGraphicsScene(), mVpm(0), mCoupled(0), mPortSel1(0), mPortSel2(0),
        mConnection(0), mDragStartPoint(), mDragCurrPoint(),
        mModelSelType(MIDDLE), mIsEnteringCoupled(false)
{
    setBackgroundBrush(getBrushBackground());
}



void
VpzDiagScene::init(vleVpm* vpm, const QString& className)
{
    clear();
    mVpm = vpm;
    QDomNode selModelNode = mVpm->classModelFromDoc(className);

    setFocus(selModelNode);
    QObject::connect(mVpm,
            SIGNAL(undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)),
            this,
            SLOT(onUndoRedoVpm(QDomNode, QDomNode, QDomNode, QDomNode)));

    emit initializationDone(this);

}

void
VpzDiagScene::setFocus(QDomNode selModelNode)
{
    if (mCoupled) {
        delete mCoupled;
    }
    mCoupled = new VpzMainModelItem(selModelNode, mVpm, 0, this);
    addItem(mCoupled);
    mCoupled->setPos(QPointF(0,0));
    mCoupled->update();
    unselectAllSubModels();

    emit enterCoupledModel(mCoupled->mnode);
}

void
VpzDiagScene::clear()
{
    mVpm = 0;
    delete mCoupled;
    mCoupled = 0;
    mPortSel1 = 0;
    mPortSel2 = 0;
    mConnection = 0;
    mDragStartPoint = QPointF();
    mDragCurrPoint = QPointF();
    mModelSelType = MIDDLE;
    mIsEnteringCoupled = false;

    emit initializationDone(this);
}

void
VpzDiagScene::update(const QRectF & /*rect*/)
{
    if (views().size() > 0) {
        views()[0]->verticalScrollBar()->setValue(
                views()[0]->verticalScrollBar()->minimum());
        views()[0]->horizontalScrollBar()->setValue(
                views()[0]->horizontalScrollBar()->minimum());
        mCoupled->update();
    }
}

void
VpzDiagScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QList<QGraphicsItem*> items = this->items(mouseEvent->scenePos());
    //if a port is selected do nothing
    for (int i =0; i< items.length(); i++) {
        if (VpzDiagScene::isVpzPort(items[i])) {
            return;
        }
    }
    //try enter a coupled submodel
    for (int i =0; i< items.length(); i++) {
        if (VpzDiagScene::isVpzSubModel(items[i])) {
            VpzSubModelItem* subMod = static_cast<VpzSubModelItem*>(items[i]);
            SEL_TYPE seType = subMod->getSelType(mouseEvent->scenePos());
            if ((seType == MIDDLE) and
                (mVpm->vdo()->attributeValue(subMod->mnode, "type") == "coupled")) {
                setFocus(subMod->mnode);
                mIsEnteringCoupled = true;
                return;
            }
        }
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}


void
VpzDiagScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (not mCoupled) {
        mouseEvent->ignore();
        return ;
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    mCoupled->update();
}
void
VpzDiagScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (not mCoupled) {
        mouseEvent->ignore();
        return ;
    }
    mPortSel1 = 0;
    if (mouseEvent->button() == Qt::LeftButton) {
        QList<QGraphicsItem*> items = this->items(mouseEvent->scenePos());
        //Look for a drag for connection from a VpzPortItem
        for (int i =0; i< items.length(); i++) {
            if (VpzDiagScene::isVpzPort(items[i])){
                unselectAllSubModels();
                VpzPortItem* itemPort = static_cast<VpzPortItem*>(items[i]);
                mPortSel1 = itemPort;
                QDrag* drag = new QDrag(mouseEvent->widget());
                drag->setMimeData(new QMimeData);
                QObject::connect(drag, SIGNAL(destroyed(QObject *)),
                                 this, SLOT  (onDragDestroyed(QObject *)));
                drag->start();
                return;
            }
        }
        mCoupled->removeNameEditionMode();
        //look for VpzModel selection
        for (int i =0; i< items.length(); i++) {
            if (VpzDiagScene::isVpzSubModel(items[i])) {
                VpzSubModelItem* mod = static_cast<VpzSubModelItem*>(items[i]);
                //look for incr selection of VpzSubModels with Ctrl touch
                if (mouseEvent->modifiers() == Qt::ControlModifier) {
                    mod->setSelected(not mod->isSelected());
                    return;
                } else {
                    if (not mod->isSelected()) {
                        unselectAllSubModels();
                        mod->setSelected(true);
                    }

                    //Start drag event
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
        //otherwise unselect all
        unselectAllSubModels();
        mCoupled->update();
    }
    //otherwise performs classical mousePressEvent, ie move item.
    //QGraphicsScene::mousePressEvent(mouseEvent);

}

void
VpzDiagScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (not mCoupled) {
        mouseEvent->ignore();
        return ;
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
    update(this->sceneRect());
}


void
VpzDiagScene::dragEnterEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
    //QGraphicsScene::dragEnterEvent(event);
}
void
VpzDiagScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}
void
VpzDiagScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    QPointF prevDragPoint = mDragCurrPoint;
    mDragCurrPoint = event->scenePos();

    if (mPortSel1) {
        if (mConnection) {
            mConnection->setLine(QLineF(
                    mConnection->line().p1(), event->scenePos()));
            mConnection->update();
        } else {
            mConnection = new QGraphicsLineItem(QLineF(
                    mPortSel1->getConnectionPoint(),
                    event->scenePos()),0);
        }
        return;
    }

    QList<VpzSubModelItem*> sels = mCoupled->getSelectedSubModels();
    if (sels.size() == 1) {
        //move VpzSubModel, and resize capabilities
        VpzSubModelItem* mod = sels.at(0);
        QPointF p = event->scenePos();
        double oldx = mod->pos().x();
        double oldy = mod->pos().y();
        switch (mModelSelType){
        case RIGHT: {
            double oldwidth = mod->boundingRect().width();
            double ri =  p.x() - (oldx + oldwidth);
            double newwidth = oldwidth  + ri;
            mVpm->setWidthToModel(mod->mnode, newwidth);
            mod->update();
            mCoupled->update();
            break;
        } case BOTTOM: {
            double oldheight = mod->boundingRect().height();
            double bi =  p.y() - (oldy + oldheight);
            double newheight = oldheight  + bi;
            mVpm->setHeightToModel(mod->mnode, newheight);
            mod->update();
            mCoupled->update();
            break;
        } case MIDDLE: {
            double x = std::max(mCoupled->widthInputPorts() + mCoupled->margin,
                    mod->pos().x()+mDragCurrPoint.x() - prevDragPoint.x());
            double y = std::max(mCoupled->margin,
                    mod->pos().y()+mDragCurrPoint.y() - prevDragPoint.y());
            if ((mDragCurrPoint.x() != prevDragPoint.x()) or
                    (mDragCurrPoint.y() != prevDragPoint.y())) {
                //avoid a modif on a doubleClicked
                mVpm->setPositionToModel(mod->mnode, x, y);
            }
            break;
        } default:
            break;
        }
        mod->update();
        mCoupled->update();
    } else {
        QRectF r = mCoupled->subModelsBoundingRect(true);
        double deltaX = std::max(
                (mCoupled->widthInputPorts() + mCoupled->margin) - r.x(),
                mDragCurrPoint.x() - prevDragPoint.x());
        double deltaY = std::max(
                mCoupled->margin -r.y(),
                mDragCurrPoint.y() - prevDragPoint.y());

        //move VpzSubModel only
        QList<QDomNode> modsToMove;
        QList<double> newXs;
        QList<double> newYs;

        for (int i =0;i<sels.length();i++) {
            VpzSubModelItem* mod = sels.at(i);
            double x = mod->pos().x()+deltaX;
            double y = mod->pos().y()+deltaY;
            modsToMove.append(mod->mnode);
            newXs.append(x);
            newYs.append(y);
            mVpm->setAttributeValue(mod->mnode.toElement(), "x",
                    QVariant(x).toString());
            mVpm->setAttributeValue(mod->mnode.toElement(), "y",
                    QVariant(y).toString());
        }
        mVpm->setPositionToModel(modsToMove, newXs, newYs);
        mCoupled->update();
    }
}

void
VpzDiagScene::dropEvent(QGraphicsSceneDragDropEvent * event)
{
    if (mPortSel1) {
        QList<QGraphicsItem*> items = this->items(event->scenePos());
        //look for a second port for a connection
        bool found = false;
        for (int i =0; i< items.length(); i++) {

            if ((not found) and (items[i] != mPortSel1)
                    and (VpzDiagScene::isVpzPort(items[i]))){
                VpzPortItem* itemPort = static_cast<VpzPortItem*>(items[i]);
                mPortSel2 = itemPort;
                mVpm->addModelConnection(mPortSel1->mnode, mPortSel2->mnode);
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
VpzDiagScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    mPortSel1 = 0;
    mPortSel2 = 0;

    QList<QGraphicsItem*> sels = items(event->scenePos());
    QGraphicsItem* sel = 0;

    for (int i=0; i<sels.length(); i++) {
        if (isVpzPort(sels.at(i))) {
            sel = sels.at(i);
        } else if (isVpzConnectionLine(sels.at(i))) {
            sel = sels.at(i);
        } else if (isVpzSubModel(sels.at(i)) and
                not isVpzSubModel(sel) and
                not isVpzPort(sel) and
                not isVpzConnectionLine(sel)) {
            sel = sels.at(i);
        } else if (isVpzMainModel(sels.at(i)) and
                not isVpzSubModel(sel) and
                not isVpzPort(sel) and
                not isVpzConnectionLine(sel)) {
            sel = sels.at(i);
        }
    }

    QList<VpzSubModelItem*> selMods = mCoupled->getSelectedSubModels();
    QMenu menu;
    QAction* action;
    action = menu.addAction("Copy models");
    action->setData(VDMA_copy_models);
    action->setEnabled(
            (sel and isVpzSubModel(sel) and (selMods.size() >= 1)
                 and (selMods.contains(static_cast<VpzSubModelItem*>(sel))))
         or (sel and isVpzSubModel(sel) and (selMods.size() == 0)));
    action = menu.addAction("Paste models");
    action->setData(VDMA_paste_models);
    action->setEnabled(sel and isVpzMainModel(sel) and not
            (static_cast<VpzMainModelItem*>(sel)->isAtomic()));
    menu.addSeparator();
    action = menu.addAction("Edit name");
    action->setData(VDMA_Edit_name);
    action->setEnabled((sel and isVpzPort(sel))
            or (sel and isVpzSubModel(sel) and (selMods.size() == 1)
                    and (selMods.contains(static_cast<VpzSubModelItem*>(sel))))
            or (sel and isVpzSubModel(sel) and (selMods.size() == 0))
            or (sel and isVpzMainModel(sel)));
    action = menu.addAction("Remove");
    action->setData(VDMA_Remove);
    action->setEnabled((sel and isVpzPort(sel))
            or (sel and isVpzSubModel(sel) and (selMods.size() >= 1)
                    and (selMods.contains(static_cast<VpzSubModelItem*>(sel))))
            or (sel and isVpzSubModel(sel) and (selMods.size() == 0))
            or (sel and isVpzConnectionLine(sel)));
    menu.addSeparator();
    action = menu.addAction("Add input port");
    action->setData(VDMA_Add_input_port);
    action->setEnabled(
            (sel and isVpzSubModel(sel) and (selMods.size() == 1)
                 and (selMods.contains(static_cast<VpzSubModelItem*>(sel))))
            or (sel and isVpzSubModel(sel) and (selMods.size() == 0))
            or (sel and isVpzMainModel(sel)));
    action = menu.addAction("Add output port");
    menu.addSeparator();
    action->setData(VDMA_Add_output_port);
    action->setEnabled(
                (sel and isVpzSubModel(sel) and (selMods.size() == 1)
                     and (selMods.contains(static_cast<VpzSubModelItem*>(sel))))
                or (sel and isVpzSubModel(sel) and (selMods.size() == 0))
                or (sel and isVpzMainModel(sel)));
    action = menu.addAction("Add atomic model");
    action->setData(VDMA_Add_atomic_model);
    action->setEnabled(sel and isVpzMainModel(sel) and not
                       (static_cast<VpzMainModelItem*>(sel)->isAtomic()));
    action = menu.addAction("Add coupled port");
    action->setData(VDMA_Add_coupled_model);
    action->setEnabled(sel and isVpzMainModel(sel) and not
                       (static_cast<VpzMainModelItem*>(sel)->isAtomic()));

    if (sel) {
        action = menu.exec(event->screenPos());
        if (action) {
            int actCode = action->data().toInt();
            if (actCode == VDMA_copy_models) {
                mModelsCopies.clear();
                for (int i =0; i< selMods.size(); i++) {
                    mModelsCopies.append(selMods.at(i)->mnode);
                }
            } else if (actCode == VDMA_paste_models) {

                if (mModelsCopies.size() > 0) {
                    double x = mVpm->vdo()->attributeValue(
                            mModelsCopies[0], "x").toDouble();
                    double y = mVpm->vdo()->attributeValue(
                            mModelsCopies[0], "y").toDouble();
                    for (int i =1; i<mModelsCopies.size(); i++) {
                        x = std::min(x, mVpm->vdo()->attributeValue(
                                mModelsCopies[i], "x").toDouble());
                        y = std::min(y, mVpm->vdo()->attributeValue(
                                mModelsCopies[i], "y").toDouble());
                    }
                    QPointF pcopies(x, y);
                    qreal xtranslation = event->scenePos().x() - pcopies.x();
                    qreal ytranslation = event->scenePos().y() - pcopies.y();
                    mVpm->copyModelsToModel(mModelsCopies, mCoupled->mnode,
                            xtranslation, ytranslation);
                    mCoupled->initializeFromDom();
                }
            } else if (actCode == VDMA_Edit_name) {
                if (isVpzPort(sel)) {
                    VpzPortItem* it = static_cast<VpzPortItem*>(sel);
                    it->setNameEdition(true);
                } else if(isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    it->setNameEdition(true);
                } else if(isVpzMainModel(sel)) {
                    VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                    it->setNameEdition(true);
                }
            } else if (actCode == VDMA_Remove) {
                if (isVpzPort(sel)) {
                    VpzPortItem* it = static_cast<VpzPortItem*>(sel);
                    mVpm->rmModelPort(it->mnode);
                    removeItem(it);
                    it->setParentItem(NULL);
                    delete it;
                    mCoupled->update();
                } else if(isVpzConnectionLine(sel)){
                    VpzConnectionLineItem* it =
                            static_cast<VpzConnectionLineItem*>(sel);
                    mVpm->rmModelConnection(it->mnode);
                    mCoupled->update();
                } else if(isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    if (selMods.contains(it)) {
                        for (int i =0; i< selMods.size(); i++) {
                            it = selMods.at(i);
                            it->setSelected(false);
                            mVpm->rmModel(it->mnode);
                            removeItem(it);
                            delete it;
                        }
                    } else {
                        it->setSelected(false);
                        mVpm->rmModel(it->mnode);
                        removeItem(it);
                        delete it;
                    }
                    mCoupled->update();
                }
            } else if (actCode == VDMA_Add_input_port) {
                if (isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    mVpm->addModelInputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                } else if(isVpzMainModel(sel)) {
                    VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                    mVpm->addModelInputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                }
            } else if(actCode == VDMA_Add_output_port) {
                if (isVpzSubModel(sel)) {
                    VpzSubModelItem* it = static_cast<VpzSubModelItem*>(sel);
                    mVpm->addModelOutputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                } else if(isVpzMainModel(sel)){

                    VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                    mVpm->addModelOutputPort(it->mnode);
                    it->initializeFromDom();
                    it->update();
                    mCoupled->update();
                }
            } else if(actCode == VDMA_Add_atomic_model) {
                VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                mVpm->addModel(it->mnode, "atomic", event->scenePos());
                it->initializeFromDom();
                it->update();
            } else if(actCode == VDMA_Add_coupled_model) {
                VpzMainModelItem* it = static_cast<VpzMainModelItem*>(sel);
                mVpm->addModel(it->mnode, "coupled", event->scenePos());
                it->initializeFromDom();
                it->update();
            }
        }
    }


    //QGraphicsScene::contextMenuEvent(event);
}

QBrush
VpzDiagScene::getBrushAtomicModel()
{
    return QBrush(QColor(245,245,245));
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
    return QPen(QColor(127,127,127), 3, Qt::SolidLine);
}

QBrush
VpzDiagScene::getBrushBackground()
{
    return QBrush(QColor(240,240,240));
}

QFont
VpzDiagScene::getFontPort()
{
    return QFont("Cyrillic"/*"Greek"*/,8,QFont::Normal,true);
}

QFont
VpzDiagScene::getFontModel()
{
    return QFont("Cyrillic",9,QFont::DemiBold,false);
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
    QList<VpzSubModelItem *> sels = mCoupled->getSelectedSubModels();
    if (sels.size() == 1) {
        QString res =  mVpm->vdo()->getXQuery(sels.at(0)->mnode);
        return (res);
    } else {
        return "";
    }

}

VpzPortItem*
VpzDiagScene::getPort(const QString& portName, const QString& subMod)
{
    QList<QGraphicsItem *>  children = mCoupled->childItems();
    if (subMod == "") {
        for (int i =0; i<children.length(); i++) {
            if (VpzDiagScene::isVpzPort(children.at(i))) {
                VpzPortItem* portItem =
                        static_cast<VpzPortItem*>(children.at(i));
                if (portItem->getPortName() == portName) {
                    return portItem;
                }
            }
        }
    } else {
        for (int i =0; i<children.length(); i++) {
            if (VpzDiagScene::isVpzSubModel(children.at(i))) {
                VpzSubModelItem* modItem = static_cast<VpzSubModelItem*>(children.at(i));
                if (modItem->getModelName() == subMod) {
                    QList<QGraphicsItem *>  childrenMod = modItem->childItems();
                    for (int j =0; j<childrenMod.length(); j++) {
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
    for (int i=0; i<sels.size();i++) {
        res.append(mVpm->vdo()->getXQuery(sels.at(i)->mnode));
    }
    return res;
}
void
VpzDiagScene::unselectAllSubModels()
{
    QList<VpzSubModelItem *> sels = mCoupled->getSelectedSubModels();
    for (int j =0; j< sels.length(); j++) {
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
}

bool
VpzDiagScene::isVpzPort(QGraphicsItem* item)
{
    return item and ((item->type() == VpzPortTypeIn)
                     or (item->type() == VpzPortTypeOut));
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
VpzDiagScene::onDragDestroyed(QObject */*obj*/)
{
    delete mConnection;
    mConnection = 0;
}

void
VpzDiagScene::onUndoRedoVpm(QDomNode oldValVpz, QDomNode newValVpz,
        QDomNode /*oldValVpm*/, QDomNode /*newValVpm*/)
{

    QString curr_query = mVpm->vdo()->getXQuery(mCoupled->mnode);
    //try to reinitialize with current node
    QDomNode cplNode = mVpm->vdo()->getNodeFromXQuery(
            mVpm->vdo()->getXQuery(mCoupled->mnode));
    if (cplNode.isNull()) {
        //try to reinitialize with merge newValVpz and current
        QString new_query = mVpm->vdo()->getXQuery(newValVpz);
        QString merged_query = mVpm->mergeQueries(new_query, curr_query);
        cplNode = mVpm->vdo()->getNodeFromXQuery(merged_query);
        if (cplNode.isNull()) {
            //try to reinitialize with missing model in oldValVpz
            QString old_query = mVpm->vdo()->getXQuery(oldValVpz);
            if (oldValVpz.nodeName() == "model") {
                old_query = old_query +"/submodels";
            }
            QDomNode root_oldValVpz = mVpm->vdo()->getNodeFromXQuery(old_query,
                    oldValVpz);
            QList<QDomNode> oldMods =
                    mVpm->childNodesWithoutText(root_oldValVpz, "model");
            QString subQuery = mVpm->subQuery(merged_query, 0,-1);
            QList<QDomNode> newMods = mVpm->childNodesWithoutText(
                    mVpm->vdo()->getNodeFromXQuery(subQuery), "model");
            for (int i =0; i< newMods.size(); i++) {
                if (not oldMods.contains(newMods.at(i))) {
                    cplNode = newMods.at(i);
                }
            }
            if (cplNode.isNull()) {
                //default got up to the first model
                bool stop = false;
                while (not stop) {
                    merged_query = mVpm->subQuery(merged_query, 0, -1);
                    if (merged_query == "") {
                        stop = true;
                    } else {
                        cplNode = mVpm->vdo()->getNodeFromXQuery(merged_query);
                        if (not cplNode.isNull()
                                and (cplNode.nodeName() == "model")){
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
    setFocus(cplNode);
    emit initializationDone(this);
}

bool
VpzDiagScene::isVpzMainModel(QGraphicsItem* item)
{
    return item->type() == VpzMainModelType;
}

}}//namespaces
