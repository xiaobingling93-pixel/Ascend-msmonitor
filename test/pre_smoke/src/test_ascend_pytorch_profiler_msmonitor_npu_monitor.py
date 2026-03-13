# Copyright 2026 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
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
import glob
import sys
import logging

from check_tools.db_check import DBManager
from check_tools.file_check import FileChecker
from check_tools.table_fields import TableFields

logging.basicConfig(level=logging.INFO,
                    format='\n%(asctime)s %(filename)s [line:%(lineno)d] [%(levelname)s] %(message)s')

def test_profiler_with_msmonitor_npu_monitor(npu_monitor_path):
    # 校验db类型的npu_monitor数据结果
    _check_monitor_files(npu_monitor_path)

def _check_monitor_files(npu_monitor_path):
    db_path = glob.glob(
        f"{npu_monitor_path}/*.db"
    )
    assert db_path, f"No db file found in {npu_monitor_path}"
    expect_tables = ["CANN_API", "COMMUNICATION_OP", "COMPUTE_TASK_INFO", "TASK"]
    for db in db_path:
        for table_name in expect_tables:
            # 1. Check table exist
            FileChecker.check_db_table_exist(db, table_name)

if __name__ == '__main__':
    prof_path = sys.argv[1]
    test_profiler_with_msmonitor_npu_monitor(prof_path)
