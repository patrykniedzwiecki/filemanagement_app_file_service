/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_H

#include "iremote_broker.h"

namespace OHOS::FileManagement::Backup {
class IServiceReverse : public IRemoteBroker {
public:
    enum class Scenario {
        UNDEFINED,
        BACKUP,
        RESTORE,
    };

    enum {
        SERVICER_BACKUP_ON_FILE_READY,
        SERVICER_BACKUP_ON_SUB_TASK_STARTED,
        SERVICER_BACKUP_ON_SUB_TASK_FINISHED,
        SERVICER_BACKUP_ON_TASK_FINISHED,

        SERVICER_RESTORE_ON_SUB_TASK_STARTED,
        SERVICER_RESTORE_ON_SUB_TASK_FINISHED,
        SERVICER_RESTORE_ON_TASK_FINISHED,
    };

public:
    virtual void BackupOnFileReady(std::string appId, std::string fileName, int fd) = 0;
    virtual void BackupOnSubTaskStarted(int32_t errCode, std::string appId) = 0;
    virtual void BackupOnSubTaskFinished(int32_t errCode, std::string appId) = 0;
    virtual void BackupOnTaskFinished(int32_t errCode) = 0;

    virtual void RestoreOnSubTaskStarted(int32_t errCode, std::string appId) = 0;
    virtual void RestoreOnSubTaskFinished(int32_t errCode, std::string appId) = 0;
    virtual void RestoreOnTaskFinished(int32_t errCode) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileManagement.Backup.IServiceReverse")
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_H