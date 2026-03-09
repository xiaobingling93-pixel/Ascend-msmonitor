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
cur_dir=$(dirname $(readlink -f $0))
export ASCEND_GLOBAL_LOG_LEVEL=1
export MSMONITOR_USE_DAEMON=1
output_dir=$(realpath $1)

pkill -9 python
pkill -9 dynolog

main(){
    file_name=$(basename $0 .sh)
    testcase_result_dir="${output_dir}/${file_name}"
    if [ ! -d ${testcase_result_dir} ]; then
        mkdir -p ${testcase_result_dir}
    else
        rm -rf ${testcase_result_dir}/*
    fi
    start_time=$(date "+%s")
    # 1. 拉起dynolog后台
    cd ${cur_dir}/../../third_party/dynolog/build/bin/
    if [ ! -f ./dynolog ]; then
        echo "${file_name} fail ${duration_time}, Error: dynolog file does not exist"
        exit 1
    fi
    ./dynolog --enable-ipc-monitor --certs-dir NO_CERTS --use_JSON > ${testcase_result_dir}/dynolog.txt 2>&1 &
    dynolog_pid=$!
    # 2. 拉起训练
    cd ${cur_dir}/src/
    rm -rf /dev/shm/DynamicProfileNpuShm*
    export LD_PRELOAD=/usr/local/Ascend/ascend-toolkit/latest/lib64/libmspti.so
    python ${cur_dir}/src/dynamic_model_train.py > ${testcase_result_dir}/plog.txt 2>&1 &
    train_pid=$!
    sleep 20
    # 3. 发送dyno消息
    cd ${cur_dir}/../../third_party/dynolog/build/bin/
    if [ ! -f ./dyno ]; then
        echo "${file_name} fail ${duration_time}, Error: dyno file does not exist"
        exit 1
    fi
    ./dyno --certs-dir NO_CERTS npu-monitor --npu-monitor-start --duration 10 --report-interval-s 1 --mspti-activity-kind Marker,Kernel,API,Communication --log-file ${testcase_result_dir}/npu_monitor > ${testcase_result_dir}/dyno.txt 2>&1 &
    if [ 0 -ne $? ]; then
        echo "${file_name} fail ${duration_time}"
        exit 1
    fi
    sleep 20
    cd ${cur_dir}/src/
    timeout 300 tail -f ${testcase_result_dir}/plog.txt | while read line; do
        if [[ "$line" == *"model train over..."* ]]; then
            (kill -9 "$train_pid" 2>&1) >> "${testcase_result_dir}/plog.txt" 2>&1
            (kill -9 "$dynolog_pid" 2>&1) >> "${testcase_result_dir}/plog.txt" 2>&1
            break
        fi
    done
    # 检查timeout的退出状态
    timeout_exit_code=$?
    if [ $timeout_exit_code -eq 124 ]; then
        echo "${file_name} fail ${duration_time}"
        (kill -9 "$train_pid" 2>&1) >> "${testcase_result_dir}/plog.txt" 2>&1
        (kill -9 "$dynolog_pid" 2>&1) >> "${testcase_result_dir}/plog.txt" 2>&1
        exit 1
    fi
    # 4. 校验结果
    python ${cur_dir}/src/${file_name}.py ${testcase_result_dir}/npu_monitor
    if [ 0 -ne $? ]; then
        echo "${file_name} fail ${duration_time}"
        exit 1
    fi
    end_time=$(date "+%s")
    duration_time=$(( ${end_time} - ${start_time} ))
    # 校验plog有无报错
    grep "ERROR" ${testcase_result_dir}/plog.txt | grep "PROFILING"
    if [ 0 -eq $? ]; then
        echo "${file_name} fail ${duration_time}"
        exit 1
    fi

    echo "${file_name} pass ${duration_time}"
}

main