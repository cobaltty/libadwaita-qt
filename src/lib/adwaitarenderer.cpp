/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 * Copyright (C) 2014-2018 Martin Bříza <m@rtinbriza.cz>                 *
 * Copyright (C) 2019-2021 Jan Grulich <jgrulich@redhat.com>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *************************************************************************/

#include "adwaitarenderer.h"
#include "adwaitacolors.h"

#include <QPainter>
#include <QPainterPath>
#include <QRect>

#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace Adwaita
{

static qreal frameRadius(qreal bias = 0)
{
    return qMax(qreal(Metrics::Frame_FrameRadius) - 0.5 + bias, 0.0);
}

static QPainterPath roundedPath(const QRectF &rect, Corners corners, qreal radius)
{
    QPainterPath path;

    // simple cases
    if (corners == 0) {
        path.addRect(rect);
        return path;
    }

    if (corners == AllCorners) {
        path.addRoundedRect(rect, radius, radius);
        return path;
    }

    QSizeF cornerSize(2 * radius, 2 * radius);

    // rotate counterclockwise
    // top left corner
    if (corners & CornerTopLeft) {
        path.moveTo(rect.topLeft() + QPointF(radius, 0));
        path.arcTo(QRectF(rect.topLeft(), cornerSize), 90, 90);
    } else {
        path.moveTo(rect.topLeft());
    }

    // bottom left corner
    if (corners & CornerBottomLeft) {
        path.lineTo(rect.bottomLeft() - QPointF(0, radius));
        path.arcTo(QRectF(rect.bottomLeft() - QPointF(0, 2 * radius), cornerSize), 180, 90);
    } else {
        path.lineTo(rect.bottomLeft());
    }

    // bottom right corner
    if (corners & CornerBottomRight) {
        path.lineTo(rect.bottomRight() - QPointF(radius, 0));
        path.arcTo(QRectF(rect.bottomRight() - QPointF(2 * radius, 2 * radius), cornerSize), 270, 90);
    } else {
        path.lineTo(rect.bottomRight());
    }

    // top right corner
    if (corners & CornerTopRight) {
        path.lineTo(rect.topRight() + QPointF(0, radius));
        path.arcTo(QRectF(rect.topRight() - QPointF(2 * radius, 0), cornerSize), 0, 90);
    } else {
        path.lineTo(rect.topRight());
    }

    path.closeSubpath();
    return path;
}

void Renderer::renderDebugFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHints(QPainter::Antialiasing);
    options.painter()->setBrush(Qt::NoBrush);
    options.painter()->setPen(Qt::red);
    options.painter()->drawRect(QRectF(options.rect()).adjusted(0.5, 0.5, -0.5, -0.5));
    options.painter()->restore();
}

void Renderer::renderFocusRect(const StyleOptions &options, Sides sides)
{
    if (!options.painter()) {
        return;
    }

    if (!options.color().isValid()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHints(QPainter::Antialiasing);
    options.painter()->setBrush(options.color());

    if (!(options.outlineColor().isValid() && sides)) {
        options.painter()->setPen(Qt::NoPen);
        options.painter()->drawRect(options.rect());
    } else {
        options.painter()->setClipRect(options.rect());

        QRectF copy(options.rect());
        copy.adjust(0.5, 0.5, -0.5, -0.5);

        qreal radius(frameRadius(-1.0));
        if (!(sides & SideTop)) {
            copy.adjust(0, -radius, 0, 0);
        }
        if (!(sides & SideBottom)) {
            copy.adjust(0, 0, 0, radius);
        }
        if (!(sides & SideLeft)) {
            copy.adjust(-radius, 0, 0, 0);
        }
        if (!(sides & SideRight)) {
            copy.adjust(0, 0, radius, 0);
        }

        options.painter()->setPen(options.outlineColor());
        // options.painter()->setBrush( Qt::NoBrush );
        options.painter()->drawRoundedRect(copy, radius, radius);
    }

    options.painter()->restore();
}

void Renderer::renderFocusLine(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    if (!options.color().isValid()) {
        return;
    }

    options.painter()->save();

    QPen pen(options.color(), 1);
    pen.setStyle(Qt::DotLine);

    options.painter()->setRenderHint(QPainter::Antialiasing, false);
    options.painter()->setPen(pen);
    options.painter()->setBrush(Qt::NoBrush);

    options.painter()->drawRoundedRect(options.rect(), 1, 1);

    options.painter()->restore();
}

void Renderer::renderMenuItemHighlight(const StyleOptions &options)
{
    // Libadwaita menu item highlight: rounded pill filling most of the item.
    // Small insets to keep pill inside the menu frame rounded corners.
    if (!options.painter() || !options.color().isValid()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);
    options.painter()->setPen(Qt::NoPen);
    options.painter()->setBrush(options.color());

    constexpr qreal menuHighlightRadius = 6.0;
    constexpr qreal hInset = 3.0;
    constexpr qreal vInset = 0.0;
    QRectF highlightRect(options.rect());
    highlightRect.adjust(hInset, vInset, -hInset, -vInset);
    options.painter()->drawRoundedRect(highlightRect, menuHighlightRadius, menuHighlightRadius);

    options.painter()->restore();
}

void Renderer::renderFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing);

    QRectF frameRect(options.rect().adjusted(1, 1, -1, -1));
    qreal radius(frameRadius());

    // set pen
    if (options.outlineColor().isValid()) {
        if (options.hasFocus()) {
            options.painter()->setPen(QPen(options.outlineColor(), 2));
            frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        } else {
            options.painter()->setPen(options.outlineColor());
        }
        frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        radius = qMax(radius - 1, qreal(0.0));

    } else {
        options.painter()->setPen(Qt::NoPen);
    }

    // set brush
    if (options.color().isValid()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }

    // render
    options.painter()->drawRoundedRect(frameRect, radius, radius);
    options.painter()->restore();
}

