/***************************************************************************
 *   Copyright (C) 2012 by Alex Graf                                       *
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
 *                                                                         *
 *   It is prohibited to serve or run this code over network p.e. as web   *
 *   service in combination with closed source.                            *
 ***************************************************************************/

function Flight(map)
{
	var flight = this;

	this.map = map;
	this.glider = new google.maps.Marker({
		map: route.getMap(),
		draggable: true,
		raiseOnDrag: false
	});
	this.stpos = null;
	this.trackPts = null;
	this.sogList = null;
	this.varioList = null;
	this.altList = null;
	this.timeList = null;

	google.maps.event.addListener(this.glider, 'dragstart', function(event) {fl_dragstart(flight);});
	google.maps.event.addListener(this.glider, 'drag', function(event) {fl_drag(flight);});
}

Flight.prototype.getMap = function()
{
	return this.map;
}

Flight.prototype.setTrackPts = function(trackPts)
{
	this.trackPts = trackPts;
}

Flight.prototype.getPositionAt = function(index)
{
	var pos = null;

	if((this.trackPts != null) && (index < this.trackPts.length)
	{
		pos = this.trackPts[index];
	}

	return pos;
}

Flight.prototype.setSogList = function(sogList)
{
	this.sogList = sogList;
}

Flight.prototype.getSogAt = function(index)
{
	var sog = null;

	if((this.sogList != null) && (index < this.sogList.length)
	{
		sog = this.sogList[index];
	}

	return sog;
}

Flight.prototype.setVarioList = function(varioList)
{
	this.varioList = varioList;
}

Flight.prototype.getVarioAt = function(index)
{
	var vario = null;

	if((this.varioList != null) && (index < this.varioList.length)
	{
		vario = this.varioList[index];
	}

	return vario;
}

Flight.prototype.setAltList= function(altList)
{
	this.altList = altList;
}

Flight.prototype.getAltAt = function(index)
{
	var alt = null;

	if((this.altList != null) && (index < this.altList.length)
	{
		alt = this.altList[index];
	}

	return alt;
}

Flight.prototype.setTimeList = function(timeList)
{
	this.timeList = timeList;
}

Flight.prototype.getTimeAt = function(index)
{
	var time = null;

	if((this.timeList != null) && (index < this.timeList.length)
	{
		time = this.timeList[index];
	}

	return time;
}

Flight.prototype.moveGliderTo = function(index)
{
	var pos;

	pos = this.getPositionAt(index);

	if(pos != null)
	{
		this.glider.setPosition(pos);
	}
}

/*
	This is an ugly hack, to restore position while drag. Because Qt 4.6 won't display
	markers which are not draggable. In a later version, this should be fixed through
	setting markers draggable=this.editable.
*/
Flight.prototype.storePos = function()
{
	this.stpos = new google.maps.LatLng(this.glider.getPosition().lat(), this.glider.getPosition().lng());
};

Flight.prototype.restorePos = function()
{
	if(this.storePos != null)
	{
		this.glider.setPosition(this.stpos);
	}
}

function fl_dragstart(flight)
{
	flight.storePos();
}

function fl_drag(flight)
{
	flight.restorePos();
}