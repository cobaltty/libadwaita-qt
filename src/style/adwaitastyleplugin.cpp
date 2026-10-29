
/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
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

#include "adwaitastyleplugin.h"
#include "adwaitastyle.h"
#include "adwaita.h"

#include <QApplication>
#include <QDebug>

namespace Adwaita
{

QStyle *StylePlugin::create(const QString &key)
{
    qWarning() << "LibAdwaita PLUGIN: create() called with key:" << key;
    const QString lower = key.toLower();

    if (lower == QStringLiteral("libadwaita") || lower == QStringLiteral("adwaita")) {
        qWarning() << "LibAdwaita PLUGIN: returning custom Adwaita style";
        return new Style(Adwaita::Adwaita);
    }

    if (lower == QStringLiteral("libadwaita-dark") || lower == QStringLiteral("adwaita-dark")) {
        return new Style(Adwaita::AdwaitaDark);
    }

    if (lower == QStringLiteral("libadwaita-light") || lower == QStringLiteral("adwaita-light")) {
        return new Style(Adwaita::Adwaita);
    }

    if (lower == QStringLiteral("libadwaita-highcontrast") ||
        lower == QStringLiteral("highcontrast")) {
        return new Style(Adwaita::AdwaitaHighcontrast);
    }

    if (lower == QStringLiteral("libadwaita-highcontrastinverse") ||
        lower == QStringLiteral("highcontrastinverse")) {
        return new Style(Adwaita::AdwaitaHighcontrastInverse);
    }

    return nullptr;
}

StylePlugin::~StylePlugin()
{
}

QStringList StylePlugin::keys() const
{
    return QStringList()
        << QStringLiteral("LibAdwaita") << QStringLiteral("LibAdwaita-Dark") << QStringLiteral("LibAdwaita-Light")
        << QStringLiteral("Adwaita") << QStringLiteral("Adwaita-Dark") << QStringLiteral("Adwaita-Light")
        << QStringLiteral("LibAdwaita-HighContrast") << QStringLiteral("LibAdwaita-HighContrastInverse");
}

} // namespace Adwaita
