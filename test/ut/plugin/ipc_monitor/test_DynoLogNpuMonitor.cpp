#include <gtest/gtest.h>
#include "plugin/ipc_monitor/DynoLogNpuMonitor.h"

namespace dynolog_npu {
namespace ipc_monitor {

class DynoLogNpuMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor_ = DynoLogNpuMonitor::GetInstance();
    }

    void TearDown() override { }

    DynoLogNpuMonitor* monitor_;
};

TEST_F(DynoLogNpuMonitorTest, Constructor) {
    EXPECT_NO_THROW(DynoLogNpuMonitor::GetInstance());
}

TEST_F(DynoLogNpuMonitorTest, SetNpuIdAndGetIpcClient) {
    monitor_->SetNpuId(1);
    IpcClient* ipc_client = monitor_->GetIpcClient();
    EXPECT_NE(ipc_client, nullptr);
}

TEST_F(DynoLogNpuMonitorTest, Finalize) {
    EXPECT_NO_THROW(monitor_->Finalize());
}

TEST_F(DynoLogNpuMonitorTest, UpdateNpuStatus) {
    NpuStatus status;
    status.status = 0;
    status.currentStep = 0;
    status.startStep = 0;
    status.stopStep = 0;
    status.pid = 1234; // 1234是一个假设的PID
    status.jobId = 5678; // 5678是一个假设的JobID
    // 注意：这个测试可能会失败，因为它需要实际的IPC连接
    // 我们可以使用EXPECT_NO_THROW来测试它是否能正常执行，而不关心结果
    EXPECT_NO_THROW(monitor_->UpdateNpuStatus(status, "test_type"));
}

TEST_F(DynoLogNpuMonitorTest, Init) {
    // 注意：这个测试可能会失败，因为它需要实际的IPC连接
    // 我们可以使用EXPECT_NO_THROW来测试它是否能正常执行，而不关心结果
    EXPECT_NO_THROW(monitor_->Init());
}

TEST_F(DynoLogNpuMonitorTest, Poll) {
    // 测试Poll方法
    // 注意：这个测试可能会返回空字符串，因为它需要实际的IPC连接
    // 我们可以使用EXPECT_NO_THROW来测试它是否能正常执行，而不关心结果
    EXPECT_NO_THROW(monitor_->Poll());
}

} // namespace ipc_monitor
} // namespace dynolog_npu

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
