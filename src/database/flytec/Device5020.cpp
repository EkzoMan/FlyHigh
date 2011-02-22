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
 
#include <QString>

#include <sys/ioctl.h>
#include <linux/serial.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "Device5020.h"

Device5020::Device5020()
{
	m_tlg = "";
	m_ttyFd = -1;
	m_tout = 0;
}

Device5020::~Device5020()
{
}

bool Device5020::openDevice(const QString &dev, int baud)
{
	struct termios sTermSet;
	int ret;
	bool success;

	m_ttyFd = open(dev.toAscii().constData(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	success = (m_ttyFd > 0);

	if(isatty(m_ttyFd))
	{
		bzero(&sTermSet, sizeof(sTermSet)); 
		sTermSet.c_iflag = IGNPAR | IXON | IXOFF;
		sTermSet.c_cflag = B57600 | CS8 | CLOCAL | CREAD;
		sTermSet.c_lflag = 0;
		sTermSet.c_cc[VSTART] = 0x11;     /* DC1 */
		sTermSet.c_cc[VSTOP] = 0x13;     /* DC3 */
		sTermSet.c_cc[VTIME] = 0;   /* inter-character timer unused */
		sTermSet.c_cc[VMIN] = 0;   /* 0 not blocking if 1 blocking read until 1 chars received */

		ret = tcsetattr(m_ttyFd, TCSANOW, &sTermSet);
		success = (ret == 0);

		flush();
	}

	return success;
}

bool Device5020::isOpen()
{
	return isatty(m_ttyFd);
}

void Device5020::closeDevice()
{
	if(m_ttyFd != -1)
	{
		close(m_ttyFd);
	}
}

bool Device5020::recieveTlg(int tout)
{
	int charNr = 0;
	bool validTlg = false;
	char ch;

	m_tlg = "";
	startTimer(tout);

	do
	{
		if(getChar(ch))
		{
			charNr++;
			m_tlg += ch;
			validTlg = (ch == '\n');

			if(validTlg || (charNr >= MaxTlgSize))
			{
				break;
			}

			if(m_tlg.length() >= MaxTlgSize)
			{
				m_tlg = "";
			}
		}
	}while(!isElapsed());

	return validTlg;
}

const QString& Device5020::getTlg()
{
	return m_tlg;
}

bool Device5020::sendTlg(const QString &tlg)
{
	return writeBuffer(tlg.toAscii().constData(), tlg.length());
}

void Device5020::flush()
{
	char ch;

	m_tlg = "";
	while(getChar(ch)){}; // Flush buffer
}

bool Device5020::getChar(char &ch)
{
	int nRead;

	nRead = read(m_ttyFd, &ch, 1);

	return (nRead == 1);
}

bool Device5020::writeBuffer(const char *pBuff, int len)
{
	int nWrite;
	bool success;

	nWrite = write(m_ttyFd, pBuff, len);
	success = (nWrite > 0);

	if(success)
	{
		success = (nWrite == len);
	}

	return success;
}

void Device5020::startTimer(int tout)
{
	struct timeb tb;

	ftime(&tb);
	m_tout = (tb.time % 1000) * 1000 + tb.millitm + tout;
}

bool Device5020::isElapsed()
{
	struct timeb tb;
	int curTime;

	ftime(&tb);
	curTime = (tb.time % 1000) * 1000 + tb.millitm;

	return (curTime >= m_tout);
}
