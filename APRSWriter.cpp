/*
 *   Copyright (C) 2010,2011,2012,2018 by Jonathan Naylor G4KLX
 *   Copyright (C) 2021 by Geoffrey Merck F4FXL / KC3FRA
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

#include <cassert>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <cassert>

#include "StringUtils.h"
#include "Log.h"
#include "APRSWriter.h"
#include "DStarDefines.h"
#include "Defs.h"
#include "Log.h"

CAPRSWriter::CAPRSWriter(const std::string& hostname, unsigned int port, const std::string& gateway, const std::string& password, const std::string& address) :
m_thread(NULL),
m_idTimer(1000U),
m_gateway(),
m_address(),
m_port(0U),
m_socket(NULL),
m_array()
#ifdef USE_GPSD
, m_gpsdEnabled(false),
m_gpsdAddress(),
m_gpsdPort(),
m_gpsdData()
#endif
{
	assert(!hostname.empty());
	assert(port > 0U);
	assert(!gateway.empty());
	assert(!password.empty());

	m_thread = new CAPRSWriterThread(gateway, password, address, hostname, port);

	m_gateway = gateway;
	m_gateway = m_gateway.substr(0, LONG_CALLSIGN_LENGTH - 1U);
	boost::trim(m_gateway);
}

CAPRSWriter::~CAPRSWriter()
{
	for(auto it = m_array.begin(); it != m_array.end(); it++) {
		delete it->second;
	}

	m_array.clear();
}

void CAPRSWriter::setPortFixed(const std::string& callsign, const std::string& band, double frequency, double offset, double range, double latitude, double longitude, double agl)
{
	std::string temp = callsign;
	temp.resize(LONG_CALLSIGN_LENGTH - 1U, ' ');
	temp += band;

	m_array[temp] = new CAPRSEntry(callsign, band, frequency, offset, range, latitude, longitude, agl);
}

#ifdef USE_GPSD
void CAPRSWriter::setPortGPSD(const std::string& callsign, const std::string& band, double frequency, double offset, double range, const std::string& address, const std::string& port)
{
	assert(!address.empty());
	assert(!port.empty());

	std::string temp = callsign;
	temp.resize(LONG_CALLSIGN_LENGTH - 1U, ' ');
	temp.append(band);

	m_array[temp] = new CAPRSEntry(callsign, band, frequency, offset, range, 0.0, 0.0, 0.0);

	m_gpsdEnabled = true;
	m_gpsdAddress = address;
	m_gpsdPort    = port;
}
#endif

bool CAPRSWriter::open()
{
#ifdef USE_GPSD
	if (m_gpsdEnabled) {
		int ret = ::gps_open(m_gpsdAddress.c_str(), m_gpsdPort.c_str(), &m_gpsdData);
		if (ret != 0) {
			CLog::logError("Error when opening access to gpsd - %d - %s", errno, ::gps_errstr(errno));
			return false;
		}

		::gps_stream(&m_gpsdData, WATCH_ENABLE | WATCH_JSON, NULL);

		CLog::logError("Connected to GPSD");
	}
#endif

	if (m_socket != NULL) {
		bool ret = m_socket->open();
		if (!ret) {
			delete m_socket;
			m_socket = NULL;
			return false;
		}

		// Poll the GPS every minute
		m_idTimer.setTimeout(60U);
	} else {
		m_idTimer.setTimeout(20U * 60U);
	}

	m_idTimer.start();

	return m_thread->start();
}

void CAPRSWriter::writeHeader(const std::string& callsign, const CHeaderData& header)
{
	CAPRSEntry* entry = m_array[callsign];
	if (entry == NULL) {
		CLog::logError("Cannot find the callsign \"%s\" in the APRS array", callsign.c_str());
		return;
	}

	entry->reset();

	CAPRSCollector* collector = entry->getCollector();

	collector->writeHeader(header.getMyCall1());
}

void CAPRSWriter::writeData(const std::string& callsign, const CAMBEData& data)
{
	if (data.isEnd())
		return;

	CAPRSEntry* entry = m_array[callsign];
	if (entry == NULL) {
		CLog::logError("Cannot find the callsign \"%s\" in the APRS array", callsign.c_str());
		return;
	}

	CAPRSCollector* collector = entry->getCollector();

	if (data.isSync()) {
		collector->sync();
		return;
	}

	unsigned char buffer[400U];
	data.getData(buffer, DV_FRAME_MAX_LENGTH_BYTES);

	bool complete = collector->writeData(buffer + VOICE_FRAME_LENGTH_BYTES);
	if (!complete)
		return;

	if (!m_thread->isConnected()) {
		collector->reset();
		return;
	}

	// Check the transmission timer
	bool ok = entry->isOK();
	if (!ok) {
		collector->reset();
		return;
	}

	unsigned int length = collector->getData(buffer, 400U);
	std::string text((char*)buffer, length);

	auto n = text.find(':');
	if (n == std::string::npos) {
		collector->reset();
		return;
	}

	std::string header = text.substr(0, n);
	std::string body   = text.substr(n + 1U);

	// If we already have a q-construct, don't send it on
	n = header.find('q');
	if (n != std::string::npos)
		return;

	// Remove the trailing \r
	n = body.find('\r');
	if (n != std::string::npos)
		body = body.substr(0, n);

	std::string output = CStringUtils::string_format("%s,qAR,%s-%s:%s", header.c_str(), entry->getCallsign().c_str(), entry->getBand().c_str(), body.c_str());

	char ascii[500U];
	::memset(ascii, 0x00, 500U);
	for (unsigned int i = 0U; i < output.length(); i++)
		ascii[i] = output[i];

	m_thread->write(ascii);

	collector->reset();
}

void CAPRSWriter::clock(unsigned int ms)
{
	m_idTimer.clock(ms);

	m_thread->clock(ms);

#ifdef USE_GPSD
	if (m_gpsdEnabled) {
		if (m_idTimer.hasExpired()) {
			sendIdFramesMobile();
			m_idTimer.start();
		}

	} else {
#endif
		if (m_idTimer.hasExpired()) {
			sendIdFramesFixed();
			m_idTimer.start();
		}
#ifdef USE_GPSD
	}
#endif

	for (auto it = m_array.begin(); it != m_array.end(); ++it) {
		if(it->second != NULL)
			it->second->clock(ms);
	}
}

bool CAPRSWriter::isConnected() const
{
	return m_thread->isConnected();
}

void CAPRSWriter::close()
{
#ifdef USE_GPSD
	if (m_gpsdEnabled) {
		::gps_stream(&m_gpsdData, WATCH_DISABLE, NULL);
		::gps_close(&m_gpsdData);
	}
#endif

	if (m_socket != NULL) {
		m_socket->close();
		delete m_socket;
	}

	m_thread->stop();
}

bool CAPRSWriter::pollGPS()
{
	assert(m_socket != NULL);

	return m_socket->write((unsigned char*)"ircDDBGateway", 13U, m_address, m_port);
}

void CAPRSWriter::sendIdFramesFixed()
{
	if (!m_thread->isConnected())
		return;

	time_t now;
	::time(&now);
	struct tm* tm = ::gmtime(&now);

	for (auto it = m_array.begin(); it != m_array.end(); ++it) {
		CAPRSEntry* entry = it->second;
		if (entry == NULL)
			continue;

		// Default values aren't passed on
		if (entry->getLatitude() == 0.0 && entry->getLongitude() == 0.0)
			continue;

		std::string desc;
		if (entry->getBand().length() > 1U) {
			if (entry->getFrequency() != 0.0)
				desc = CStringUtils::string_format("Data %.5lfMHz", entry->getFrequency());
			else
				desc = "Data";
		} else {
			if (entry->getFrequency() != 0.0)
				desc = CStringUtils::string_format("Voice %.5lfMHz %c%.4lfMHz",
						entry->getFrequency(),
						entry->getOffset() < 0.0 ? '-' : '+',
						::fabs(entry->getOffset()));
			else
				desc = "Voice";
		}

		std::string band;
		if (entry->getFrequency() >= 1200.0)
			band = "1.2";
		else if (entry->getFrequency() >= 420.0)
			band = "440";
		else if (entry->getFrequency() >= 144.0)
			band = "2m";
		else if (entry->getFrequency() >= 50.0)
			band = "6m";
		else if (entry->getFrequency() >= 28.0)
			band = "10m";

		double tempLat  = ::fabs(entry->getLatitude());
		double tempLong = ::fabs(entry->getLongitude());

		double latitude  = ::floor(tempLat);
		double longitude = ::floor(tempLong);

		latitude  = (tempLat  - latitude)  * 60.0 + latitude  * 100.0;
		longitude = (tempLong - longitude) * 60.0 + longitude * 100.0;

		std::string lat;
		if (latitude >= 1000.0F)
			lat = CStringUtils::string_format("%.2lf", latitude);
		else if (latitude >= 100.0F)
			lat = CStringUtils::string_format("0%.2lf", latitude);
		else if (latitude >= 10.0F)
			lat = CStringUtils::string_format("00%.2lf", latitude);
		else
			lat = CStringUtils::string_format("000%.2lf", latitude);

		std::string lon;
		if (longitude >= 10000.0F)
			lon = CStringUtils::string_format("%.2lf", longitude);
		else if (longitude >= 1000.0F)
			lon = CStringUtils::string_format("0%.2lf", longitude);
		else if (longitude >= 100.0F)
			lon = CStringUtils::string_format("00%.2lf", longitude);
		else if (longitude >= 10.0F)
			lon = CStringUtils::string_format("000%.2lf", longitude);
		else
			lon = CStringUtils::string_format("0000%.2lf", longitude);

		// Convert commas to periods in the latitude and longitude
		boost::replace_all(lat, ",", ".");
		boost::replace_all(lon, ",", ".");

		std::string output;
		output = CStringUtils::string_format("%s-S>APD5T1,TCPIP*,qAC,%s-GS:;%-7s%-2s*%02d%02d%02dz%s%cD%s%caRNG%04.0lf/A=%06.0lf %s %s",
			m_gateway.c_str(), m_gateway.c_str(), entry->getCallsign().c_str(), entry->getBand().c_str(),
			tm->tm_mday, tm->tm_hour, tm->tm_min,
			lat.c_str(), (entry->getLatitude() < 0.0F)  ? 'S' : 'N',
			lon.c_str(), (entry->getLongitude() < 0.0F) ? 'W' : 'E',
			entry->getRange() * 0.6214, entry->getAGL() * 3.28, band.c_str(), desc.c_str());

		char ascii[300U];
		::memset(ascii, 0x00, 300U);
		for (unsigned int i = 0U; i < output.length(); i++)
			ascii[i] = output[i];

		m_thread->write(ascii);

		if (entry->getBand().length() == 1U) {
			output = CStringUtils::string_format("%s-%s>APD5T2,TCPIP*,qAC,%s-%sS:!%s%cD%s%c&RNG%04.0lf/A=%06.0lf %s %s",
				entry->getCallsign().c_str(), entry->getBand().c_str(), entry->getCallsign().c_str(), entry->getBand().c_str(),
				lat.c_str(), (entry->getLatitude() < 0.0F)  ? 'S' : 'N',
				lon.c_str(), (entry->getLongitude() < 0.0F) ? 'W' : 'E',
				entry->getRange() * 0.6214, entry->getAGL() * 3.28, band.c_str(), desc.c_str());

			::memset(ascii, 0x00, 300U);
			for (unsigned int i = 0U; i < output.length(); i++)
				ascii[i] = output[i];

			m_thread->write(ascii);
		}
	}
}

#ifdef USE_GPSD
void CAPRSWriter::sendIdFramesMobile()
{
	if (!m_gpsdEnabled)
		return;

	if (!::gps_waiting(&m_gpsdData, 0))
		return;

#if GPSD_API_MAJOR_VERSION >= 7
	if (::gps_read(&m_gpsdData, NULL, 0) <= 0)
		return;
#else
	if (::gps_read(&m_gpsdData) <= 0)
		return;
#endif

#if GPSD_API_MAJOR_VERSION >= 11 // F4FXL 2021-12-31 not sure if 11 is porper version, best guess as i could not find any change log
	if(m_gpsdData.fix.mode < MODE_3D)
		return;
#else
	if (m_gpsdData.status != STATUS_FIX)
		return;
#endif

	bool latlonSet   = (m_gpsdData.set & LATLON_SET) == LATLON_SET;
	bool altitudeSet = (m_gpsdData.set & ALTITUDE_SET) == ALTITUDE_SET;
	bool velocitySet = (m_gpsdData.set & SPEED_SET) == SPEED_SET;
	bool bearingSet  = (m_gpsdData.set & TRACK_SET) == TRACK_SET;

	if (!latlonSet)
		return;

	float rawLatitude  = float(m_gpsdData.fix.latitude);
	float rawLongitude = float(m_gpsdData.fix.longitude);
#if GPSD_API_MAJOR_VERSION >= 9
	float rawAltitude  = float(m_gpsdData.fix.altMSL);
#else
	float rawAltitude  = float(m_gpsdData.fix.altitude);
#endif
	float rawVelocity  = float(m_gpsdData.fix.speed);
	float rawBearing   = float(m_gpsdData.fix.track);

	time_t now;
	::time(&now);
	struct tm* tm = ::gmtime(&now);

	for (auto it = m_array.begin(); it != m_array.end(); ++it) {
		CAPRSEntry* entry = it->second;
		if (entry == NULL)
			continue;

		std::string desc;
		if (entry->getBand().length() > 1U) {
			if (entry->getFrequency() != 0.0)
				desc = CStringUtils::string_format("Data %.5lfMHz", entry->getFrequency());
			else
				desc = "Data";
		} else {
			if (entry->getFrequency() != 0.0)
				desc = CStringUtils::string_format("Voice %.5lfMHz %c%.4lfMHz",
						entry->getFrequency(),
						entry->getOffset() < 0.0 ? '-' : '+',
						::fabs(entry->getOffset()));
			else
				desc = "Voice";
		}

		std::string band;
		if (entry->getFrequency() >= 1200.0)
			band = "1.2";
		else if (entry->getFrequency() >= 420.0)
			band = "440";
		else if (entry->getFrequency() >= 144.0)
			band = "2m";
		else if (entry->getFrequency() >= 50.0)
			band = "6m";
		else if (entry->getFrequency() >= 28.0)
			band = "10m";

		double tempLat  = ::fabs(rawLatitude);
		double tempLong = ::fabs(rawLongitude);

		double latitude  = ::floor(tempLat);
		double longitude = ::floor(tempLong);

		latitude  = (tempLat  - latitude)  * 60.0 + latitude  * 100.0;
		longitude = (tempLong - longitude) * 60.0 + longitude * 100.0;

		std::string lat;
		if (latitude >= 1000.0F)
			lat = CStringUtils::string_format("%.2lf", latitude);
		else if (latitude >= 100.0F)
			lat = CStringUtils::string_format("0%.2lf", latitude);
		else if (latitude >= 10.0F)
			lat = CStringUtils::string_format("00%.2lf", latitude);
		else
			lat = CStringUtils::string_format("000%.2lf", latitude);

		std::string lon;
		if (longitude >= 10000.0F)
			lon = CStringUtils::string_format("%.2lf", longitude);
		else if (longitude >= 1000.0F)
			lon = CStringUtils::string_format("0%.2lf", longitude);
		else if (longitude >= 100.0F)
			lon = CStringUtils::string_format("00%.2lf", longitude);
		else if (longitude >= 10.0F)
			lon = CStringUtils::string_format("000%.2lf", longitude);
		else
			lon = CStringUtils::string_format("0000%.2lf", longitude);

		// Convert commas to periods in the latitude and longitude
		boost::replace_all(lat, ",", ".");
		boost::replace_all(lon, ",", ".");

		std::string output1;
		output1 = CStringUtils::string_format("%s-S>APD5T1,TCPIP*,qAC,%s-GS:;%-7s%-2s*%02d%02d%02dz%s%cD%s%ca/A=%06.0lf",
			m_gateway.c_str(), m_gateway.c_str(), entry->getCallsign().c_str(), entry->getBand().c_str(),
			tm->tm_mday, tm->tm_hour, tm->tm_min,
			lat.c_str(), (rawLatitude < 0.0)  ? 'S' : 'N',
			lon.c_str(), (rawLongitude < 0.0) ? 'W' : 'E',
			rawAltitude * 3.28);

		std::string output2;
		if (bearingSet && velocitySet)
			output2 = CStringUtils::string_format("%03.0lf/%03.0lf", rawBearing, rawVelocity * 0.539957F);

		std::string output3;
		output3 = CStringUtils::string_format("RNG%04.0lf %s %s\r\n", entry->getRange() * 0.6214, band.c_str(), desc.c_str());

		char ascii[300U];
		::memset(ascii, 0x00, 300U);
		unsigned int n = 0U;
		for (unsigned int i = 0U; i < output1.length(); i++, n++)
			ascii[n] = output1[i];
		for (unsigned int i = 0U; i < output2.length(); i++, n++)
			ascii[n] = output2[i];
		for (unsigned int i = 0U; i < output3.length(); i++, n++)
			ascii[n] = output3[i];

		CLog::logDebug("APRS ==> %s%s%s", output1.c_str(), output2.c_str(), output3.c_str());

		m_thread->write(ascii);

		if (entry->getBand().length() == 1U) {
			if (altitudeSet)
				output1 = CStringUtils::string_format("%s-%s>APD5T2,TCPIP*,qAC,%s-%sS:!%s%cD%s%c&/A=%06.0lf",
					entry->getCallsign().c_str(), entry->getBand().c_str(), entry->getCallsign().c_str(), entry->getBand().c_str(),
					lat.c_str(), (rawLatitude < 0.0)  ? 'S' : 'N',
					lon.c_str(), (rawLongitude < 0.0) ? 'W' : 'E',
					rawAltitude * 3.28);
			else
				output1 = CStringUtils::string_format("%s-%s>APD5T2,TCPIP*,qAC,%s-%sS:!%s%cD%s%c&",
					entry->getCallsign().c_str(), entry->getBand().c_str(), entry->getCallsign().c_str(), entry->getBand().c_str(),
					lat.c_str(), (rawLatitude < 0.0)  ? 'S' : 'N',
					lon.c_str(), (rawLongitude < 0.0) ? 'W' : 'E');

			::memset(ascii, 0x00, 300U);
			unsigned int n = 0U;
			for (unsigned int i = 0U; i < output1.length(); i++, n++)
				ascii[n] = output1[i];
			for (unsigned int i = 0U; i < output2.length(); i++, n++)
				ascii[n] = output2[i];
			for (unsigned int i = 0U; i < output3.length(); i++, n++)
				ascii[n] = output3[i];

			CLog::logDebug("APRS ==> %s%s%s", output1.c_str(), output2.c_str(), output3.c_str());

			m_thread->write(ascii);
		}
	}
}
#endif
