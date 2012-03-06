/***************************************************************************
 *   Copyright (C) 2004 by Alex Graf                                       *
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
 *   along with this program; if not,  write to the                        *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QAction>
#include <QCursor>
#include <QDateTime>
#include <QFileDialog>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include "IDataBase.h"
#include "ISql.h"
#include "IGPSDevice.h"
#include "IWayPointForm.h"
#include "ProgressDlg.h"
#include "WayPointWindow.h"
#include "WayPoint.h"
#include "WptFileParser.h"
#include "WebMapWayPointView.h"

WayPointWindow::WayPointWindow(QWidget* parent, const QString &name, Qt::WindowFlags wflags,
                               IDataBase::SourceType src, WayPoint::Type type)
	:TableWindow(parent, name, wflags)
{
	QStringList nameList;
	QAction* pAction;
	QTableWidget *pTable = TableWindow::getTable();

  m_pWayPointView = NULL;
  m_wpType = type;
  m_externSelect = false;

	switch(src)
	{
		case IDataBase::SqlDB:
		{
			m_pDb = ISql::pInstance();
      connect(m_pDb, SIGNAL(wayPointsChanged()), this, SLOT(file_update()));
		}
		break;
		case IDataBase::GPSdevice:
		{
			m_pDb = IGPSDevice::pInstance();
      connect(m_pDb, SIGNAL(wayPointsChanged()), this, SLOT(file_update()));
		}
		break;
    case IDataBase::File:
		{
			m_pDb = NULL;
		}
		break;
		default:
			Q_ASSERT(false);
		break;
	}

  if(src != IDataBase::File)
  {
    pAction = new QAction(tr("New..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_addNewWp()));
    MDIWindow::addAction(pAction);
  }

  if(src == IDataBase::SqlDB)
  {
    pAction = new QAction(tr("Edit..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_Edit()));
    MDIWindow::addAction(pAction, true);
  }

  if(src != IDataBase::File)
  {
    // 6016 doesn't support delete of single waypoints
    if(!((src == IDataBase::GPSdevice) &&
      (IGPSDevice::pInstance()->deviceId() == IFlyHighRC::DevFlytec6015)))
    {
      pAction = new QAction(tr("Delete"), this);
      connect(pAction, SIGNAL(triggered()), this, SLOT(file_delete()));
      MDIWindow::addAction(pAction);
    }
  }

  // for sql db danger, only used by 6015
  if((src == IDataBase::GPSdevice) &&
     (IGPSDevice::pInstance()->deviceId() == IFlyHighRC::DevFlytec6015))
  {
    pAction = new QAction(tr("Delete All"), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_deleteAll()));
    MDIWindow::addAction(pAction);
  }

  if(src != IDataBase::GPSdevice)
  {
    pAction = new QAction(tr("Add to GPS..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_AddToGps()));
    MDIWindow::addAction(pAction, true);
  }

  if(src != IDataBase::SqlDB)
  {
    pAction = new QAction(tr("Add to DB..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_AddToSqlDB()));
    MDIWindow::addAction(pAction, true);
  }

  if(src != IDataBase::File)
  {
    pAction = new QAction(tr("&Update"), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_update()));
    MDIWindow::addAction(pAction);
  }

  // import/export
	pAction = new QAction(this);
	pAction->setSeparator(true);
	MDIWindow::addAction(pAction);

  if(src == IDataBase::File)
  {
    pAction = new QAction(tr("&Import File..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_open()));
    MDIWindow::addAction(pAction);
  }

  // view
	pAction = new QAction(this);
	pAction->setSeparator(true);
	MDIWindow::addAction(pAction, true);

	pAction = new QAction(tr("View &Web Map..."), this);
	connect(pAction, SIGNAL(triggered()), this, SLOT(file_viewWebMap()));
	MDIWindow::addAction(pAction, true);

  if(src == IDataBase::SqlDB)
  {
    pAction = new QAction(tr("Edit Web &Map..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_editWebMap()));
    MDIWindow::addAction(pAction, true);
  }

	TableWindow::setWindowIcon(QIcon(":/document.xpm"));

	// configure the table
	pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	pTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

	// header
	nameList += tr("Name");
	nameList += tr("Country");
	nameList += tr("Spot");
	nameList += tr("Longitude\n[Deg,min]");
	nameList += tr("Latitude\n[Deg,min]");
	nameList += tr("Altitude\n[m]");
	nameList += tr("Description");
	setupHeader(nameList);

	pTable->setColumnWidth(Name, 150);
	pTable->setColumnWidth(Country, 70);
	pTable->setColumnWidth(Spot, 120);
	pTable->setColumnWidth(Longitude, 90);
	pTable->setColumnWidth(Latitude, 90);
	pTable->setColumnWidth(Altitude, 70);
	pTable->setColumnWidth(Description, 500);

  if(src == IDataBase::File)
  {
    file_open();
  }
  else
  {
    file_update();
  }
}

WayPointWindow::~WayPointWindow()
{
}

void WayPointWindow::closeEvent(QCloseEvent *pEvent)
{
  if(m_pWayPointView != NULL)
  {
    delete m_pWayPointView;
    m_pWayPointView = NULL;
  }
}

void WayPointWindow::selectionChanged()
{
  int row;

	if((m_pWayPointView != NULL) && !m_externSelect)
	{
	  row = getTable()->currentRow();

	  if((row >= 0) && (row < m_wpList.size()))
	  {
      m_pWayPointView->selectWayPoint(m_wpList[row].id());
	  }
	}
}

void WayPointWindow::file_update()
{
	ProgressDlg progDlg(this);
	uint wpNr;
	uint maxWpNr;

	if(m_pDb != NULL)
	{
    m_wpList.clear();
    TableWindow::setNumRows(0); // clear table, because of different nr of waypoints

    if(m_pDb->open())
    {
      progDlg.beginProgress(tr("reading waypoints..."), m_pDb);
      m_pDb->wayPointList(m_wpType, m_wpList);
      progDlg.endProgress();
      m_pDb->close();
    }

    maxWpNr = m_wpList.size();
    TableWindow::setNumRows(maxWpNr);

    for(wpNr=0; wpNr<maxWpNr; wpNr++)
    {
      setWpToRow(wpNr, m_wpList[wpNr]);
    }
	}
}

void WayPointWindow::file_AddToGps()
{
  WayPoint::WayPointListType wpList;
  ProgressDlg progDlg(this);

	selectionToList(wpList);

	if((wpList.size() > 0) && IGPSDevice::pInstance()->open())
	{
	  TableWindow::setCursor(QCursor(Qt::WaitCursor));
    progDlg.beginProgress(tr("write waypoints..."), IGPSDevice::pInstance());
    IGPSDevice::pInstance()->add(wpList);
    progDlg.endProgress();
		TableWindow::unsetCursor();
		IGPSDevice::pInstance()->close();
	}

/**
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
			  wpList.push_back(m_wpList[row]);
			}
		}

    progDlg.beginProgress(tr("write waypoints..."), IGPSDevice::pInstance());
    IGPSDevice::pInstance()->add(wpList);
    progDlg.endProgress();
		TableWindow::unsetCursor();
		IGPSDevice::pInstance()->close();
	}
*/
}

