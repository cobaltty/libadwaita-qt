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

#include "adwaitacolors.h"
#include "adwaitacolors_p.h"
#include "animations/adwaitaanimationdata.h"

#include <QtGlobal>
#include <QGuiApplication>
#include <QtMath>

namespace Adwaita
{

Q_GLOBAL_STATIC(ColorsPrivate, colorsGlobal)

static const qreal arrowShade = 0.15;

// ── Named color ID enum ──────────────────────────────────────────
// Indexes into AdwaitaPalette.  Used by adwaitaColor() so callers
// don't need to know the struct layout.
enum ColorId {
    BaseColor = 0,
    TextColor,
    BgColor,
    FgColor,
    SelectedFgColor,
    SelectedBgColor,
    SelectedBordersColor,
    BordersColor,
    AltBordersColor,
    BordersEdge,
    LinkColor,
    LinkVisitedColor,
    TopHilight,
    DarkFill,
    HeaderbarBgColor,
    MenuColor,
    MenuSelectedColor,
    SidebarBgColor,
    ScrollbarBgColor,
    ScrollbarSliderColor,
    ScrollbarSliderHoverColor,
    ScrollbarSliderActiveColor,
    WarningColor,
    ErrorColor,
    SuccessColor,
    DestructiveColor,
    OsdFgColor,
    OsdTextColor,
    OsdBgColor,
    OsdInsensitiveBgColor,
    OsdInsensitiveFgColor,
    OsdBordersColor,
    TooltipBordersColor,
    ShadowColor,
    DropTargetColor,
    InsensitiveFgColor,
    InsensitiveBgColor,
    InsensitiveBordersColor,
    BackdropBaseColor,
    BackdropTextColor,
    BackdropBgColor,
    BackdropFgColor,
    BackdropInsensitiveColor,
    BackdropSelectedFgColor,
    BackdropBordersColor,
    BackdropDarkFill,
    SuggestedBgColor,
    SuggestedBorderColor,
    ProgressBgColor,
    ProgressBorderColor,
    CheckradioBgColor,
    CheckradioFgColor,
    CheckradioBordersColor,
    SwitchBgColor,
    SwitchBordersColor,
    FocusBorderColor,
    AltFocusBorderColor,
    ColorIdCount
};

// Resolve a ColorId to the actual QColor in a palette struct.
static const QColor &resolveColorId(const AdwaitaPalette &p, int id)
{
    // Offsets must match the ColorId enum order exactly.
    static const auto offsets = [] {
        // Pointer-to-member tricks won't work across QColor's internal
        // layout, so we use an array of lambda accessors instead.
        using F = const QColor &(*) (const AdwaitaPalette &);
        static const F table[] = {
            [](const AdwaitaPalette &s) -> const QColor &{ return s.base; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.text; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.bg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.fg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.selectedFg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.selectedBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.selectedBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.borders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.altBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.bordersEdge; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.link; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.linkVisited; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.topHilight; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.darkFill; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.headerbarBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.menuColor; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.menuSelected; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.sidebarBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.scrollbarBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.scrollbarSlider; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.scrollbarSliderHover; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.scrollbarSliderActive; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.warning; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.error; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.success; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.destructive; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.osdFg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.osdText; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.osdBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.osdInsensitiveBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.osdInsensitiveFg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.osdBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.tooltipBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.shadow; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.dropTarget; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.insensitiveFg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.insensitiveBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.insensitiveBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropBase; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropText; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropFg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropInsensitive; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropSelectedFg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.backdropDarkFill; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.suggestedBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.suggestedBorder; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.progressBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.progressBorder; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.checkradioBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.checkradioFg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.checkradioBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.switchBg; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.switchBorders; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.focusBorder; },
            [](const AdwaitaPalette &s) -> const QColor &{ return s.altFocusBorder; },
        };
        return table;
    }();
    return offsets[id](p);
}

// ── AdwaitaPalette: official GNOME/Libadwaita hex values ──────────

const AdwaitaPalette &AdwaitaPalette::light()
{
    static const AdwaitaPalette p = {
        /* base               */ QColor("#ffffff"),
        /* text               */ Qt::black,
        /* bg                 */ QColor("#fafafa"),
        /* fg                 */ QColor("#2e3436"),
        /* selectedFg         */ Qt::white,
        /* selectedBg         */ QColor("#3584e4"),
        /* selectedBorders    */ QColor("#185fb4"),
        /* borders            */ QColor("#cdc7c2"),
        /* altBorders         */ QColor("#bfb8b1"),
        /* bordersEdge        */ QColor(255, 255, 255, 204),
        /* link               */ QColor("#1b6acb"),
        /* linkVisited        */ QColor("#15539e"),
        /* topHilight         */ QColor(255, 255, 255, 204),
        /* darkFill           */ QColor("#e1dedb"),
        /* headerbarBg        */ Qt::white,
        /* menuColor          */ QColor("#ffffff"),
        /* menuSelected       */ QColor("#e8e6e3"),
        /* sidebarBg          */ QColor("#fbfafa"),
        /* scrollbarBg        */ QColor("#cecece"),
        /* scrollbarSlider    */ QColor("#7e8182"),
        /* scrollbarSliderHover */ QColor("#565b5c"),
        /* scrollbarSliderActive */ QColor("#1b6acb"),
        /* warning            */ QColor("#f57900"),
        /* error              */ QColor("#cc0000"),
        /* success            */ QColor("#33d17a"),
        /* destructive        */ QColor("#e01b24"),
        /* osdFg              */ QColor("#eeeeec"),
        /* osdText            */ Qt::white,
        /* osdBg              */ QColor(53, 53, 53, 178),
        /* osdInsensitiveBg   */ QColor(71, 71, 71, 127),
        /* osdInsensitiveFg   */ QColor("#919190"),
        /* osdBorders         */ QColor(0, 0, 0, 178),
        /* tooltipBorders     */ QColor(255, 255, 255, 25),
        /* shadow             */ QColor(0, 0, 0, 25),
        /* dropTarget         */ QColor("#2ec27e"),
        /* insensitiveFg      */ QColor("#929595"),
        /* insensitiveBg      */ QColor("#faf9f8"),
        /* insensitiveBorders */ QColor("#d5d0cc"),
        /* backdropBase       */ QColor("#fcfcfc"),
        /* backdropText       */ QColor("#323232"),
        /* backdropBg         */ QColor("#fafafa"),
        /* backdropFg         */ QColor("#929595"),
        /* backdropInsensitive */ QColor("#d4cfca"),
        /* backdropSelectedFg */ QColor("#fcfcfc"),
        /* backdropBorders    */ QColor("#d5d0cc"),
        /* backdropDarkFill   */ QColor("#eae8e6"),
        /* suggestedBg        */ QColor("#3584e4"),
        /* suggestedBorder    */ QColor("#185fb4"),
        /* progressBg         */ QColor("#3584e4"),
        /* progressBorder     */ QColor("#185fb4"),
        /* checkradioBg       */ QColor("#3584e4"),
        /* checkradioFg       */ Qt::white,
        /* checkradioBorders  */ QColor("#1b6acb"),
        /* switchBg           */ QColor("#3584e4"),
        /* switchBorders      */ QColor("#185fb4"),
        /* focusBorder        */ QColor(53, 132, 228, 127),
        /* altFocusBorder     */ QColor(255, 255, 255, 204),
    };
    return p;
}

const AdwaitaPalette &AdwaitaPalette::dark()
{
    static const AdwaitaPalette p = {
        /* base               */ QColor("#1e1e1e"),
        /* text               */ Qt::white,
        /* bg                 */ QColor("#242424"),
        /* fg                 */ QColor("#eeeeec"),
        /* selectedFg         */ Qt::white,
        /* selectedBg         */ QColor("#3584e4"),
        /* selectedBorders    */ QColor("#185fb4"),
        /* borders            */ QColor("#1b1b1b"),
        /* altBorders         */ QColor("#070707"),
        /* bordersEdge        */ QColor(238, 238, 236, 17),
        /* link               */ QColor("#3584e4"),
        /* linkVisited        */ QColor("#1b6acb"),
        /* topHilight         */ QColor(238, 238, 236, 17),
        /* darkFill           */ QColor("#282828"),
        /* headerbarBg        */ QColor("#242424"),
        /* menuColor          */ QColor("#383838"),
        /* menuSelected       */ QColor("#202020"),
        /* sidebarBg          */ QColor("#313131"),
        /* scrollbarBg        */ QColor("#313131"),
        /* scrollbarSlider    */ QColor("#a4a4a3"),
        /* scrollbarSliderHover */ QColor("#c9c9c7"),
        /* scrollbarSliderActive */ QColor("#1b6acb"),
        /* warning            */ QColor("#f57900"),
        /* error              */ QColor("#cc0000"),
        /* success            */ QColor("#26ab62"),
        /* destructive        */ QColor("#b2161d"),
        /* osdFg              */ QColor("#eeeeec"),
        /* osdText            */ Qt::white,
        /* osdBg              */ QColor(38, 38, 38, 178),
        /* osdInsensitiveBg   */ QColor(58, 58, 57, 127),
        /* osdInsensitiveFg   */ QColor("#8a8a89"),
        /* osdBorders         */ QColor(0, 0, 0, 178),
        /* tooltipBorders     */ QColor(255, 255, 255, 25),
        /* shadow             */ QColor(0, 0, 0, 25),
        /* dropTarget         */ QColor("#26a269"),
        /* insensitiveFg      */ QColor("#919190"),
        /* insensitiveBg      */ QColor("#323232"),
        /* insensitiveBorders */ QColor("#202020"),
        /* backdropBase       */ QColor("#1e1e1e"),
        /* backdropText       */ QColor("#d6d6d6"),
        /* backdropBg         */ QColor("#242424"),
        /* backdropFg         */ QColor("#919190"),
        /* backdropInsensitive */ QColor("#5b5b5b"),
        /* backdropSelectedFg */ QColor("#d6d6d6"),
        /* backdropBorders    */ QColor("#202020"),
        /* backdropDarkFill   */ QColor("#2e2e2e"),
        /* suggestedBg        */ QColor("#3584e4"),
        /* suggestedBorder    */ QColor("#185fb4"),
        /* progressBg         */ QColor("#3584e4"),
        /* progressBorder     */ QColor("#185fb4"),
        /* checkradioBg       */ QColor("#3584e4"),
        /* checkradioFg       */ Qt::white,
        /* checkradioBorders  */ QColor("#092444"),
        /* switchBg           */ QColor("#3584e4"),
        /* switchBorders      */ QColor("#185fb4"),
        /* focusBorder        */ QColor(21, 83, 158, 178),
        /* altFocusBorder     */ QColor(255, 255, 255, 76),
    };
    return p;
}

// ── ColorsPrivate ─────────────────────────────────────────────────

ColorsPrivate::ColorsPrivate()
{
}

QColor ColorsPrivate::adwaitaColor(int colorId, ColorVariant variant) const
{
    const AdwaitaPalette &p = (variant == AdwaitaDark) ? AdwaitaPalette::dark() : AdwaitaPalette::light();
    return resolveColorId(p, colorId);
}

// ── Color adjustment utilities ────────────────────────────────────

QColor Colors::alphaColor(QColor color, qreal alpha)
{
    if (alpha >= 0 && alpha < 1.0) {
        color.setAlphaF(alpha * color.alphaF());
    }
    return color;
}

QColor Colors::darken(const QColor &color, qreal amount)
{
    float h, s, l, a;
    color.getHslF(&h, &s, &l, &a);
    return QColor::fromHslF(h, s, qBound(0.0f, l - amount, 1.0f), a);
}

QColor Colors::desaturate(const QColor &color, qreal amount)
{
    float h, s, l, a;
    color.getHslF(&h, &s, &l, &a);
    return QColor::fromHslF(h, qBound(0.0f, s - amount, 1.0f), l, a);
}

QColor Colors::lighten(const QColor &color, qreal amount)
{
    float h, s, l, a;
    color.getHslF(&h, &s, &l, &a);
    return QColor::fromHslF(h, s, qBound(0.0f, l + amount, 1.0f), a);
}

QColor Colors::mix(const QColor &c1, const QColor &c2, qreal bias)
{
    if (bias <= 0.0) return c1;
    if (bias >= 1.0) return c2;
    if (std::isnan(bias)) return c1;

    return QColor::fromRgbF(
        (1.0 - bias) * c1.redF()   + bias * c2.redF(),
        (1.0 - bias) * c1.greenF() + bias * c2.greenF(),
        (1.0 - bias) * c1.blueF()  + bias * c2.blueF(),
        (1.0 - bias) * c1.alphaF() + bias * c2.alphaF());
}

QColor Colors::transparentize(const QColor &color, qreal amount)
{
    float h, s, l, a;
    color.getHslF(&h, &s, &l, &a);
    return QColor::fromHslF(h, s, l, qBound(0.0f, a - amount, 1.0f));
}

QColor Colors::contrastTextColor(const QColor &bg)
{
    // Perceived luminance (ITU-R BT.601)
    const double luma = 0.299 * bg.redF() + 0.587 * bg.greenF() + 0.114 * bg.blueF();
    // If background is light (e.g. pastel yellow/lime), use dark text; otherwise use pure white
    return (luma > 0.62) ? QColor(20, 20, 20) : Qt::white;
}

// ── Public API: palette construction ──────────────────────────────

QPalette Colors::disabledPalette(const QPalette &source, qreal ratio)
{
    QPalette copy(source);
    const QList<QPalette::ColorRole> roles = {
        QPalette::Window, QPalette::Highlight, QPalette::WindowText,
        QPalette::ButtonText, QPalette::Text, QPalette::Button
    };
    for (const auto &role : roles) {
        copy.setColor(role, Colors::mix(
            source.color(QPalette::Active, role),
            source.color(QPalette::Disabled, role),
            1.0 - ratio));
    }
    return copy;
}

QPalette Colors::palette(ColorVariant variant)
{
    // Resolve variant: Unknown defaults to dark.
    if (variant == ColorVariant::Unknown) {
        variant = ColorVariant::AdwaitaDark;
    }

    const AdwaitaPalette &p = (variant == AdwaitaDark)
        ? AdwaitaPalette::dark()
        : AdwaitaPalette::light();

    QPalette palette;

    // ── Active ────────────────────────────────────────────────────
    palette.setColor(QPalette::Active, QPalette::Window,          p.bg);
    palette.setColor(QPalette::Active, QPalette::WindowText,      p.fg);
    palette.setColor(QPalette::Active, QPalette::Base,            p.base);
    palette.setColor(QPalette::Active, QPalette::AlternateBase,   p.base);
    palette.setColor(QPalette::Active, QPalette::ToolTipBase,     p.osdBg);
    palette.setColor(QPalette::Active, QPalette::ToolTipText,     p.osdText);
    palette.setColor(QPalette::Active, QPalette::PlaceholderText, p.insensitiveFg);
    palette.setColor(QPalette::Active, QPalette::Text,            p.fg);
    palette.setColor(QPalette::Active, QPalette::Button,          p.bg);
    palette.setColor(QPalette::Active, QPalette::ButtonText,      p.fg);
    palette.setColor(QPalette::Active, QPalette::BrightText,      p.text);
    palette.setColor(QPalette::Active, QPalette::Light,           Colors::lighten(p.bg));
    palette.setColor(QPalette::Active, QPalette::Midlight,        Colors::mix(Colors::lighten(p.bg), p.bg));
    palette.setColor(QPalette::Active, QPalette::Mid,             Colors::mix(Colors::darken(p.bg), p.bg));
    palette.setColor(QPalette::Active, QPalette::Dark,            Colors::darken(p.bg));
    palette.setColor(QPalette::Active, QPalette::Shadow,          p.shadow);
    palette.setColor(QPalette::Active, QPalette::Highlight,       p.selectedBg);
    palette.setColor(QPalette::Active, QPalette::HighlightedText, p.selectedFg);
    palette.setColor(QPalette::Active, QPalette::Link,            p.link);
    palette.setColor(QPalette::Active, QPalette::LinkVisited,     p.linkVisited);

    // ── Inactive (identical surfaces -- no Frankenstein split) ─────
    palette.setColor(QPalette::Inactive, QPalette::Window,          p.bg);
    palette.setColor(QPalette::Inactive, QPalette::WindowText,      p.fg);
    palette.setColor(QPalette::Inactive, QPalette::Base,            p.base);
    palette.setColor(QPalette::Inactive, QPalette::AlternateBase,   p.base);
    palette.setColor(QPalette::Inactive, QPalette::ToolTipBase,     p.osdBg);
    palette.setColor(QPalette::Inactive, QPalette::ToolTipText,     p.osdText);
    palette.setColor(QPalette::Inactive, QPalette::PlaceholderText, p.insensitiveFg);
    palette.setColor(QPalette::Inactive, QPalette::Text,            p.fg);
    palette.setColor(QPalette::Inactive, QPalette::Button,          p.bg);
    palette.setColor(QPalette::Inactive, QPalette::ButtonText,      p.fg);
    palette.setColor(QPalette::Inactive, QPalette::BrightText,      p.text);
    palette.setColor(QPalette::Inactive, QPalette::Light,           Colors::lighten(p.bg));
    palette.setColor(QPalette::Inactive, QPalette::Midlight,        Colors::mix(Colors::lighten(p.bg), p.bg));
    palette.setColor(QPalette::Inactive, QPalette::Mid,             Colors::mix(Colors::darken(p.bg), p.bg));
    palette.setColor(QPalette::Inactive, QPalette::Dark,            Colors::darken(p.bg));
    palette.setColor(QPalette::Inactive, QPalette::Shadow,          p.shadow);
    palette.setColor(QPalette::Inactive, QPalette::Highlight,       p.selectedBg);
    palette.setColor(QPalette::Inactive, QPalette::HighlightedText, p.selectedFg);
    palette.setColor(QPalette::Inactive, QPalette::Link,            p.link);
    palette.setColor(QPalette::Inactive, QPalette::LinkVisited,     p.linkVisited);

    // ── Disabled (dark surfaces, muted text) ──────────────────────
    palette.setColor(QPalette::Disabled, QPalette::Window,          p.bg);
    palette.setColor(QPalette::Disabled, QPalette::WindowText,      p.insensitiveFg);
    palette.setColor(QPalette::Disabled, QPalette::Base,            p.base);
    palette.setColor(QPalette::Disabled, QPalette::AlternateBase,   p.base);
    palette.setColor(QPalette::Disabled, QPalette::ToolTipBase,     p.osdBg);
    palette.setColor(QPalette::Disabled, QPalette::ToolTipText,     p.osdText);
    palette.setColor(QPalette::Disabled, QPalette::PlaceholderText, p.insensitiveFg);
    palette.setColor(QPalette::Disabled, QPalette::Text,            p.insensitiveFg);
    palette.setColor(QPalette::Disabled, QPalette::Button,          p.insensitiveBg);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText,      p.insensitiveFg);
    palette.setColor(QPalette::Disabled, QPalette::BrightText,      p.text);
    palette.setColor(QPalette::Disabled, QPalette::Light,           Colors::lighten(p.bg));
    palette.setColor(QPalette::Disabled, QPalette::Midlight,        Colors::mix(Colors::lighten(p.bg), p.bg));
    palette.setColor(QPalette::Disabled, QPalette::Mid,             Colors::mix(Colors::darken(p.bg), p.bg));
    palette.setColor(QPalette::Disabled, QPalette::Dark,            Colors::darken(p.bg));
    palette.setColor(QPalette::Disabled, QPalette::Shadow,          p.shadow);
    palette.setColor(QPalette::Disabled, QPalette::Highlight,       p.selectedBg);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, p.selectedFg);
    palette.setColor(QPalette::Disabled, QPalette::Link,            p.link);
    palette.setColor(QPalette::Disabled, QPalette::LinkVisited,     p.linkVisited);

    // ── All groups: force surface colors globally ──────────────────
    // Explicitly set Base and Window for QPalette::All so that no
    // color group can fall back to a stale/default value after a
    // dynamic variant switch (e.g. via XDG Desktop Portal).
    palette.setColor(QPalette::All, QPalette::Window,          p.bg);
    palette.setColor(QPalette::All, QPalette::WindowText,      p.fg);
    palette.setColor(QPalette::All, QPalette::Base,            p.base);
    palette.setColor(QPalette::All, QPalette::AlternateBase,   p.base);
    palette.setColor(QPalette::All, QPalette::Text,            p.fg);
    palette.setColor(QPalette::All, QPalette::Button,          p.bg);
    palette.setColor(QPalette::All, QPalette::ButtonText,      p.fg);
    palette.setColor(QPalette::All, QPalette::Highlight,       p.selectedBg);
    palette.setColor(QPalette::All, QPalette::HighlightedText, p.selectedFg);

    return palette;
}

