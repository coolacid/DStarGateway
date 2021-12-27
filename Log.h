/*
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
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "StringUtils.h"

enum LOG_SEVERITY {
    LS_TRACE = 1,
    LS_DEBUG,
    LS_INFO,
    LS_WARNING,
    LS_ERROR,
    LS_FATAL
};

class CLog
{
private:
    static LOG_SEVERITY m_level;
    static std::string m_file;
    static bool m_logToConsole;

    static void getTimeStamp(std::string & s);

public:
    
    static void initialize(const std::string& logfile, LOG_SEVERITY logLevel, bool logToConsole);

    template<typename... Args> static void logDebug(const std::string & f, Args... args)
    {
        log(LS_DEBUG, f, args...);
    }

    template<typename... Args> static void logInfo(const std::string & f, Args... args)
    {
        log(LS_INFO, f, args...);
    }

    template<typename... Args> static void logWarning(const std::string & f, Args... args)
    {
        log(LS_WARNING, f, args...);
    }

    template<typename... Args> static void logError(const std::string & f, Args... args)
    {
        log(LS_ERROR, f, args...);
    }

    template<typename... Args> static void logFatal(const std::string & f, Args... args)
    {
        log(LS_FATAL, f, args...);
    }

    template<typename... Args> static void log(LOG_SEVERITY severity, const std::string & f, Args... args)
    {
        if(severity >= CLog::m_level || CLog::m_file.empty()) {
            std::string severityStr;
            switch (severity)
            {
            case LS_DEBUG:
                severityStr = "DEBUG  ";
                break;
            case LS_ERROR:
                severityStr = "ERROR  ";
                break;
            case LS_FATAL:
                severityStr = "FATAL  ";
                break;
            case LS_INFO :
                severityStr = "INFO   ";
                break;
            case LS_WARNING:
                severityStr = "WARNING";
                break;
            case LS_TRACE:
                severityStr = "TRACE  ";
                break;
            default:
                break;
            }

            std::string message = CStringUtils::string_format(f, args...);
            boost::trim(message);
            std::string timeUtc;
            getTimeStamp(timeUtc);
            std::stringstream s;
            s << "[" <<  timeUtc << "] [" << severityStr << "] " << message << std::endl;

            if(CLog::m_logToConsole || CLog::m_file.empty())
                std::cout << s.str();

            std::ofstream file;
            file.open(CLog::m_file, std::ios::app);
            if(file.is_open()) {
                file << s.str();
                file.close();
            }
        }
    }
};
