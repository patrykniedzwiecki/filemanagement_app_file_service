/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include <atomic>
#include <cerrno>
#include <condition_variable>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_filesystem/b_file.h"
#include "b_resources/b_constants.h"
#include "backup_kit_inner.h"
#include "directory_ex.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class Session {
public:
    void UpdateBundleReceivedFiles(const BundleName &bundleName)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].currentFiles++;
        TryClearBundleOfMap(bundleName);
    }

    void SetBundleTotalFiles(const BundleName &bundleName, const uint32_t &existingFiles)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].totalFiles = existingFiles;
        TryClearBundleOfMap(bundleName);
    }

    void TryNotify(bool flag = false)
    {
        if (flag == true) {
            ready_ = true;
            cv_.notify_all();
        } else if (bundleStatusMap_.size() == 0) {
            ready_ = true;
            cv_.notify_all();
        }
    }

    void Wait()
    {
        unique_lock<mutex> lk(lock_);
        cv_.wait(lk, [&] { return ready_; });
    }

    unique_ptr<BSessionBackup> session_ = {};

private:
    struct BundleStatus {
        uint32_t currentFiles = 0;
        uint32_t totalFiles = -1;
    };

    void TryClearBundleOfMap(const BundleName &bundleName)
    {
        if (bundleStatusMap_[bundleName].currentFiles == bundleStatusMap_[bundleName].totalFiles) {
            bundleStatusMap_.erase(bundleName);
        }
    }

    map<string, BundleStatus> bundleStatusMap_;
    mutable condition_variable cv_;
    mutex lock_;
    bool ready_ = false;
};

static string GenHelpMsg()
{
    return "the functionality of the backup api. Arg list:\n"
           "path_cap_file bundleName1 bundleName2...";
}

static void OnFileReady(shared_ptr<Session> ctx, const BFileInfo &fileInfo, UniqueFd fd)
{
    printf("FileReady owner = %s, fileName = %s, sn = %d, fd = %d\n", fileInfo.owner.c_str(), fileInfo.fileName.c_str(),
           fileInfo.sn, fd.Get());
    string tmpPath = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + fileInfo.owner;
    if (access(tmpPath.data(), F_OK) != 0 && mkdir(tmpPath.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    struct stat stat = {};
    if (fstat(fd, &stat) == -1) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    UniqueFd fdLocal(open((tmpPath + "/" + fileInfo.fileName).data(), O_WRONLY | O_CREAT, S_IRWXU));
    if (fdLocal < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    if (sendfile(fdLocal, fd, nullptr, stat.st_size) == -1) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    ctx->UpdateBundleReceivedFiles(fileInfo.owner);
    ctx->TryNotify();
}

static void OnBundleStarted(ErrCode err, const BundleName name)
{
    printf("BundleStarted errCode = %d, BundleName = %s\n", err, name.c_str());
}

static void OnBundleFinished(shared_ptr<Session> ctx, ErrCode err, const BundleName name, uint32_t existingFiles)
{
    printf("BundleFinished errCode = %d, BundleName = %s, existingFiles = %d\n", err, name.c_str(), existingFiles);
    ctx->SetBundleTotalFiles(name, existingFiles);
    ctx->TryNotify();
}

static void OnAllBundlesFinished(shared_ptr<Session> ctx, ErrCode err)
{
    if (err == 0) {
        printf("backup successful\n");
    } else {
        printf("Failed to Unplanned Abort error: %d\n", err);
        ctx->TryNotify(true);
        return;
    }
    ctx->TryNotify();
}

static void OnBackupServiceDied()
{
    printf("backupServiceDied\n");
}

static int32_t InitPathCapFile(string_view pathCapFile, ToolsOp::CRefVStrView args)
{
    std::vector<BundleName> bundleNames;
    for (auto &&bundleName : args) {
        bundleNames.emplace_back(bundleName.data());
    }

    UniqueFd fd(open(pathCapFile.data(), O_RDONLY));
    if (fd < 0) {
        fprintf(stderr, "Failed to open file error: %d %s\n", errno, strerror(errno));
        return -errno;
    }

    if (access((BConstants::BACKUP_TOOL_RECEIVE_DIR).data(), F_OK) != 0 &&
        mkdir((BConstants::BACKUP_TOOL_RECEIVE_DIR).data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    auto ctx = make_shared<Session>();
    ctx->session_ = BSessionBackup::Init(
        move(fd), bundleNames,
        BSessionBackup::Callbacks {
            .onFileReady = bind(OnFileReady, ctx, placeholders::_1, placeholders::_2),
            .onBundleStarted = OnBundleStarted,
            .onBundleFinished = bind(OnBundleFinished, ctx, placeholders::_1, placeholders::_2, placeholders::_3),
            .onAllBundlesFinished = bind(OnAllBundlesFinished, ctx, placeholders::_1),
            .onBackupServiceDied = OnBackupServiceDied,
        });
    if (ctx->session_ == nullptr) {
        printf("Failed to init backup");
        return -EPERM;
    }
    int ret = ctx->session_->Start();
    if (ret != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "backup start error");
    }
    ctx->Wait();
    return 0;
}

static int Exec(ToolsOp::CRefVStrView args)
{
    if (args.empty()) {
        fprintf(stderr, "Please input the name of API to backup\n");
        return -EINVAL;
    }

    std::vector<string_view> argsWithoutHead(args.begin() + 1, args.end());
    return InitPathCapFile(args.front(), argsWithoutHead);
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"backup"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace OHOS::FileManagement::Backup