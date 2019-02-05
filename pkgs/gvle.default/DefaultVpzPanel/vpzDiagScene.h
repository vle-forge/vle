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

#ifndef gvle_VPZ_DIAG_SCENE_H
#define gvle_VPZ_DIAG_SCENE_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QUndoStack>
#include <QUndoView>
#include <QWidget>

#include "gvle_widgets.h"
#include "vlevpz.hpp"

#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

#include <QtDebug>

namespace vle {
namespace gvle {

class VpzDiagScene;

enum
{
    VpzPortTypeIn = QGraphicsItem::UserType + 1,
    VpzPortTypeOut = QGraphicsItem::UserType + 2,
    VpzSubModelType = QGraphicsItem::UserType + 3,
    VpzMainModelType = QGraphicsItem::UserType + 4,
    VpzConnectionLineType = QGraphicsItem::UserType + 5
};
enum SEL_TYPE
{
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
    MIDDLE
};

enum VPZ_DIAG_MENU_ACTION
{
    VDMA_copy_models_notrack,
    VDMA_copy_models_track,
    VDMA_paste_models,
    VDMA_Edit_name,
    VDMA_Remove,
    VDMA_Add_input_port,
    VDMA_Add_output_port,
    VDMA_Add_atomic_model,
    VDMA_Add_coupled_model,
    VDMA_Zoom_In,
    VDMA_Zoom_Out,
    VDMA_Configure_model,
    VDMA_Unconfigure_model,
    VDMA_Clear_conf_model,
    VDMA_Debug_atomic,
    VDMA_Save_SVG
};

class VpzConnectionLineItem : public QGraphicsItem
{
public:
    VpzConnectionLineItem(QDomNode node,
                          vleVpz* vpz,
                          QLineF l,
                          QGraphicsItem* parent,
                          VpzDiagScene* scene);
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = 0) override;
    QRectF boundingRect() const override;
    void update(const QRectF& rect = QRectF());
    int type() const override;

    vleVpz* mVpz;
    QDomNode mnode;
};

class VpzPortItem : public QGraphicsObject
{
    Q_OBJECT
public:
    VpzPortItem(QDomNode node,
                vleVpz* vpz,
                bool input,
                QGraphicsItem* parent,
                VpzDiagScene* scene);
    QRectF boundingRect() const override;
    void update(const QRectF& rect = QRectF());
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = 0) override;
    QPointF getConnectionPoint();
    QString getPortName();
    VleLineEditItem* getTextItem() const;
    QGraphicsPixmapItem* getPixItem() const;
    void setNameEdition(bool val);
    int type() const override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* evt) override;

public:
    vleVpz* mVpz;
    QDomNode mnode;
    bool mInput;
};

class VpzModelItem : public QGraphicsItem
{
public:
    VpzModelItem(QDomNode node,
                 vleVpz* vpz,
                 QGraphicsItem* parent,
                 QGraphicsScene* scene);
    VleLineEditItem* getTitle() const;
    QGraphicsRectItem* getRectangle() const;

    QList<VpzPortItem*> getInPorts();
    QList<VpzPortItem*> getOutPorts();

    VpzPortItem* getInPort(const QString& name);
    VpzPortItem* getOutPort(const QString& name);
    void clearPorts();
    void clearTitle();
    QString getModelName();
    double widthInputPorts();
    double widthOutputPorts();
    double heightPort();
    void setNameEdition(bool val);

    vleVpz* mVpz;
    QDomNode mnode;
    double margin;
    double rectWidth;
    double rectHeight;
};

class VpzSubModelItem : public VpzModelItem
{

public:
    VpzSubModelItem(QDomNode node,
                    vleVpz* vpz,
                    QGraphicsItem* parent,
                    QGraphicsScene* scene);
    ~VpzSubModelItem() override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = 0) override;
    void initializeFromDom();
    void update(const QRectF& rect = QRectF());
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    SEL_TYPE getSelType(QPointF selPoint);
    QString getModelName();
    int type() const override;
    void removeNameEditionMode();
};

