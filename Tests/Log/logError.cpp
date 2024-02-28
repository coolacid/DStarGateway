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
#include <gmock/gmock.h>

#include "Log.h"
#include "LogSeverity.h"
#include "FakeLogTarget.h"

using ::testing::EndsWith;

namespace LogErrorTests
{
    class Log_logError: public ::testing::Test {
        protected:
            CFakeLogTarget * m_logTarget;

        void SetUp() override
        {
            m_logTarget = new CFakeLogTarget(LOG_ERROR);
            CLog::addTarget((CLogTarget *)m_logTarget);
        }

        void TearDown() override
        {
            CLog::finalise();
        }
    };

    TEST_F(Log_logError, OneMessage) {
        CLog::logError("One Message");

        EXPECT_EQ(1, m_logTarget->m_messages.size()) << "There should be  one message in the log.";
        EXPECT_THAT(m_logTarget->m_messages[0].c_str(), EndsWith("[ERROR  ] One Message\n"));
    }

    TEST_F(Log_logError, WrongLevel) {
        CLog::logDebug("One Message");

        EXPECT_EQ(0, m_logTarget->m_messages.size()) << "There should be no message in the log.";
    }

    TEST_F(Log_logError, TwoMessage) {
        CLog::logError("One Message");
        CLog::logError("Two Message");

        EXPECT_EQ(2, m_logTarget->m_messages.size()) << "There should be exactly two message in the log.";
        EXPECT_THAT(m_logTarget->m_messages[0].c_str(), EndsWith("[ERROR  ] One Message\n"));
        EXPECT_THAT(m_logTarget->m_messages[1].c_str(), EndsWith("[ERROR  ] Two Message\n"));
    }

    TEST_F(Log_logError, ThreeIdenticalMessage) {
        CLog::logError("One Message");
        CLog::logError("One Message");
        CLog::logError("One Message");

        EXPECT_EQ(1, m_logTarget->m_messages.size()) << "There should be one message in the log.";
        EXPECT_THAT(m_logTarget->m_messages[0].c_str(), EndsWith("[ERROR  ] One Message\n"));
    }

    TEST_F(Log_logError, NineIdenticalMessageOneDifferent) {
        CLog::logError("One Message");
        CLog::logError("One Message");
        CLog::logError("One Message");        
        CLog::logError("One Message");
        CLog::logError("One Message");       
        CLog::logError("One Message");
        CLog::logError("One Message");        
        CLog::logError("One Message");
        CLog::logError("One Message");
        CLog::logError("Another Message");

        EXPECT_EQ(3, m_logTarget->m_messages.size()) << "There should be two message in the log.";
        EXPECT_THAT(m_logTarget->m_messages[0].c_str(), EndsWith("[ERROR  ] One Message\n"));
        EXPECT_THAT(m_logTarget->m_messages[1].c_str(), EndsWith("[ERROR  ] Previous message repeated 8 times\n"));
        EXPECT_THAT(m_logTarget->m_messages[2].c_str(), EndsWith("[ERROR  ] Another Message\n"));
    }
}