void WayPointWindow::file_delete()
{
  WayPoint::WayPointListType wpList;
  ProgressDlg progDlg(this);

	selectionToList(wpList);

	if((wpList.size() > 0) && m_pDb->open())
	{
	  TableWindow::setCursor(QCursor(Qt::WaitCursor));
    progDlg.beginProgress(tr("delete waypoints..."), m_pDb);
    m_pDb->delWayPoints(wpList);
    progDlg.endProgress();
		TableWindow::unsetCursor();
		m_pDb->close();
	}

/**
	numSel = getTable()->numSelections();

	if((numSel > 0) && m_pDb->open())
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));

		for(sel=0; sel<numSel; sel++)
		{
			topRow = getTable()->selection(sel).topRow();
			bottomRow = getTable()->selection(sel).bottomRow();

			for(row=topRow; row<=bottomRow; row++)
			{
				wpList.push_back(m_wpList[row]);
			}
		}

    progDlg.beginProgress(tr("delete waypoints..."), m_pDb);
    m_pDb->delWayPoints(wpList);
    progDlg.endProgress();
		TableWindow::unsetCursor();
		m_pDb->close();
	}
*/
}

void WayPointWindow::file_deleteAll()
{
	if(m_pDb->open())
	{
    TableWindow::setCursor(QCursor(Qt::WaitCursor));
    m_pDb->delAllWayPoints();
    TableWindow::unsetCursor();
		m_pDb->close();
	}
}

