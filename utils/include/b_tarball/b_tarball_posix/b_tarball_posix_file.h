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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_H

#include "b_tarball_posix_extended_data.h"
#include "b_tarball_posix_extended_header.h"
#include "b_tarball_posix_file_data.h"
#include "b_tarball_posix_pax_header.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixFile {
public:
    void Publish(const UniqueFd &outFile);

public:
    BTarballPosixFile() = default;
    explicit BTarballPosixFile(const std::string &pathName);
    ~BTarballPosixFile() = default;

private:
    std::string pathName_;
    UniqueFd file_ {-1};
    BTarballPosixExtendedData extData_;
    BTarballPosixExtendedHeader extHdr_;
    BTarballPosixPaxHeader paxHdr_;
    BTarballPosixFileData fileData_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_H