void Renderer::renderSquareFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF frameRect(options.rect().adjusted(1, 1, -1, -1));
    const qreal radius = frameRadius();

    if (options.color().isValid()) {
        options.painter()->setPen(options.color());
        options.painter()->setBrush(Qt::NoBrush);
        frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        options.painter()->drawRoundedRect(frameRect, radius, radius);
    }

    if (options.hasFocus()) {
        options.painter()->setPen(QPen(options.color(), 1));
        options.painter()->setBrush(Qt::NoBrush);
        options.painter()->drawRoundedRect(options.rect().adjusted(0, 0, -1, -1), radius, radius);
    }

    options.painter()->restore();
}

void Renderer::renderFlatFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing);

    QRectF frameRect(options.rect().adjusted(1, 1, -1, -1));
    qreal radius(frameRadius());

    // set pen
    if (options.outlineColor().isValid()) {
        if (options.hasFocus()) {
            options.painter()->setPen(QPen(options.outlineColor(), 2));
            frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        } else {
            options.painter()->setPen(options.outlineColor());
        }
        frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        radius = qMax(radius - 1, qreal(0.0));
    } else {
        options.painter()->setPen(Qt::NoPen);
    }

    // set brush
    if (options.color().isValid()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(frameRect.adjusted(2 * radius, 0, 0, 0));
    path.addRoundedRect(frameRect.adjusted(0, 0, - 2 * radius, 0), radius, radius);

    options.painter()->drawPath(path.simplified());
    options.painter()->restore();

    // render
    //options.painter()->drawRoundedRect( frameRect, radius, radius );
}


void Renderer::renderFlatRoundedButtonFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    // setup options.painter()
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // copy options.rect()
    QRectF frameRect(options.rect());
    frameRect.adjust(1, 1, -1, -1);
    qreal radius(frameRadius());

    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));

        frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        radius = qMax(radius - 1, qreal(0.0));
    } else {
        options.painter()->setPen(Qt::NoPen);
    }

    // content
        if (options.color().isValid() && options.active()) {
        options.painter()->setBrush(Colors::buttonBackgroundGradient(options));
    } else if (!options.active()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }

    // render
    options.painter()->drawEllipse(frameRect);
    options.painter()->restore();
}

void Renderer::renderSidePanelFrame(const StyleOptions &options, Side side)
{
    if (!options.painter()) {
        return;
    }

    // check color
    if (!options.outlineColor().isValid()) {
        return;
    }

    // adjust options.rect()
    QRectF frameRect(options.rect().adjusted(1, 1, -1, -1));
    frameRect.adjust(0.5, 0.5, -0.5, -0.5);

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing);
    options.painter()->setPen(options.outlineColor());

    // render
    switch (side) {
    case SideLeft:
        frameRect.adjust(0, 1, 0, -1);
        options.painter()->drawLine(frameRect.topRight(), frameRect.bottomRight());
        break;
    case SideTop:
        frameRect.adjust(1, 0, -1, 0);
        options.painter()->drawLine(frameRect.topLeft(), frameRect.topRight());
        break;
    case SideRight:
        frameRect.adjust(0, 1, 0, -1);
        options.painter()->drawLine(frameRect.topLeft(), frameRect.bottomLeft());
        break;
    case SideBottom:
        frameRect.adjust(1, 0, -1, 0);
        options.painter()->drawLine(frameRect.bottomLeft(), frameRect.bottomRight());
        break;
    case AllSides: {
        qreal radius(frameRadius(-1.0));
        options.painter()->drawRoundedRect(frameRect, radius, radius);
        break;
    }
    default:
        break;
    }

    options.painter()->restore();
}

