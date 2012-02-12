#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <cmath>
#include <QVBoxLayout>
#include <chaos/common/utility/SingleBufferCircularBuffer.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/random.hpp>
class GraphWidget : public QWidget
{
    struct PlotBufferAndCurve{
        chaos::DataBuffer *curveBuffer;
        QwtPlotCurve *curve;
        ~PlotBufferAndCurve(){
            if(curve) delete(curve);
        }
    };
    friend class MainWindow;
    Q_OBJECT
    QwtPlot *plot;
    QVBoxLayout *vbox;
    std::vector<double> xs;
    boost::mutex manageMutex;
    boost::mt19937 rng;
    boost::uniform_int<> one_to_six;
    boost::variate_generator< boost::mt19937, boost::uniform_int<> > randInt;
    std::map<std::string, boost::shared_ptr<PlotBufferAndCurve> > plotMap;
public:
    explicit GraphWidget(QWidget *parent = 0);
    void addNewPlot(chaos::DataBuffer *dataBuffer, std::string& plotName);
    void removePlot(std::string& plotName);
    void update();
    bool hasPlot(std::string& plotName);
    void replot();
    void clearAllPlot();
signals:
    void updatePlot();
public slots:

};

#endif // GRAPHWIDGET_H
