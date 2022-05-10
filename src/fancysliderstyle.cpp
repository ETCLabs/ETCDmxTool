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

#include "fancysliderstyle.h"
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QPainterPath>

void FancyFaderStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if(control == QStyle::CC_Slider)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option);
        // Draw the filled part of the bar
        QRect sliderRect = subControlRect(control, slider, SC_SliderGroove, widget);
        QRect handleRect = this->subControlRect(control, slider, SC_SliderHandle, widget);;

        QRect filledRect = sliderRect;
        filledRect.setHeight(slider->rect.height() - handleRect.center().y());
        filledRect.moveBottom(sliderRect.bottom());
        painter->setPen(QPen(QColor("#0d0d0d"), 2));
        painter->setBrush(QBrush(QColor("#0d0d0d")));

        QPainterPath filledPath;
        filledPath.addRoundedRect(filledRect, 5, 5);
        if(slider->tickInterval>0)
            painter->drawPath(filledPath);

        // Draw the overall border

        painter->setPen(QPen(QColor("#262524"), 2));
        painter->setBrush(Qt::NoBrush);
        QPainterPath borderPath;
        borderPath.addRoundedRect(sliderRect, 5, 5);
        painter->drawPath(borderPath);

        // Draw the handle
        painter->setPen(Qt::black);
        if(slider->state & QStyle::State_HasFocus)
        {
            QLinearGradient gradient(handleRect.topLeft(), handleRect.bottomLeft());
            gradient.setColorAt(0.0, QColor("#0094cb"));
            gradient.setColorAt(1.0, QColor("#0086b8"));
            painter->setBrush(QBrush(gradient));
        }
        else
        {
            QLinearGradient gradient(handleRect.topLeft(), handleRect.bottomLeft());
            gradient.setColorAt(0.0, QColor("#d9d9d9"));
            gradient.setColorAt(1.0, QColor("#c4c4c4"));
            painter->setBrush(QBrush(gradient));
        }
        QPainterPath handlePath;
        handlePath.addRoundedRect(handleRect, 5, 5);
        painter->drawPath(handlePath);

        // Draw the level text
        if(slider->state & QStyle::State_HasFocus)
        {
            painter->setPen(QColor("#e5e5e5"));
        }
        else
        {
            painter->setPen(QColor("#333333"));
        }
        if(slider->tickInterval!=0)
            painter->drawText(handleRect, Qt::AlignCenter, QString::number(slider->sliderValue));

    }
    else
    {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
}

QRect FancyFaderStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                      SubControl subControl, const QWidget *widget) const
{
    if(control==CC_Slider)
    {
        if(subControl == SC_SliderGroove)
        {
            //QRect result = QProxyStyle::subControlRect(control, option, subControl, widget);
            QRect result = widget->rect();
            result.adjust(10, 5, -10, -5);
            return result;
        }
        if(subControl == SC_SliderHandle)
        {
            QRect result = QProxyStyle::subControlRect(control, option, subControl, widget);
            QPoint center = result.center();
            result.setWidth(widget->width()-2);
            result.adjust(0, 2, 0, -2);
            result.moveCenter(center);
            return result;
        }
    }

    return QProxyStyle::subControlRect(control, option, subControl, widget);
}


int FancyFaderStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    if(metric==PM_SliderLength)
        return 30;
    return QProxyStyle::pixelMetric(metric, option, widget);
}