void Renderer::renderMenuFrame(const StyleOptions &options, bool roundCorners)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF r = QRectF(options.rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal radius = roundCorners ? qreal(Metrics::Menu_FrameRadius) : 0.0;

    QPainterPath path;
    if (radius > 0.0) {
        path.addRoundedRect(r, radius, radius);
    } else {
        path.addRect(r);
    }

    // fill
    if (options.color().isValid()) {
        options.painter()->fillPath(path, options.color());
    }

    // outline
    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));
        options.painter()->drawPath(path);
    }

    options.painter()->restore();
}

void Renderer::renderButtonFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // Libadwaita button corner radius: 8px
    constexpr qreal btnRadius = 8.0;
    QRectF r = QRectF(options.rect()).adjusted(0.5, 0.5, -0.5, -0.5);

    QPainterPath path;
    path.addRoundedRect(r, btnRadius, btnRadius);

    // fill
    if (options.color().isValid()) {
        options.painter()->fillPath(path, options.color());
    }

    // outline
    if (options.outlineColor().isValid()) {
        qreal penWidth = options.hasFocus() ? 2.0 : 1.0;
        options.painter()->setPen(QPen(options.outlineColor(), penWidth));
        options.painter()->drawPath(path);
    }

    options.painter()->restore();
}

void Renderer::renderComboBoxFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // Libadwaita ComboBox: 8px corner radius
    constexpr qreal comboRadius = qreal(Metrics::ComboBox_FrameRadius);
    QRectF r = QRectF(options.rect()).adjusted(0.5, 0.5, -0.5, -0.5);

    QPainterPath path;
    path.addRoundedRect(r, comboRadius, comboRadius);

    // fill
    if (options.color().isValid()) {
        options.painter()->fillPath(path, options.color());
    }

    // outline
    if (options.outlineColor().isValid()) {
        qreal penWidth = options.hasFocus() ? 2.0 : 1.0;
        options.painter()->setPen(QPen(options.outlineColor(), penWidth));
        options.painter()->drawPath(path);
    }

    options.painter()->restore();
}

void Renderer::renderCardFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // adw-gtk3 card: 12px radius, subtle fill + hairline border
    constexpr qreal cardRadius = qreal(Metrics::GroupBox_CardRadius);
    QRectF r = QRectF(options.rect()).adjusted(0.5, 0.5, -0.5, -0.5);

    QPainterPath path;
    path.addRoundedRect(r, cardRadius, cardRadius);

    // fill
    if (options.color().isValid()) {
        options.painter()->fillPath(path, options.color());
    }

    // outline
    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));
        options.painter()->setBrush(Qt::NoBrush);
        options.painter()->drawPath(path);
    }

    options.painter()->restore();
}

void Renderer::renderCheckBoxFrame(const StyleOptions &options)
{
    if (!options.painter() || options.inMenu()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF frameRect(options.rect());
    frameRect.adjust(2, 2, -2, -2);
    constexpr qreal radius = 5.0;

    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));
    } else {
        options.painter()->setPen(Qt::NoPen);
    }

    if (options.color().isValid()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }

    options.painter()->drawRoundedRect(frameRect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    options.painter()->restore();
}

void Renderer::renderFlatButtonFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // copy options.rect()
    QRectF frameRect(options.rect());
    frameRect.adjust(1, 1, -1, -1);
    qreal radius(frameRadius());

    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));

        frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        radius = qMax(radius - 1, qreal(0.0));
    } else {
        options.painter()->setPen(Qt::NoPen);
    }

    // content — flat fill, no gradient, no bevel
    if (options.color().isValid()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(frameRect.adjusted(2 * radius, 0, 0, 0), radius, radius);
    path.addRect(frameRect.adjusted(0, 0, -2 * radius, 0));
    options.painter()->drawPath(path.simplified());

    options.painter()->restore();
}

