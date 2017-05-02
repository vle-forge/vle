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

#include "filevpzclasses.h"
#include "ui_filevpzclasses.h"
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QtDebug>

namespace vle {
namespace gvle {

FileVpzClasses::FileVpzClasses(Logger* log,
                               const vle::utils::ContextPtr& ctx,
                               QWidget* parent)
  : QWidget(parent)
  , mScene(log, ctx)
  , ui(new Ui::FileVpzClasses)
  , mVpz(0)
  , mSelClass("<None>")
{

    this->setAcceptDrops(true);

    ui->setupUi(this);
    ui->graphicsView->setAcceptDrops(true);
    ui->classesList->setEditable(false);
    ui->classesList->setEnabled(false);

    QObject::connect(ui->classesList,
                     SIGNAL(currentIndexChanged(const QString&)),
                     this,
                     SLOT(onCurrentIndexChanged(const QString&)));
    QObject::connect(ui->classesList,
                     SIGNAL(editTextChanged(const QString&)),
                     this,
                     SLOT(onTextChanged(const QString&)));
    QObject::connect(
      ui->pushCopy, SIGNAL(clicked(bool)), this, SLOT(onPushCopy(bool)));
    QObject::connect(
      ui->pushDelete, SIGNAL(clicked(bool)), this, SLOT(onPushDelete(bool)));
    //    QObject::connect(ui->pushNew,
    //                SIGNAL(clicked(bool)),
    //                this, SLOT(onPushNew(bool)));

    QAction* act = mMenuButton.addAction("atomic");
    QObject::connect(
      act, SIGNAL(triggered(bool)), this, SLOT(onNewAtomicTriggered(bool)));
    act = mMenuButton.addAction("coupled");
    QObject::connect(
      act, SIGNAL(triggered(bool)), this, SLOT(onNewCoupledTriggered(bool)));
    ui->pushNew->setMenu(&mMenuButton);
}

FileVpzClasses::~FileVpzClasses()
{
    delete ui;
}

void
FileVpzClasses::setVpz(vleVpz* vpz)
{
    mVpz = vpz;
    reload();
}

void
FileVpzClasses::reload()
{

    // reload classes list
    bool oldBlock = ui->classesList->blockSignals(true);
    ui->classesList->setEnabled(true);
    ui->classesList->setEditable(true);
    ui->classesList->clear();
    std::vector<std::string> classes;
    mVpz->fillWithClassesFromDoc(classes);
    ui->classesList->addItem(QString("<None>"));
    std::vector<std::string>::const_iterator itb = classes.begin();
    std::vector<std::string>::const_iterator ite = classes.end();
    for (; itb != ite; itb++) {
        ui->classesList->addItem(QString(itb->c_str()));
    }
    ui->classesList->blockSignals(oldBlock);
    mSelClass = "<None>";
    mScene.init(mVpz, mSelClass);
}

void
FileVpzClasses::onUndoRedoVpz(QDomNode oldValVpz,
                              QDomNode newValVpz,
                              QDomNode oldValVpm,
                              QDomNode newValVpm)
{
    if (oldValVpz.nodeName() == "classes") {
        // the action is the creation of a class, nothing to do do for the
        // scene
        // except clearing the scene
        reload();
    } else {
        mScene.onUndoRedoVpz(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}

void
FileVpzClasses::onNewAtomicTriggered(bool /*checked*/)
{
    QString name = mVpz->addClassToDoc(true);
    ui->classesList->addItem(name);
    ui->classesList->setCurrentIndex(ui->classesList->count() - 1);
}

void
FileVpzClasses::onNewCoupledTriggered(bool /*checked*/)
{
    QString name = mVpz->addClassToDoc(false);
    ui->classesList->addItem(name);
    ui->classesList->setCurrentIndex(ui->classesList->count() - 1);
}

void
FileVpzClasses::onTextChanged(const QString& text)
{
    if (mSelClass != text and mSelClass != "<None>" and text != "<None>" and
        not mVpz->existClassFromDoc(text) and text != "") {
        mVpz->renameClassToDoc(mSelClass, text);
        int it = ui->classesList->currentIndex();
        ui->classesList->setItemText(it, text);
        mSelClass = text;
        mScene.init(mVpz, mSelClass);
        // reload();
    }
}

void
FileVpzClasses::onPushCopy(bool /*checked*/)
{
    if (mSelClass != "<None>") {
        QString name = mVpz->copyClassToDoc(mSelClass);
        ui->classesList->addItem(name);
        ui->classesList->setCurrentIndex(ui->classesList->count() - 1);
    }
}
void
FileVpzClasses::onPushDelete(bool /*checked*/)
{
    if (mSelClass != "<None>") {
        mVpz->removeClassToDoc(mSelClass);
        ui->classesList->removeItem(ui->classesList->currentIndex());
        ui->classesList->setCurrentIndex(0);
    }
}

void
FileVpzClasses::onCurrentIndexChanged(const QString& /*text*/)
{
    if (mVpz) {
        mSelClass = ui->classesList->currentText();
        mScene.init(mVpz, mSelClass);
        ui->graphicsView->setSceneRect(QRect(0, 0, 0, 0));
        ui->graphicsView->setScene(&mScene);
        mScene.update();
        if (mSelClass != "<None>") {
            // vpz get first model of class
            ui->pushCopy->setEnabled(true);
            ui->pushDelete->setEnabled(true);
        } else {
            ui->pushCopy->setEnabled(false);
            ui->pushDelete->setEnabled(false);
        }
    }
}

void
FileVpzClasses::onChanged(const QList<QRectF>& /*region*/)
{
    // qDebug() << " FileVpzClasses::onChanged " << region << "\n";
}
void
FileVpzClasses::onSceneRectChanged(const QRectF& /*rect*/)
{
    // qDebug() << " FileVpzClasses::onSceneRectChanged " << rect << "\n";
}
void
FileVpzClasses::onSelectionChanged()
{
    // qDebug() << " FileVpzClasses::onSelectionChanged " << "\n";
}

void
FileVpzClasses::onMenu(const QPoint& /*pt*/)
{
}
}
} // namespaces
