/*
 *	GraphWidget.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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
#include <chaos/common/cconstants.h>
#endif
#include <qwt_system_clock.h>


class QwtPlotDirectPainter;

class GraphWidget : public QWidget
{
    struct PlotBufferAndCurve{
        chaos::DataType::DataType dataType;
        chaos::DataBuffer *curveBuffer;
        QwtPlotCurve *curve;
        ~PlotBufferAndCurve(){
            if(curve) delete(curve);
        }
    };
    struct PlotPtrBufferAndCurve{
        chaos::DataType::DataType dataType;
        chaos::PointerBuffer *curvePointer;
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
    void addNewPlot(chaos::DataBuffer *dataBuffer, std::string& plotName, chaos::DataType::DataType dataType);
    void addNewPlot(chaos::PointerBuffer *ptrBuffer, std::string& plotName, chaos::DataType::DataType dataType);
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
