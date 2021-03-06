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
#ifndef IFlightForm_h
#define IFlightForm_h

#include "Flight.h"

class QString;
class QWidget;
class FlightFormImpl;
class Flight;

class IFlightForm
{
	public:
		IFlightForm(QWidget* parent, const QString &caption, Flight *pFlight = NULL);

		~IFlightForm();

		bool exec();

		void enableInput(bool b);

		void setFlight(Flight *pFlight);

	private:
		FlightFormImpl *m_pFlightForm;
};

#endif
