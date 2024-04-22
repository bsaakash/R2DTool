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

#include "IntensityMeasure.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>

IntensityMeasure::IntensityMeasure(QWidget *parent) : SimCenterAppWidget(parent)
{

}


QString IntensityMeasure::type() const
{
    return m_type;
}


bool IntensityMeasure::setType(const QString &type)
{
    if(m_type != type && validTypes().contains(type, Qt::CaseInsensitive))
    {
        m_type = type;
        emit typeChanged(m_type);
        return true;
    }
    return false;
}


const QStringList &IntensityMeasure::validTypes()
{
    static QStringList validTypes = QStringList()
                                    << "Peak Ground Acceleration (PGA)"
                                    << "Spectral Accelerations (SA)";

    return validTypes;
}


QList<double> IntensityMeasure::periods() const
{
    return m_periods;
}


void IntensityMeasure::setPeriods(const QList<double> &periods)
{
    m_periods = periods;
}


void IntensityMeasure::setPeriods(const QString &periods)
{
    periodsText = periods;
}


void IntensityMeasure::addPeriod(double period)
{
    this->m_periods.append(period);
    qSort(this->m_periods);
}


// set intensity measure levels
void IntensityMeasure::setImtLevels(const QString &value)
{
    while(imtLevels.count())
    {
        imtLevels.pop_back();
    }
    // remove spaces
    QString tmp = value;
    tmp.remove(" ");
    // split by ","
    QList<QString> tmpList = tmp.split(",");
    // check size of the list (must be three)
    if (tmpList.count() != 3)
    {
        QString errMsg = QString("Please provide three number for mininum, maximun intensity levels and the number of intervals.");
        qDebug() << errMsg;
        return;
    }
    // load the data
    for (int i = 0; i != 3; ++i)
    {
        if (tmpList[i].toDouble() <=0)
        {
            QString errMsg = QString("Only taken positive numbers.");
            qDebug() << errMsg;
            return;
        }
        imtLevels.append(tmpList[i].toDouble());
    }
}


void IntensityMeasure::setImtScale(const QString &value)
{
    if(imtScale != value)
    {
        imtScale = value;
        emit this->imtScaleChanged(imtScale);
    }
}


void IntensityMeasure::setImtTruc(double value)
{
    imtTruc = value;
}


double IntensityMeasure::getImtTruc() const
{
    return imtTruc;
}


bool IntensityMeasure::outputToJSON(QJsonObject &jsonObject)
{
    if(m_type.compare("Spectral Accelerations (SA)") == 0)
        jsonObject.insert("Type", "SA");
    else if(m_type.compare("Peak Ground Acceleration (PGA)") == 0)
        jsonObject.insert("Type", "PGA");
    else
    {
        jsonObject.insert("Type", "UNKNOWN");
        qDebug()<<"Warning, the type of intensity measure "<<m_type<<" is not recognized";
    }

    QJsonArray arrayPeriods;

    for(auto&& it : m_periods)
    {
        arrayPeriods.append(it);
    }

    if(jsonObject["Type"] != "PGA")
        jsonObject.insert("Periods", arrayPeriods);
    else
        jsonObject.insert("Period", 0);

    jsonObject.insert("Levels", imtLevels); // intensity measure levels
    jsonObject.insert("Scale", imtScale); // intensity measure level scale
    jsonObject.insert("Truncation", imtTruc); // truncation level

    return true;
}


bool IntensityMeasure::inputFromJSON(QJsonObject &/*jsonObject*/)
{
    return true;
}


void IntensityMeasure::reset(void)
{

}