// ── Widget color query functions ──────────────────────────────────

QColor Colors::hoverColor(const StyleOptions &options)
{
    return options.palette().highlight().color();
}

QColor Colors::focusColor(const StyleOptions &options)
{
    // Use the palette Highlight color (dynamic accent) instead of hardcoded blue
    return options.palette().color(QPalette::Highlight);
}

QColor Colors::negativeText(const StyleOptions &options)
{
    return colorsGlobal->adwaitaColor(ErrorColor, options.colorVariant());
}

QColor Colors::selectedMenuColor(const StyleOptions &options)
{
    return colorsGlobal->adwaitaColor(MenuSelectedColor, options.colorVariant());
}

QColor Colors::shadowColor(const StyleOptions &options)
{
    return alphaColor(options.palette().color(QPalette::Shadow), 0.15);
}

QColor Colors::titleBarColor(const StyleOptions &options)
{
    return options.palette().color(
        options.active() ? QPalette::Active : QPalette::Inactive, QPalette::Window);
}

QColor Colors::titleBarTextColor(const StyleOptions &options)
{
    return options.palette().color(
        options.active() ? QPalette::Active : QPalette::Inactive, QPalette::WindowText);
}

QColor Colors::arrowOutlineColor(const StyleOptions &options)
{
    switch (options.colorRole()) {
    case QPalette::Text:
        return mix(options.palette().color(options.colorGroup(), QPalette::Text),
                   options.palette().color(options.colorGroup(), QPalette::Base), arrowShade);
    case QPalette::WindowText:
        return mix(options.palette().color(options.colorGroup(), QPalette::WindowText),
                   options.palette().color(options.colorGroup(), QPalette::Window), arrowShade);
    case QPalette::ButtonText:
        return mix(options.palette().color(options.colorGroup(), QPalette::ButtonText),
                   options.palette().color(options.colorGroup(), QPalette::Button), arrowShade);
    default:
        return options.palette().text().color();
    }
}