void Renderer::renderToolButtonFrame(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // do nothing for invalid color
    if (!options.color().isValid())
        return;

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHints(QPainter::Antialiasing);

    // Enlarge the hover/active pill bounds for better visibility
    QRectF baseRect(QRectF(options.rect()).adjusted(-2, -2, 2, 2));
    constexpr qreal pillRadius = 6.0;

    if (options.sunken()) {
        // Pressed: solid fill with rounded corners
        options.painter()->setPen(Qt::NoPen);
        options.painter()->setBrush(options.color());
        QRectF contentRect(baseRect.adjusted(1, 1, -1, -1));
        options.painter()->drawRoundedRect(contentRect, pillRadius, pillRadius);
    } else {
        // Hover: rounded pill overlay with subtle background tint
        options.painter()->setPen(Qt::NoPen);
        options.painter()->setBrush(options.color());
        QRectF hoverRect(baseRect.adjusted(1, 1, -1, -1));
        options.painter()->drawRoundedRect(hoverRect, pillRadius, pillRadius);
    }
    options.painter()->restore();
}

void Renderer::renderToolBoxFrame(const StyleOptions &options, int tabWidth)
{
    if (!options.painter()) {
        return;
    }

    if (!options.outlineColor().isValid()) {
        return;
    }

    // round radius
    qreal radius(frameRadius());
    QSizeF cornerSize(2 * radius, 2 * radius);

    // if options.rect() - tabwidth is even, need to increase tabWidth by 1 unit
    // for anti aliasing
    if (!((options.rect().width() - tabWidth) % 2)) {
        ++tabWidth;
    }

    // adjust options.rect() for antialiasing
    QRectF baseRect(options.rect());
    baseRect.adjust(0.5, 0.5, -0.5, -0.5);

    // create path
    QPainterPath path;
    path.moveTo(0, baseRect.height() - 1);
    path.lineTo((baseRect.width() - tabWidth) / 2 - radius, baseRect.height() - 1);
    path.arcTo(QRectF(QPointF((baseRect.width() - tabWidth) / 2 - 2 * radius, baseRect.height() - 1 - 2 * radius), cornerSize), 270, 90);
    path.lineTo((baseRect.width() - tabWidth) / 2, radius);
    path.arcTo(QRectF(QPointF((baseRect.width() - tabWidth) / 2, 0), cornerSize), 180, -90);
    path.lineTo((baseRect.width() + tabWidth) / 2 - 1 - radius, 0);
    path.arcTo(QRectF(QPointF((baseRect.width() + tabWidth) / 2  - 1 - 2 * radius, 0), cornerSize), 90, -90);
    path.lineTo((baseRect.width() + tabWidth) / 2 - 1, baseRect.height() - 1 - radius);
    path.arcTo(QRectF(QPointF((baseRect.width() + tabWidth) / 2 - 1, baseRect.height() - 1 - 2 * radius), cornerSize), 180, 90);
    path.lineTo(baseRect.width() - 1, baseRect.height() - 1);

    // render
    options.painter()->save();
    options.painter()->setRenderHints(QPainter::Antialiasing);
    options.painter()->setBrush(Qt::NoBrush);
    options.painter()->setPen(options.outlineColor());
    options.painter()->translate(baseRect.topLeft());
    options.painter()->drawPath(path);
    options.painter()->restore();

    return;
}

void Renderer::renderTabWidgetFrame(const StyleOptions &options, Corners corners)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->setRenderHint(QPainter::Antialiasing);

    QRectF frameRect(options.rect().adjusted(1, 1, -1, -1));
    qreal radius(frameRadius());

    options.painter()->save();

    // set pen
    if (options.outlineColor().isValid()) {
        options.painter()->setPen(options.outlineColor());
        frameRect.adjust(0.5, 0.5, -0.5, -0.5);
        radius = qMax(radius - 1, qreal(0.0));
    } else {
        options.painter()->setPen(Qt::NoPen);
    }

    // set brush
    if (options.color().isValid()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }

    // render
    QPainterPath path(roundedPath(frameRect, corners, radius));
    options.painter()->drawPath(path);
    options.painter()->restore();
}

void Renderer::renderSelection(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);
    options.painter()->setPen(Qt::NoPen);
    options.painter()->setBrush(options.color());

    // adw-gtk3 inset list item: floating pill with
    // 3px top/bottom margin, 4px left/right margin.
    constexpr qreal selectionRadius = 6.0;
    constexpr qreal vInset = 3.0;
    constexpr qreal hInset = 4.0;
    QRectF selRect(options.rect());
    selRect.adjust(hInset, vInset, -hInset, -vInset);
    options.painter()->drawRoundedRect(selRect, selectionRadius, selectionRadius);

    options.painter()->restore();
}

