/***************************************************************************
 *   Copyright (C) 2011 by Alex Graf                                       *
 *   grafal@sourceforge.net                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QObject>
#include <QWebFrame>
#include <math.h>
#include "PolyLineEncoder.h"
#include "WebMap.h"
#include "WebMapFlight.h"

#include <QDebug>

WebMapFlight::WebMapFlight(WebMap *pWebMap)
{
	m_pWebMap = pWebMap;
	m_plotEn = false;
}

WebMapFlight::~WebMapFlight()
{
}

void WebMapFlight::init()
{
	QString code = "fl_init(720, 860);";
	QWebFrame *pFrame;

	pFrame = m_pWebMap->page()->mainFrame();
	pFrame->evaluateJavaScript(code);
}

void WebMapFlight::setFlightPointList(const QDate &date, const FlightPointList &fpList)
{
	QString code;
	QWebFrame *pFrame;
	WayPoint::WayPointListType wpList;
	PolyLineEncoder encoder;
	QString encPoints;
	QString encLevels;
	QString value = "%1";
	QString latLonArg = "[%1,%2]";
	QString strTime = "";
	QString strAlt = "";
	QString strLatLon = "";
	QTime time;
	WayPoint wp;
	uint fpNr;
	uint fpListSize;
	uint start;
	uint duration;
	int minAlt;
	int maxAlt;
	int epochDate;
	int epochTime;
	int secsOfDay;
	int prevSecsOfDay;
	bool first = true;

	fpListSize = fpList.size();

	if(fpListSize > 0)
	{
		minAlt = fpList.at(0).wp.altitude();
		maxAlt = minAlt;
		time = fpList.at(0).time;
		start = time.hour() * 3600 + time.minute() * 60 + time.second();
		time = fpList.at(fpListSize - 1).time;
		duration = time.hour() * 3600 + time.minute() * 60 + time.second() - start;
    epochDate = QDateTime(date).toTime_t();
    prevSecsOfDay = start;

		for(fpNr=0; fpNr<fpListSize; fpNr++)
		{
			wpList.push_back(fpList.at(fpNr).wp);

			if(!first)
			{
				strTime += ",";
				strAlt += ",";
				strLatLon += ",";
			}

			first = false;

			// time
      time = fpList.at(fpNr).time;
			secsOfDay = time.hour() * 3600 + time.minute() * 60 + time.second();

			if(secsOfDay < prevSecsOfDay)
			{
			  epochDate += 86400; // next day
			}

      epochTime = (epochDate + secsOfDay);
      prevSecsOfDay = secsOfDay;
			strTime += value.arg(epochTime);

			// altitude
      wp = fpList.at(fpNr).wp;
			minAlt = qMin(wp.altitude(), minAlt);
			maxAlt = qMax(wp.altitude(), maxAlt);
			strAlt += value.arg(wp.altitude());

			// lat, lon
			strLatLon += latLonArg.arg(wp.latitude()).arg(wp.longitude());
		}

		minAlt = floor(minAlt / 100.0) * 100;
		maxAlt = ceil(maxAlt / 100.0) * 100;

		pFrame = m_pWebMap->page()->mainFrame();
		code = "fl_setFlightTime([%1], %2, %3);";
		pFrame->evaluateJavaScript(code.arg(strTime).arg(start).arg(duration));
		code = "fl_setFlightAlt([%1], %2, %3);";
		pFrame->evaluateJavaScript(code.arg(strAlt).arg(minAlt).arg(maxAlt));
		code = "fl_setFlightLatLon([%1]);";
		pFrame->evaluateJavaScript(code.arg(strLatLon));
	}
}

void WebMapFlight::setSogList(const FlightPointList::SogListType &sogList)
{
	QString code = "fl_setSog([%1]);";
	QWebFrame *pFrame;
	QString value = "%1";
	QString strSog = "";
	uint itemNr;
	uint listSize;
	bool first = true;

	listSize = sogList.size();

	if(listSize > 0)
	{
		for(itemNr=0; itemNr<listSize; itemNr++)
		{
			if(!first)
			{
				strSog += ",";
			}

			first = false;

			// sog
			strSog += value.arg(round(sogList.at(itemNr) * 10.0) / 10.0);
		}

		pFrame = m_pWebMap->page()->mainFrame();
		pFrame->evaluateJavaScript(code.arg(strSog));
	}
}

void WebMapFlight::setVarioList(const FlightPointList::VarioListType &varioList)
{
	QString code = "fl_setVario([%1]);";
	QWebFrame *pFrame;
	QString value = "%1";
	QString strVario = "";
	uint itemNr;
	uint listSize;
	bool first = true;

	listSize = varioList.size();

	if(listSize > 0)
	{
		for(itemNr=0; itemNr<listSize; itemNr++)
		{
			if(!first)
			{
				strVario += ",";
			}

			first = false;

			// sog
			strVario += value.arg(round(varioList.at(itemNr) * 10.0) / 10.0);
		}

		pFrame = m_pWebMap->page()->mainFrame();
		pFrame->evaluateJavaScript(code.arg(strVario));
	}
}

void WebMapFlight::showPlot()
{
	QString code = "fl_showPlot();";
	QWebFrame *pFrame;

	if(m_plotEn)
	{
		pFrame = m_pWebMap->page()->mainFrame();
		pFrame->evaluateJavaScript(code);
	}
}

void WebMapFlight::setPlotEnable(bool en)
{
	m_plotEn = en;
}
