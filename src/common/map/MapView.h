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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _MapView_h
#define _MapView_h

#include <QMainWindow>
#include "WayPoint.h"

class QKeyEvent;
class QScrollArea;

class MapView: public QMainWindow
{
	public:
		MapView(QWidget *parent=0, const char *name=0, Qt::WindowFlags wFlags=0);

		virtual ~MapView();

		void showWayPointList(WayPoint::WayPointListType &wpList);

	protected:
		void keyPressEvent(QKeyEvent *pEvent);

	private:
    QScrollArea *m_pScrollArea;
    QPoint m_prevMousePos;

    bool eventFilter(QObject *pObject, QEvent *pEvent);

		void mouseMove(QMouseEvent *pEvent);

		void wheel(QWheelEvent *pEvent);
};

#endif