QColor Colors::buttonOutlineColor(const StyleOptions &options)
{
    // Flat buttons: no border in any mode, only focus ring.
    if (options.hasFocus()) {
        return focusColor(options);
    }
    return QColor(); // invalid → Qt::NoPen
}

QColor Colors::indicatorOutlineColor(const StyleOptions &options)
{
    if (options.inMenu()) {
        if (options.palette().currentColorGroup() == QPalette::Disabled) {
            return buttonOutlineColor(options);
        }
        const qreal amount = (options.colorVariant() == AdwaitaDark
                              || options.colorVariant() == AdwaitaHighcontrastInverse) ? 0.18 : 0.24;
        return darken(options.palette().color(QPalette::Window), amount);
    }

    const bool checked = (options.checkboxState() == CheckOn
                          || options.checkboxState() == CheckPartial
                          || options.radioButtonState() == RadioOn);

    if (checked) {
        const QColor highlight = options.palette().color(QPalette::Highlight);
        const bool isDark = (options.colorVariant() == AdwaitaDark);
        return isDark ? Colors::darken(highlight, 0.15) : Colors::darken(highlight, 0.08);
    }

    // Unchecked border
    const QColor fg = options.palette().color(QPalette::WindowText);
    if (options.mouseOver() || options.animationMode() == AnimationHover) {
        return alphaColor(fg, 0.35);
    }
    return alphaColor(fg, 0.22);
}

