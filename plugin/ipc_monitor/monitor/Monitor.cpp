/*
 * Copyright (C) 2026-2026. Huawei Technologies Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "monitor/Monitor.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include "monitor/MonitorProcessManager.h"
#include "MsptiMonitor.h"

namespace dynolog_npu {
namespace ipc_monitor {
namespace monitor {
constexpr uint32_t DEFAULT_CAPACITY = 1024;
Monitor::Monitor()
{
    // init log
    InitMsMonitorLog();
}

void Monitor::Start(const std::vector<msptiActivityKind>& kinds)
{
    const static std::unordered_set<msptiActivityKind> kSupportedKinds = {
        msptiActivityKind::MSPTI_ACTIVITY_KIND_API,
        msptiActivityKind::MSPTI_ACTIVITY_KIND_KERNEL,
        msptiActivityKind::MSPTI_ACTIVITY_KIND_MARKER,
        msptiActivityKind::MSPTI_ACTIVITY_KIND_COMMUNICATION,
        msptiActivityKind::MSPTI_ACTIVITY_KIND_ACL_API,
        msptiActivityKind::MSPTI_ACTIVITY_KIND_NODE_API,
        msptiActivityKind::MSPTI_ACTIVITY_KIND_RUNTIME_API
    };

    auto validKinds = kinds;
    validKinds.erase(std::remove_if(validKinds.begin(), validKinds.end(),
        [&kSupportedKinds](msptiActivityKind kind) { return kSupportedKinds.find(kind) == kSupportedKinds.end(); }),
        validKinds.end());

    if (validKinds.empty()) {
        LOG(WARNING) << "Invalid MsptiActivityKind";
        return;
    }

    auto msptiMonitor = MsptiMonitor::GetInstance();
    if (msptiMonitor->IsStarted()) {
        LOG(WARNING) << "MsptiMonitor already started";
        return;
    }

    Clear();

    std::shared_ptr<MonitorProcessManager> processManager{nullptr};
    MakeSharedPtr(processManager);
    msptiMonitor->SetDataProcessor(processManager);
    msptiMonitor->Start();
    kinds_ = std::unordered_set<msptiActivityKind>(validKinds.begin(), validKinds.end());

    for (auto kind : kinds_) {
        msptiMonitor->EnableActivity(kind);
    }

    LOG(INFO) << "monitor started";
}

void Monitor::Stop()
{
    auto msptiMonitor = MsptiMonitor::GetInstance();
    if (!msptiMonitor->IsStarted()) {
        LOG(WARNING) << "MsptiMonitor not started";
        return;
    }
    msptiMonitor->Stop();
    LOG(INFO) << "monitor stopped";
}

void Monitor::Clear()
{
    apiData_.clear();
    aclApiData_.clear();
    nodeApiData_.clear();
    runtimeApiData_.clear();
    kernelData_.clear();
    communicationData_.clear();
    markerData_.clear();
    kinds_.clear();

    apiData_.reserve(DEFAULT_CAPACITY);
    aclApiData_.reserve(DEFAULT_CAPACITY);
    nodeApiData_.reserve(DEFAULT_CAPACITY);
    runtimeApiData_.reserve(DEFAULT_CAPACITY);
    kernelData_.reserve(DEFAULT_CAPACITY);
    communicationData_.reserve(DEFAULT_CAPACITY);
    markerData_.reserve(DEFAULT_CAPACITY);
}

void Monitor::ReportAPIData(API &&api, msptiActivityKind kind)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    switch (kind) {
        case msptiActivityKind::MSPTI_ACTIVITY_KIND_API:
            apiData_.emplace_back(std::move(api));
            break;
        case msptiActivityKind::MSPTI_ACTIVITY_KIND_ACL_API:
            aclApiData_.emplace_back(std::move(api));
            break;
        case msptiActivityKind::MSPTI_ACTIVITY_KIND_NODE_API:
            nodeApiData_.emplace_back(std::move(api));
            break;
        case msptiActivityKind::MSPTI_ACTIVITY_KIND_RUNTIME_API:
            runtimeApiData_.emplace_back(std::move(api));
            break;
        default:
            LOG(WARNING) << "Not supported MsptiActivityKind: " << kind;
            break;
    }
}

void Monitor::ReportKernelData(Kernel &&kernel)
{
    std::lock_guard<std::mutex> lock(kernelMutex_);
    kernelData_.emplace_back(std::move(kernel));
}

void Monitor::ReportCommunicationData(Communication &&communication)
{
    std::lock_guard<std::mutex> lock(communicationMutex_);
    communicationData_.emplace_back(std::move(communication));
}

void Monitor::ReportMarkerData(Marker &&marker)
{
    std::lock_guard<std::mutex> lock(markerMutex_);
    markerData_.emplace_back(std::move(marker));
}
} // namespace monitor
} // namespace ipc_monitor
} // namespace dynolog_npu