void Renderer::renderSeparator(const StyleOptions &options, bool vertical)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, false);
    options.painter()->setBrush(Qt::NoBrush);
    options.painter()->setPen(options.color());

    if (vertical) {
        options.painter()->translate(options.rect().width() / 2, 0);
        options.painter()->drawLine(options.rect().topLeft(), options.rect().bottomLeft());
    } else {
        options.painter()->translate(0, options.rect().height() / 2);
        options.painter()->drawLine(options.rect().topLeft(), options.rect().topRight());
    }

    options.painter()->restore();
}

void Renderer::renderCheckBoxBackground(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // copy options.rect() and radius
    QRectF frameRect(options.rect());
    frameRect.adjust(3, 3, -3, -3);
    qreal radius(frameRadius());

    options.painter()->setPen(options.outlineColor());
    options.painter()->setBrush(options.color());
    options.painter()->drawRoundedRect(frameRect, radius, radius);
    options.painter()->restore();
}

void Renderer::renderCheckBox(const StyleOptions &options, const QColor &tickColor, qreal animation)
{
    if (!options.painter()) {
        return;
    }

    if (!options.inMenu()) {
        renderCheckBoxFrame(options);
    } else if (options.checkboxState() == CheckOff) {
        return;
    }

    // Libadwaita checkmark: bold 2.4px stroke, round cap, round join, exact SVG geometry
    const qreal strokeWidth = 2.4;

    // mark
    if (options.checkboxState() == CheckOn) {
        options.painter()->save();
        options.painter()->setRenderHint(QPainter::Antialiasing, true);
        options.painter()->setBrush(Qt::NoBrush);

        QColor markColor = tickColor;
        if (options.inMenu()) {
            markColor = (options.mouseOver() || (options.state() & QStyle::State_Selected))
                ? options.palette().color(QPalette::HighlightedText)
                : options.palette().color(QPalette::WindowText);
        } else if (!markColor.isValid()) {
            markColor = Qt::white;
        }

        QPen pen(markColor, strokeWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        options.painter()->setPen(pen);

        QRectF frameRect(options.rect());
        if (!options.inMenu()) {
            frameRect.adjust(2, 2, -2, -2);
        }

        QPainterPath path;
        path.moveTo(frameRect.left() + frameRect.width() * 0.20, frameRect.top() + frameRect.height() * 0.50);
        path.lineTo(frameRect.left() + frameRect.width() * 0.40, frameRect.top() + frameRect.height() * 0.76);
        path.lineTo(frameRect.left() + frameRect.width() * 0.82, frameRect.top() + frameRect.height() * 0.26);

        options.painter()->drawPath(path);
        options.painter()->restore();
    } else if (options.checkboxState() == CheckPartial) {
        options.painter()->save();
        options.painter()->setRenderHint(QPainter::Antialiasing, true);
        QColor markColor = tickColor.isValid() ? tickColor : Qt::white;
        QPen pen(markColor, strokeWidth, Qt::SolidLine, Qt::RoundCap);
        options.painter()->setPen(pen);

        QRectF frameRect(options.rect());
        frameRect.adjust(2, 2, -2, -2);
        qreal y = frameRect.center().y();
        qreal pad = frameRect.width() * 0.22;
        options.painter()->drawLine(QPointF(frameRect.left() + pad, y), QPointF(frameRect.right() - pad, y));
        options.painter()->restore();
    } else if (options.checkboxState() == CheckAnimated) {
        options.painter()->save();
        options.painter()->setRenderHint(QPainter::Antialiasing, true);
        options.painter()->setBrush(Qt::NoBrush);

        QColor markColor = tickColor.isValid() ? tickColor : Qt::white;
        QPen pen(markColor, strokeWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        options.painter()->setPen(pen);

        QRectF frameRect(options.rect());
        frameRect.adjust(2, 2, -2, -2);

        QPainterPath path;
        path.moveTo(frameRect.left() + frameRect.width() * 0.20, frameRect.top() + frameRect.height() * 0.50);
        path.lineTo(frameRect.left() + frameRect.width() * 0.40, frameRect.top() + frameRect.height() * 0.76);
        path.lineTo(frameRect.left() + frameRect.width() * 0.82, frameRect.top() + frameRect.height() * 0.26);

        options.painter()->save();
        options.painter()->translate(frameRect.center());
        options.painter()->scale(animation, animation);
        options.painter()->translate(-frameRect.center());
        options.painter()->drawPath(path);
        options.painter()->restore();

        options.painter()->restore();
    }
}

void Renderer::renderRadioButtonBackground(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // copy options.rect()
    QRectF frameRect(options.rect());
    frameRect.adjust(3, 3, -3, -3);
    if (options.sunken()) {
        frameRect.translate(1, 1);
    }

    options.painter()->setPen(options.outlineColor());
    options.painter()->setBrush(options.color());
    options.painter()->drawEllipse(frameRect);
    options.painter()->restore();
}

void Renderer::renderRadioButton(const StyleOptions &options, const QColor &tickColor, qreal animation)
{
    if (!options.painter()) {
        return;
    }

    if (options.inMenu()) {
        const bool isChecked = (options.radioButtonState() == RadioOn || options.checkboxState() == CheckOn);
        if (!isChecked) {
            return;
        }
        options.painter()->save();
        options.painter()->setRenderHint(QPainter::Antialiasing, true);
        QColor dotColor = (options.mouseOver() || (options.state() & QStyle::State_Selected))
            ? options.palette().color(QPalette::HighlightedText)
            : options.palette().color(QPalette::WindowText);
        options.painter()->setBrush(dotColor);
        options.painter()->setPen(Qt::NoPen);
        options.painter()->drawEllipse(QRectF(options.rect()).center(), 3.5, 3.5);
        options.painter()->restore();
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF frameRect(options.rect());
    frameRect.adjust(2, 2, -2, -2);

    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));
    } else {
        options.painter()->setPen(Qt::NoPen);
    }

    if (options.color().isValid()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }

    QRectF contentRect(frameRect.adjusted(0.5, 0.5, -0.5, -0.5));
    options.painter()->drawEllipse(contentRect);

    // mark: checked radio button has a solid white circle in center
    const bool isChecked = (options.radioButtonState() == RadioOn || options.checkboxState() == CheckOn);
    if (isChecked) {
        options.painter()->setBrush(Qt::white);
        options.painter()->setPen(Qt::NoPen);

        QPointF center = contentRect.center();
        constexpr qreal dotRadius = 3.5;
        options.painter()->drawEllipse(center, dotRadius, dotRadius);
    } else if (options.radioButtonState() == RadioAnimated && animation > 0) {
        options.painter()->setBrush(Qt::white);
        options.painter()->setPen(Qt::NoPen);

        QPointF center = contentRect.center();
        qreal dotRadius = 3.5 * animation;
        options.painter()->drawEllipse(center, dotRadius, dotRadius);
    }

    options.painter()->restore();
}

