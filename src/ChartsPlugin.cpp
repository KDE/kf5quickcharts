#include "ChartsPlugin.h"

#include "ModelRole.h"
#include "PieChart.h"

void KQuickControlsAddonsChartsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QString::fromLatin1(uri) == QLatin1String("org.kde.quickcharts"));

    Q_INIT_RESOURCE(shaders);

    qmlRegisterType<PieChart>(uri, 1, 0, "PieChart");
    qmlRegisterUncreatableType<DataSource>(uri, 1, 0, "DataSource", QStringLiteral("Just a base class"));
    qmlRegisterType<ModelRole>(uri, 1, 0, "ModelRole");
}
