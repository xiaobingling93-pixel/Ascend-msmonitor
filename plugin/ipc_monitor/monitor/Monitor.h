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

#ifndef MSMONITOR_MONITOR_H
#define MSMONITOR_MONITOR_H

#include <vector>
#include <mutex>
#include <unordered_set>
#include "monitor/ActivityData.h"
#include "mspti.h"
#include "singleton.h"

namespace dynolog_npu {
namespace ipc_monitor {
namespace monitor {
class Monitor : public Singleton<Monitor> {
public:
    Monitor();
    virtual ~Monitor() = default;
    void Start(const std::vector<msptiActivityKind>& kinds);
    void Stop();
    std::unordered_set<msptiActivityKind> GetKinds() const { return kinds_; };
    std::vector<API> GetAPIData() const { return apiData_; };
    std::vector<API> GetAclApiData() const { return aclApiData_; };
    std::vector<API> GetNodeApiData() const { return nodeApiData_; };
    std::vector<API> GetRuntimeApiData() const { return runtimeApiData_; };
    std::vector<Kernel> GetKernelData() const { return kernelData_; };
    std::vector<Communication> GetCommunicationData() const { return communicationData_; };
    std::vector<Marker> GetMarkerData() const { return markerData_; };

    void ReportAPIData(API&& api, msptiActivityKind kind);
    void ReportKernelData(Kernel&& kernel);
    void ReportCommunicationData(Communication&& communication);
    void ReportMarkerData(Marker&& marker);

private:
    void Clear();

private:
    std::unordered_set<msptiActivityKind> kinds_;
    std::mutex apiMutex_;
    std::vector<API> apiData_;
    std::vector<API> aclApiData_;
    std::vector<API> nodeApiData_;
    std::vector<API> runtimeApiData_;
    std::mutex kernelMutex_;
    std::vector<Kernel> kernelData_;
    std::mutex communicationMutex_;
    std::vector<Communication> communicationData_;
    std::mutex markerMutex_;
    std::vector<Marker> markerData_;
};
} // namespace monitor
} // namespace ipc_monitor
} // namespace dynolog_npu

#endif // MSMONITOR_MONITOR_H