void Renderer::renderSliderGroove(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF baseRect(options.rect());
    qreal radius(0.5 * Metrics::Slider_GrooveThickness);

    // content
    if (options.color().isValid()) {
        options.painter()->setPen(Qt::NoPen);
        options.painter()->setBrush(options.color());
        options.painter()->drawRoundedRect(baseRect, radius, radius);
    }

    options.painter()->restore();
}

void Renderer::renderSliderHandle(const StyleOptions &options, Side ticks, qreal angle)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // copy options.rect()
    QRectF frameRect(options.rect());
    frameRect.adjust(1, 1, -1, -1);

    // detect theme mode
    const bool isDark = (options.colorVariant() == AdwaitaDark);

    // Libadwaita slider handle: white circle with visible border in both modes
    QColor border = Colors::alphaColor(Qt::black, isDark ? 0.30 : 0.18);
    options.painter()->setPen(QPen(border, 1.0));
    options.painter()->setBrush(Qt::white);
    frameRect.adjust(0.5, 0.5, -0.5, -0.5);

    QRect r(options.rect().right() - options.rect().height(), options.rect().top(), options.rect().height(), options.rect().height());
    r.adjust(4.5, 3.5, -2.5, -3.5);

    // Subtle drop shadow behind handle
    options.painter()->setPen(Qt::NoPen);
    options.painter()->setBrush(QColor(0, 0, 0, isDark ? 60 : 35));
    options.painter()->drawEllipse(QRectF(r).adjusted(0, 1.0, 0, 1.0));
    options.painter()->setPen(QPen(border, 1.0));
    options.painter()->setBrush(Qt::white);

    // Libadwaita slider handle: always a clean circular pill
    QPainterPath circle;
    circle.addEllipse(r);
    circle.closeSubpath();

    QTransform rotate;
    rotate.translate(frameRect.center().x(), frameRect.center().y());
    rotate.rotate(angle);
    rotate.translate(-frameRect.center().x(), -frameRect.center().y());
    options.painter()->drawPolygon(circle.toFillPolygon(rotate));
    options.painter()->restore();
}