QColor Colors::frameOutlineColor(const StyleOptions &options)
{
    // Modern Libadwaita frames:
    // Dark: subtle white hairline rgba(255,255,255,0.08)
    // Light: $borders_color = gtkmix(currentColor, $window_bg_color, 15%)
    //        = 85% bg + 15% fg ≈ subtle dark edge
    const bool isDark = (options.colorVariant() == AdwaitaDark);
    const QColor fg = options.palette().color(QPalette::WindowText);

    if (isDark) {
        return alphaColor(fg, 0.08);
    }

    // Light mode: subtle border from adw-gtk3 $borders_color
    const QColor window = options.palette().color(QPalette::Window);
    return Colors::mix(window, fg, 0.15);
}

QColor Colors::inputOutlineColor(const StyleOptions &options)
{
    QColor baseOutline = frameOutlineColor(options);
    if (options.animationMode() == AnimationFocus) {
        return mix(baseOutline, focusColor(options), options.opacity());
    } else if (options.hasFocus()) {
        return focusColor(options);
    }
    return baseOutline;
}

QColor Colors::sidePanelOutlineColor(const StyleOptions &options)
{
    QColor outline(options.palette().color(QPalette::Inactive, QPalette::Highlight));
    QColor focus(options.palette().color(QPalette::Active, QPalette::Highlight));
    if (options.animationMode() == AnimationFocus) {
        outline = mix(outline, focus, options.opacity());
    } else if (options.hasFocus()) {
        outline = focus;
    }
    return outline;
}

