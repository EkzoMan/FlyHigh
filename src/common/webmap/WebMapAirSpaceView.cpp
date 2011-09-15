/***************************************************************************
 *   Copyright (C) 2010 by Alex Graf                                       *
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

#include "WebMap.h"
#include "WebMapAirSpaceView.h"

WebMapAirSpaceView::WebMapAirSpaceView(const QString &name)
{
	QWidget::setWindowTitle(name);
	resize(1000, 850);

	m_pAirSpaceList = NULL;
	m_pWebMap = new WebMap(this, WebMap::MapAirSpace);
	connect(m_pWebMap, SIGNAL(mapReady()), this, SLOT(mapReady()));
	connect(m_pWebMap, SIGNAL(finished(int)), this, SLOT(finished(int)));
  connect(m_pWebMap, SIGNAL(lineChanged(int)), this, SLOT(lineChanged(int)));
}

WebMapAirSpaceView::~WebMapAirSpaceView()
{
	delete m_pWebMap;
}

void WebMapAirSpaceView::setAirSpaceList(AirSpaceList *pAirSpaceList)
{
	m_pAirSpaceList = pAirSpaceList;
}

void WebMapAirSpaceView::loadMap()
{
	m_pWebMap->loadMap("qrc:/airspace.html");
}

void WebMapAirSpaceView::selectAirSpace(int nr)
{
	m_pWebMap->selectAirSpace(nr);
}

void WebMapAirSpaceView::resizeEvent(QResizeEvent *pEvent)
{
	m_pWebMap->setGeometry(QRect(0, 0, width(), height()));
}

void WebMapAirSpaceView::mapReady()
{
	m_pWebMap->setGeometry(QRect(0, 0, width(), height()));

	if(m_pAirSpaceList != NULL)
	{
		m_pWebMap->setAirSpaceList(*m_pAirSpaceList);
	}

	m_pWebMap->XCLoad();
}

void WebMapAirSpaceView::finished(int res)
{
	done(res);
}

void WebMapAirSpaceView::lineChanged(int line)
{
  emit airSpaceChanged(line);
}