/***************************************************************************
 *   Copyright (C) 2013 by Alex Graf                                       *
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
#ifndef _ELEVATIONTILE_H
#define _ELEVATIONTILE_H

#include <QString>
#include "BoundBox.h"

class SrtmReader;

class ElevationTile
{
  public:
    enum Type {Undef, Srtm1, Srtm3};

    ElevationTile();

    ElevationTile(const QString &path);

    ~ElevationTile();

    void open();

    void close();

    double elevation(const LatLng &pos);

    void setBoundBox(const BoundBox &bbox);

    const BoundBox& boundBox() const;

    void setPath(const QString &path);

    const QString& path() const;

    void setType(Type type);

    Type type() const;

  private:
    QString m_path;
    SrtmReader *m_pSrtmReader;
    BoundBox m_bbox;
    Type m_type;
};
#endif
