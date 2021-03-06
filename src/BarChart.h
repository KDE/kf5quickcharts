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

#ifndef BARCHART_H
#define BARCHART_H

#include "XYChart.h"

/**
 * A bar chart.
 */
class BarChart : public XYChart
{
    Q_OBJECT
    /**
     * The spacing between bars for each value source.
     *
     * Note that spacing between each X axis value is determined automatically based on barWidth, spacing and
     * total chart width.
     */
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    /**
     * The width of individual bars in the chart.
     *
     * If set to WidthMode::AutoWidth, the width will be calculated automatically based on total chart width and
     * item count.
     */
    Q_PROPERTY(qreal barWidth READ barWidth WRITE setBarWidth NOTIFY barWidthChanged)

public:
    /**
     * Helper enum to provide an easy way to set barWidth to auto.
     */
    enum WidthMode { AutoWidth = -2 };
    Q_ENUM(WidthMode)

    explicit BarChart(QQuickItem *parent = nullptr);

    qreal spacing() const;
    void setSpacing(qreal newSpacing);
    Q_SIGNAL void spacingChanged();

    qreal barWidth() const;
    void setBarWidth(qreal newBarWidth);
    Q_SIGNAL void barWidthChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *) override;
    void onDataChanged() override;

private:
    qreal m_spacing = 0.0;
    qreal m_barWidth = AutoWidth;
    QVector<QVector<QPair<qreal, QColor>>> m_values;
};

#endif // BARCHART_H
