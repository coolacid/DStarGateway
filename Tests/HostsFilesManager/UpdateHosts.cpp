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
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <arpa/inet.h>

#include "HostsFilesManager.h"
#include "CacheManager.h"
#include "DStarDefines.h"

namespace HostsFilesManagerTests
{
    class HostsFilesManager_UpdateHosts : public ::testing::Test
    {
        protected:
        std::string m_internetPath;
        std::string m_customPath;
        CCacheManager * m_cache;

        HostsFilesManager_UpdateHosts() :
        m_internetPath(),
        m_customPath(),
        m_cache(nullptr)
        {

        }


        void SetUp() override
        {
            char buf[2048];
            auto size = ::readlink("/proc/self/exe", buf, 2048);
            if(size > 0) {
                std::string path(buf, size);
                auto lastSlashPos = path.find_last_of('/');
                path.resize(lastSlashPos);

                m_internetPath = path + "/HostsFilesManager/internet/";
                m_customPath = path + "/HostsFilesManager/custom/";
            }

            CHostsFilesManager::setDCS(false, "");
            CHostsFilesManager::setDextra(false, "");
            CHostsFilesManager::setXLX(false, "");
            CHostsFilesManager::setDPlus(false, "");

            if(m_cache != nullptr) delete m_cache;
            m_cache = new CCacheManager();
        }

        static bool dummyDownload(const std::string & a, const std::string & b)
        {
            std::cout << a << std::endl << b;
            return true;
        }
    };

    TEST_F(HostsFilesManager_UpdateHosts, DExtraFromInternet)
    {
        CHostsFilesManager::setDextra(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, "specify invalid custom path as we do not want to override hosts from the internet");
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);
        
        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("XRF123 G");

        EXPECT_NE(gw, nullptr) << "DExtra host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "XRF123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("1.1.1.1")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DEXTRA) << "Protocol mismatch";
    }


    TEST_F(HostsFilesManager_UpdateHosts, DExtraFromInternetCustomOverride)
    {
        CHostsFilesManager::setDextra(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, m_customPath);
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);

        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("XRF123 G");

        EXPECT_NE(gw, nullptr) << "DExtra host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "XRF123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("2.2.2.2")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DEXTRA) << "Protocol mismatch";
    }

    TEST_F(HostsFilesManager_UpdateHosts, DCSFromInternet)
    {
        CHostsFilesManager::setDCS(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, "specify invalid custom path as we do not want to override hosts from the internet");
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);
        
        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("DCS123 G");

        EXPECT_NE(gw, nullptr) << "DCS host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "DCS123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("1.1.1.1")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DCS) << "Protocol mismatch";
    }


    TEST_F(HostsFilesManager_UpdateHosts, DCSFromInternetCustomOverride)
    {
        CHostsFilesManager::setDCS(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, m_customPath);
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);

        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("DCS123 G");

        EXPECT_NE(gw, nullptr) << "DCS host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "DCS123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("2.2.2.2")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DCS) << "Protocol mismatch";
    }

    TEST_F(HostsFilesManager_UpdateHosts, DPlusFromInternet)
    {
        CHostsFilesManager::setDPlus(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, "specify invalid custom path as we do not want to override hosts from the internet");
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);
        
        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("REF123 G");

        EXPECT_NE(gw, nullptr) << "DPlus host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "REF123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("1.1.1.1")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DPLUS) << "Protocol mismatch";
    }


    TEST_F(HostsFilesManager_UpdateHosts, DPlusFromInternetCustomOverride)
    {
        CHostsFilesManager::setDPlus(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, m_customPath);
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);

        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("REF123 G");

        EXPECT_NE(gw, nullptr) << "DPlus host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "REF123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("2.2.2.2")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DPLUS) << "Protocol mismatch";
    }

    TEST_F(HostsFilesManager_UpdateHosts, XLXFromInternet)
    {
        CHostsFilesManager::setXLX(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, "specify invalid custom path as we do not want to override hosts from the internet");
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);
        
        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("XLX123 G");

        EXPECT_NE(gw, nullptr) << "XLX host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "XLX123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("1.1.1.1")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DCS) << "Protocol mismatch";
    }


    TEST_F(HostsFilesManager_UpdateHosts, XLXFromInternetCustomOverride)
    {
        CHostsFilesManager::setXLX(true, "files are actually not downloaded, we want to to check that DEXtra Hosts are stored as DExtra");
        CHostsFilesManager::setHostFilesDirectories(m_internetPath, m_customPath);
        CHostsFilesManager::setDownloadCallback(HostsFilesManager_UpdateHosts::dummyDownload);
        CHostsFilesManager::setCache(m_cache);

        CHostsFilesManager::UpdateHosts();
        auto gw = m_cache->findGateway("XLX123 G");

        EXPECT_NE(gw, nullptr) << "XLX host not found";
        EXPECT_STREQ(gw->getGateway().c_str(), "XLX123 G");
        EXPECT_EQ(gw->getAddress().s_addr, ::inet_addr("2.2.2.2")) << "Address missmatch";
        EXPECT_EQ(gw->getProtocol(), DP_DCS) << "Protocol mismatch";
    }
}