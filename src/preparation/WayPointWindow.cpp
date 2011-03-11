/***************************************************************************
 *   Copyright (C) 2004 by Alex Graf                                       *
 *   grafal@sourceforge.net                                                         *
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
 *   along with this program; if not,  write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qcursor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <q3table.h>
#include <qdatetime.h>
#include <qmenubar.h>
#include "IDataBase.h"
#include "ISql.h"
#include "IGPSDevice.h"
#include "ProgressDlg.h"
#include "IWayPointForm.h"
#include "WayPointWindow.h"
#include "WayPoint.h"

#include <stdio.h>

WayPointWindow::WayPointWindow(QWidget* parent, const char* name, Qt::WindowFlags wflags, IDataBase::SourceType src)
	:TableWindow(parent, name, wflags)
{
	QString caption;
	QStringList nameList;
	Q3Table *pTable = TableWindow::getTable();

	QMenu* pFileMenu = menuBar()->addMenu(tr("&File"));
	QAction* pUpdateAct = new QAction(tr("&Update"), this);
	connect(pUpdateAct,SIGNAL(triggered()), this, SLOT(file_update()));
	pFileMenu->addAction(pUpdateAct);

	switch(src)
	{
		case IDataBase::SqlDB:
		{
			m_pDb = ISql::pInstance();
			caption = "WayPoints from DB";
			QAction* pAddAct = new QAction(tr("&Add to GPS..."), this);
			connect(pAddAct,SIGNAL(triggered()), this, SLOT(file_AddToGps()));
			pFileMenu->addAction(pAddAct);
		}
		break;
		case IDataBase::GPSdevice:
		{
			m_pDb = IGPSDevice::pInstance();
			caption = "WayPoints from GPS";
			QAction* pAddAct = new QAction(tr("&Add to DB..."), this);
			connect(pAddAct,SIGNAL(triggered()), this, SLOT(file_AddToSqlDB()));
			pFileMenu->addAction(pAddAct);
		}
		break;
		default:
			Q_ASSERT(false);
		break;
	}
	
	QAction* pNewAct = new QAction(tr("&New Waypoint..."), this);
	connect(pNewAct,SIGNAL(triggered()), this, SLOT(file_addNewWp()));
	pFileMenu->addAction(pNewAct);
	QAction* pDelAct = new QAction(tr("&Delete"), this);
	connect(pDelAct,SIGNAL(triggered()), this, SLOT(file_delete()));
	pFileMenu->addAction(pDelAct);

	TableWindow::setWindowTitle(caption);
	TableWindow::setWindowIcon(QIcon(":/document.xpm"));
	
	// configure the table
	pTable->setReadOnly(true);
	pTable->setSelectionMode(Q3Table::MultiRow);

	// header
	nameList += "Name";
	nameList += "Country";
	nameList += "Spot";
	nameList += "Longitude\n[Deg,min]";
	nameList += "Latitude\n[Deg,min]";
	nameList += "Altitude\n[m]";
	nameList += "Description";
	setupHeader(nameList);

	pTable->setColumnWidth(Name, 150);	
	pTable->setColumnWidth(Country, 70);
	pTable->setColumnWidth(Spot, 120);
	pTable->setColumnWidth(Longitude, 90);
	pTable->setColumnWidth(Latitude, 90);
	pTable->setColumnWidth(Altitude, 70);
	pTable->setColumnWidth(Description, 500);
	
	m_lastModified = 0;
}

bool WayPointWindow::periodicalUpdate()
{
	int lastModified;
	
	lastModified = m_pDb->wayPointsLastModified();
	
	if(m_lastModified < lastModified)
	{
		file_update();
		m_lastModified = lastModified;
	}
	
	return true;
}

void WayPointWindow::file_update()
{
	WayPoint wp;
	Q3Table *pTable = TableWindow::getTable();
	ProgressDlg progDlg(this);
	uint wpNr;
	uint maxWpNr;
	
	m_wpList.clear();
	pTable->setNumRows(0); // clear table, because of different nr of waypoints

	if(m_pDb->open())
	{
		progDlg.beginProgress("reading waypoints...", m_pDb);
		m_pDb->wayPointList(m_wpList);
		progDlg.endProgress();
		m_pDb->close();
	}

	maxWpNr = m_wpList.size();
	pTable->setNumRows(maxWpNr);
	
	for(wpNr=0; wpNr<maxWpNr; wpNr++)
	{
		setWpToRow(wpNr, m_wpList[wpNr]);
	}
}

void WayPointWindow::file_AddToGps()
{
	int numSel;
	int sel;
	int topRow;
	int bottomRow;
	int row;

	numSel = getTable()->numSelections();

	if((numSel > 0) && IGPSDevice::pInstance()->open())
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));

		for(sel=0; sel<numSel; sel++)
		{
			topRow = getTable()->selection(sel).topRow();
			bottomRow = getTable()->selection(sel).bottomRow();

			for(row=topRow; row<=bottomRow; row++)
			{
				IGPSDevice::pInstance()->add(m_wpList[row]);
			}
		}

		TableWindow::unsetCursor();
		IGPSDevice::pInstance()->close();
	}
}

void WayPointWindow::file_delete()
{
	int numSel;
	int sel;
	int topRow;
	int bottomRow;
	int row;

	numSel = getTable()->numSelections();

	if((numSel > 0) && IGPSDevice::pInstance()->open())
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));

		for(sel=0; sel<numSel; sel++)
		{
			topRow = getTable()->selection(sel).topRow();
			bottomRow = getTable()->selection(sel).bottomRow();

			for(row=topRow; row<=bottomRow; row++)
			{
				m_pDb->delWayPoint(m_wpList[row]);
			}
		}

		TableWindow::unsetCursor();
		IGPSDevice::pInstance()->close();
	}
}

void WayPointWindow::file_addNewWp()
{
	WayPoint wp;
	IWayPointForm wayPointForm(this, tr("New WayPoint"), &wp);

	if(wayPointForm.exec() && m_pDb->open())
	{
		m_pDb->add(wp);
		m_pDb->close();
	}
}

void WayPointWindow::file_AddToSqlDB()
{
	int row;
	
	row = getTable()->currentRow();
	
	if((row >= 0) && ISql::pInstance()->open())
	{
		IWayPointForm wayPointForm(this, tr("Add WayPoint to DB"), &m_wpList[row]);
		
		if(wayPointForm.exec())
		{
			TableWindow::setCursor(QCursor(Qt::WaitCursor));
			ISql::pInstance()->add(m_wpList[row]);
			TableWindow::unsetCursor();
		}

		ISql::pInstance()->close();
	}
}

void WayPointWindow::setWpToRow(uint row, WayPoint &wp)
{
	QString str;
	Q3Table *pTable = TableWindow::getTable();

	pTable->setText(row, Country, wp.country());
	pTable->setText(row, Spot, wp.spot());
	pTable->setText(row, Name, wp.name());
		
	str.sprintf("%.5f", wp.longitude());
	pTable->setText(row, Longitude, str);
	
	str.sprintf("%.5f", wp.latitude());
	pTable->setText(row, Latitude, str);
	
	str.sprintf("%i", wp.altitude());
	pTable->setText(row, Altitude, str);
	
	pTable->setText(row, Description, wp.description());
}