void WayPointWindow::file_addNewWp()
{
	WayPoint wp;
	IWayPointForm wayPointForm(this, tr("New WayPoint"), &wp);

	if(wayPointForm.exec() && m_pDb->open())
	{
	  wp.setType(m_wpType);
		m_pDb->add(wp);
		m_pDb->close();
	}
}

void WayPointWindow::file_AddToSqlDB()
{
  WayPoint::WayPointListType wpList;
  ProgressDlg progDlg(this);

	selectionToList(wpList);

	if((wpList.size() > 0) && ISql::pInstance()->open())
	{
	  TableWindow::setCursor(QCursor(Qt::WaitCursor));
    progDlg.beginProgress(tr("add waypoints..."), ISql::pInstance());
    ISql::pInstance()->add(wpList);
    progDlg.endProgress();
		TableWindow::unsetCursor();
		ISql::pInstance()->close();
	}

/**
	numSel = getTable()->numSelections();

	if((numSel > 0) && ISql::pInstance()->open())
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));

		for(sel=0; sel<numSel; sel++)
		{
			topRow = getTable()->selection(sel).topRow();
			bottomRow = getTable()->selection(sel).bottomRow();

			for(row=topRow; row<=bottomRow; row++)
			{
				wpList.push_back(m_wpList[row]);
			}
		}

    progDlg.beginProgress(tr("add waypoints..."), ISql::pInstance());
    ISql::pInstance()->add(wpList);
    progDlg.endProgress();
		TableWindow::unsetCursor();
		ISql::pInstance()->close();
	}
*/
}

void WayPointWindow::file_open()
{
	QString fileName;
	WptFileParser parser;
	uint wptNr;
	uint maxWptNr;

	fileName = QFileDialog::getOpenFileName(this,
																					"WayPoint File",
																					IFlyHighRC::pInstance()->lastDir(),
																					"WayPoint Files (*.wpt)");

	if(fileName != "")
	{
		IFlyHighRC::pInstance()->setLastDir(QFileInfo(fileName).absoluteDir().absolutePath());
		TableWindow::setCursor(QCursor(Qt::WaitCursor));

		if(parser.parse(fileName, m_wpList))
		{
			maxWptNr = m_wpList.size();
			TableWindow::setNumRows(maxWptNr);

			for(wptNr=0; wptNr<maxWptNr; wptNr++)
			{
				setWpToRow(wptNr, m_wpList.at(wptNr));
			}

			TableWindow::selectRow(0);
			selectionChanged();
		}

		TableWindow::unsetCursor();
	}
}

void WayPointWindow::file_Edit()
{
	int row;

	row = getTable()->currentRow();

	if(row >= 0)
	{
    IWayPointForm wayPointForm(this, tr("New WayPoint"), &m_wpList[row]);

    if(wayPointForm.exec() && m_pDb->open())
    {
      m_pDb->update(m_wpList[row]);
      m_pDb->close();
    }
	}
}

