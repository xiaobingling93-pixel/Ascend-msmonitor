/*
 * Copyright (C) 2025-2025. Huawei Technologies Co., Ltd. All rights reserved.
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
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "ipc_monitor/PyDynamicMonitorProxy.h"
#include "ipc_monitor/mspti_monitor/MsptiMonitor.h"
#include "ipc_monitor/monitor/ActivityData.h"
#include "ipc_monitor/monitor/Monitor.h"

namespace py = pybind11;

void init_monitor_module(py::module& m)
{
    using namespace dynolog_npu::ipc_monitor::monitor;
    auto monitor_m = m.def_submodule("monitor");
    py::enum_<msptiActivityKind>(monitor_m, "ActivityKind")
        .value("API", msptiActivityKind::MSPTI_ACTIVITY_KIND_API)
        .value("Kernel", msptiActivityKind::MSPTI_ACTIVITY_KIND_KERNEL)
        .value("Communication", msptiActivityKind::MSPTI_ACTIVITY_KIND_COMMUNICATION)
        .value("Marker", msptiActivityKind::MSPTI_ACTIVITY_KIND_MARKER)
        .value("AclAPI", msptiActivityKind::MSPTI_ACTIVITY_KIND_ACL_API)
        .value("NodeAPI", msptiActivityKind::MSPTI_ACTIVITY_KIND_NODE_API)
        .value("RuntimeAPI", msptiActivityKind::MSPTI_ACTIVITY_KIND_RUNTIME_API);
    py::class_<API>(monitor_m, "API")
        .def(py::init<>())
        .def_readwrite("name", &API::name)
        .def_readwrite("startNs", &API::startNs)
        .def_readwrite("endNs", &API::endNs)
        .def_readwrite("pid", &API::pid)
        .def_readwrite("tid", &API::tid)
        .def_readwrite("correlationId", &API::correlationId)
        .def("to_tuple", &API::to_tuple);
    py::class_<Kernel>(monitor_m, "Kernel")
        .def(py::init<>())
        .def_readwrite("name", &Kernel::name)
        .def_readwrite("startNs", &Kernel::startNs)
        .def_readwrite("endNs", &Kernel::endNs)
        .def_readwrite("deviceId", &Kernel::deviceId)
        .def_readwrite("streamId", &Kernel::streamId)
        .def_readwrite("correlationId", &Kernel::correlationId)
        .def_readwrite("type", &Kernel::type)
        .def("to_tuple", &Kernel::to_tuple);
    py::class_<Communication>(monitor_m, "Communication")
        .def(py::init<>())
        .def_readwrite("name", &Communication::name)
        .def_readwrite("startNs", &Communication::startNs)
        .def_readwrite("endNs", &Communication::endNs)
        .def_readwrite("deviceId", &Communication::deviceId)
        .def_readwrite("streamId", &Communication::streamId)
        .def_readwrite("count", &Communication::count)
        .def_readwrite("dataType", &Communication::dataType)
        .def_readwrite("commName", &Communication::commName)
        .def_readwrite("algType", &Communication::algType)
        .def_readwrite("correlationId", &Communication::correlationId)
        .def("to_tuple", &Communication::to_tuple);
    py::class_<Marker>(monitor_m, "Marker")
        .def(py::init<>())
        .def_readwrite("name", &Marker::name)
        .def_readwrite("sourceKind", &Marker::sourceKind)
        .def_readwrite("domain", &Marker::domain)
        .def_readwrite("id", &Marker::id)
        .def_readwrite("startNs", &Marker::startNs)
        .def_readwrite("endNs", &Marker::endNs)
        .def_readwrite("pid", &Marker::pid)
        .def_readwrite("tid", &Marker::tid)
        .def_readwrite("deviceId", &Marker::deviceId)
        .def_readwrite("streamId", &Marker::streamId)
        .def("to_tuple", &Marker::to_tuple);

    monitor_m.def("start_monitor", [](const std::vector<msptiActivityKind>& kinds) -> void {
        Monitor::GetInstance()->Start(kinds);
    });
    monitor_m.def("stop_monitor", []() -> void {
        Monitor::GetInstance()->Stop();
    });
    monitor_m.def("get_kinds", []() {
        return Monitor::GetInstance()->GetKinds();
    });
    monitor_m.def("get_api_data", []() {
        return Monitor::GetInstance()->GetAPIData();
    }, py::return_value_policy::move);
    monitor_m.def("get_acl_api_data", []() {
        return Monitor::GetInstance()->GetAclApiData();
    }, py::return_value_policy::move);
    monitor_m.def("get_node_api_data", []() {
        return Monitor::GetInstance()->GetNodeApiData();
    }, py::return_value_policy::move);
    monitor_m.def("get_runtime_api_data", []() {
        return Monitor::GetInstance()->GetRuntimeApiData();
    }, py::return_value_policy::move);
    monitor_m.def("get_kernel_data", []() {
        return Monitor::GetInstance()->GetKernelData();
    }, py::return_value_policy::move);
    monitor_m.def("get_communication_data", []() {
        return Monitor::GetInstance()->GetCommunicationData();
    }, py::return_value_policy::move);
    monitor_m.def("get_marker_data", []() {
        return Monitor::GetInstance()->GetMarkerData();
    }, py::return_value_policy::move);
}

PYBIND11_MODULE(IPCMonitor_C, m) {
    m.def("init_dyno", [](int npu_id) -> bool {
        return dynolog_npu::ipc_monitor::PyDynamicMonitorProxy::GetInstance()->InitDyno(npu_id);
    }, py::arg("npu_id"));
    m.def("poll_dyno", []() -> std::string {
        return dynolog_npu::ipc_monitor::PyDynamicMonitorProxy::GetInstance()->PollDyno();
    });
    m.def("enable_dyno_npu_monitor", [](std::unordered_map<std::string, std::string>& config_map) -> void {
        dynolog_npu::ipc_monitor::PyDynamicMonitorProxy::GetInstance()->EnableMsptiMonitor(config_map);
    }, py::arg("config_map"));
    m.def("finalize_dyno", []() -> void {
        dynolog_npu::ipc_monitor::PyDynamicMonitorProxy::GetInstance()->FinalizeDyno();
    });
    m.def("set_cluster_config_data", [](const std::unordered_map<std::string, std::string>& cluster_config) -> void {
        dynolog_npu::ipc_monitor::MsptiMonitor::GetInstance()->SetClusterConfigData(cluster_config);
    }, py::arg("cluster_config"));
    m.def("update_profiler_status", [](std::unordered_map<std::string, std::string>& status) -> void {
        dynolog_npu::ipc_monitor::PyDynamicMonitorProxy::GetInstance()->UpdateProfilerStatus(status);
    }, py::arg("status"));

    init_monitor_module(m);
}
