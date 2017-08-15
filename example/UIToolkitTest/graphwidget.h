/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <cmath>
#include <QVBoxLayout>
#ifndef Q_MOC_RUN
#include <chaos/common/utility/SingleBufferCircularBuffer.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/random.hpp>
#include <chaos/common/chaos_constants.h>
#endif
#include <qwt_system_clock.h>


class QwtPlotDirectPainter;

class GraphWidget : public QWidget
{
    struct PlotBufferAndCurve{
        chaos::DataType::DataType dataType;
        chaos::common::utility::DataBuffer *curveBuffer;
        QwtPlotCurve *curve;
        ~PlotBufferAndCurve(){
            if(curve) delete(curve);
        }
    };
    struct PlotPtrBufferAndCurve{
        chaos::DataType::DataType dataType;
        chaos::common::utility::PointerBuffer *curvePointer;
        QwtPlotCurve *curve;
        ~PlotPtrBufferAndCurve(){
            if(curve) delete(curve);
        }
    };
    friend class MainWindow;
    Q_OBJECT
    QwtPlot *plot;
    QwtPlotGrid *grid;
    QVBoxLayout *vbox;
    int numberOfPoint;
    int d_timerId;
    QwtPlotDirectPainter *d_directPainter;

    std::vector<double> xs;
    std::map<std::string, boost::shared_ptr<PlotBufferAndCurve> > plotMap;
    std::map<std::string, boost::shared_ptr<PlotPtrBufferAndCurve> > pointerPlotMap;
public:
    boost::mutex manageMutex;
    explicit GraphWidget(QWidget *parent = 0);
    ~GraphWidget();
    void addNewPlot(chaos::common::utility::DataBuffer *dataBuffer, std::string& plotName, chaos::DataType::DataType dataType);
    void addNewPlot(chaos::common::utility::PointerBuffer *ptrBuffer, std::string& plotName, chaos::DataType::DataType dataType);
    void removePlot(std::string& plotName);
    void update();
    bool hasPlot(std::string& plotName);
    void clearAllPlot();
    void start();
    void stop();
    void timerEvent(QTimerEvent *event);
    void setPointNumber(int points);
    void setMinMaxGrid(int min, int max);
signals:
public slots:

};

#endif // GRAPHWIDGET_H
