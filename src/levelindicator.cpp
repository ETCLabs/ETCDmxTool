// Copyright (c) 2017 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "levelindicator.h"
#include <QPainter>
#include <QPainterPath>

LevelIndicator::LevelIndicator(QWidget *parent) : QWidget(parent) , m_value(0)
{
}



void LevelIndicator::setValue(quint8 value)
{
    m_value = value;
    update();
}

void LevelIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background rounded rect
    QRect bounds;
    bounds.setWidth(qMin(30, this->rect().width()));
    bounds.setHeight(this->rect().height()-6);
    bounds.moveCenter(this->rect().center());
    QPainterPath boundsPath;
    boundsPath.addRoundedRect(bounds, 10, 10);
    painter.fillPath(boundsPath, QBrush(QColor("#e5e5e5")));

    // Draw level bar
    QRect levelBar = bounds;
    levelBar.setHeight(levelBar.height() * m_value / 255);
    levelBar.moveBottom(bounds.bottom());
    QPainterPath levelPath;
    levelPath.addRoundedRect(levelBar, 10, 10);
    painter.fillPath(levelPath, QBrush(QColor("#00aeef")));

    // Draw bounding rect
    QPen boundsPen(QColor("#262524"));
    boundsPen.setWidth(2);
    painter.strokePath(boundsPath, boundsPen);

}