QColor Colors::sliderOutlineColor(const StyleOptions &options)
{
    QColor outline(mix(options.palette().color(QPalette::Window),
                       options.palette().color(QPalette::Shadow), 0.5));
    if (options.animationMode() == AnimationHover) {
        QColor h(hoverColor(options));
        QColor f(focusColor(options));
        outline = options.hasFocus() ? mix(f, h, options.opacity())
                                     : mix(outline, h, options.opacity());
    } else if (options.mouseOver()) {
        outline = hoverColor(options);
    } else if (options.animationMode() == AnimationFocus) {
        outline = mix(outline, focusColor(options), options.opacity());
    } else if (options.hasFocus()) {
        outline = focusColor(options);
    }
    return outline;
}

QColor Colors::buttonBackgroundColor(const StyleOptions &options)
{
    const bool isDark = (options.colorVariant() == AdwaitaDark);
    const bool isDisabled = options.palette().currentColorGroup() == QPalette::Disabled;
    const QColor window = options.palette().color(QPalette::Window);
    const QColor fg = options.palette().color(QPalette::WindowText);

    // Disabled: gtkmix($tc, $c, 5%) = 95% bg + 5% fg
    if (isDisabled) {
        return Colors::mix(window, fg, 0.05);
    }

    if (isDark) {
        // Dark mode: gtkmix($tc, $c, X%) — mix foreground into background
        if (options.animationMode() == AnimationPressed || options.sunken()) {
            return Colors::mix(window, fg, 0.30);
        }
        if (options.animationMode() == AnimationHover || options.mouseOver()) {
            return Colors::mix(window, fg, 0.15);
        }
        return Colors::mix(window, fg, 0.10);
    }

    // Light mode: rgba(0,0,0,X) overlays for clear separation from window
    if (options.animationMode() == AnimationPressed || options.sunken()) {
        return alphaColor(QColor(0, 0, 0), 0.16);
    }
    if (options.animationMode() == AnimationHover || options.mouseOver()) {
        return alphaColor(QColor(0, 0, 0), 0.11);
    }
    return alphaColor(QColor(0, 0, 0), 0.07);
}

