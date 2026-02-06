# Copyright (c) 2026, Huawei Technologies Co., Ltd.
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0  (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import xlsxwriter
from ._ipcmonitor_C import ipcmonitor_C_module
from .file_manager import FileManager
from .singleton import Singleton


ActivityKind = ipcmonitor_C_module.monitor.ActivityKind

@Singleton
class Monitor:

    API_HEADER = ["Name", "Start(us)", "End(us)", "Pid", "Tid", "Correlation ID", "Duration(us)"]

    HEADER = {
        ActivityKind.API: API_HEADER,
        ActivityKind.AclAPI: API_HEADER,
        ActivityKind.NodeAPI: API_HEADER,
        ActivityKind.RuntimeAPI: API_HEADER,
        ActivityKind.Kernel: ["Name", "Start(us)", "End(us)", "Device ID", "Stream ID", "Correlation ID", "Type", "Duration(us)"],
        ActivityKind.Communication: ["Name", "Start(us)", "End(us)", "Device ID", "Stream ID",
                                     "Count", "DataType", "CommName", "AlgType", "Correlation ID", "Duration(us)"],
        ActivityKind.Marker: ["Name", "SourceKind", "Domain", "ID", "Start(us)", "End(us)",
                              "Pid", "Tid", "Device ID", "Stream ID", "Duration(us)"]
    }

    GET_DATA_FUNC = {
        ActivityKind.API: ipcmonitor_C_module.monitor.get_api_data,
        ActivityKind.AclAPI: ipcmonitor_C_module.monitor.get_acl_api_data,
        ActivityKind.NodeAPI: ipcmonitor_C_module.monitor.get_node_api_data,
        ActivityKind.RuntimeAPI: ipcmonitor_C_module.monitor.get_runtime_api_data,
        ActivityKind.Kernel: ipcmonitor_C_module.monitor.get_kernel_data,
        ActivityKind.Communication: ipcmonitor_C_module.monitor.get_communication_data,
        ActivityKind.Marker: ipcmonitor_C_module.monitor.get_marker_data
    }

    NS_TO_US = 1000.0

    @classmethod
    def start(cls, kinds: list[ActivityKind]):
        if not kinds or not isinstance(kinds, list) \
            or not all(isinstance(kind, ActivityKind) for kind in kinds):
            print("[ERROR] Invalid activity kind list")
            return
        ipcmonitor_C_module.monitor.start_monitor(kinds)

    @classmethod
    def stop(cls):
        ipcmonitor_C_module.monitor.stop_monitor()

    @classmethod
    def get_result(cls) -> dict:
        kinds = ipcmonitor_C_module.monitor.get_kinds()
        if not kinds:
            print("[WARNING] No valid activity kind")
            return {}

        result = {}
        for kind in kinds:
            if kind not in cls.GET_DATA_FUNC:
                print(f"[WARNING] Unsupported activity kind: {kind}")
                continue
            result[kind] = cls.GET_DATA_FUNC[kind]()
        return result

    @classmethod
    def save(cls, file_path: str):
        if not file_path or not isinstance(file_path, str):
            print("[ERROR] Invalid file path")
            return

        if not file_path.endswith(".xlsx"):
            file_path += ".xlsx"

        if os.path.exists(file_path):
            print(f"[WARNING] File already exists: {file_path}, will be overwritten")

        try:
            FileManager.create_file_safety(file_path)

            result = cls.get_result()

            with xlsxwriter.Workbook(file_path) as workbook:
                for kind, data in result.items():
                    worksheet = workbook.add_worksheet(kind.name)
                    worksheet.write_row(0, 0, cls.HEADER[kind])
                    for i, row in enumerate(data, start=1):
                        worksheet.write_row(i, 0, [str(item) for item in row.to_tuple()] +
                                            [(row.endNs - row.startNs) / cls.NS_TO_US])

        except Exception as err:
            print(f"[ERROR] Failed to save file: {file_path}, error: {err}")
