/*
 *	GraphWidget.cpp
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
#include "graphwidget.h"

#ifndef Q_MOC_RUN
#include <chaos/common/global.h>
#include <boost/pointer_cast.hpp>
#endif

#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_directpainter.h>
#include <qevent.h>


using namespace boost;
GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent)
{
    // QVBoxLayout verticalBox;
    plot = new QwtPlot(QwtText("Chaos Attribute Plot"));
    plot->setGeometry(0,0,640,400);
    plot->setAxisScale(QwtPlot::yLeft, -10, +10);
    plot->insertLegend(new QwtLegend(plot), QwtPlot::BottomLegend);
    vbox = new QVBoxLayout();
    vbox->addWidget(plot,1);
    setLayout(vbox);

    grid = new QwtPlotGrid();
    //enable the x and y axis lines
    grid->enableX(false);
    grid->enableY(true);

    //set the X and Y division and scale to that of the channels
    QwtScaleDiv axisInfo = plot->axisScaleDiv(QwtPlot::xBottom);
    grid->setXDiv(axisInfo);
    axisInfo = plot->axisScaleDiv(QwtPlot::yLeft);
    grid->setYDiv(axisInfo);
    grid->attach(plot);
    setPointNumber(30);
    d_timerId = -1;
    d_directPainter = new QwtPlotDirectPainter();
}

void GraphWidget::setMinMaxGrid(int min, int max) {
    plot->setAxisScale(QwtPlot::yLeft, min, max);
    plot->replot();
}

void GraphWidget::setPointNumber(int points){
    boost::mutex::scoped_lock  lock(manageMutex);
    numberOfPoint = points;
    plot->setAxisScale(QwtPlot::xBottom, 1, points);
    xs.clear();
    for (double x = 1; x <= numberOfPoint; x++)
    {
        xs.push_back(x);
    }
    plot->replot();
}

GraphWidget::~GraphWidget() {
    delete(d_directPainter);
}

void GraphWidget::addNewPlot(chaos::PointerBuffer *pointerBuffer, std::string& plotName , chaos::DataType::DataType dataType){
    boost::mutex::scoped_lock  lock(manageMutex);
    if(plotMap.count(plotName)>0) return;
    boost::shared_ptr<PlotPtrBufferAndCurve> newPlotInfo(new PlotPtrBufferAndCurve());
    boost::mt19937 rng((const uint_fast32_t) std::time(0) );
    boost::uniform_int<> one_to_six( 0, 255 );
    boost::variate_generator< boost::mt19937, boost::uniform_int<> > randInt(rng, one_to_six);

    QwtPlotCurve *c = new QwtPlotCurve(plotName.c_str());
    c->setPen(QPen(QColor(randInt(), randInt(), randInt())));
    c->setRenderHint(QwtPlotItem::RenderAntialiased);
    c->setStyle(QwtPlotCurve::NoCurve);
    c->attach(plot);
    c->setLegendAttribute(QwtPlotCurve::LegendShowLine);
    newPlotInfo->curve = c;
    newPlotInfo->curvePointer = pointerBuffer;
    newPlotInfo->dataType = dataType;
    pointerPlotMap.insert(std::make_pair(plotName, newPlotInfo));
}

void GraphWidget::addNewPlot(chaos::DataBuffer *dataBuffer, std::string& plotName , chaos::DataType::DataType dataType){
    boost::mutex::scoped_lock  lock(manageMutex);
    if(plotMap.count(plotName)>0) return;
    boost::shared_ptr<PlotBufferAndCurve> newPlotInfo(new PlotBufferAndCurve());
    boost::mt19937 rng((const uint_fast32_t) std::time(0) );
    boost::uniform_int<> one_to_six( 0, 255 );
    boost::variate_generator< boost::mt19937, boost::uniform_int<> > randInt(rng, one_to_six);

    QwtPlotCurve *c = new QwtPlotCurve(plotName.c_str());
    c->setPen(QPen(QColor(randInt(), randInt(), randInt())));
    c->setRenderHint(QwtPlotItem::RenderAntialiased);
    c->setStyle(QwtPlotCurve::Lines);
    c->attach(plot);
    c->setLegendAttribute(QwtPlotCurve::LegendShowLine);
    c->setRenderHint(QwtPlotItem::RenderAntialiased, false);
    c->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);

    newPlotInfo->curve = c;
    newPlotInfo->curveBuffer = dataBuffer;
    newPlotInfo->dataType = dataType;
    plotMap.insert(std::make_pair(plotName, newPlotInfo));
}

void GraphWidget::removePlot(std::string& plotName) {
    boost::mutex::scoped_lock  lock(manageMutex);
    if(plotMap.count(plotName)){
        plotMap[plotName]->curve->detach();
        plotMap.erase(plotName);
    } else if (pointerPlotMap.count(plotName)){
        pointerPlotMap[plotName]->curve->detach();
        pointerPlotMap.erase(plotName);
    }
}

bool GraphWidget::hasPlot(std::string& plotName) {
    boost::mutex::scoped_lock  lock(manageMutex);
    return plotMap.count(plotName)>0 || pointerPlotMap.count(plotName)>0;
}

void GraphWidget::update() {
    PlotBufferAndCurve *tmpPlotInfoPtr = NULL;
    PlotPtrBufferAndCurve *tmpPointeInfoPtr = NULL;
    std::vector<double> ys;
    for(std::map<std::string, boost::shared_ptr<PlotBufferAndCurve> >::iterator iter = plotMap.begin();
        iter != plotMap.end();
        iter++){
        ys.clear();
        tmpPlotInfoPtr = iter->second.get();
        if(tmpPlotInfoPtr->dataType == chaos::DataType::TYPE_INT32){
            int *wPtr = boost::reinterpret_pointer_cast<int>(tmpPlotInfoPtr->curveBuffer->getWritePointer());
            int *bPtr = boost::reinterpret_pointer_cast<int>(tmpPlotInfoPtr->curveBuffer->getBasePointer());
            int64_t historyDim = tmpPlotInfoPtr->curveBuffer->getDimension()-tmpPlotInfoPtr->curveBuffer->getWriteBufferPosition();
            int64_t recentToRead = tmpPlotInfoPtr->curveBuffer->getWriteBufferPosition();

            for (int idx = 0; idx < historyDim-1; idx++) {
                double historyDouble(*(wPtr + idx));
                ys.push_back(historyDouble);
            }
            if(bPtr != wPtr){
                for (int idx = 0; idx < recentToRead; idx++) {
                    double recentDouble(*(bPtr + idx));
                    ys.push_back(recentDouble);
                }
            }
        }else if(tmpPlotInfoPtr->dataType == chaos::DataType::TYPE_DOUBLE){
            double_t *wPtr = boost::reinterpret_pointer_cast<double_t>(tmpPlotInfoPtr->curveBuffer->getWritePointer());
            double_t *bPtr = boost::reinterpret_pointer_cast<double_t>(tmpPlotInfoPtr->curveBuffer->getBasePointer());
            int64_t historyDim = tmpPlotInfoPtr->curveBuffer->getDimension()-tmpPlotInfoPtr->curveBuffer->getWriteBufferPosition();
            int64_t recentToRead = tmpPlotInfoPtr->curveBuffer->getWriteBufferPosition();

            for (int idx = 0; idx < historyDim; idx++) {
                double historyDouble(*(wPtr + idx));
                ys.push_back(historyDouble);
            }
            if(bPtr != wPtr){
                for (int idx = 0; idx < recentToRead; idx++) {
                    double recentDouble(*(bPtr + idx));
                    ys.push_back(recentDouble);
                }
            }
        }
        tmpPlotInfoPtr->curve->setSamples(&xs[0],&ys[0], ys.size());
        //d_directPainter->drawSeries(tmpPlotInfoPtr->curve, 0, 29);
    }

    for(std::map<std::string, boost::shared_ptr<PlotPtrBufferAndCurve> >::iterator iter = pointerPlotMap.begin();
        iter != pointerPlotMap.end();
        iter++){
        ys.clear();
        tmpPointeInfoPtr = iter->second.get();
        if(tmpPointeInfoPtr->dataType == chaos::DataType::TYPE_BYTEARRAY){
            int32_t tipedBufLen = 0;
            boost::shared_ptr<double_t> sinWavePtr = tmpPointeInfoPtr->curvePointer->getTypedPtr<double_t>(tipedBufLen);
            double *tmpPtr = sinWavePtr.get();
            if(!tmpPtr) continue;
            for (int idx = 0; idx < tipedBufLen; idx++) {
                double historyDouble(*(tmpPtr + idx));
                ys.push_back(historyDouble);
            }
        }
        tmpPointeInfoPtr->curve->setSamples(&xs[0],&ys[0], ys.size());
    }
//
// emit updatePlot();
}

void GraphWidget::clearAllPlot() {
    boost::mutex::scoped_lock  lock(manageMutex);
    for(std::map<std::string, boost::shared_ptr<PlotBufferAndCurve> >::iterator iter = plotMap.begin();
        iter != plotMap.end();
        iter++){
        plotMap.erase(iter);
    }
    for(std::map<std::string, boost::shared_ptr<PlotPtrBufferAndCurve> >::iterator iter = pointerPlotMap.begin();
        iter != pointerPlotMap.end();
        iter++){
        pointerPlotMap.erase(iter);
    }
    plot->replot();
}
void GraphWidget::start()
{
    d_timerId = startTimer(100);
}

void GraphWidget::stop()
{
    if(d_timerId != -1) killTimer(d_timerId);
    d_timerId = -1;
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == d_timerId )
    {
        boost::mutex::scoped_lock  lock(manageMutex);
        update();
        plot->replot();
    }
    QWidget::timerEvent(event);
}
