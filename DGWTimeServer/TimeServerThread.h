/*
 *   Copyright (C) 2012,2013 by Jonathan Naylor G4KLX
 *   Copyright (C) 2022 by Geoffrey Merck F4FXL / KC3FRA
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

#include <unordered_map>

#include "SlowDataEncoder.h"
#include "UDPReaderWriter.h"
#include "TimeServerDefs.h"
#include "HeaderData.h"
#include "AMBEData.h"
#include "Thread.h"

class CIndexRecord {
public:
	CIndexRecord(const std::string& name, unsigned int start, unsigned int length) :
	m_name(name),
	m_start(start),
	m_length(length)
	{
	}

	std::string getName() const
	{
		return m_name;
	}

	unsigned int getStart() const
	{
		return m_start;
	}

	unsigned int getLength() const
	{
		return m_length;
	}

private:
	std::string     m_name;
	unsigned int m_start;
	unsigned int m_length;
};


class CTimeServerThread : public CThread
{
public:
	CTimeServerThread();
	~CTimeServerThread();

	bool setGateway(const std::string& callsign, const std::string& rpt1, const std::string& rpt2, const std::string& rpt3, const std::string& rpt4, const std::string& address, const std::string& dataPath);
	void setAnnouncements(LANGUAGE language, FORMAT format, INTERVAL interval);

	void * Entry();
	void kill();

private:
	std::string         m_callsign;
	std::string         m_callsignA;
	std::string         m_callsignB;
	std::string         m_callsignC;
	std::string         m_callsignD;
	std::string         m_callsignG;
	in_addr          m_address;
	std::string		 m_addressStr;
	LANGUAGE         m_language;
	FORMAT           m_format;
	INTERVAL         m_interval;
	unsigned char*   m_ambe;
	unsigned int     m_ambeLength;
	std::unordered_map<std::string, CIndexRecord*> m_index;
	unsigned int     m_seqNo;
	unsigned int     m_in;
	CSlowDataEncoder m_encoder;
	CAMBEData**      m_data;
	bool             m_killed;
	std::string		 m_dataPath;

	void sendTime(unsigned int hour, unsigned int min);

	std::vector<std::string> sendTimeEnGB1(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeEnGB2(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeEnUS1(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeEnUS2(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeDeDE1(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeDeDE2(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeFrFR(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeNlNL(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeSeSE(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeEsES(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimeNoNO(unsigned int hour, unsigned int min);
	std::vector<std::string> sendTimePtPT(unsigned int hour, unsigned int min);

	bool send(const std::vector<std::string>& words, unsigned int hour, unsigned int min);
	bool sendHeader(CUDPReaderWriter& socket, const CHeaderData& header);
	bool sendData(CUDPReaderWriter& socket, const CAMBEData& data);

	bool loadAMBE();
	bool readAMBE(const std::string& dir, const std::string& name);
	bool readIndex(const std::string& dir, const std::string& name);

	bool lookup(const std::string& id);
	void end();
};