QLinearGradient Colors::buttonBackgroundGradient(const StyleOptions &options)
{
    // Flat surface: top and bottom are the same color (no vertical
    // gradient).  This preserves the modern Libadwaita look and avoids
    // any legacy 3D bevel appearance.  The start and stop are identical
    // so the gradient degenerates to a solid fill.
    const QColor solid = buttonBackgroundColor(options);
    QLinearGradient g(options.rect().bottomLeft(), options.rect().topLeft());
    g.setColorAt(0, solid);
    g.setColorAt(1, solid);
    return g;
}

QColor Colors::checkBoxIndicatorColor(const StyleOptions &options)
{
    return Qt::white;
}

QColor Colors::headerTextColor(const StyleOptions &options)
{
    QColor col(options.palette().color(QPalette::WindowText));
    if (options.state() & QStyle::State_Enabled) {
        if (options.state() & QStyle::State_Sunken)    return Colors::alphaColor(col, 0.9);
        if (options.state() & QStyle::State_MouseOver) return Colors::alphaColor(col, 0.7);
    }
    return Colors::alphaColor(col, 0.5);
}

QColor Colors::indicatorBackgroundColor(const StyleOptions &options)
{
    if (options.inMenu()) {
        // Checked menu items: use accent color background for visible checkmark
        if (options.checkboxState() != CheckOff || options.radioButtonState() != RadioOff) {
            return options.palette().color(QPalette::Highlight);
        }
        return Qt::transparent;
    }

    const bool checked = (options.checkboxState() == CheckOn
                          || options.checkboxState() == CheckPartial
                          || options.radioButtonState() == RadioOn);

    if (checked) {
        return options.palette().color(QPalette::Highlight);
    }

    // Unchecked: clean white in light mode, subtle white overlay in dark mode
    const bool isDark = (options.colorVariant() == AdwaitaDark);
    if (isDark) {
        return alphaColor(QColor(255, 255, 255), 0.08);
    }
    return options.palette().color(QPalette::Base);
}