class VpzMainModelItem : public VpzModelItem
{

public:
    enum SEL_TYPE
    {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT,
        MIDDLE
    };

    VpzMainModelItem(QDomNode node,
                     vleVpz* vpz,
                     QGraphicsItem* parent,
                     QGraphicsScene* scene);
    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = 0) override;
    void initializeFromDom();
    void clearLines(
      const QList<VpzSubModelItem*>& sels = QList<VpzSubModelItem*>());
    void clearSubModels();
    void addConnLines(
      const QList<VpzSubModelItem*>& sels = QList<VpzSubModelItem*>());
    void update(const QRectF& rect = QRectF());

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

    QRectF subModelsBoundingRect(bool onlySelected);
    VpzSubModelItem* getSubModel(const QString& subMod);
    QList<VpzSubModelItem*> getSubModels();
    QList<VpzSubModelItem*> getSelectedSubModels();
    QList<QDomNode> getSelectedSubModelsNode();
    QList<VpzConnectionLineItem*> getConnLines();
    void removeNameEditionMode();
    bool isAtomic();
    int type() const override;
};

class VpzDiagScene : public QGraphicsScene
{
    Q_OBJECT
public:
    VpzDiagScene(Logger* log, const vle::utils::ContextPtr& ctx);
    /**
     * @brief initialize the scene with a vpz file
     * @param vpz, the vpz file
     * @param className, if empty then the main model is shown
     * @param emitter, the object that send the signal:
     *   undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)
     */
    void init(vleVpz* vpz, const QString& className);
    QString getClass();
    void setFocus(QDomNode selModelNode, bool refreshAll = false);
    void clear();
    void update(const QRectF& rect = QRectF());
    void setScale(const qreal z);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;
    void contextMenuEvent(
      QGraphicsSceneContextMenuEvent* contextMenuEvent) override;
    void populateConfigureMenu(QMenu* menu);
    void populateDataConfigureMenu(QMenu* menu);
    void doConfigureMenu(QDomNode model, const QString& meta);

    QBrush getBrushAtomicModel();
    QBrush getBrushCoupledModel();
    QPen getPenNotSelectedModel();
    QPen getPenSelectedModel();
    QPen getPenConnection();
    QBrush getBrushBackground();
    QFont getFontPort();
    QFont getFontModel();
    QColor getColorModelNameSelected();
    QColor getColorModelNameNotSelected();

    QString getXQueryOfTheSelectedModel();
    VpzPortItem* getPort(const QString& portName, const QString& subMod);
    QStringList getSelectedModels();
    void unselectAllSubModels();
    void selectSubModel(const QString& nameSub);

    static bool isVpzPort(QGraphicsItem* item);
    static bool isVpzInputPort(QGraphicsItem* item);
    static bool isVpzOutputPort(QGraphicsItem* item);
    static bool isVpzSubModel(QGraphicsItem* item);
    static bool isVpzMainModel(QGraphicsItem* item);
    static bool isVpzConnectionLine(QGraphicsItem* item);

    static void setActionType(QAction* action, VPZ_DIAG_MENU_ACTION type);
    static VPZ_DIAG_MENU_ACTION getActionType(QAction* action);

    bool isVpzSubModelConfigured(QGraphicsItem* item);

    void saveSVG();
signals:
    void enterCoupledModel(QDomNode node);
    void initializationDone(VpzDiagScene* scene);

public slots:
    void onDragDestroyed(QObject* obj = 0);
    void onUndoRedoVpz(QDomNode oldValVpz,
                       QDomNode newValVpz,
                       QDomNode oldValVpm,
                       QDomNode newValVpm);

public:
    vleVpz* mVpz;
    QString mClass;
    VpzMainModelItem* mCoupled;
    VpzPortItem* mPortSel1;
    VpzPortItem* mPortSel2;
    QGraphicsLineItem* mConnection;
    QPointF mDragStartPoint;
    QPointF mDragCurrPoint;
    SEL_TYPE mModelSelType;
    bool mIsEnteringCoupled;
    Logger* mLog;
    vle::utils::ContextPtr mCtx;
};
}
} // namespaces

#endif
