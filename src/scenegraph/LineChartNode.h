#ifndef LINECHARTNODE_H
#define LINECHARTNODE_H

#include <QSGNode>
#include <QColor>

class QRectF;
class LineChartMaterial;
class LineSegmentNode;

/**
 * @todo write docs
 */
class LineChartNode : public QSGNode
{
public:
    LineChartNode();

    /**
     * Destructor
     */
    ~LineChartNode();

    void setRect(const QRectF &rect);
    void setLineWidth(float width);
    void setLineColor(const QColor& color);
    void setFillColor(const QColor& color);
    void setValues(const QVector<qreal>& values);

private:
    void updatePoints();

    QRectF m_rect;
    float m_lineWidth = 0.0;
    QColor m_lineColor;
    QColor m_fillColor;
    QVector<qreal> m_values;
    QVector<LineSegmentNode*> m_segments;
};

#endif // LINECHARTNODE_H