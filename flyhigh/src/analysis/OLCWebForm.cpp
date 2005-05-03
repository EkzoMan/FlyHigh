/***************************************************************************
 *   Copyright (C) 2005 by Alex Graf                                     *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qfile.h>
#include <qtextstream.h>
#include "OLCWebForm.h"

OLCWebForm::OLCWebForm()
{
}

void OLCWebForm::setPilot(const QString &name)
{
	int pos;
	
	pos = name.find(' ');
	m_firstName = name.left(pos);
	m_surName = name.right(name.length() - pos - 1);
}

void OLCWebForm::setTakeOffLoc(const QString &takeoff)
{
	m_takeoffLoc = takeoff;
}

void OLCWebForm::setCallSign(const QString &callsign)
{
	m_callsign = callsign;
}

void OLCWebForm::setDate(const QDate &date)
{
	m_date = date;
}

void OLCWebForm::setGlider(const QString &name)
{
	m_glider = name;
}

void OLCWebForm::setDeparture(const FlightPointList::FlightPointType &dep)
{
	m_departure = dep;
}

void OLCWebForm::set1stWayPoint(const WayPoint &wp)
{
	m_1stWayPoint = wp;
}

void OLCWebForm::set2ndWayPoint(const WayPoint &wp)
{
	m_2ndWayPoint = wp;
}

void OLCWebForm::set3rdWayPoint(const WayPoint &wp)
{
	m_3rdWayPoint = wp;
}

void OLCWebForm::setFinish(const FlightPointList::FlightPointType &fin)
{
	m_finish = fin;
}

void OLCWebForm::setComment(const QString &comment)
{
	m_comment = comment;
}

bool OLCWebForm::save(const QString & name)
{
	QFile file;
	QString str;
	bool success;
	
	file.setName(name);
	success = file.open(IO_WriteOnly);
	
	if(success)
	{
		QTextStream s(&file);
	
		s << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" >\n";
		s << "<HTML>\n";
		s << "<HEAD>\n";
		s << "	<TITLE>HGPG Online-CONTEST International flight claim</TITLE>\n";
		s << "	<META http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n";
		s << "	<LINK href=\"/holc/css/styles.css\" rel=\"stylesheet\" type=\"text/css\">\n";
		s << "	<STYLE type=\"text/css\">\n";
		s << "		<!--\n";
		s << "			.navi { font-family: Arial, Helvetica, sans-serif; font-size: 10px; font-weight: normal; text-decoration: none}\n";
		s << "			.ta { font-family: Arial, Helvetica, sans-serif; font-size: 11px; font-weight: normal; text-decoration: none}\n";
		s << "			.td { font-family: Arial, Helvetica, sans-serif; font-size: 11px; font-weight: normal; text-decoration: none}\n";
		s << "			.td { font-family: Arial, Helvetica, sans-serif; font-size: 11px; font-weight: normal; text-decoration: none}\n";
		s << "		-->\n";
		s << "	</STYLE>\n";
		s << "</HEAD>\n";
		s << "<BODY bgcolor=\"#FFFFFF\" text=\"#000000\" link=\"#0000CC\" vlink=\"#0000CC\" alink=\"#0000CC\" leftmargin=\"0\" topmargin=\"0\" marginwidth=\"0\" marginheight=\"0\">\n";
		s << "	<CENTER>\n";
		s << "		<FONT FACE=Arial COLOR='#666666' SIZE='+2'><B> flight claim for <i>HGPG</i> Online-<i>CONTEST</i> International 2005</B></FONT><BR>\n";
		s << "		<FORM action=\"http://www.onlinecontest.org/olc-cgi/2005/holc-i/olc\" method=get>\n";
		s << "			<TABLE>\n";
		s << "				<TR>\n";
		s << "				<TD>First/Given name </TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "				<INPUT TYPE=TEXT MAXLENGTH=60 SIZE=60 NAME=\"OLCvnolc\" VALUE=\"" << m_firstName << "\">\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD>Surname</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=TEXT MAXLENGTH=60 SIZE=60 NAME=\"na\" VALUE=\"" << m_surName << "\">\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD>Take-off location</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=\"TEXT\" MAXLENGTH=\"20\" SIZE=\"20\" NAME=\"sta\" VALUE=\"" << m_takeoffLoc << "\">&nbsp;&nbsp;no ICAO-identifiers please!\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD>Callsign</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=\"TEXT\" MAXLENGTH=\"20\" SIZE=\"20\" NAME=\"gid\" VALUE=\"" << m_callsign <<"\">\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		
		// IGC file name
		s << "			<TR>\n";
		olcFileName(str);
		
		s << "				<TD>IGC-filename</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=\"TEXT\" MAXLENGTH=\"8\" SIZE=\"8\" NAME=\"igcfn\" VALUE=\"" << str <<"\">.igc\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		
		// flight date
		streamFlightDate(s);
		
		s << "			<TR>\n";
		s << "				<TD>Model of glider</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=\"TEXT\" MAXLENGTH=\"50\" SIZE=\"50\" NAME=\"gty\" VALUE=\"" << m_glider << "\">&nbsp;&nbsp;&nbsp;&nbsp;enter [manufacturer glidertype]\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD>Take-off time</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=\"TEXT\" MAXLENGTH=\"8\" SIZE=\"8\" NAME=\"t0\" VALUE=\"" << m_departure.time.toString(Qt::ISODate) << "\">(hh:mm:ss) (UTC)\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD>Departure time</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=\"TEXT\" MAXLENGTH=\"8\" SIZE=\"8\" NAME=\"s0\" VALUE=\"" << m_departure.time.toString(Qt::ISODate) << "\">(hh:mm:ss) (UTC)\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD>\n";
	
		streamWayPoint(s, 0, m_departure.wp);
		streamWayPoint(s, 1, m_1stWayPoint);
		streamWayPoint(s, 2, m_2ndWayPoint);
		streamWayPoint(s, 3, m_3rdWayPoint);
		streamWayPoint(s, 4, m_finish.wp);
	
		s << "			<TR>\n";
		s << "				<TD>Finish time</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<INPUT TYPE=\"TEXT\" MAXLENGTH=\"8\" SIZE=\"8\" NAME=\"s4\" VALUE=\"" << m_finish.time.toString(Qt::ISODate) << "\">(hh:mm:ss) (UTC)\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD>Comment Pilot</TD>\n";
		s << "				<TD COLSPAN=2>\n";
		s << "					<TEXTAREA ROWS=\"5\" COLS=\"60\" NAME=\"cpilo\" VALUE=\"" << m_comment << "\"></TEXTAREA>\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			<TR>\n";
		s << "				<TD></TD>\n";
		s << "				<TD>\n";
		s << "					<INPUT TYPE=\"SUBMIT\" NAME=\"test\" VALUE=\"Check entry\"> &nbsp;\n";
		s << "				</TD>\n";
		s << "			</TR>\n";
		s << "			</TABLE>\n";
		s << "		</FORM>\n";
		s << "	</CENTER>\n";
		s << "</BODY>\n";
		s << "</HTML>\n";
		
		file.close();
	}
	
	return success;
}

void OLCWebForm::olcFileName(QString &fileName)
{
	fileName = getOLCchar(m_date.year() % 100);
	fileName += getOLCchar(m_date.month());
	fileName += getOLCchar(m_date.day());
	fileName += m_surName.left(4).lower();
	fileName += "1";
}

void OLCWebForm::streamWayPoint(QTextStream& s, uint wpNr, WayPoint &wp)
{
	double lat;
	double lon;
	double min;
	int dmin;
	QString strDeg;
	QString strMin;
	QString strDmin;
	QString strWpNr;
	
	strWpNr.sprintf("%i", wpNr);
	
	// begin of table row
	s << "			<TR>\n";
	s << "				<TD>\n";
	s << "					<FONT COLOR=00FF00>";
	
	switch(wpNr)
	{
		case 0:
			s << "Departure";
		break;
		case 1:
			s << "1st waypoint";
		break;
		case 2:
			s << "2nd waypoint";
		break;
		case 3:
			s << "3rd waypoint";
		break;
		case 4:
			s << "Finish";
		break;
	}
	
	s << "</FONT>\n";
	s << "				</TD>\n";
	s << "				<TD>\n";
	
	// latitude
	s << "					<SELECT NAME=\"w" << strWpNr << "bh\">\n";
	
	lat = wp.latitude();
	
	if(lat < 0)
	{
		s << "						<OPTION VALUE=\"N\">N\n";
		s << "						<OPTION VALUE=\"S\" SELECTED>S\n";
		lat *= -1.0;
	}
	else
	{
		s << "						<OPTION VALUE=\"N\" SELECTED>N\n";
		s << "						<OPTION VALUE=\"S\">S\n";
	}
	
	s << "					</SELECT>\n";
	
	min = (lat - (int)lat) * 60.0;
	dmin = (int)((min - (int)min) * 1000);
	strDeg.sprintf("%2i", (int)lat);
	strMin.sprintf("%2i", (int)min);
	strDmin.sprintf("%3i", dmin);
	
	s << "					<INPUT TYPE=\"text\" VALUE=\"" << strDeg <<  "\" MAXLENGTH=\"2\" SIZE=\"2\" NAME=\"w";
	s << strWpNr << "bg\">:<INPUT TYPE=\"text\" VALUE=\"" << strMin << "\" MAXLENGTH=\"2\" SIZE=\"2\" NAME=\"w";
	s << strWpNr << "bm\">.<INPUT TYPE=\"text\" VALUE=\"" << strDmin << "\" MAXLENGTH=\"3\" SIZE=\"3\" NAME=\"w";
	s << strWpNr << "bmd\">&nbsp; &nbsp;\n";
	
	// longitude
	s << "					<SELECT NAME=\"w" << strWpNr << "lh\">\n";
	
	lon = wp.longitude();
	
	if(lon < 0)
	{
		s << "						<OPTION VALUE=\"E\">E\n";
		s << "						<OPTION VALUE=\"W\" SELECTED>W\n";
		lon *= -1.0;
	}
	else
	{
		s << "						<OPTION VALUE=\"E\" SELECTED>E\n";
		s << "						<OPTION VALUE=\"W\">W\n";
	}

	s << "					</SELECT>\n";
	
	min = (lon - (int)lon) * 60.0;
	dmin = (int)((min - (int)min) * 1000);
	strDeg.sprintf("%2i", (int)lon);
	strMin.sprintf("%2i", (int)min);
	strDmin.sprintf("%3i", dmin);
	
	s << "					<INPUT TYPE=\"text\" VALUE=\"" << strDeg <<  "\" MAXLENGTH=\"2\" SIZE=\"2\" NAME=\"w";
	s << strWpNr << "lg\">:<INPUT TYPE=\"text\" VALUE=\"" << strMin << "\" MAXLENGTH=\"2\" SIZE=\"2\" NAME=\"w";
	s << strWpNr << "lm\">.<INPUT TYPE=\"text\" VALUE=\"" << strDmin << "\" MAXLENGTH=\"3\" SIZE=\"3\" NAME=\"w";
	s << strWpNr << "lmd\">&nbsp; &nbsp;\n";
	
	// end of table row
	s << "				</TD>\n";
	s << "				<TD> </TD>\n";
	s << "			</TR>\n";
}

void OLCWebForm::streamFlightDate(QTextStream& s)
{
// igcParser.date().toString("dd.MM.yyyy")

	s << "			<TR>\n";
	s << "				<TD>Date of flight</TD>\n";
	s << "				<TD COLSPAN=2>\n";
	s << "					<SELECT NAME=\"ft\" >\n";
	s << "						<OPTION VALUE=147666 >19 April 2005 54J &nbsp;OLC\n";
	s << "						<OPTION VALUE=147667 >20 April 2005 54K &nbsp;OLC\n";
	s << "						<OPTION VALUE=147668 >21 April 2005 54L &nbsp;OLC\n";
	s << "						<OPTION VALUE=147669 >22 April 2005 54M &nbsp;OLC\n";
	s << "						<OPTION VALUE=147670 >23 April 2005 54N &nbsp;OLC\n";
	s << "						<OPTION VALUE=147671 >24 April 2005 54O &nbsp;OLC\n";
	s << "						<OPTION VALUE=147672 >25 April 2005 54P &nbsp;OLC\n";
	s << "						<OPTION VALUE=147673 >26 April 2005 54Q &nbsp;OLC\n";
	s << "						<OPTION VALUE=147674 >27 April 2005 54R &nbsp;OLC\n";
	s << "						<OPTION VALUE=147675 >28 April 2005 54S &nbsp;OLC\n";
	s << "						<OPTION VALUE=147676 >29 April 2005 54T &nbsp;OLC\n";
	s << "						<OPTION VALUE=147677 >30 April 2005 54U &nbsp;OLC\n";
	s << "						<OPTION VALUE=147678 > 1 May 2005 551 &nbsp;OLC\n";
	s << "						<OPTION VALUE=147679 > 2 May 2005 552 &nbsp;OLC\n";
	s << "						<OPTION VALUE=147680 SELECTED> 3 May 2005 553 &nbsp;OLC\n";
	s << "					</SELECT>\n";
	s << "				</TD>\n";
	s << "			</TR>\n";
}

char OLCWebForm::getOLCchar(int value)
{
	char resChar;
	
	if((value >= 0) && (value <=9))
	{
		resChar = '0' + value;
	}
	else if((value >= 10) && (value <=31))
	{
		resChar = 'a' + value - 10;
	}
	else
	{
		resChar = '0';
	}
	
	return resChar;
}
