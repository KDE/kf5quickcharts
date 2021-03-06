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

#ifndef BARCHARTNODE_H
#define BARCHARTNODE_H

#include <QSGGeometryNode>

/**
 * @todo write docs
 */
class BarChartNode : public QSGGeometryNode
{
public:
    /**
     * Default constructor
     */
    BarChartNode();

    /**
     * Destructor
     */
    ~BarChartNode();

    void setRect(const QRectF &rect);
    void setValues(const QVector<QPair<QVector2D, QColor>> &values);
    void setBarWidth(qreal width);
    void update();

private:
    void bar(QSGGeometry::ColoredPoint2D *vertices, quint16 *indices, int &index, const QRectF &bar, const QColor &color);

    QRectF m_rect;
    QVector<QPair<QVector2D, QColor>> m_values;
    qreal m_barWidth = 0.0;
    QSGGeometry *m_geometry = nullptr;
};

#endif // BARCHARTNODE_H
