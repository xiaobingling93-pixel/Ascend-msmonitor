# msmonitor-plugin模块说明

## IPCMonitor

提供IPC(Inter-Process Communication)通信接口，用于实现
1. IPC控制通道: profiler backend向dynolog daemon获取profiler配置
2. IPC数据通道: mspti monitor向dynolog daemon发送性能数据

## PyDynamicMonitorProxy接口说明

* `init_dyno` 向dynolog daemon发送注册请求
  * input: npu_id(int)
  * return: None
* `poll_dyno` 向dynolog daemon获取Profiler控制参数
  * input: None
  * return: str，返回控制参数
* `enable_dyno_npu_monitor` 开启mspti监控
  * input: cfg_map(Dict[str,str]) 参数配置
  * return: None
* `finalize_dyno` 释放msmonitor中相关资源、线程
  * input: None
  * return: None
* `update_profiler_status` 上报profiler_status
  * input: status(Dict[str,str])
  * return: None

## 安装方式

msmonitor-plugin模块安装请参见《[msMonitor工具安装指南](../docs/zh/install_guide.md)》。