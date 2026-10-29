/*************************************************************************
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

#ifndef ADWAITA_COLORS_P_H
#define ADWAITA_COLORS_P_H

#include "adwaita.h"

#include <QColor>
#include <QColor>

namespace Adwaita
{

// ── Structured Libadwaita color palette ───────────────────────────
// Official GNOME/Libadwaita (GNOME 42+) hex values.  Each struct is
// a self-contained snapshot of one theme variant so the palette can
// be switched at runtime without any filesystem I/O.

struct AdwaitaPalette
{
    // Surface colors
    QColor base;
    QColor text;
    QColor bg;
    QColor fg;

    // Selection
    QColor selectedFg;
    QColor selectedBg;
    QColor selectedBorders;

    // Borders
    QColor borders;
    QColor altBorders;
    QColor bordersEdge;

    // Links
    QColor link;
    QColor linkVisited;

    // Accent / fills
    QColor topHilight;
    QColor darkFill;
    QColor headerbarBg;
    QColor menuColor;
    QColor menuSelected;
    QColor sidebarBg;

    // Scrollbar
    QColor scrollbarBg;
    QColor scrollbarSlider;
    QColor scrollbarSliderHover;
    QColor scrollbarSliderActive;

    // Status
    QColor warning;
    QColor error;
    QColor success;
    QColor destructive;

    // OSD / tooltips
    QColor osdFg;
    QColor osdText;
    QColor osdBg;
    QColor osdInsensitiveBg;
    QColor osdInsensitiveFg;
    QColor osdBorders;
    QColor tooltipBorders;
    QColor shadow;
    QColor dropTarget;

    // Insensitive state
    QColor insensitiveFg;
    QColor insensitiveBg;
    QColor insensitiveBorders;

    // Backdrop (inactive window) state
    QColor backdropBase;
    QColor backdropText;
    QColor backdropBg;
    QColor backdropFg;
    QColor backdropInsensitive;
    QColor backdropSelectedFg;
    QColor backdropBorders;
    QColor backdropDarkFill;

    // Special-purpose widget colors
    QColor suggestedBg;
    QColor suggestedBorder;
    QColor progressBg;
    QColor progressBorder;
    QColor checkradioBg;
    QColor checkradioFg;
    QColor checkradioBorders;
    QColor switchBg;
    QColor switchBorders;
    QColor focusBorder;
    QColor altFocusBorder;

    static const AdwaitaPalette &light();
    static const AdwaitaPalette &dark();
};

// ── ColorsPrivate ───────────────────────────────────────────────
// Provides indexed access into AdwaitaPalette.  Named palette colors
// are served directly from AdwaitaPalette::light()/dark().

class ColorsPrivate
{
public:
    ColorsPrivate();

    QColor adwaitaColor(int colorId, ColorVariant variant) const;
};

} // namespace Adwaita

#endif // ADWAITA_COLORS_P_H
