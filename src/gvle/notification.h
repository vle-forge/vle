/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2018 INRA http://www.inra.fr
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

#ifndef gvle_notification_HPP
#define gvle_notification_HPP

#include <QGridLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

namespace vle {
namespace gvle {

/**
 * @brief  A PopUp notification.
 *
 * @code
 * class MainWindow : public QMainWindow
 * {
 *     Q_OBJECT
 *
 * public:
 *     explicit MainWindow(QWidget *parent = 0);
 *     ~MainWindow();
 *
 *     void hello();
 *
 * private:
 *     vle::gvle::notification *popup;
 * };
 *
 * void MainWindow::hello()
 * {
 *     popup->setPopupText("hello world!");
 *     popUp->show();
 * }
 * @endcode
 */
class notification : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal m_popupOpacity READ popupOpacity WRITE setPopupOpacity)

    void setPopupOpacity(qreal opacity);
    qreal popupOpacity() const;

public:
    explicit notification(QWidget* parent = 0);

protected:
    void paintEvent(QPaintEvent* event);

public slots:
    void setPopupText(const QString& text);
    void show();

private slots:
    void hideAnimation();
    void hide();

private:
    QLabel m_label;
    QGridLayout m_layout;
    QPropertyAnimation m_animation;
    qreal m_popupOpacity;
    QTimer* m_timer;
};
}
} // namespace vle gvle

#endif
