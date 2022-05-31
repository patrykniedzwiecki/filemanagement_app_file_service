/*
 * 版权所有 (c) 华为技术有限公司 2022
 *
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC模块的IO，具体业务逻辑实现在service.cpp中
 *     - 注意点2：所有调用开头处打印 Begin 字样，通过BError返回正常结果/错误码，这是出于防抵赖的目的
 */
#include "module_ipc/service_stub.h"

#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service_reverse_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ServiceStub::ServiceStub()
{
    opToInterfaceMap_[SERVICE_CMD_INIT_RESTORE_SESSION] = &ServiceStub::CmdInitRestoreSession;
    opToInterfaceMap_[SERVICE_CMD_INIT_BACKUP_SESSION] = &ServiceStub::CmdInitBackupSession;
    opToInterfaceMap_[SERVICE_CMD_GET_LOCAL_CAPABILITIES] = &ServiceStub::CmdGetLocalCapabilities;
    opToInterfaceMap_[SERVICE_CMD_GET_FILE_ON_SERVICE_END] = &ServiceStub::CmdGetFileOnServiceEnd;
    opToInterfaceMap_[SERVICE_CMD_PUBLISH_FILE] = &ServiceStub::CmdPublishFile;
}

int32_t ServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGI("Begin to call procedure indexed %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        stringstream ss;
        ss << "Cannot response request " << code << ": unknown procedure";
        return BError(BError::Codes::SA_INVAL_ARG, ss.str());
    }

    const std::u16string descriptor = ServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::SA_INVAL_ARG, "Invalid remote descriptor");
    }

    return (this->*(interfaceIndex->second))(data, reply);
}

int32_t ServiceStub::CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    auto remote = data.ReadRemoteObject();
    if (!remote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }
    auto iremote = iface_cast<IServiceReverse>(remote);
    if (!iremote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }

    std::vector<string> bundleNames;
    if (!data.ReadStringVector(&bundleNames)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }

    int32_t res = InitRestoreSession(iremote, bundleNames);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdInitBackupSession(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    auto remote = data.ReadRemoteObject();
    if (!remote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }
    auto iremote = iface_cast<IServiceReverse>(remote);
    if (!remote) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive the reverse stub");
    }

    UniqueFd fd(data.ReadFileDescriptor());
    if (fd < 0) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fd");
    }

    std::vector<string> bundleNames;
    if (!data.ReadStringVector(&bundleNames)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive bundleNames");
    }

    int res = InitBackupSession(iremote, move(fd), bundleNames);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    UniqueFd fd(GetLocalCapabilities());
    if (!reply.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to send out the file");
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetFileOnServiceEnd(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    auto [errCode, tmpFileSN, fd] = GetFileOnServiceEnd();
    if (!reply.WriteInt32(errCode) || !reply.WriteUint32(tmpFileSN) || !reply.WriteFileDescriptor(fd)) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to send the result");
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdPublishFile(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    unique_ptr<BFileInfo> fileInfo(data.ReadParcelable<BFileInfo>());
    if (!fileInfo) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to receive fileInfo");
    }
    int res = PublishFile(*fileInfo);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup
