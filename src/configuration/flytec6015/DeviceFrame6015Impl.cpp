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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qcombobox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include "IFlyHighRC.h"
#include "Glider.h"
#include "IGliderForm.h"
#include "ISql.h"

#include "DeviceFrame6015Impl.h"

DeviceFrame6015Impl::DeviceFrame6015Impl(QWidget* parent, const char* name, WFlags fl)
: DeviceFrame6015(parent,name,fl)
{
	updateGlider();
}

DeviceFrame6015Impl::~DeviceFrame6015Impl()
{
}

void DeviceFrame6015Impl::newGlider()
{
	Glider glider;
	IGliderForm newGlider(this, "New Glider", &glider);
	
	if(newGlider.exec())
	{
		ISql::pInstance()->add(glider);
		updateGlider();
	}
}

void DeviceFrame6015Impl::update(QByteArray &arr)
{
/*
	Pilot dbPilot;
	QString dbPilotName;
	QString dbGlider;
	QString pilotName;
	QString callsign;
	QString glider;
	DeviceInfoType devInfo;
	char str[FT_STRING_SIZE];
	int syncRes;
	
	if(ft_deviceInfoRead(&devInfo) == 0)
	{
		lineEdit_SerialNr->setText(QString("%1").arg(devInfo.serialNr));
		lineEdit_SwVersion->setText(QString(devInfo.swVersion));
		lineEdit_DeviceIdent->setText(QString(devInfo.deviceIdent));
	}

	// pilot name
	ft_ftstring2string(str, (char*)&arr[PILOT_NAME_POS]);
	pilotName = str;
	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), dbPilot);
	dbPilot.fullName(dbPilotName);

	if(pilotName != dbPilotName.left(pilotName.length()))
	{
		syncRes = QMessageBox::question(this, "Different pilots", "Pilot on GPS differ from pilot in database. Set pilot?",
			 	"DB to GPS", "Ignore");
		
		switch(syncRes)
		{
			case 0: // From database
				pilotName = dbPilotName;
			break;
			default:
			break;
		}
	}
	
	lineEdit_PilotName->setText(pilotName);
	
	// glider
	ft_ftstring2string(str, (char*)&arr[GLYDER_TYPE_POS]);
	glider = str;
	dbGlider = m_gliderList.at(comboBoxModel->currentItem()).model();
//	dbGlider = dbPilot.glider().model();
	
	if(glider != dbGlider)
	{
		syncRes = QMessageBox::question(this, "Different gliders",
				"Glider on GPS differ from glider in database. Set glider?",
				"DB to Dialog", "Ignore");
		
		switch(syncRes)
		{
			case 0: // From Pilot
				dbPilot.glider().fullName(dbGlider);
				selectGlider(dbGlider);
			break;
			default:
			break;
		}
	}
	
	// callsign
	ft_ftstring2string(str, (char*)&arr[GLYDER_ID_POS]);
	callsign = str;
	
	if(callsign != dbPilot.callSign().left(callsign.length()))
	{
		syncRes = QMessageBox::question(this, "Different callsigns", "Callsign on GPS differ from callsign in database. Set callsign?",
			 	"DB to GPS", "Ignore");
		
		switch(syncRes)
		{
			case 0: // From database
				callsign = dbPilot.callSign();
			break;
			default:
			break;
		}
	}
	
	// glider id
	lineEdit_GliderID->setText(callsign);

	// battery type
	comboBox_BattType->setCurrentItem(arr[BATT_TYPE_POS]);
*/
}

void DeviceFrame6015Impl::store(QByteArray &arr)
{
/*
	QString pilotName;
	Pilot dbPilot;
	QString dbPilotName;
	QString dbGlider;
	QString callsign;
	QString rcCallsign;
	QString glider;
	QString rcGlider;
	int syncRes;
	
	// pilot
	pilotName = lineEdit_PilotName->text();
	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), dbPilot);
	dbPilot.fullName(dbPilotName);
	
	if(pilotName != dbPilotName.left(pilotName.length()))
	{
		syncRes = QMessageBox::question(this, "Different pilots", "Pilot in database differ from pilot in dialog. Set pilot?",
				"DB to GPS", "Ignore");
		
		switch(syncRes)
		{
			case 0: // From database
				pilotName = dbPilotName;
				lineEdit_PilotName->setText(dbPilotName);
			break;
			default:
			break;
		}
	}
	
	ft_string2ftstring(pilotName.ascii(), (char*)&arr[PILOT_NAME_POS]);
	
	// glider
	m_gliderList.at(comboBoxModel->currentItem()).fullName(glider);
	dbPilot.glider().fullName(dbGlider);
	
	if(glider != dbGlider)
	{
		syncRes = QMessageBox::question(this, "Different gliders",
				"Glider in database differ from glider in dialog. Set glider?",
				"DB to GPS", "Ignore");
		
		switch(syncRes)
		{
			case 0: // From Pilot
				selectGlider(glider);
				glider = dbPilot.glider().model();
			break;
			default:
				glider = m_gliderList.at(comboBoxModel->currentItem()).model();
			break;
		}
	}
	else
	{
		glider = m_gliderList.at(comboBoxModel->currentItem()).model();
	}

	ft_string2ftstring(glider.ascii(), (char*)&arr[GLYDER_TYPE_POS]);

	// callsign
	callsign = lineEdit_GliderID->text();
	
	if(callsign != dbPilot.callSign().left(callsign.length()))
	{
		syncRes = QMessageBox::question(this, "Different callsigns", "Callsign in database differ from callsign in dialog. Set callsign?",
				"DB to GPS", "Ignore");
		
		switch(syncRes)
		{
			case 0: // From database
				callsign = dbPilot.callSign();
				lineEdit_PilotName->setText(callsign);
			break;
			default:
			break;
		}
	}
	
	// glider id
	ft_string2ftstring(callsign.ascii(), (char*)&arr[GLYDER_ID_POS]);

	// battery type
	arr[BATT_TYPE_POS] = comboBox_BattType->currentItem();
*/
}

void DeviceFrame6015Impl::updateGlider()
{
	QStringList list;
	QString gliderName;
	Glider::GliderListType::iterator it;
	Pilot dbPilot;
	
	comboBoxModel->clear();
	m_gliderList.clear();
	ISql::pInstance()->gliderList(m_gliderList);
	
	for(it=m_gliderList.begin(); it!=m_gliderList.end(); it++)
	{
		(*it).fullName(gliderName);
		comboBoxModel->insertItem(gliderName);
	}

	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), dbPilot);
	dbPilot.glider().fullName(gliderName);
	selectGlider(gliderName);
}

void DeviceFrame6015Impl::selectGlider(const QString &name)
{
	QString str;
	int index;
	int maxIndex;
	bool found = false;
	
	maxIndex = comboBoxModel->count();
	
	for(index=0; index<maxIndex; index++)
	{
		found = (comboBoxModel->text(index) == name);
		
		if(found)
		{
			comboBoxModel->setCurrentItem(index);
			break;
		}
	}
}

#include "DeviceFrame6015Impl.moc"
