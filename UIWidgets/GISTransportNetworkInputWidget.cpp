/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written by: Stevan Gavrilovic

#include "GISTransportNetworkInputWidget.h"
#include "QGISVisualizationWidget.h"
#include "GISAssetInputWidget.h"

#include <qgslinesymbol.h>
#include <qgsmarkersymbol.h>

#include <QFileDialog>
#include <QSplitter>
#include <QGroupBox>
#include <QVBoxLayout>

GISTransportNetworkInputWidget::GISTransportNetworkInputWidget(QWidget *parent, VisualizationWidget* visWidget) : SimCenterAppWidget(parent)
{
    theVisualizationWidget = static_cast<QGISVisualizationWidget*>(visWidget);
    assert(theVisualizationWidget);

    theNodesWidget = new GISAssetInputWidget(this, theVisualizationWidget, "Transport Network Nodes");

    theNodesWidget->setLabel1("Load Transportation network node information from a GIS file");

    thePipelinesWidget = new GISAssetInputWidget(this, theVisualizationWidget, "Transport Network Pipelines");

    thePipelinesWidget->setLabel1("Load Transportation network link information from a GIS file");

    connect(theNodesWidget,&GISAssetInputWidget::doneLoadingComponents,this,&GISTransportNetworkInputWidget::handleAssetsLoaded);
    connect(thePipelinesWidget,&GISAssetInputWidget::doneLoadingComponents,this,&GISTransportNetworkInputWidget::handleAssetsLoaded);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical);

    QGroupBox* nodesGB = new QGroupBox("Transport Network Nodes");
    nodesGB->setFlat(true);
    QVBoxLayout* nodesGBlayout = new QVBoxLayout(nodesGB);
    nodesGBlayout->addWidget(theNodesWidget);

    QGroupBox* pipelinesGB = new QGroupBox("Transport Network Pipelines");
    pipelinesGB->setFlat(true);
    QVBoxLayout* pipesGBlayout = new QVBoxLayout(pipelinesGB);
    pipesGBlayout->addWidget(thePipelinesWidget);

    verticalSplitter->addWidget(nodesGB);
    verticalSplitter->addWidget(pipelinesGB);

    mainLayout->addWidget(verticalSplitter);

    // Testing to remove start
    // theNodesWidget->setPathToComponentFile("/Users/steve/Desktop/SimCenter/Examples/SeasideTransport/Nodes/Seaside_wter_nodes.shp");
    // theNodesWidget->loadAssetData();
    // theNodesWidget->setCRS(QgsCoordinateReferenceSystem("EPSG:4326"));


    // thePipelinesWidget->setPathToComponentFile("/Users/steve/Desktop/SimCenter/Examples/SeasideTransport/Pipelines/Seaside_Transport_pipelines_wgs84.shp");
    // thePipelinesWidget->loadAssetData();
    // thePipelinesWidget->setCRS(QgsCoordinateReferenceSystem("EPSG:4326"));

    // inpFileLineEdit->setText("/Users/steve/Desktop/SogaExample/central.inp");
    // Testing to remove end
}


GISTransportNetworkInputWidget::~GISTransportNetworkInputWidget()
{

}


bool GISTransportNetworkInputWidget::copyFiles(QString &destName)
{

    auto res = theNodesWidget->copyFiles(destName);

    if(!res)
        return res;

    // The file containing the network pipelines
    res = thePipelinesWidget->copyFiles(destName);

    return res;
}


bool GISTransportNetworkInputWidget::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"]="GIS_to_TransportNETWORK";

    QJsonObject data;

    QJsonObject nodeData;
    // The file containing the network nodes
    auto res = theNodesWidget->outputAppDataToJSON(nodeData);

    if(!res)
    {
        this->errorMessage("Error, could not get the .json output from the nodes widget in GIS_to_TransportNETWORK");
        return false;
    }

    data["TransportNetworkNodes"] = nodeData;

    QJsonObject pipelineData;
    // The file containing the network pipelines
    res = thePipelinesWidget->outputAppDataToJSON(pipelineData);

    if(!res)
    {
        this->errorMessage("Error, could not get the .json output from the pipelines widget in GIS_to_TransportNETWORK");
        return false;
    }

    data["TransportNetworkPipelines"] = pipelineData;

    jsonObject["ApplicationData"] = data;

    return true;
}


bool GISTransportNetworkInputWidget::inputAppDataFromJSON(QJsonObject &jsonObject)
{

    // Check the app type
    if (jsonObject.contains("Application")) {
        if ("GIS_to_TransportNETWORK" != jsonObject["Application"].toString()) {
            this->errorMessage("GISTransportNetworkInputWidget::inputFRommJSON app name conflict");
            return false;
        }
    }


    if (!jsonObject.contains("ApplicationData"))
    {
        this->errorMessage("Could not find the 'ApplicationData' key in 'GISTransportNetworkInputWidget' input");
        return false;
    }

    QJsonObject appData = jsonObject["ApplicationData"].toObject();


    if (!appData.contains("TransportNetworkNodes") && !appData.contains("TransportNetworkPipelines"))
    {
        this->errorMessage("GISTransportNetworkInputWidget needs TransportNetworkNodes and TransportNetworkPipelines");
        return false;
    }

    QJsonObject nodesData = appData["TransportNetworkNodes"].toObject();

    // Input the nodes
    auto res = theNodesWidget->inputAppDataFromJSON(nodesData);

    if(!res)
        return res;


    QJsonObject pipelinesData = appData["TransportNetworkPipelines"].toObject();


    // Input the pipes
    res = thePipelinesWidget->inputAppDataFromJSON(pipelinesData);

    if(!res)
        return res;


    return true;
}


int GISTransportNetworkInputWidget::loadPipelinesVisualization()
{
    pipelinesMainLayer = thePipelinesWidget->getMainLayer();

    if(pipelinesMainLayer==nullptr)
        return -1;


    QgsLineSymbol* markerSymbol = new QgsLineSymbol();

    markerSymbol->setWidth(0.8);
    markerSymbol->setColor(Qt::darkBlue);
    theVisualizationWidget->createSimpleRenderer(markerSymbol,pipelinesMainLayer);

    //    auto numFeat = mainLayer->featureCount();

    theVisualizationWidget->zoomToLayer(pipelinesMainLayer);

    return 0;
}


int GISTransportNetworkInputWidget::loadNodesVisualization()
{
    nodesMainLayer = theNodesWidget->getMainLayer();

    if(nodesMainLayer==nullptr)
        return -1;

    QgsSymbol* markerSymbol = new QgsMarkerSymbol();

    markerSymbol->setColor(Qt::blue);
    theVisualizationWidget->createSimpleRenderer(markerSymbol,nodesMainLayer);

    //    auto numFeat = mainLayer->featureCount();

    theVisualizationWidget->zoomToLayer(nodesMainLayer);

    return 0;
}


void GISTransportNetworkInputWidget::clear()
{
    theNodesWidget->clear();
    thePipelinesWidget->clear();

    nodesMainLayer = nullptr;
    pipelinesMainLayer = nullptr;
}


void GISTransportNetworkInputWidget::handleAssetsLoaded()
{
    if(theNodesWidget->isEmpty() || thePipelinesWidget->isEmpty())
        return;

    auto res = this->loadNodesVisualization();

    if(res != 0)
    {
        this->errorMessage("Error, failed to load the Transport network nodes visualization");
        return;
    }

    res = this->loadPipelinesVisualization();

    if(res != 0)
    {
        this->errorMessage("Error, failed to load the Transport network pipelines visualization");
        return;
    }

}

