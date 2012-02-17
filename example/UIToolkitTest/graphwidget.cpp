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
#include <chaos/common/global.h>
#include <boost/pointer_cast.hpp>
#include <qwt_symbol.h>
#include <qwt_legend_item.h>
using namespace boost;
GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent)
{
    // QVBoxLayout verticalBox;
    plot = new QwtPlot(QwtText("Chaos Attribute Plot"));
    plot->setGeometry(0,0,640,400);
    plot->setAxisScale(QwtPlot::xBottom, 1, 30);
    plot->insertLegend(new QwtLegend(plot), QwtPlot::BottomLegend);
    vbox = new QVBoxLayout();
    vbox->addWidget(plot,1);
    setLayout(vbox);

    for (double x = 1; x <= 30; x++)
    {
        xs.push_back(x);
    }
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
    c->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
    newPlotInfo->curve = c;
    newPlotInfo->curveBuffer = dataBuffer;
    newPlotInfo->dataType = dataType;
    plotMap.insert(std::make_pair(plotName, newPlotInfo));
}

void GraphWidget::removePlot(std::string& plotName) {
    boost::mutex::scoped_lock  lock(manageMutex);
    plotMap[plotName]->curve->detach();
    plotMap.erase(plotName);
}

bool GraphWidget::hasPlot(std::string& plotName) {
    boost::mutex::scoped_lock  lock(manageMutex);
    return plotMap.count(plotName)>0;
}

void GraphWidget::update() {
    boost::mutex::scoped_lock  lock(manageMutex);
    PlotBufferAndCurve *tmpPlotInfoPtr = NULL;
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
        }
        tmpPlotInfoPtr->curve->setSamples(&xs[0],&ys[0], 29);
    }
    //
    emit updatePlot();
}

void GraphWidget::replot() {
    boost::mutex::scoped_lock  lock(manageMutex);
    plot->replot();
}

void GraphWidget::clearAllPlot() {
    boost::mutex::scoped_lock  lock(manageMutex);
    for(std::map<std::string, boost::shared_ptr<PlotBufferAndCurve> >::iterator iter = plotMap.begin();
        iter != plotMap.end();
        iter++){
        plotMap.erase(iter);
    }
}