void Renderer::renderDialGroove(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF baseRect(options.rect());

    // content
    if (options.color().isValid()) {
        qreal penWidth(Metrics::Slider_GrooveThickness);
        QRectF grooveRect(options.rect().adjusted(penWidth / 2, penWidth / 2, -penWidth / 2, -penWidth / 2));

        options.painter()->setPen(QPen(options.color(), penWidth));
        options.painter()->setBrush(Qt::NoBrush);
        options.painter()->drawEllipse(grooveRect);
    }

    return;
}

void Renderer::renderDialContents(const StyleOptions &options, qreal first, qreal second)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF baseRect(options.rect());

    // content
    if (options.color().isValid()) {
        // setup groove options.rect()
        qreal penWidth(Metrics::Slider_GrooveThickness);
        QRectF grooveRect(options.rect().adjusted(penWidth / 2, penWidth / 2, -penWidth / 2, -penWidth / 2));

        // setup angles
        int angleStart(first * 180 * 16 / M_PI);
        int angleSpan((second - first) * 180 * 16 / M_PI);

        // setup pen
        if (angleSpan != 0) {
            QPen pen(options.color(), penWidth);
            pen.setCapStyle(Qt::RoundCap);
            options.painter()->setPen(pen);
            options.painter()->setBrush(Qt::NoBrush);
            options.painter()->drawArc(grooveRect, angleStart, angleSpan);
        }
    }

    options.painter()->restore();
}

void Renderer::renderProgressBarGroove(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);
    options.painter()->setRenderHint(QPainter::SmoothPixmapTransform, true);

    QRectF baseRect(options.rect());
    qreal radius = qMin(baseRect.width(), baseRect.height()) / 2.0;

    // content
    if (options.color().isValid()) {
        options.painter()->setBrush(options.color());
    } else {
        options.painter()->setBrush(Qt::NoBrush);
    }
    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));
    } else {
        options.painter()->setPen(Qt::NoPen);
    }
    options.painter()->drawRoundedRect(baseRect, radius, radius);

    options.painter()->restore();
}

void Renderer::renderProgressBarContents(const StyleOptions &options)
{
    return renderProgressBarGroove(options);
}

void Renderer::renderProgressBarBusyContents(const StyleOptions &options, bool horizontal, bool reverse, int progress)
{
    Q_UNUSED(reverse)

    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF baseRect(options.rect());
    QRectF contentRect;
    if (horizontal) {
        contentRect = QRectF(baseRect.left(), baseRect.top(), Metrics::ProgressBar_BusyIndicatorSize, baseRect.height());
        contentRect.translate(fabs(progress - 50) / 50.0 * (baseRect.width() - contentRect.width()), 0);
    } else {
        contentRect = QRectF(baseRect.left(), baseRect.top(), baseRect.width(), Metrics::ProgressBar_BusyIndicatorSize);
        contentRect.translate(0, fabs(progress - 50) / 50.0 * (baseRect.height() - contentRect.height()));
    }

    qreal radius = qMin(contentRect.width(), contentRect.height()) / 2.0;

    options.painter()->setBrush(options.color());
    options.painter()->setPen(Qt::NoPen);
    options.painter()->drawRoundedRect(contentRect, radius, radius);
    options.painter()->restore();
}

void Renderer::renderScrollBarGroove(const StyleOptions &options)
{
    return renderScrollBarHandle(options);
}

void Renderer::renderScrollBarHandle(const StyleOptions &options)
{
    if (!options.painter()) {
        return;
    }

    // setup options.painter()
    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    QRectF baseRect(options.rect());
    qreal metric(options.rect().width() < options.rect().height() ? options.rect().width() : options.rect().height());
    qreal radius(0.5 * metric);

    // content
    if (options.color().isValid()) {
        options.painter()->setPen(Qt::NoPen);
        options.painter()->setBrush(options.color());
        options.painter()->drawRoundedRect(baseRect, radius, radius);
    }

    options.painter()->restore();
}

void Renderer::renderToolBarHandle(const StyleOptions &options)
{
    return renderProgressBarGroove(options);
}

