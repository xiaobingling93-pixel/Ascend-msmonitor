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

#ifndef MSMONITOR_ACTIVITY_DATA_H
#define MSMONITOR_ACTIVITY_DATA_H

#include <string>
#include <cstdint>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dynolog_npu {
namespace ipc_monitor {
namespace monitor {
constexpr double NS_TO_US = 1000.0;
struct API {
    std::string name;
    uint64_t startNs;
    uint64_t endNs;
    uint32_t pid;
    uint32_t tid;
    uint64_t correlationId;
    py::tuple to_tuple() const {
        return py::make_tuple(name, startNs / NS_TO_US, endNs / NS_TO_US, pid, tid, correlationId);
    }
};

struct Kernel {
    std::string name;
    uint64_t startNs;
    uint64_t endNs;
    uint32_t deviceId;
    uint32_t streamId;
    uint64_t correlationId;
    std::string type;
    py::tuple to_tuple() const {
        return py::make_tuple(name, startNs / NS_TO_US, endNs / NS_TO_US, deviceId, streamId, correlationId, type);
    }
};

struct Communication {
    std::string name;
    uint64_t startNs;
    uint64_t endNs;
    uint32_t deviceId;
    uint32_t streamId;
    uint64_t count;
    std::string dataType;
    std::string commName;
    std::string algType;
    uint64_t correlationId;
    py::tuple to_tuple() const {
        return py::make_tuple(
            name, startNs / NS_TO_US, endNs / NS_TO_US, deviceId, streamId, count, dataType, commName, algType, correlationId);
    }
};

struct Marker {
    std::string name;
    std::string sourceKind;
    std::string domain;
    uint64_t id;
    uint64_t startNs;
    uint64_t endNs;
    uint32_t pid;
    uint32_t tid;
    uint32_t deviceId;
    uint32_t streamId;
    py::tuple to_tuple() const {
        return py::make_tuple(
            name, sourceKind, domain, id, startNs / NS_TO_US, endNs / NS_TO_US, pid, tid, deviceId, streamId);
    }
};
} // namespace monitor
} // namespace ipc_monitor
} // namespace dynolog_npu
#endif // MSMONITOR_ACTIVITY_DATA_H