QLinearGradient Colors::indicatorBackgroundGradient(const StyleOptions &options)
{
    // Flat surface: same color top and bottom.  No vertical gradient.
    const QColor solid = indicatorBackgroundColor(options);
    QLinearGradient g(options.rect().bottomLeft(), options.rect().topLeft());
    if (solid.isValid()) {
        g.setColorAt(0, solid);
        g.setColorAt(1, solid);
    }
    return g;
}

QColor Colors::frameBackgroundColor(const StyleOptions &options)
{
    return Colors::mix(options.palette().color(options.colorGroup(), QPalette::Window),
                       options.palette().color(options.colorGroup(), QPalette::Base), 0.3);
}

QColor Colors::scrollBarHandleColor(const StyleOptions &options)
{
    const QColor fgC = options.palette().color(QPalette::WindowText);

    // Libadwaita neutral alpha scrollbars (never turns accent color)
    qreal alpha = 0.35;
    if (options.animationMode() == AnimationPressed || options.sunken()) {
        alpha = 0.70;
    } else if (options.animationMode() == AnimationHover || options.mouseOver()) {
        alpha = 0.55;
    }

    return alphaColor(fgC, alpha);
}

QColor Colors::separatorColor(const StyleOptions &options)
{
    return buttonOutlineColor(options);
}

