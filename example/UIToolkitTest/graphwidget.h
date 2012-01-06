#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <cmath>
#include <QVBoxLayout>

class GraphWidget : public QWidget
{
    Q_OBJECT
    QwtPlot *plot;
    QwtPlotCurve *sine;
    QVBoxLayout *vbox;
public:
    explicit GraphWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // GRAPHWIDGET_H
