/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstddef>
#include <cstdio>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "b_file_info.h"
#include "iservice_registry.h"
#include "service_mock.h"
#include "service_proxy.h"
#include "service_reverse_mock.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string FILE_NAME = "1.tar";
} // namespace

class ServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;
    shared_ptr<ServiceProxy> proxy_ = nullptr;
    sptr<ServiceMock> mock_ = nullptr;
    sptr<ServiceReverseMock> remote_ = nullptr;
};

void ServiceProxyTest::SetUp()
{
    mock_ = sptr(new ServiceMock());
    proxy_ = make_shared<ServiceProxy>(mock_);
    remote_ = sptr(new ServiceReverseMock());
}

void ServiceProxyTest::TearDown()
{
    proxy_ = nullptr;
    mock_ = nullptr;
    remote_ = nullptr;
}

/**
 * @tc.number: SUB_Service_proxy_InitRestoreSession_0100
 * @tc.name: SUB_Service_proxy_InitRestoreSession_0100
 * @tc.desc: 测试 InitRestoreSession 注册restore Session接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H037V
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_InitRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_InitRestoreSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeSendRequest));
    std::vector<string> bundleNames;
    int32_t result = proxy_->InitRestoreSession(remote_, bundleNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    result = proxy_->InitRestoreSession(remote_, bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    result = proxy_->InitRestoreSession(nullptr, bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_InitRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_InitBackupSession_0100
 * @tc.name: SUB_Service_proxy_InitBackupSession_0100
 * @tc.desc: 测试 InitBackupSession 注册backup Session接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_InitBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_InitBackupSession_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeSendRequest));
    std::vector<string> bundleNames;

    TestManager tm("BackupSession_GetFd_0100");
    std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    int32_t result = proxy_->InitBackupSession(remote_, move(fd), bundleNames);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    result = proxy_->InitRestoreSession(remote_, bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    result = proxy_->InitBackupSession(nullptr, move(fd), bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    result = proxy_->InitBackupSession(remote_, UniqueFd(-1), bundleNames);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_InitBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_proxy_Start_0100
 * @tc.name: SUB_Service_proxy_Start_0100
 * @tc.desc: 测试 Start 启动备份恢复流程接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_Start_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_Start_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeSendRequest));
    int32_t result = proxy_->Start();
    EXPECT_EQ(result, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    result = proxy_->Start();
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_Start_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetLocalCapabilities_0100
 * @tc.name: SUB_Service_proxy_GetLocalCapabilities_0100
 * @tc.desc: 测试 GetLocalCapabilities 获取能力文件接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0377
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetLocalCapabilities_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeGetLocalSendRequest));
    UniqueFd fd = proxy_->GetLocalCapabilities();
    EXPECT_GT(fd, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    UniqueFd fdErr = proxy_->GetLocalCapabilities();
    EXPECT_LT(fdErr, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_proxy_PublishFile_0100
 * @tc.name: SUB_Service_proxy_PublishFile_0100
 * @tc.desc: 测试 PublishFile 推送接口调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_PublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_PublishFile_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeSendRequest));
    string bundleName = "com.example.app2backup";
    string fileName = "1.tar";
    BFileInfo fileInfo(bundleName, fileName, -1);
    int32_t result = proxy_->PublishFile(fileInfo);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    result = proxy_->PublishFile(fileInfo);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_PublishFile_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppFileReady_0100
 * @tc.name: SUB_Service_proxy_AppFileReady_0100
 * @tc.desc: 测试 AppFileReady 文件就绪调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0380
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppFileReady_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeSendRequest));

    string bundleName = "com.example.app2backup";
    TestManager tm("AppFileReady_GetFd_0100");
    std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));

    int32_t result = proxy_->AppFileReady(bundleName, move(fd));
    EXPECT_EQ(result, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    TestManager tmErr("AppFileReady_GetFd_0200");
    UniqueFd fdErr(open(tmErr.GetRootDirCurTest().append(FILE_NAME).data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    result = proxy_->AppFileReady(bundleName, move(fdErr));
    EXPECT_NE(result, BError(BError::Codes::OK));
    result = proxy_->AppFileReady(bundleName, UniqueFd(-1));
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppFileReady_0100";
}

/**
 * @tc.number: SUB_Service_proxy_AppDone_0100
 * @tc.name: SUB_Service_proxy_AppDone_0100
 * @tc.desc: 测试 AppDone ext备份恢复流程结束调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0380
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_AppDone_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_AppDone_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeSendRequest));
    int32_t result = proxy_->AppDone(BError(BError::Codes::OK));
    EXPECT_EQ(result, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    result = proxy_->AppDone(BError(BError::Codes::OK));
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_AppDone_0100";
}

/**
 * @tc.number: SUB_Service_proxy_GetExtFileName_0100
 * @tc.name: SUB_Service_proxy_GetExtFileName_0100
 * @tc.desc: 测试 GetExtFileName 获取真实文件调用成功和失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0379
 */
HWTEST_F(ServiceProxyTest, SUB_Service_proxy_GetExtFileName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceProxyTest-begin SUB_Service_proxy_GetExtFileName_0100";
    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceMock::InvokeSendRequest));
    string bundleName = "com.example.app2backup";
    string fileName = "1.tar";
    int32_t result = proxy_->GetExtFileName(bundleName, fileName);
    EXPECT_EQ(result, BError(BError::Codes::OK));

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).Times(1).WillOnce(Return(EPERM));
    result = proxy_->GetExtFileName(bundleName, fileName);
    EXPECT_NE(result, BError(BError::Codes::OK));
    GTEST_LOG_(INFO) << "ServiceProxyTest-end SUB_Service_proxy_GetExtFileName_0100";
}
} // namespace OHOS::FileManagement::Backup