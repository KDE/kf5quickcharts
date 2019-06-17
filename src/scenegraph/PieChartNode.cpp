#include "PieChartNode_p.h"

#include <QColor>
#include <QSGGeometry>
#include <cmath>

#include "PieChartMaterial_p.h"

static const qreal pi = std::acos(-1.0);
static const qreal sectionSize = pi * 0.5;

inline QVector2D rotated(const QVector2D vector, qreal angle)
{
    auto newX = vector.x() * std::cos(angle) - vector.y() * std::sin(angle);
    auto newY = vector.x() * std::sin(angle) + vector.y() * std::cos(angle);
    return QVector2D(newX, newY);
}

PieChartNode::PieChartNode()
    : PieChartNode(QRectF {})
{
}

PieChartNode::PieChartNode(const QRectF &rect)
{
    m_geometry = new QSGGeometry { QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 };
    QSGGeometry::updateTexturedRectGeometry(m_geometry, rect, QRectF { 0, 0, 1, 1 });
    setGeometry(m_geometry);

    m_material = new PieChartMaterial {};
    setMaterial(m_material);

    setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial);
}

PieChartNode::~PieChartNode()
{
}

void PieChartNode::setRect(const QRectF &rect)
{
    if (rect == m_rect)
        return;

    m_rect = rect;
    QSGGeometry::updateTexturedRectGeometry(m_geometry, m_rect, QRectF { 0, 0, 1, 1 });
    markDirty(QSGNode::DirtyGeometry);

    auto minDimension = qMin(m_rect.width(), m_rect.height());

    QVector2D aspect { 1.0, 1.0 };
    aspect.setX(rect.width() / minDimension);
    aspect.setY(rect.height() / minDimension);
    m_material->setAspectRatio(aspect);

    m_material->setInnerDimension((minDimension / 2 - m_borderWidth) / (minDimension / 2));
}

void PieChartNode::setBorderWidth(qreal width)
{
    if (qFuzzyCompare(width, m_borderWidth))
        return;

    m_borderWidth = width;
    auto minDimension = qMin(m_rect.height(), m_rect.width()) / 2;
    m_material->setInnerDimension((minDimension - m_borderWidth) / minDimension);
}

void PieChartNode::setColors(const QVector<QColor> &colors)
{
    m_colors = colors;
    updateTriangles();
}

void PieChartNode::setSections(const QVector<qreal> &sections)
{
    m_sections = sections;
    updateTriangles();
}

void PieChartNode::updateTriangles()
{
    if (m_sections.isEmpty() || m_sections.size() != m_colors.size())
        return;

    QVector<QVector4D> triangleColors;
    QVector<QVector2D> trianglePoints;

    auto total = std::accumulate(m_sections.begin(), m_sections.end(), 0.0);
    static const float overlap = 0.01;

    QVector2D point = total < 1.0 ? QVector2D{ 0.0, -2.0 } : rotated(QVector2D{ 0.0, -2.0 }, -overlap);
    auto index = 0;
    auto current = m_sections.at(0) * pi * 2.0 + (total < 1.0 ? 0.0 : overlap);

    while (index < m_sections.size()) {
        auto angle = (current - sectionSize > 0.0) ? sectionSize : current;

        trianglePoints << point;
        trianglePoints << rotated(point, index >= m_sections.size() - 1 ? angle : angle + overlap);

        auto color = QVector4D { float(m_colors.at(index).redF()), float(m_colors.at(index).greenF()), float(m_colors.at(index).blueF()), float(m_colors.at(index).alphaF()) };
        triangleColors << color;

        point = rotated(point, angle);
        current -= angle;

        while (qFuzzyCompare(current, 0.0)) {
            index++;
            if (index < m_sections.size()) {
                current = m_sections.at(index) * pi * 2.0;
            } else {
                break;
            }
        }
    }

    m_material->setTriangles(trianglePoints);
    m_material->setColors(triangleColors);

    markDirty(QSGNode::DirtyMaterial);
}
