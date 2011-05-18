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

#include <QSqlDatabase>
#include <QDebug>
#include "QueryStore.h"

// initialize statics
QueryStore* QueryStore::m_pInst = NULL;

QueryStore* QueryStore::pInstance()
{
	if(m_pInst == NULL)
	{
		m_pInst = new QueryStore();
	}

	return m_pInst;
}

QueryStore::QueryStore()
{
	init();
}

void QueryStore::init()
{
	m_QueryMap.insert("migrate-read-routeitems",
										"SELECT Id, RouteId, WayPointId FROM RouteItems");
	m_QueryMap.insert("migrate-write-routeitems",
										"INSERT INTO RouteItems (Id, RouteId, WayPointId) VALUES (?,?,?)");
	m_QueryMap.insert("migrate-read-waypoints",
										"SELECT Id, Name, Spot, Country, Longitude, Latitude, Altitude, Description FROM WayPoints");
	m_QueryMap.insert("migrate-write-waypoints",
										"INSERT INTO WayPoints (Id, Name, Spot, Country, Longitude, Latitude, Altitude, Description) VALUES (?,?,?,?,?,?,?,?)");
	m_QueryMap.insert("migrate-read-gliders",
										"SELECT Id, Manufacturer, Model, Serial FROM Gliders");
	m_QueryMap.insert("migrate-write-gliders",
										"INSERT INTO Gliders (Id, Manufacturer, Model, Serial) VALUES (?,?,?,?)");
	m_QueryMap.insert("migrate-read-servicings",
										"SELECT Id, GliderId, Date, Responsibility, Comment FROM Servicings");
	m_QueryMap.insert("migrate-write-servicings",
										"INSERT INTO Servicings (Id, GliderId, Date, Responsibility, Comment) VALUES (?,?,?,?,?)");
	m_QueryMap.insert("migrate-read-lastmodified",
										"SELECT Id, Name, Time FROM LastModified");
	m_QueryMap.insert("migrate-write-lastmodified",
										"INSERT INTO LastModified (Id, Name, Time) VALUES (?,?,?)");
	m_QueryMap.insert("migrate-read-pilots",
										"SELECT Id, FirstName, LastName, BirthDate, CallSign, GliderId FROM Pilots");
	m_QueryMap.insert("migrate-write-pilots",
										"INSERT INTO Pilots (Id, FirstName, LastName, BirthDate, CallSign, GliderId) VALUES (?,?,?,?,?,?)");
	m_QueryMap.insert("migrate-read-routes",
										"SELECT Id, Name, Type FROM Routes");
	m_QueryMap.insert("migrate-write-routes",
										"INSERT INTO Routes (Id, Name, Type) VALUES (?,?,?)");
	m_QueryMap.insert("migrate-read-flights",
										"SELECT Id, Number, PilotId, Date, Time, GliderId, StartPtId, LandPtId, Duration, Distance, Comment, IGCFile FROM Flights");
	m_QueryMap.insert("migrate-write-flights",
										"INSERT INTO Flights (Id, Number, PilotId, Date, Time, GliderId, StartPtId, LandPtId, Duration, Distance, Comment, IGCFile) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)");

	// does not work with sqlite: http://bugreports.qt.nokia.com/browse/QTBUG-8689
	/*m_QueryMap.insert("migrate-drop-tables",
										"DROP TABLE IF EXISTS Flights; "
										"DROP TABLE IF EXISTS Routes; "
										"DROP TABLE IF EXISTS LastModified; "
										"DROP TABLE IF EXISTS Pilots; "
										"DROP TABLE IF EXISTS Servicings; "
										"DROP TABLE IF EXISTS Gliders; "
										"DROP TABLE IF EXISTS WayPoints; "
										"DROP TABLE IF EXISTS RouteItems");*/

	m_QueryMap.insert("migrate-drop-flights",
										"DROP TABLE IF EXISTS Flights");
	m_QueryMap.insert("migrate-drop-routes",
										"DROP TABLE IF EXISTS Routes");
	m_QueryMap.insert("migrate-drop-lastmodified",
										"DROP TABLE IF EXISTS LastModified");
	m_QueryMap.insert("migrate-drop-pilots",
										"DROP TABLE IF EXISTS Pilots");
	m_QueryMap.insert("migrate-drop-servicings",
										"DROP TABLE IF EXISTS Servicings");
	m_QueryMap.insert("migrate-drop-gliders",
										"DROP TABLE IF EXISTS Gliders");
	m_QueryMap.insert("migrate-drop-waypoints",
										"DROP TABLE IF EXISTS WayPoints");
	m_QueryMap.insert("migrate-drop-routeitems",
										"DROP TABLE IF EXISTS RouteItems");


	m_QueryMapMysql.insert("setup-create-gliders",
												 "CREATE TABLE `Gliders`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`Manufacturer` VARCHAR(16) CHARACTER SET utf8 NULL DEFAULT '',"
												 "`Model` VARCHAR(16) CHARACTER SET utf8 NOT NULL,"
												 "`Serial` VARCHAR(16) CHARACTER SET utf8 NULL DEFAULT '',"
												 "PRIMARY KEY(`Id`),"
												 "UNIQUE KEY(`Manufacturer`, `Model`, `Serial`)"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-gliders",
													"CREATE TABLE `Gliders`"
													"("
													"`Id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
													"`Manufacturer` VARCHAR(16) DEFAULT '',"
													"`Model` VARCHAR(16) NOT NULL,"
													"`Serial` VARCHAR(16) DEFAULT '')");

	m_QueryMapMysql.insert("setup-create-pilots",
												 "CREATE TABLE `Pilots`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`FirstName` VARCHAR(16) CHARACTER SET utf8 NOT NULL,"
												 "`LastName` VARCHAR(16) CHARACTER SET utf8 NOT NULL,"
												 "`BirthDate` DATE NULL DEFAULT '0000-00-00',"
												 "`CallSign` VARCHAR(16) CHARACTER SET utf8 NOT NULL,"
												 "`GliderId` INT NULL DEFAULT '0',"
												 "PRIMARY KEY(`Id`),"
												 "UNIQUE KEY byPilot(`FirstName`, `LastName`, `BirthDate`),"
												 "FOREIGN KEY(`GliderId`) REFERENCES Gliders(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-pilots",
													"CREATE TABLE `Pilots`"
													"("
													"`Id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
													"`FirstName` VARCHAR(16) NOT NULL,"
													"`LastName` VARCHAR(16) NOT NULL,"
													"`BirthDate` DATE DEFAULT '0000-00-00',"
													"`CallSign` VARCHAR(16) NOT NULL,"
													"`GliderId` INTEGER DEFAULT '0',"
													"CONSTRAINT Pilots_fk1 FOREIGN KEY (`GliderId`)"
													" REFERENCES `Gliders` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE)");

	m_QueryMapMysql.insert("setup-create-waypoints",
												 "CREATE TABLE `WayPoints`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`Name` VARCHAR(16) NOT NULL,"
												 "`Spot` VARCHAR(16) NULL DEFAULT '',"
												 "`Country` VARCHAR(2) NULL DEFAULT '',"
												 "`Longitude` FLOAT NOT NULL,"
												 "`Latitude` FLOAT NOT NULL,"
												 "`Altitude` INT NOT NULL,"
												 "`Description` VARCHAR(200) NULL DEFAULT '',"
												 "PRIMARY KEY(`Id`),"
												 "UNIQUE KEY byWayPoints(`Name`, `Spot`, `Country`)"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-waypoints",
													"CREATE TABLE `WayPoints`"
													"("
													"`Id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
													"`Name` VARCHAR(16) NOT NULL,"
													"`Spot` VARCHAR(16) DEFAULT '',"
													"`Country` VARCHAR(2) DEFAULT '',"
													"`Longitude` FLOAT NOT NULL,"
													"`Latitude` FLOAT NOT NULL,"
													"`Altitude` INTEGER NOT NULL,"
													"`Description` VARCHAR(200) DEFAULT '')");

	m_QueryMapMysql.insert("setup-create-flights",
												 "CREATE TABLE `Flights`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`Number` INT NOT NULL,"
												 "`PilotId` INT NOT NULL,"
												 "`Date` DATE NOT NULL,"
												 "`Time` TIME NOT NULL,"
												 "`GliderId` INT NOT NULL,"
												 "`StartPtId` INT NOT NULL,"
												 "`LandPtId` INT NOT NULL,"
												 "`Duration` INT NOT NULL,"
												 "`Distance` INT NOT NULL,"
												 "`Comment` VARCHAR(500) NULL DEFAULT '',"
												 "`IGCFile` MEDIUMBLOB,"
												 "PRIMARY KEY(`Id`),"
												 "UNIQUE KEY byFlight(`PilotId`, `Number`),"
												 "FOREIGN KEY(`PilotId`) REFERENCES Pilots(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
												 "FOREIGN KEY(`GliderId`) REFERENCES Gliders(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
												 "FOREIGN KEY(`StartPtId`) REFERENCES WayPoints(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
												 "FOREIGN KEY(`LandPtId`) REFERENCES WayPoints(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-flights",
													"CREATE TABLE `Flights`"
													"("
													"`Id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
													"`Number` INTEGER NOT NULL,"
													"`PilotId` INTEGER NOT NULL,"
													"`Date` DATE NOT NULL,"
													"`Time` TIME NOT NULL,"
													"`GliderId` INTEGER NOT NULL,"
													"`StartPtId` INTEGER NOT NULL,"
													"`LandPtId` INTEGER NOT NULL,"
													"`Duration` INTEGER NOT NULL,"
													"`Distance` INTEGER NOT NULL,"
													"`Comment` VARCHAR(500) DEFAULT '',"
													"`IGCFile` MEDIUMBLOB,"
													"CONSTRAINT Flights_fk1 FOREIGN KEY (`PilotId`)"
													" REFERENCES `Pilots` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
													"CONSTRAINT Flights_fk2 FOREIGN KEY (`GliderId`)"
													" REFERENCES `Gliders` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
													"CONSTRAINT Flights_fk3 FOREIGN KEY (`StartPtId`)"
													" REFERENCES `WayPoints` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
													"CONSTRAINT Flights_fk4 FOREIGN KEY (`LandPtId`)"
													" REFERENCES `WayPoints` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE)");

	m_QueryMapMysql.insert("setup-create-routes",
												 "CREATE TABLE `Routes`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`Name` VARCHAR(16) NOT NULL,"
												 "`Type` INT NULL DEFAULT 0,"
												 "PRIMARY KEY(`Id`),"
												 "UNIQUE KEY byRoutes(`Name`)"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-routes",
													"CREATE TABLE `Routes`"
													"("
													"`Id` INTEGER PRIMARY KEY AUTOINCREMENT,"
													"`Name` VARCHAR(16) NOT NULL,"
													"`Type` INTEGER DEFAULT 0)");

	m_QueryMapMysql.insert("setup-create-routeitems",
												 "CREATE TABLE `RouteItems`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`RouteId` INT NOT NULL,"
												 "`WayPointId` INT NOT NULL,"
												 "PRIMARY KEY(`Id`),"
												 "FOREIGN KEY(`RouteId`) REFERENCES Routes(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
												 "FOREIGN KEY(`WayPointId`) REFERENCES WayPoints(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-routeitems",
													"CREATE TABLE `RouteItems`"
													"("
													"`Id` INTEGER PRIMARY KEY AUTOINCREMENT,"
													"`RouteId` INTEGER NOT NULL,"
													"`WayPointId` INTEGER NOT NULL,"
													"CONSTRAINT RouteItems_fk1 FOREIGN KEY (`RouteId`)"
													" REFERENCES `Routes` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE,"
													"CONSTRAINT RouteItems_fk2 FOREIGN KEY (`WayPointId`)"
													" REFERENCES `WayPoints` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE)");

	m_QueryMapMysql.insert("setup-create-servicings",
												 "CREATE TABLE `Servicings`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`GliderId` INT NOT NULL,"
												 "`Date` DATE NOT NULL,"
												 "`Responsibility` VARCHAR(16) NOT NULL,"
												 "`Comment` VARCHAR(200) NULL DEFAULT '',"
												 "PRIMARY KEY(`Id`),"
												 "FOREIGN KEY(`GliderId`) REFERENCES Gliders(`Id`) ON DELETE RESTRICT ON UPDATE CASCADE"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-servicings",
													"CREATE TABLE `Servicings`"
													"("
													"`Id` INTEGER PRIMARY KEY AUTOINCREMENT,"
													"`GliderId` INTEGER NOT NULL,"
													"`Date` DATE NOT NULL,"
													"`Responsibility` VARCHAR(16) NOT NULL,"
													"`Comment` VARCHAR(200) DEFAULT '',"
													"CONSTRAINT Servicings_fk1 FOREIGN KEY (`GliderId`)"
													" REFERENCES `Gliders` (`Id`) ON DELETE RESTRICT ON UPDATE CASCADE)");

	m_QueryMapMysql.insert("setup-create-lastmodified",
												 "CREATE TABLE `LastModified`"
												 "("
												 "`Id` INT NULL AUTO_INCREMENT,"
												 "`Name` VARCHAR(16) NOT NULL,"
												 "`Time` datetime NOT NULL,"
												 "PRIMARY KEY(`Id`),"
												 "UNIQUE KEY (`Name`)"
												 ")ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");
	m_QueryMapSqlite.insert("setup-create-lastmodified",
													"CREATE TABLE `LastModified`"
													"("
													"`Id` INTEGER PRIMARY KEY AUTOINCREMENT,"
													"`Name` VARCHAR(16) NOT NULL,"
													"`Time` datetime NOT NULL)");
}

QString QueryStore::getQuery(const QString& name, const QSqlDatabase& db) const
{
	// check if there is a common query
	TQueryMap::const_iterator iter=m_QueryMap.constFind(name);
	if (iter!=m_QueryMap.constEnd())
	{
		return iter.value();
	}
	else
	{
		// check for mysql query
		if (db.driverName()=="QMYSQL")
		{
			iter = m_QueryMapMysql.constFind(name);
			if (iter!=m_QueryMapMysql.constEnd())
			{
				return iter.value();
			}
		}
		else if (db.driverName()=="QSQLITE")
		{
			iter = m_QueryMapSqlite.constFind(name);
			if (iter!=m_QueryMapSqlite.constEnd())
			{
				return iter.value();
			}
		}
	}

	// somethings' wrong...
	qDebug() << "query '" << name << "' not found for driver " << db.driverName();

	return "";
}