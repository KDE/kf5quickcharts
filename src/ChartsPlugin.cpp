#include "ChartsPlugin.h"

#include "Chart.h"
#include "PieChart.h"
#include "XYChart.h"
#include "LineChart.h"
#include "BarChart.h"
#include "RangeGroup.h"

#include "decorations/GridLines.h"
#include "decorations/AxisLabels.h"
#include "decorations/LegendModel.h"

#include "datasource/ModelSource.h"
#include "datasource/SingleValueSource.h"
#include "datasource/ArraySource.h"
#include "datasource/ModelHistorySource.h"
#include "datasource/ChartAxisSource.h"

QuickChartsPlugin::QuickChartsPlugin(QObject* parent)
    : QQmlExtensionPlugin(parent)
{
}

void QuickChartsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QString::fromLatin1(uri) == QLatin1String("org.kde.quickcharts"));

    Q_INIT_RESOURCE(shaders);

    qmlRegisterType<PieChart>(uri, 1, 0, "PieChart");
    qmlRegisterType<LineChart>(uri, 1, 0, "LineChart");
    qmlRegisterType<BarChart>(uri, 1, 0, "BarChart");
    qmlRegisterUncreatableType<XYChart>(uri, 1, 0, "XYChart", QStringLiteral("Just a base class"));
    qmlRegisterUncreatableType<Chart>(uri, 1, 0, "Chart", QStringLiteral("Just a base class"));

    qmlRegisterUncreatableType<ChartDataSource>(uri, 1, 0, "ChartDataSource", QStringLiteral("Just a base class"));
    qmlRegisterType<ModelSource>(uri, 1, 0, "ModelSource");
    qmlRegisterType<SingleValueSource>(uri, 1, 0, "SingleValueSource");
    qmlRegisterType<ArraySource>(uri, 1, 0, "ArraySource");
    qmlRegisterType<ModelHistorySource>(uri, 1, 0, "ModelHistorySource");
    qmlRegisterType<ChartAxisSource>(uri, 1, 0, "ChartAxisSource");

    qmlRegisterUncreatableType<RangeGroup>(uri, 1, 0, "Range", QStringLiteral("Used as a grouped property"));

    qmlRegisterType<GridLines>(uri, 1, 0, "GridLines");
    qmlRegisterUncreatableType<LinePropertiesGroup>(uri, 1, 0, "LinePropertiesGroup", QStringLiteral("Used as a grouped property"));
    qmlRegisterType<AxisLabels>(uri, 1, 0, "AxisLabels");
    qmlRegisterUncreatableType<AxisLabelsAttached>(uri, 1, 0, "AxisLabelsAttached", QStringLiteral("Attached property"));
    qmlRegisterType<LegendModel>(uri, 1, 0, "LegendModel");
}