QColor Colors::toolButtonColor(const StyleOptions &options)
{
    // adw-gtk3 flat button: transparent normal, gtkmix hover.
    // Normal: transparent (undecorated)
    // Hover: gtkmix($tc, $c, 15%) = 85% bg + 15% fg
    const QColor window = options.palette().color(QPalette::Window);
    const QColor fg = options.palette().color(QPalette::WindowText);

    if (options.sunken() || (options.animationMode() != AnimationNone
                            && options.animationMode() != AnimationHover)) {
        return buttonBackgroundColor(options);
    }

    if (options.animationMode() == AnimationHover || options.mouseOver()) {
        return Colors::mix(window, fg, 0.15);
    }

    return Qt::transparent;
}

QColor Colors::tabBarColor(const StyleOptions &options)
{
    const QColor window = options.palette().window().color();
    const bool isActive = options.state() & QStyle::State_Selected;

    // Active tab: subtle surface from window + shadow mix
    if (isActive) {
        return Colors::mix(window, options.palette().shadow().color(), 0.15);
    }

    // Inactive tab: transparent — let the tab bar background show through
    return Qt::transparent;
}

QColor Colors::cardBackgroundColor(const StyleOptions &options)
{
    // adw-gtk3 card background:
    // Light: #ffffff
    // Dark: rgba(255,255,255,0.08) — subtle white overlay on window bg
    const bool isDark = (options.colorVariant() == AdwaitaDark);
    const QColor window = options.palette().color(QPalette::Window);

    if (isDark) {
        // Dark mode: subtle white overlay
        return alphaColor(QColor(255, 255, 255), 0.08);
    }

    // Light mode: white card on slightly off-white window
    return QColor("#ffffff");
}

} // namespace Adwaita
