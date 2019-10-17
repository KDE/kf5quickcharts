/*
 * This file is part of Quick Charts.
 * Copyright 2019 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RangeGroup.h"

#include <cmath>

#include <QVector>

RangeGroup::RangeGroup(QObject *parent)
    : QObject(parent)
{
    connect(this, &RangeGroup::fromChanged, this, &RangeGroup::rangeChanged);
    connect(this, &RangeGroup::toChanged, this, &RangeGroup::rangeChanged);
    connect(this, &RangeGroup::automaticChanged, this, &RangeGroup::rangeChanged);
    connect(this, &RangeGroup::minimumChanged, this, &RangeGroup::rangeChanged);
    connect(this, &RangeGroup::incrementChanged, this, &RangeGroup::rangeChanged);
}

qreal RangeGroup::from() const
{
    return m_from;
}

void RangeGroup::setFrom(qreal from)
{
    if (qFuzzyCompare(m_from, from)) {
        return;
    }

    m_from = from;
    Q_EMIT fromChanged();
}

qreal RangeGroup::to() const
{
    return m_to;
}

void RangeGroup::setTo(qreal to)
{
    if (qFuzzyCompare(m_to, to)) {
        return;
    }

    m_to = to;
    Q_EMIT toChanged();
}

bool RangeGroup::automatic() const
{
    return m_automatic;
}

void RangeGroup::setAutomatic(bool automatic)
{
    if (m_automatic == automatic) {
        return;
    }

    m_automatic = automatic;
    Q_EMIT automaticChanged();
}

qreal RangeGroup::distance() const
{
    return m_to - m_from;
}

qreal RangeGroup::minimum() const
{
    return m_minimum;
}

void RangeGroup::setMinimum(qreal newMinimum)
{
    if (newMinimum == m_minimum) {
        return;
    }

    m_minimum = newMinimum;
    Q_EMIT minimumChanged();
}

qreal RangeGroup::increment() const
{
    return m_increment;
}

void RangeGroup::setIncrement(qreal newIncrement)
{
    if (newIncrement == m_increment) {
        return;
    }

    m_increment = newIncrement;
    Q_EMIT incrementChanged();
}

bool RangeGroup::isValid() const
{
    return m_automatic || (m_to > m_from);
}

RangeGroup::RangeResult RangeGroup::calculateRange(const QVector<ChartDataSource *> &sources,
                                                   std::function<qreal(ChartDataSource*)> minimumCallback,
                                                   std::function<qreal(ChartDataSource*)> maximumCallback)
{
    RangeResult result;

    auto min = std::numeric_limits<qreal>::max();
    auto max = std::numeric_limits<qreal>::min();

    if (!m_automatic) {
        min = m_from;
        max = m_to;
    } else {
        std::for_each(sources.begin(), sources.end(), [&](ChartDataSource *source) {
            min = std::min(min, minimumCallback(source));
            max = std::max(max, maximumCallback(source));
        });
    }

    max = std::max(max, m_minimum);
    if (m_increment > 0.0) {
        max = m_increment * std::ceil(max / m_increment);
    }

    result.start = min;
    result.end = max;
    result.distance = max - min;

    return result;
}
