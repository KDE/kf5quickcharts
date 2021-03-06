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

#include "LegendModel.h"

#include "Chart.h"
#include "datasource/ChartDataSource.h"

LegendModel::LegendModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> LegendModel::roleNames() const
{
    static QHash<int, QByteArray> names = {
        {NameRole, "name"},
        {ColorRole, "color"},
        {ValueRole, "value"},
    };

    return names;
}

int LegendModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_items.size();
}

QVariant LegendModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::ParentIsInvalid | CheckIndexOption::IndexIsValid))
        return QVariant{};

    switch (role) {
    case NameRole:
        return m_items.at(index.row()).name;
    case ColorRole:
        return m_items.at(index.row()).color;
    case ValueRole:
        return m_items.at(index.row()).value;
    }

    return QVariant{};
}

Chart *LegendModel::chart() const
{
    return m_chart;
}

void LegendModel::setChart(Chart *newChart)
{
    if (newChart == m_chart) {
        return;
    }

    if (m_chart) {
        for (const auto &connection : qAsConst(m_connections)) {
            disconnect(connection);
        }
        m_connections.clear();
    }

    m_chart = newChart;
    queueUpdate();
    Q_EMIT chartChanged();
}

int LegendModel::sourceIndex() const
{
    return m_sourceIndex;
}

void LegendModel::setSourceIndex(int index)
{
    if (index == m_sourceIndex) {
        return;
    }

    m_sourceIndex = index;
    queueUpdate();
    Q_EMIT sourceIndexChanged();
}

void LegendModel::queueUpdate()
{
    if (!m_updateQueued) {
        m_updateQueued = true;
        QMetaObject::invokeMethod(this, &LegendModel::update, Qt::QueuedConnection);
    }
}

void LegendModel::update()
{
    m_updateQueued = false;

    if (!m_chart)
        return;

    beginResetModel();
    m_items.clear();

    ChartDataSource *colorSource = m_chart->colorSource();
    ChartDataSource *nameSource = m_chart->nameSource();
    ChartDataSource *valueSource = nullptr;

    m_connections.push_back(connect(m_chart, &Chart::colorSourceChanged, this, &LegendModel::queueUpdate, Qt::UniqueConnection));
    m_connections.push_back(connect(m_chart, &Chart::nameSourceChanged, this, &LegendModel::queueUpdate, Qt::UniqueConnection));

    auto sources = m_chart->valueSources();
    int itemCount = countItems();

    std::transform(sources.cbegin(), sources.cend(), std::back_inserter(m_connections), [this](ChartDataSource *source) {
        return connect(source, &ChartDataSource::dataChanged, this, &LegendModel::updateData, Qt::UniqueConnection);
    });

    m_connections.push_back(connect(m_chart, &Chart::valueSourcesChanged, this, &LegendModel::queueUpdate, Qt::UniqueConnection));

    if ((!colorSource && !nameSource) || itemCount <= 0) {
        endResetModel();
        return;
    }

    if (colorSource)
        m_connections.push_back(connect(colorSource, &ChartDataSource::dataChanged, this, &LegendModel::updateData, Qt::UniqueConnection));

    if (nameSource)
        m_connections.push_back(connect(nameSource, &ChartDataSource::dataChanged, this, &LegendModel::updateData, Qt::UniqueConnection));

    for (int i = 0; i < itemCount; ++i) {
        LegendItem item;
        item.name = nameSource ? nameSource->item(i).toString() : QString();
        item.color = colorSource ? colorSource->item(i).value<QColor>() : QColor();

        if (m_sourceIndex < 0) {
            item.value = m_chart->valueSources().at(i)->item(0);
        } else {
            item.value = valueSource ? valueSource->item(i) : QVariant{};
        }

        m_items.push_back(item);
    }

    endResetModel();
}

void LegendModel::updateData()
{
    ChartDataSource *colorSource = m_chart->colorSource();
    ChartDataSource *nameSource = m_chart->nameSource();
    ChartDataSource *valueSource = nullptr;

    auto itemCount = countItems();

    if (itemCount != int(m_items.size())) {
        // Number of items changed, so trigger a full update
        queueUpdate();
        return;
    }

    std::for_each(m_items.begin(), m_items.end(), [&, i = 0](LegendItem &item) mutable {
        item.name = nameSource ? nameSource->item(i).toString() : QString();
        item.color = colorSource ? colorSource->item(i).value<QColor>() : QColor();

        if (m_sourceIndex < 0) {
            item.value = m_chart->valueSources().at(i)->item(0);
        } else {
            item.value = valueSource ? valueSource->item(i) : QVariant{};
        }

        i++;
    });

    Q_EMIT dataChanged(index(0, 0), index(itemCount - 1, 0), {NameRole, ColorRole, ValueRole});
}

int LegendModel::countItems()
{
    auto sources = m_chart->valueSources();
    int itemCount = 0;

    switch (m_chart->indexingMode()) {
    case Chart::IndexSourceValues:
        if (sources.count() > 0) {
            itemCount = sources.at(0)->itemCount();
        }
        break;
    case Chart::IndexEachSource:
        itemCount = sources.count();
        break;
    case Chart::IndexAllValues:
        itemCount = std::accumulate(sources.cbegin(), sources.cend(), 0, [](int current, ChartDataSource *source) -> int {
            return current + source->itemCount();
        });
        break;
    }

    return itemCount;
}
