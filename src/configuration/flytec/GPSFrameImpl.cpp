/***************************************************************************
 *   Copyright (C) 2004 by grafal,,,                                       *
 *   grafal@spirit                                                         *
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

#include <qspinbox.h>
#include <qcombobox.h>

#include "GPSFrameImpl.h"
extern "C"
{
	#include "flytec_al.h"
}

GPSFrameImpl::GPSFrameImpl(QWidget* parent, const char* name, WFlags fl)
: GPSFrame(parent,name,fl)
{
}

GPSFrameImpl::~GPSFrameImpl()
{
}

void GPSFrameImpl::update(QByteArray &arr)
{
	char i8value;

	comboBox_GridSys->setCurrentItem(arr[GRID_SYS_POS]);
	
	i8value = arr[UTC_OFFSET_POS];
	spinBox_UTCoffset->setValue(i8value);

	spinBox_GeoID->setValue(arr[GEO_ID_POS]);
}

void GPSFrameImpl::store(QByteArray &arr)
{
	// Grid System
	arr[GRID_SYS_POS] = comboBox_GridSys->currentItem();
	
	// UTC Offset
	arr[UTC_OFFSET_POS] = spinBox_UTCoffset->value();

	// Geodic ID
	arr[GEO_ID_POS] = spinBox_GeoID->value();
}

#include "GPSFrameImpl.moc"
