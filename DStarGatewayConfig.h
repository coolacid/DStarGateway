/*
 *   Copyright (C) 2010,2011,2012,2014 by Jonathan Naylor G4KLX
 *   Copyright (c) 2021 by Geoffrey Merck F4FXL / KC3FRA
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#pragma once

#include <string>
#include <vector>
#include <libconfig.h++>
#include "Defs.h"

using namespace libconfig;

typedef struct {
	GATEWAY_TYPE type;
	std::string callsign;
	std::string address;
	std::string hbAddress;
	unsigned int hbPort;
	std::string icomAddress;
	unsigned int icomPort;
	double latitude;
	double longitude;
	std::string description1;
	std::string description2;
	std::string url; 
	TEXT_LANG language;
} TGateway;

typedef struct {
	std::string band;
	std::string callsign;
	std::string reflector;
	std::string address;
	unsigned int port;
	HW_TYPE hwType;
	bool reflectorAtStartup;
	RECONNECT reflectorReconnect;
#ifdef USE_DRATS
	bool dRatsEnabled;
#endif
	double frequency;
	double offset;
	double range;
	double latitude;
	double longitude;
	double agl;
	std::string description1;
	std::string description2;
	std::string url;
	char band1;
	char band2;
	char band3;
} TRepeater;

typedef struct {
	std::string hostname;
	std::string username;
	std::string password;
	bool isQuadNet;
} TircDDB;

typedef struct {
	std::string logDir;
	std::string dataDir;
} Tpaths;

typedef struct {
	bool enabled;
	std::string hostname;
	unsigned int port;
	std::string password;
} TAPRS;

class CDStarGatewayConfig {
public:
	CDStarGatewayConfig(const std::string &pathname);
	~CDStarGatewayConfig();

	bool load();
	void getGateway(TGateway & gateway) const;
	void getIrcDDB(unsigned int ircddbIndex, TircDDB & ircddb) const;
	unsigned int getIrcDDBCount() const;
	void getRepeater(unsigned int repeaterIndex, TRepeater & repeater) const;
	unsigned int getRepeaterCount() const;
	void getPaths(Tpaths & paths) const;
	void getAPRS(TAPRS & aprs) const;

private:
	bool open(Config & cfg);
	bool loadGateway(const Config & cfg);
	bool loadIrcDDB(const Config & cfg);
	bool loadRepeaters(const Config & cfg);
	bool loadPaths(const Config & cfg);
	bool loadAPRS(const Config & cfg);
	bool get_value(const Config &cfg, const std::string &path, unsigned int &value, unsigned int min, unsigned int max, unsigned int default_value);
	bool get_value(const Config &cfg, const std::string &path, int &value, int min, int max, int default_value);
	bool get_value(const Config &cfg, const std::string &path, double &value, double min, double max, double default_value);
	bool get_value(const Config &cfg, const std::string &path, bool &value, bool default_value);
	bool get_value(const Config &cfg, const std::string &path, std::string &value, int min, int max, const std::string &default_value);
	bool get_value(const Config &cfg, const std::string &path, std::string &value, int min, int max, const std::string &default_value, bool emptyToDefault);
	bool get_value(const Config &cfg, const std::string &path, std::string &value, int min, int max, const std::string &default_value, bool emptyToDefault, const std::vector<std::string>& allowedValues);

	std::string m_fileName;
	TGateway m_gateway;
	Tpaths m_paths;
	TAPRS m_aprs;
	std::vector<TRepeater *> m_repeaters;
	std::vector<TircDDB *> m_ircDDB;
};