void WayPointWindow::file_editWebMap()
{
	if((m_pWayPointView == NULL) && (m_wpList.size() >= 0))
	{
    menu()->setEnabled(false);
		m_pWayPointView = new WebMapWayPointView(tr("Edit WayPoints"));
    connect(m_pWayPointView, SIGNAL(finished(int)), this, SLOT(wayPointViewFinished(int)));
		connect(m_pWayPointView, SIGNAL(wayPointsChanged(WayPoint::WayPointListType&)), this,
            SLOT(wayPointsChanged(WayPoint::WayPointListType&)));
    connect(m_pWayPointView, SIGNAL(wayPointChanged(int)), this, SLOT(wayPointChanged(int)));
		m_pWayPointView->setWayPointList(&m_wpList);
		m_pWayPointView->loadMap();
    m_pWayPointView->setWindowModality(Qt::NonModal);
		m_pWayPointView->show();
	}
}

void WayPointWindow::file_viewWebMap()
{
	if((m_pWayPointView == NULL) && (m_wpList.size() > 0))
	{
    menu()->setEnabled(false);
		m_pWayPointView = new WebMapWayPointView(tr("View WayPoints"));
    connect(m_pWayPointView, SIGNAL(finished(int)), this, SLOT(wayPointViewFinished(int)));
    connect(m_pWayPointView, SIGNAL(wayPointChanged(int)), this, SLOT(wayPointChanged(int)));
    m_pWayPointView->setEditable(false);
		m_pWayPointView->setWayPointList(&m_wpList);
		m_pWayPointView->loadMap();
    m_pWayPointView->setWindowModality(Qt::NonModal);
		m_pWayPointView->show();
	}
}

void WayPointWindow::wayPointViewFinished(int res)
{
  m_pWayPointView = NULL;
  menu()->setEnabled(true);
}

void WayPointWindow::wayPointsChanged(WayPoint::WayPointListType &wpList)
{
  if(m_pDb->open())
  {
    m_externSelect = true;
    m_pDb->update(wpList);
    m_externSelect = false;
    m_pDb->close();
  }
}

void WayPointWindow::wayPointChanged(int id)
{
  int row;
  int listSize;

  listSize = m_wpList.size();

  for(row=0; row<listSize; row++)
  {
    if(m_wpList[row].id() == id)
    {
      m_externSelect = true;
      TableWindow::selectRow(row);
      m_externSelect = false;
      break;
    }
  }
}

void WayPointWindow::setWpToRow(uint row, const WayPoint &wp)
{
	QString str;
	QTableWidget *pTable = TableWindow::getTable();

	pTable->item(row, Country)->setText(wp.country());
	pTable->item(row, Spot)->setText(wp.spot());
	pTable->item(row, Name)->setText(wp.name());

	str.sprintf("%.5f", wp.longitude());
	pTable->item(row, Longitude)->setText(str);

	str.sprintf("%.5f", wp.latitude());
	pTable->item(row, Latitude)->setText(str);

	str.sprintf("%i", wp.altitude());
	pTable->item(row, Altitude)->setText(str);

	pTable->item(row, Description)->setText(wp.description());
}

void WayPointWindow::selectionToList(WayPoint::WayPointListType &wpList)
{
  QList<QTableWidgetSelectionRange> selRange;
  QList<QTableWidgetSelectionRange>::iterator rangeIt;
  QTableWidget *pTable;
	int topRow;
	int bottomRow;
	int row;

  pTable = TableWindow::getTable();
  selRange = pTable->selectedRanges();

  for(rangeIt=selRange.begin(); rangeIt!=selRange.end(); rangeIt++)
  {
    topRow = (*rangeIt).topRow();
    bottomRow = (*rangeIt).bottomRow();

    for(row=topRow; row<=bottomRow; row++)
    {
      wpList.push_back(m_wpList[row]);
    }
  }
}
