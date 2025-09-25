/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.power.stats-service.pixel"

#include <PowerStatsAidl.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <dataproviders/DisplayStateResidencyDataProvider.h>
#include <dataproviders/GenericStateResidencyDataProvider.h>
#include <dataproviders/PixelStateResidencyDataProvider.h>
#include <dataproviders/WlanStateResidencyDataProvider.h>
#include <log/log.h>

using aidl::android::hardware::power::stats::PowerStats;
using aidl::android::hardware::power::stats::DisplayStateResidencyDataProvider;
using aidl::android::hardware::power::stats::GenericStateResidencyDataProvider;
using aidl::android::hardware::power::stats::PixelStateResidencyDataProvider;
using aidl::android::hardware::power::stats::WlanStateResidencyDataProvider;

using StateResidencyConfig = GenericStateResidencyDataProvider::StateResidencyConfig;
using PowerEntityConfig = GenericStateResidencyDataProvider::PowerEntityConfig;

int main() {
    LOG(INFO) << "Pixel PowerStats HAL AIDL Service is starting.";

    bool isDebuggable = android::base::GetBoolProperty("ro.debuggable", false);

    ABinderProcess_setThreadPoolMaxThreadCount(0);

    std::shared_ptr<PowerStats> service = ndk::SharedRefBase::make<PowerStats>();

    // Add power entities related to rpmh
    const uint64_t RPM_CLK = 19200;  // RPM runs at 19.2Mhz. Divide by 19200 for msec
    std::function<uint64_t(uint64_t)> rpmConvertToMs = [](uint64_t a) { return a / RPM_CLK; };
    const std::vector<StateResidencyConfig> rpmStateResidencyConfigs = {
            {.name = "Sleep",
             .entryCountSupported = true,
             .entryCountPrefix = "Sleep Count:",
             .totalTimeSupported = true,
             .totalTimePrefix = "Sleep Accumulated Duration:",
             .totalTimeTransform = rpmConvertToMs,
             .lastEntrySupported = true,
             .lastEntryPrefix = "Sleep Last Entered At:",
             .lastEntryTransform = rpmConvertToMs}};
    std::vector<PowerEntityConfig> rpmCfgs = {
            {rpmStateResidencyConfigs, "APSS", "APSS"},
            {rpmStateResidencyConfigs, "MPSS", "MPSS"},
            {rpmStateResidencyConfigs, "ADSP", "ADSP"},
            {rpmStateResidencyConfigs, "ADSP_ISLAND", "ADSP_ISLAND"},
            {rpmStateResidencyConfigs, "CDSP", "CDSP"},
    };

    service->addStateResidencyDataProvider(std::make_unique<GenericStateResidencyDataProvider>(
            "/sys/power/rpmh_stats/master_stats", rpmCfgs));

    // Add SoC power entity
    std::vector<StateResidencyConfig> socStateResidencyConfigs = {
            {.name = "AOSD",
             .header = "RPM Mode:aosd",
             .entryCountSupported = true,
             .entryCountPrefix = "count:",
             .totalTimeSupported = true,
             .totalTimePrefix = "actual last sleep(msec):",
             .lastEntrySupported = false},
            {.name = "CXSD",
             .header = "RPM Mode:cxsd",
             .entryCountSupported = true,
             .entryCountPrefix = "count:",
             .totalTimeSupported = true,
             .totalTimePrefix = "actual last sleep(msec):",
             .lastEntrySupported = false}};

    std::vector<PowerEntityConfig> socCfgs = {
            {socStateResidencyConfigs, "SoC", "SoC"},
    };

    service->addStateResidencyDataProvider(std::make_unique<GenericStateResidencyDataProvider>(
            "/sys/power/system_sleep/stats", socCfgs));

    if (isDebuggable) {
        // Add WLAN power entity
        service->addStateResidencyDataProvider(std::make_unique<WlanStateResidencyDataProvider>(
                "WLAN", "/sys/kernel/wifi/power_stats"));
    }

    std::vector<std::string> states = {
            "Off\n",
            "LP\n",
            "On: 1080x2340@60\n",
            "On: 1080x2340@90\n",
    };

    service->addStateResidencyDataProvider(std::make_unique<DisplayStateResidencyDataProvider>(
            "Display", "/sys/class/backlight/panel0-backlight/state", states));

    // Add NFC power entity
    StateResidencyConfig nfcStateConfig = {.entryCountSupported = true,
                                           .entryCountPrefix = "Cumulative count:",
                                           .totalTimeSupported = true,
                                           .totalTimePrefix = "Cumulative duration msec:",
                                           .lastEntrySupported = true,
                                           .lastEntryPrefix = "Last entry timestamp msec:"};
    std::vector<std::pair<std::string, std::string>> nfcStateHeaders = {
            std::make_pair("Idle", "Idle mode:"),
            std::make_pair("Active", "Active mode:"),
            std::make_pair("Active-RW", "Active Reader/Writer mode:"),
    };

    std::vector<PowerEntityConfig> nfcCfgs = {
            {generateGenericStateResidencyConfigs(nfcStateConfig, nfcStateHeaders), "NFC",
             "NFC subsystem"},
    };

    service->addStateResidencyDataProvider(std::make_unique<GenericStateResidencyDataProvider>(
            "/sys/class/misc/st21nfc/device/power_stats", nfcCfgs));

    // Add Power Entities that require the Aidl data provider
    auto pixelSdp = std::make_unique<PixelStateResidencyDataProvider>();

    pixelSdp->addEntity("Citadel", {{0, "Last-Reset"}, {1, "Active"}, {2, "Deep-Sleep"}});

    pixelSdp->start();

    service->addStateResidencyDataProvider(std::move(pixelSdp));

    const std::string instance = std::string() + PowerStats::descriptor + "/default";
    binder_status_t status =
            AServiceManager_addService(service->asBinder().get(), instance.c_str());
    LOG_ALWAYS_FATAL_IF(status != STATUS_OK);

    ABinderProcess_joinThreadPool();

    return EXIT_FAILURE;  // should not reach
}
