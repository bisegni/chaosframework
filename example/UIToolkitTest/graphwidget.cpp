#include "graphwidget.h"

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent)
{
       // QVBoxLayout verticalBox;
    plot = new QwtPlot(QwtText("CppQwtExample1"));
    plot->setGeometry(0,0,640,400);
    plot->setAxisScale(QwtPlot::xBottom, 0.0,2.0 * M_PI);
    plot->setAxisScale(QwtPlot::yLeft,-1.0,1.0);

    sine = new QwtPlotCurve("Sine");
    std::vector<double> xs;
    std::vector<double> ys;
    for (double x = 0; x < 2.0 * M_PI; x+=(M_PI / 10.0))
    {
        xs.push_back(x);
        ys.push_back(std::sin(x));
    }
    sine->setSamples(&xs[0],&ys[0],xs.size());
    sine->attach(plot);

    vbox = new QVBoxLayout;
    vbox->addWidget(plot,1);
    setLayout(vbox);
}
