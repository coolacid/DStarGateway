/*
 *   Copyright (C) 2010,2012,2018 by Jonathan Naylor G4KLX
 *   Copyright (C) 2021-2022 by Geoffrey Merck F4FXL / KC3FRA
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

#include "DStarDefines.h"
#include "Defs.h"

class CSlowDataCollector
{
public:
    CSlowDataCollector(unsigned char slowDataType);
    virtual ~CSlowDataCollector();

    std::string getMyCall() const;
    void setMyCall(const std::string& mycall);
    bool writeData(const unsigned char* data);
    void sync();
    unsigned int getData(unsigned char* data, unsigned int length);
    void reset();
    unsigned char getDataType();

protected:
    virtual bool addData(const unsigned char* data) = 0;
    virtual unsigned int getDataInt(unsigned char* data, unsigned int length) = 0;
    virtual void resetInt() { }
    
private:
    unsigned char m_slowDataType;
    std::string m_myCall;
    SLOWDATA_STATE m_state;
    unsigned char * m_buffer;
};