/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "module_sched/sched_scheduler.h"

#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"

#include <cstdint>
#include <cstdio>

namespace OHOS::FileManagement::Backup {
using namespace std;

void SchedScheduler::Sched(string bundleName) {}

void SchedScheduler::ExecutingQueueTasks(const string &bundleName) {}

void SchedScheduler::RemoveExtConn(const string &bundleName) {}

void SchedScheduler::InstallingState(const string &bundleName) {}

void SchedScheduler::TryUnloadServiceTimer(bool force) {}

void SchedScheduler::InstallSuccess(const std::string &bundleName, const int32_t resultCode) {}
}; // namespace OHOS::FileManagement::Backup