void Renderer::renderTabBarTab(const StyleOptions &options, const QColor &background, Corners corners, bool renderFrame)
{
    if (!options.painter()) {
        return;
    }

    Q_UNUSED(corners);
    Q_UNUSED(renderFrame);

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);

    // Deflate the rect for the floating pill look
    constexpr qreal hInset = 8.0;
    constexpr qreal vInset = 7.0;
    constexpr qreal pillRadius = 6.0;
    QRectF pillRect(options.rect());
    pillRect.adjust(hInset, vInset, -hInset, -vInset);

    // Draw the pill background if it has a valid color
    if (background.isValid() && background.alpha() > 0) {
        options.painter()->setPen(Qt::NoPen);
        options.painter()->setBrush(background);
        options.painter()->drawRoundedRect(pillRect, pillRadius, pillRadius);
    }

    // Draw subtle hairline outline if provided
    if (options.outlineColor().isValid()) {
        options.painter()->setPen(QPen(options.outlineColor(), 1.0));
        options.painter()->setBrush(Qt::NoBrush);
        options.painter()->drawRoundedRect(pillRect.adjusted(0.5, 0.5, -0.5, -0.5), pillRadius, pillRadius);
    }

    options.painter()->restore();
}

void Renderer::renderArrow(const StyleOptions &options, ArrowOrientation arrowOrientation)
{
    if (!options.painter() || !options.color().isValid() || arrowOrientation == ArrowNone) {
        return;
    }

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);
    options.painter()->translate(QRectF(options.rect()).center());

    QPen pen(options.color(), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    options.painter()->setPen(pen);
    options.painter()->setBrush(Qt::NoBrush);

    QPolygonF chevron;
    switch (arrowOrientation) {
    case ArrowUp:
        chevron << QPointF(-3.5, 1.5) << QPointF(0.0, -2.0) << QPointF(3.5, 1.5);
        break;
    case ArrowDown:
        chevron << QPointF(-3.5, -1.5) << QPointF(0.0, 2.0) << QPointF(3.5, -1.5);
        break;
    case ArrowLeft:
        chevron << QPointF(1.5, -3.5) << QPointF(-2.0, 0.0) << QPointF(1.5, 3.5);
        break;
    case ArrowRight:
        chevron << QPointF(-1.5, -3.5) << QPointF(2.0, 0.0) << QPointF(-1.5, 3.5);
        break;
    default:
        break;
    }

    options.painter()->drawPolyline(chevron);
    options.painter()->restore();
}

void Renderer::renderSign(const StyleOptions &options, bool orientation)
{
    if (!options.painter() || !options.color().isValid()) {
        return;
    }

    QPen pen(options.color(), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QRectF r = QRectF(options.rect());

    options.painter()->save();
    options.painter()->setRenderHint(QPainter::Antialiasing, true);
    options.painter()->setPen(pen);
    options.painter()->setBrush(Qt::NoBrush);
    options.painter()->drawLine(r.center() - QPointF(3.5, 0), r.center() + QPointF(3.5, 0));
    if (orientation) {
        options.painter()->drawLine(r.center() - QPointF(0, 3.5), r.center() + QPointF(0, 3.5));
    }
    options.painter()->restore();
}

void Renderer::renderDecorationButton(const StyleOptions &options, ButtonType buttonType)
{
    if (!options.painter()) {
        return;
    }

    options.painter()->save();
    options.painter()->setViewport(options.rect());
    options.painter()->setWindow(0, 0, 18, 18);
    options.painter()->setRenderHints(QPainter::Antialiasing, true);

    // initialize pen
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    options.painter()->setBrush(Qt::NoBrush);

    pen.setColor(options.color());
    pen.setWidthF(2.0 * qMax(1.0, 18.0 / options.rect().width()));
    options.painter()->setPen(pen);

    switch (buttonType) {
    case ButtonClose: {
        options.painter()->drawLine(QPointF(5, 5), QPointF(13, 13));
        options.painter()->drawLine(QPointF(13, 5), QPointF(5, 13));
        break;
    }
    case ButtonMaximize: {
        options.painter()->drawRoundedRect(QRectF(5, 5, 8, 8), 1.5, 1.5);
        break;
    }
    case ButtonMinimize: {
        options.painter()->drawLine(QPointF(5, 13), QPointF(13, 13));
        break;
    }
    case ButtonRestore: {
        options.painter()->drawRoundedRect(QRectF(6, 6, 6, 6), 1.0, 1.0);
        break;
    }
    default:
        break;
    }

    options.painter()->restore();
}

} // namespace Adwaita
