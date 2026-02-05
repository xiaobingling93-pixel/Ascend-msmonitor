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
#ifndef MSMONITOR_MONITOR_PROCESS_MANAGER_H
#define MSMONITOR_MONITOR_PROCESS_MANAGER_H

#include <atomic>
#include <mutex>
#include <unordered_map>
#include "MsptiDataProcessBase.h"

namespace dynolog_npu {
namespace ipc_monitor {
namespace monitor {

struct MstxHostData {
    uint64_t timestamp;
    std::string domain;
    std::string message;
};

struct MstxDeviceData {
    uint64_t timestamp;
};

class MonitorProcessManager : public MsptiDataProcessBase {
public:
    MonitorProcessManager() : MsptiDataProcessBase("MonitorProcessManager") {}
    ~MonitorProcessManager() = default;
    ErrCode ConsumeMsptiData(msptiActivity *record) override;
    void RunPreTask() override;
    void RunPostTask() override;

private:
    void ProcessApiData(msptiActivityApi *record, msptiActivityKind kind);
    void ProcessCommunicationData(msptiActivityCommunication *record);
    void ProcessKernelData(msptiActivityKernel *record);
    void ProcessMstxData(msptiActivityMarker *record);
    void ProcessMstxHostData(msptiActivityMarker *record);
    void ProcessMstxDeviceData(msptiActivityMarker *record);

private:
    uint64_t sessionStartTime_{0};

    std::mutex dataMutex_;
    // mstx data
    std::unordered_map<uint64_t, MstxHostData> mstxMarkerHostData_;
    std::unordered_map<uint64_t, MstxHostData> mstxRangeHostData_;
    std::unordered_map<uint64_t, MstxDeviceData> mstxRangeDeviceData_;
};
} // namespace monitor
} // namespace ipc_monitor
} // namespace dynolog_npu
#endif // MSMONITOR_MONITOR_PROCESS_MANAGER_H
