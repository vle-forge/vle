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

#include "notification.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QPainter>

namespace vle {
namespace gvle {

notification::notification(QWidget* parent)
  : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool |
                   Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    m_animation.setTargetObject(this);
    m_animation.setPropertyName("m_popupOpacity");
    connect(
      &m_animation, &QAbstractAnimation::finished, this, &notification::hide);

    m_label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_label.setStyleSheet("QLabel { color : white; "
                          "margin-top: 10px;"
                          "margin-bottom: 10px;"
                          "margin-left: 10px;"
                          "margin-right: 10px; }");

    m_layout.addWidget(&m_label, 0, 0);
    setLayout(&m_layout);

    m_timer = new QTimer();
    connect(m_timer, &QTimer::timeout, this, &notification::hideAnimation);
}

void
notification::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    painter.setBrush(QBrush(QColor(0, 0, 0, 180)));
    painter.setPen(Qt::NoPen);

    painter.drawRoundedRect(roundedRect, 10, 10);
}

void
notification::setPopupText(const QString& text)
{
    m_label.setText(text);
    adjustSize();
}

void
notification::show()
{
    setWindowOpacity(0.0);

    m_animation.setDuration(150);
    m_animation.setStartValue(0.0);
    m_animation.setEndValue(1.0);

    auto* p = qobject_cast<QWidget*>(parent());
    if (p) {
        auto geo = p->geometry();

        setGeometry(geo.width() / 2 + geo.x(),
                    geo.height() / 2 + geo.y(),
                    width(),
                    height());

    } else {
        auto geo = QApplication::desktop()->availableGeometry();

        setGeometry(geo.width() - 36 - width() + geo.x(),
                    geo.height() - 36 - height() + geo.y(),
                    width(),
                    height());
    }

    QWidget::show();

    m_animation.start();
    m_timer->start(3000);
}

void
notification::hideAnimation()
{
    m_timer->stop();
    m_animation.setDuration(1000);
    m_animation.setStartValue(1.0);
    m_animation.setEndValue(0.0);
    m_animation.start();
}

void
notification::hide()
{
    if (popupOpacity() == 0.0)
        QWidget::hide();
}

void
notification::setPopupOpacity(double opacity)
{
    m_popupOpacity = opacity < 0 ? 0 : opacity > 1 ? 1 : opacity;

    setWindowOpacity(m_popupOpacity);
}

double
notification::popupOpacity() const
{
    return m_popupOpacity;
}
}
} // namespace vle gvle
