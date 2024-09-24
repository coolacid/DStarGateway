/*
 *   Copyright (C) 2021-2024 by Geoffrey Merck F4FXL / KC3FRA
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

#include <gtest/gtest.h>

#include "DTMF.h"

const unsigned char DTMF_MASK[] = {0x82U, 0x08U, 0x20U, 0x82U, 0x00U, 0x00U, 0x82U, 0x00U, 0x00U};
const unsigned char DTMF_SIG[]  = {0x82U, 0x08U, 0x20U, 0x82U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};

const unsigned char DTMF_SYM_MASK[] = {0x10U, 0x40U, 0x08U, 0x20U};
const unsigned char DTMF_SYM0[]     = {0x00U, 0x40U, 0x08U, 0x20U};
const unsigned char DTMF_SYM1[]     = {0x00U, 0x00U, 0x00U, 0x00U};
const unsigned char DTMF_SYM2[]     = {0x00U, 0x40U, 0x00U, 0x00U};
const unsigned char DTMF_SYM3[]     = {0x10U, 0x00U, 0x00U, 0x00U};
const unsigned char DTMF_SYM4[]     = {0x00U, 0x00U, 0x00U, 0x20U};
const unsigned char DTMF_SYM5[]     = {0x00U, 0x40U, 0x00U, 0x20U};
const unsigned char DTMF_SYM6[]     = {0x10U, 0x00U, 0x00U, 0x20U};
const unsigned char DTMF_SYM7[]     = {0x00U, 0x00U, 0x08U, 0x00U};
const unsigned char DTMF_SYM8[]     = {0x00U, 0x40U, 0x08U, 0x00U};
const unsigned char DTMF_SYM9[]     = {0x10U, 0x00U, 0x08U, 0x00U};
const unsigned char DTMF_SYMA[]     = {0x10U, 0x40U, 0x00U, 0x00U};
const unsigned char DTMF_SYMB[]     = {0x10U, 0x40U, 0x00U, 0x20U};
const unsigned char DTMF_SYMC[]     = {0x10U, 0x40U, 0x08U, 0x00U};
const unsigned char DTMF_SYMD[]     = {0x10U, 0x40U, 0x08U, 0x20U};
const unsigned char DTMF_SYMS[]     = {0x00U, 0x00U, 0x08U, 0x20U};
const unsigned char DTMF_SYMH[]     = {0x10U, 0x00U, 0x08U, 0x20U};

namespace DTMFTests
{
    class DTMF_decode : public ::testing::Test {
    
    };

    TEST_F(DTMF_decode, decode_valid_dtmf)
    {
        unsigned char ambe[] = {DTMF_SIG[0],
                                DTMF_SIG[1],
                                DTMF_SIG[2],
                                DTMF_SIG[3],
                                (unsigned char)(DTMF_SIG[4] | DTMF_SYM1[0]),
                                (unsigned char)(DTMF_SIG[5] | DTMF_SYM1[1]),
                                DTMF_SIG[6],
                                (unsigned char)(DTMF_SIG[7] | DTMF_SYM1[2]),
                                (unsigned char)(DTMF_SIG[8] | DTMF_SYM1[3])};

        CDTMF dtmf;
        dtmf.decode(ambe, false);
        dtmf.decode(ambe, false);
        dtmf.decode(ambe, false);
        dtmf.decode(ambe, false);
        dtmf.decode(ambe, false);
        dtmf.decode(ambe, false);
        dtmf.decode(ambe, true);
    }
}