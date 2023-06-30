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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

// Written by: Stevan Gavrilovic, Frank McKenna

#include "AssetsWidget.h"
//#include "SecondaryComponentSelection.h"
#include "VisualizationWidget.h"
#include "sectiontitle.h"
#include "SimCenterAppSelection.h"

#ifdef ARC_GIS
#include "ArcGISBuildingInputWidget.h"
#include "ArcGISGasPipelineInputWidget.h"
#endif

#ifdef Q_GIS
#include "PointAssetInputWidget.h"
#include "LineAssetInputWidget.h"
#include "CSVWaterNetworkInputWidget.h"
#include "GISAssetInputWidget.h"
#include "GISWaterNetworkInputWidget.h"
#include "CSVTransportNetworkInputWidget.h"
#include "GISTransportNetworkInputWidget.h"
#endif

// Qt headers
#include <QCheckBox>
#include <QColorTransform>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

AssetsWidget::AssetsWidget(QWidget *parent, VisualizationWidget* visWidget)
: MultiComponentR2D(QString("Assets"),parent), visualizationWidget(visWidget)
{
    buildingWidget = new SimCenterAppSelection(QString("Regional Building Inventory"), QString("Buildings"), this);
    gasPipelineWidget = new SimCenterAppSelection(QString("Regional Gas Pipelines"), QString("NaturalGasPipelines"), this);
    waterNetworkWidget = new SimCenterAppSelection(QString("Regional Water Network"), QString("WaterDistributionNetwork"), this);
    transportNetworkWidget = new SimCenterAppSelection(QString("Regional Transportation Network"), QString("TransportationDistributionNetwork"), this);

#ifdef ARC_GIS
    ArcGISBuildingInputWidget *csvBuildingInventory = new ArcGISBuildingInputWidget(this,"Buildings","CSV_to_AIM");
    buildingWidget->addComponent(QString("CSV to AIM"), QString("CSV_to_AIM"), csvBuildingInventory);
#endif

#ifdef Q_GIS
    PointAssetInputWidget *csvBuildingInventory = new PointAssetInputWidget(this, visualizationWidget, "Buildings","CSV_to_AIM");
    buildingWidget->addComponent(QString("CSV to AIM"), QString("CSV_to_AIM"), csvBuildingInventory);

    GISAssetInputWidget *GISBuildingInventory = new GISAssetInputWidget(this,visualizationWidget,"Buildings","GIS_to_AIM");
    buildingWidget->addComponent(QString("GIS File to AIM"), QString("GIS_to_AIM"), GISBuildingInventory);
#endif

#ifdef ARC_GIS
    ArcGISGasPipelineInputWidget *csvPipelineInventory = new ArcGISGasPipelineInputWidget(this,"Gas Pipelines","Gas Network");
    pipelineWidget->addComponent(QString("CSV to Pipeline"), QString("CSV_to_PIPELINE"), csvPipelineInventory);
#endif

#ifdef Q_GIS
    LineAssetInputWidget *csvPipelineInventory = new LineAssetInputWidget(this, visualizationWidget, "Gas Pipelines","Gas Network");
    gasPipelineWidget->addComponent(QString("CSV to Pipeline"), QString("CSV_to_PIPELINE"), csvPipelineInventory);


    CSVWaterNetworkInputWidget *csvWaterNetworkInventory = new CSVWaterNetworkInputWidget(this, visualizationWidget);
    waterNetworkWidget->addComponent(QString("CSV to Water Network"), QString("CSV_to_WATERNETWORK"), csvWaterNetworkInventory);

    CSVTransportNetworkInputWidget *csvTransportNetworkInventory = new CSVTransportNetworkInputWidget(this, visualizationWidget);
    transportNetworkWidget->addComponent(QString("CSV to Transportation Network"), QString("CSV_to_TRANSPORTNETWORK"), csvTransportNetworkInventory);


    GISWaterNetworkInputWidget *gisWaterNetworkInventory = new GISWaterNetworkInputWidget(this, visualizationWidget);
    waterNetworkWidget->addComponent(QString("GIS to Water Network"), QString("GIS_to_WATERNETWORK"), gisWaterNetworkInventory);

    GISTransportNetworkInputWidget *gisTransportNetworkInventory = new GISTransportNetworkInputWidget(this, visualizationWidget);
    transportNetworkWidget->addComponent(QString("GIS to Transportation Network"), QString("GIS_to_TRANSPORTNETWORK"), gisTransportNetworkInventory);
#endif

    // QString pathToPipelineInfoFile = "/Users/steve/Desktop/SimCenter/Examples/CECPipelineExample/sample_input.csv";
    // csvBuildingInventory->testFileLoad(pathToBuildingInfoFile);

    this->addComponent("Buildings", buildingWidget);
    this->addComponent("Gas Network",gasPipelineWidget);
    this->addComponent("Water Network",waterNetworkWidget);
    this->addComponent("Transportation Network", transportNetworkWidget);
    this->hideAll();
}


AssetsWidget::~AssetsWidget()
{

}


void AssetsWidget::clear(void)
{
    buildingWidget->clear();
    gasPipelineWidget->clear();
    waterNetworkWidget->clear();
}







