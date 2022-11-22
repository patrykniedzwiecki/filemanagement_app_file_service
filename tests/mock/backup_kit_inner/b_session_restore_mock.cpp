/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "b_session_restore.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "file_ex.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static BSessionRestore::Callbacks callbacks_;

BSessionRestore::~BSessionRestore() {}

unique_ptr<BSessionRestore> BSessionRestore::Init(vector<BundleName> bundlesToRestore, Callbacks callbacks)
{
    try {
        callbacks_ = callbacks;
        auto restore = make_unique<BSessionRestore>();
        return restore;
    } catch (const exception &e) {
        return nullptr;
    }
    return nullptr;
}

UniqueFd BSessionRestore::GetLocalCapabilities()
{
    TestManager tm("BSessionRestoreMock_GetFd_0100");
    string filePath = tm.GetRootDirCurTest().append("tmp");
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    return fd;
}

ErrCode BSessionRestore::PublishFile(BFileInfo fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestore::Start()
{
    callbacks_.onAllBundlesFinished(0);
    callbacks_.onAllBundlesFinished(1);
    callbacks_.onBackupServiceDied();
    callbacks_.onBundleStarted(1, "com.example.app2backup");
    callbacks_.onBundleFinished(1, "com.example.app2backup");
    callbacks_.onBundleFinished(0, "com.example.app2backup");
    callbacks_.onBundleStarted(0, "com.example.app2backup");
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestore::GetExtFileName(string &bundleName, string &fileName)
{
    return BError(BError::Codes::OK);
}

void BSessionRestore::RegisterBackupServiceDied(function<void()> functor)
{
    return;
}
} // namespace OHOS::FileManagement::Backup