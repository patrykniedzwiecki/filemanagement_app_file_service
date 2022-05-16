/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H

#include "b_session_backup.h"
#include "b_session_restore.h"
#include "service_reverse_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverse final : public ServiceReverseStub {
public:
    void BackupOnFileReady(std::string appId, std::string fileName, int fd) override;
    void BackupOnSubTaskStarted(int32_t errCode, std::string appId) override;
    void BackupOnSubTaskFinished(int32_t errCode, std::string appId) override;
    void BackupOnTaskFinished(int32_t errCode) override;

    void RestoreOnSubTaskStarted(int32_t errCode, std::string appId) override;
    void RestoreOnSubTaskFinished(int32_t errCode, std::string appId) override;
    void RestoreOnTaskFinished(int32_t errCode) override;

public:
    ServiceReverse() = delete;
    ServiceReverse(BSessionRestore::Callbacks callbacks);
    ServiceReverse(BSessionBackup::Callbacks callbacks);
    ~ServiceReverse() override = default;

private:
    Scenario scenario_ {Scenario::UNDEFINED};
    BSessionBackup::Callbacks callbacksBackup_;
    BSessionRestore::Callbacks callbacksRestore_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H