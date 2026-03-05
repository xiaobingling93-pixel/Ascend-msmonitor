# MindStudio Monitor

## 简介

MindStudio Monitor（MindStudio一站式在线监控工具，msMonitor）提供用户在集群场景性能监控定位端到端能力。

msMonitor基于[dynolog](https://github.com/facebookincubator/dynolog)开发，结合AI框架（[Ascend PyTorch Profiler](https://www.hiascend.com/document/detail/zh/mindstudio/81RC1/T&ITools/Profiling/atlasprofiling_16_0090.html#ZH-CN_TOPIC_0000002353635602__zh-cn_topic_0000002370275077_section17272160135118)、[MindSpore Profiler](https://www.hiascend.com/document/detail/zh/mindstudio/81RC1/T&ITools/Profiling/atlasprofiling_16_0087.html)）的动态采集能力和[MSPTI](https://www.hiascend.com/document/detail/zh/mindstudio/81RC1/T&ITools/Profiling/atlasprofiling_16_0021.html)，为用户提供**nputrace**和**npu-monitor**功能：

- **npu-monitor功能**：轻量常驻后台，监控关键算子耗时。
- **nputrace功能**：获取到框架、CANN以及device的详细性能数据。

![msMonitor](./docs/zh/figures/msMonitor.png)

如上图所示msMonitor分为三部分：

1. **Dynolog daemon**：dynolog守护进程，每个节点只有一个守护进程，负责接收dyno CLI的RPC请求、触发nputrace和npu-monitor功能、上报数据的处理以及最终数据的展示，dynolog的详细介绍请参见[dynolog](./docs/zh/dynolog_instruct.md)。
2. **Dyno CLI**：dyno客户端，为用户提供nputrace和npu-monitor子命令，任意节点都可以安装，dyno的详细介绍请参见[dyno](./docs/zh/dyno_instruct.md)。
3. **MSPTI Monitor**：基于MSPTI实现的监控子模块，通过调用MSPTI的API获取性能数据，并上报给Dynolog daemon。

## 目录结构

关键目录如下，详细目录介绍参见[项目目录](./docs/zh/dir_structure.md)。

```ColdFusion
├── docs                    # 项目文档目录
│   └── zh                  # 中文文档目录
├── dynolog_npu             # dynolog_npu模块代码目录
├── plugin                  # 插件模块代码目录
├── scripts                 # 构建、测试等脚本目录
│   ├── build.sh            # dynolog_npu构建脚本
│   ├── run_st.sh           # 系统测试脚本
│   └── run_ut.sh           # 单元测试脚本
├── test                    # 测试代码目录
│   ├── st                  # 系统测试用例
│   └── ut                  # 单元测试用例
├── third_party             # 第三方依赖库
└── README.md               # 项目说明文档
```

## 版本说明

msMonitor由三个文件组成，如下表所示。

其中dyno和dynolog可以被打包为deb包或者rpm包。目前msMonitor支持在[PyTorch](https://gitcode.com/Ascend/pytorch)框架和[MindSpore](https://www.mindspore.cn/)框架上运行。 最新的软件包见《[版本配套说明](#版本配套说明)》。

| 文件名                                                                                                  | 用途                                                                                                                |
|------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------|
| dyno                                                                                                 | dyno客户端二进制文件                                                                                                      |
| dynolog                                                                                              | dynolog服务端二进制文件                                                                                                   |
| mindstudio_monitor-{mindstudio_version}-cp{python_version}-cp{python_version}-linux_{system_architecture}.whl | MSPTI Monitor、IPC等公共能力工具包，{mindstudio_version}表示mindstudio版本号，{python_version}表示python版本号，{system_architecture}表示CPU架构系统 |

**版本配套说明**<a name="版本配套说明"></a>

msMonitor详细版本配套关系及对应软件包下载链接如下：

| msMonitor版本  | 发布日期   | 配套CANN版本  | 配套torch_npu版本 | 配套MindSpore版本 | 下载链接                                                     | 校验码                                                       |
| -------------- | ---------- | ------------- | ----------------- | ----------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 8.3.0(aarch64) | 2025-12-29 | 8.3.RC1及以上 | v7.3.0及以上      | 2.7.2及以上   | [aarch64_8.3.0.zip](https://ptdbg.obs.cn-north-4.myhuaweicloud.com/profiler/msmonitor/8.3.0/aarch64_8.3.0.zip) | 2c675ae346dfc1c70f5e9c7103d6f8c7e53be00dca28ed5f9cc577ac59e4bc44 |
| 8.3.0(x86)     | 2025-12-29 | 8.3.RC1及以上 | v7.3.0及以上      | 2.7.2及以上   | [x86_8.3.0.zip](https://ptdbg.obs.cn-north-4.myhuaweicloud.com/profiler/msmonitor/8.3.0/x86_8.3.0.zip) | 1a38cc141e67c50eb09ebdc757c1fd3ed54439f227459e71292b2d18bb78e7f0 |
| 8.1.0(aarch64) | 2025-07-11 | 8.1.RC1及以上 | v7.1.0及以上      | 2.7.0-rc1及以上   | [aarch64_8.1.0.zip](https://ptdbg.obs.cn-north-4.myhuaweicloud.com/profiler/msmonitor/8.1.0/aarch64_8.1.0.zip) | ce136120c0288291cc0a7803b1efc8c8416c6105e9d54c17ccf2e2510869fada |
| 8.1.0(x86)     | 2025-07-11 | 8.1.RC1及以上 | v7.1.0及以上      | 2.7.0-rc1及以上   | [x86_8.1.0.zip](https://ptdbg.obs.cn-north-4.myhuaweicloud.com/profiler/msmonitor/8.1.0/x86_8.1.0.zip) | 097d11c7994793b6389b19259269ceb3b6b7ac5ed77da3949b3f09da2103b7f2 |

## 环境部署

安装msMonitor工具。包括软件包安装和编译安装两种方式，具体请参见《[msMonitor工具安装指南](./docs/zh/install_guide.md)》。

推荐使用软件包安装，步骤如下：

1. 根据[版本配套说明](#版本配套说明)选择对应软件包并下载到Linux安装环境。

2. 校验包完整性。

   进入zip包所在目录，执行如下命令。

   ```bash
   sha256sum {name}.zip
   ```

   {name}为zip包名称。

   若打印呈现对应版本zip包一致的**校验码**，则表示下载了正确的性能工具zip安装包。示例如下：

   ```ColdFusion
   2c675ae346dfc1c70f5e9c7103d6f8c7e53be00dca28ed5f9cc577ac59e4bc44 aarch64_8.3.0.zip
   ```

3. 安装whl包。

   ```bash
   # 解压压缩包
   mkdir x86
   unzip x86_8.3.0.zip -d x86

   # 进入解压后的目录
   cd x86

   # 安装whl包，须选择与当前环境Python版本一致的whl包
   pip install mindstudio_monitor-{mindstudio_version}-cp{python_version}-cp{python_version}-linux_{system_architecture}.whl
   ```

   安装成功打印如下信息：

   ```ColdFusion
   Successfully installed mindstudio_monitor-<version> pybind11-<version>
   ```

4. 安装dynolog。

   有以下安装方式可供选择，根据用户服务器系统自行选择：

   - 方式一：使用deb软件包安装（适用于Debian/Ubuntu等系统）。

     ```bash
     dpkg -i --force-overwrite dynolog*.deb
     ```

   - 方式二：使用rpm软件包安装（适用于RedHat/Fedora/openSUSE等系统）。

     ```bash
     rpm -ivh dynolog*.rpm --nodeps
     ```

## 快速入门

npu-monitor和nputrace功能详细说明请参见[特性介绍](#特性介绍)章节，下面介绍msMonitor常见的使用场景：

1. 先使用npu-monitor功能获取关键算子耗时。
2. 当发现监控到关键算子耗时劣化，使用nputrace功能采集详细性能数据做分析。

**操作步骤**

1. 启动dynolog daemon进程。

   命令示例如下：

   ```bash
   # 命令行方式开启dynolog daemon
   dynolog --enable-ipc-monitor --certs-dir /home/server_certs

   # 如需使用Tensorboard展示数据，传入参数--metric_log_dir用于指定Tensorboard文件落盘路径
   dynolog --enable-ipc-monitor --certs-dir /home/server_certs --metric_log_dir /tmp/metric_log_dir    # dynolog daemon的日志路径为：/var/log/dynolog.log
   ```

2. 配置msMonitor环境变量。

   ```bash
   export MSMONITOR_USE_DAEMON=1
   ```

3. 设置LD_PRELOAD启动MSPTI（启动npu-monitor功能设置）。

   ```bash
   # 默认路径示例：export LD_PRELOAD=/usr/local/Ascend/ascend-toolkit/latest/lib64/libmspti.so
   export LD_PRELOAD=<CANN toolkit安装路径>/ascend-toolkit/latest/lib64/libmspti.so
   ```

4. 启动训练或推理任务。

   ```bash
   bash run_ai_task.sh
   ```

5. 使用dyno命令行触发npu-monitor监控关键算子耗时。

   ```bash
   # 开启npu-monitor，上报周期30s, 上报数据类型为Kernel
   dyno --certs-dir /home/client_certs npu-monitor --npu-monitor-start --report-interval-s 30 --mspti-activity-kind Kernel

   # 关闭npu-monitor
   dyno --certs-dir /home/client_certs npu-monitor --npu-monitor-stop
   ```

6. 使用dyno命令行触发nputrace采集详细trace数据（需要关闭npu-monitor功能才能触发nputrace功能）。

   ```bash
   # 从第10个step开始采集，采集2个step，采集框架、CANN和device数据，同时采集完后自动解析以及解析完成不做数据精简，落盘路径为/tmp/profile_data
   dyno --certs-dir /home/client_certs nputrace --start-step 10 --iterations 2 --activities CPU,NPU --analyse --data-simplification false --log-file /tmp/profile_data
   ```

## 特性介绍
>
> [!NOTE]  说明
>
> 由于底层资源限制，npu-monitor功能和nputrace不能同时开启。

执行nputrace或者npu-monitor命令后，响应结果里有一个`response`的json字符串。该字符串中的`commandStatus`字段用于标识命令是否生效：`effective`表示命令会生效，`ineffective`表示命令无效。其他字段均为dynolog的原生字段（仅状态为`effective`时存在）。

### status状态查询

```bash
dyno --certs-dir <CERT_DIR> status  # dyno和dynolog中--certs-dir传入参数值须保持一致；<CERT_DIR>可传入证书路径，如果不使用TLS证书密钥，设置为NO_CERTS。
```

输入以上命令后，会打印一个json字符串，例如：{"current_step":1,"npumonitor":"Idle","nputrace":"Ready","start_step":5,"stop_step":10}。

**状态说明：**

- Uninitialized：程序未启动或者dynolog init之前。
- Idle：没有下发命令。
- Ready：命令已下发，暂未到达指定step。
- Running：正在采集数据。

**其他说明：**

- nputrace字段有Uninitialized、Idle、Running、Ready四种状态，npumonitor字段有Uninitialized、Idle、Running三种状态。
- start_step、stop_step表示采集step的范围。PyTorch框架下有效采集范围为[start_step, stop_step)，即包含start_step，但不包含stop_step。MindSpore框架下有效采集范围为[start_step, stop_step]，包含stop_step。
- nputrace为Running或者Ready状态时，才会打印start_step和stop_step。
- current_step默认值为-1。
- MindSpore框架下nputrace没有Ready状态。

### npu-monitor特性

npu-monitor特性为用户提供轻量化监控关键指标的能力，npu-monitor基于[MSPTI](https://www.hiascend.com/document/detail/zh/mindstudio/81RC1/T&ITools/Profiling/atlasprofiling_16_0021.html)开发，用户可以通过npu-monitor查看模型运行时的计算、通信算子执行耗时。
具体使用方式请参见[npu-monitor](./docs/zh/npumonitor_instruct.md)，MindSpore框架下使用方式请参见[MindSpore框架下msMonitor的使用方法](./docs/zh/mindspore_adapter_instruct.md)。

### nputrace特性

nputrace特性为用户提供动态触发AI框架（[Ascend PyTorch Profiler](https://www.hiascend.com/document/detail/zh/mindstudio/81RC1/T&ITools/Profiling/atlasprofiling_16_0090.html)、[MindSpore Profiler](https://www.hiascend.com/document/detail/zh/mindstudio/81RC1/T&ITools/Profiling/atlasprofiling_16_0087.html)）采集解析的能力，即实现模型拉起后不需要中断模型运行，可多次触发不同配置Profiler采集解析。采集的性能数据可以使用[MindStudio Insight](https://www.hiascend.com/document/detail/zh/mindstudio/81RC1/GUI_baseddevelopmenttool/msascendinsightug/Insight_userguide_0002.html)进行可视化，效果图如下。
具体使用方式请参见[nputrace](./docs/zh/nputrace_instruct.md)，MindSpore框架下使用方式请参见[MindSpore框架下msMonitor的使用方法](./docs/zh/mindspore_adapter_instruct.md)。
![MindStudio Insight TimeLine可视化效果图](./docs/zh/figures/mindstudio_insight.png)

### Monitor特性

提供简单易用接口，采集计算类算子、通信类算子、API、Runtime API、Mstx等性能数据，用户可以根据需要选择采集的指标，具体使用方式请参见[Monitor](./docs/zh/monitor_feature.md)。

## API参考

[mindstudio_monitor模块接口参考](./docs/zh/mindstudio_monitor_api_reference.md)，包含以下特性接口：

- 与dynolog组件交互接口，请参见 “PyDynamicMonitorProxy接口说明” 章节。
- Monitor特性接口，请参见 “Monitor特性接口说明” 章节。

## FAQ

FAQ汇总了在使用msMonitor工具过程中可能遇到的问题，具体请参见[FAQ](./docs/zh/faq.md)。

## 贡献指导

介绍如何向msMonitor反馈问题、需求以及为msMonitor贡献的代码开发流程，具体请参见[为MindStudio Monitor贡献](CONTRIBUTING.md)。

## 联系我们

[![img](https://img-transfer.gitcode.com/?p=https%3A%2F%2Fimg.shields.io%2Fbadge%2FWeChat-07C160%3Fstyle%3Dfor-the-badge%26logo%3Dwechat%26logoColor%3Dwhite&projectId=Ascend/msprof&pageUrl=https%3A%2F%2Fgitcode.com%2FAscend%2Fmsprof)](https://raw.gitcode.com/kali20gakki1/Imageshack/raw/main/CDC0BEE2-8F11-477D-BD55-77A15417D7D1_4_5005_c.jpeg)

## 安全声明

MindStudio Monitor产品的安全加固信息、公网地址信息等内容，具体请参见《[安全声明](./docs/zh/security_statement.md)》。

## 免责声明

- 本工具仅供调试和开发使用，使用者需自行承担使用风险，并理解以下内容：
  - 数据处理及删除：用户在使用本工具过程中产生的数据属于用户责任范畴。建议用户在使用完毕后及时删除相关数据，以防不必要的信息泄露。
  - 数据保密与传播：使用者了解并同意不得将通过本工具产生的数据随意外发或传播。对于由此产生的信息泄露、数据泄露或其他不良后果，本工具及其开发者概不负责。
  - 用户输入安全性：用户需自行保证输入的命令行的安全性，并承担因输入不当而导致的任何安全风险或损失。对于输入命令行不当所导致的问题，本工具及其开发者概不负责。
- 免责声明范围：本免责声明适用于所有使用本工具的个人或实体。使用本工具即表示您同意并接受本声明的内容，并愿意承担因使用该功能而产生的风险和责任，如有异议请停止使用本工具。
- 在使用本工具之前，请**谨慎阅读并理解以上免责声明的内容**。对于使用本工具所产生的任何问题或疑问，请及时联系开发者。

## License

MindStudio Monitor产品的使用许可证，具体请参见[LICENSE](./LICENSE)文件。

介绍msMonitor工具docs目录下的文档适用CC-BY 4.0许可证，具体请参见[LICENSE](docs/LICENSE)文件。

## 致谢

msMonitor由华为公司的下列部门联合贡献 ：

华为公司：

- 昇腾计算MindStudio开发部

感谢来自社区的每一个PR，欢迎贡献msMonitor！
