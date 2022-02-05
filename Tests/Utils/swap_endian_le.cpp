/*
 *   Copyright (c) 2022 by Geoffrey Merck F4FXL / KC3FRA
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

#include "Utils.h"
#include "StringUtils.h"
#include "Log.h"

class Utils_swap_endian_le : public ::testing::Test {
 
};

// This test will fail on big endian systems....
TEST_F(Utils_swap_endian_le, SwapUINT32_be) {
    uint32_t test = 0x12345678U;

    uint32_t res = CUtils::swap_endian_le(test);

    EXPECT_EQ(res, 0x78563412U);
}


