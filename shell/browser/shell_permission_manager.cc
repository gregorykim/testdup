// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_permission_manager.h"

#include "base/callback.h"
#include "base/command_line.h"
#include "content/public/browser/permission_type.h"
#include "content/public/common/content_switches.h"
#include "media/base/media_switches.h"

#include "content/public/browser/browser_thread.h"
#include "content/shell/browser/shell_browser_context.h"
#include "content/shell/browser/shell_content_browser_client.h"
#include "content/browser/notifications/notification_permission_data.h"
namespace content {

namespace {

bool IsWhitelistedPermissionType(PermissionType permission) {
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__;
  return permission == PermissionType::GEOLOCATION ||
          permission == PermissionType::PUSH_MESSAGING ||
         permission == PermissionType::MIDI;
}

}  // namespace

ShellPermissionManager::ShellPermissionManager()
    : PermissionManager() {
  path_ = ShellContentBrowserClient::Get()->browser_context()->GetPath();
  path_ = path_.Append("Permission");
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&ShellPermissionManager::LazyInitialize, base::Unretained(this)));
}

ShellPermissionManager::~ShellPermissionManager() {
  task_runner_->DeleteSoon(FROM_HERE, database_.release());
}

void ShellPermissionManager::LazyInitialize() {
  if (!task_runner_) {
    base::SequencedWorkerPool* pool = BrowserThread::GetBlockingPool();
    base::SequencedWorkerPool::SequenceToken token = pool->GetSequenceToken();

    task_runner_ = pool->GetSequencedTaskRunner(token);
  }

  task_runner_->PostTask(
      FROM_HERE, base::Bind(&ShellPermissionManager::OpenDatabase,
                            base::Unretained(this)));
}

void ShellPermissionManager::OpenDatabase() {
  NotificationDatabase::Status status;
  if (database_ == NULL) {
    database_.reset(new NotificationDatabase(path_));
    status = database_->Open(true /* create_if_missing */);
  }
}

void ShellPermissionManager::ReadDBOnIO(const GURL& requesting_origin,
     const base::Callback<void(blink::mojom::PermissionStatus)>& callback) {

  NotificationDatabase::Status status;
  // if (database_ == NULL) {
  //   database_.reset(new NotificationDatabase(path_));
  //   status = database_->Open(true /* create_if_missing */);
  // }
  // std::vector<NotificationPermissionData> notification_permission_vector;
  status = database_->ReadNotificationPermission(&notification_permission_vector_);
}

void ShellPermissionManager::WriteDBOnIO(const GURL& requesting_origin, bool permission) {
  NotificationDatabase::Status status;
  // if (database_ == NULL) {
  //   database_.reset(new NotificationDatabase(path_));
  //   status = database_->Open(true /* create_if_missing */);
  // }
  status = database_->WriteNotificationPermission(requesting_origin, permission);

  // callback.Run(IsWhitelistedPermissionType(permission)
  //                  ? blink::mojom::PermissionStatus::GRANTED
  //                  : blink::mojom::PermissionStatus::DENIED);
}

void ShellPermissionManager::DestryDBOnIO() {
  if (database_ != NULL) {
    database_->Destroy();
    database_.reset();
  }
}

int ShellPermissionManager::RequestPermission(
    PermissionType permission,
    RenderFrameHost* render_frame_host,
    const GURL& requesting_origin,
    bool user_gesture,
    const base::Callback<void(blink::mojom::PermissionStatus)>& callback) {
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
  if (permission == PermissionType::NOTIFICATIONS) {
    // task_runner_->PostTask(FROM_HERE,
    //     base::Bind(&ShellPermissionManager::WriteDBOnIO, base::Unretained(this), requesting_origin, true));

    callback.Run(blink::mojom::PermissionStatus::GRANTED);
  }
  else {
    callback.Run(IsWhitelistedPermissionType(permission)
                     ? blink::mojom::PermissionStatus::GRANTED
                     : blink::mojom::PermissionStatus::DENIED);
  }
  return kNoPendingOperation;
}

int ShellPermissionManager::RequestPermissions(
    const std::vector<PermissionType>& permissions,
    content::RenderFrameHost* render_frame_host,
    const GURL& requesting_origin,
    bool user_gesture,
    const base::Callback<
        void(const std::vector<blink::mojom::PermissionStatus>&)>& callback) {
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
  std::vector<blink::mojom::PermissionStatus> result(permissions.size());
  for (const auto& permission : permissions) {
    result.push_back(IsWhitelistedPermissionType(permission)
                         ? blink::mojom::PermissionStatus::GRANTED
                         : blink::mojom::PermissionStatus::DENIED);
  }
  callback.Run(result);
  return kNoPendingOperation;
}

void ShellPermissionManager::CancelPermissionRequest(int request_id) {
}

void ShellPermissionManager::ResetPermission(
    PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin) {
}

blink::mojom::PermissionStatus ShellPermissionManager::GetPermissionStatus(
    PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin) {
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
  // Background sync browser tests require permission to be granted by default.
  // TODO(nsatragno): add a command line flag so that it's only granted for
  // tests.
  if (permission == PermissionType::BACKGROUND_SYNC)
    return blink::mojom::PermissionStatus::GRANTED;

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if ((permission == PermissionType::AUDIO_CAPTURE ||
       permission == PermissionType::VIDEO_CAPTURE) &&
      command_line->HasSwitch(switches::kUseFakeDeviceForMediaStream) &&
      command_line->HasSwitch(switches::kUseFakeUIForMediaStream)) {
    return blink::mojom::PermissionStatus::GRANTED;
  }
  if (permission == PermissionType::NOTIFICATIONS) {
    return blink::mojom::PermissionStatus::GRANTED;
  }
  return blink::mojom::PermissionStatus::DENIED;
}

void ShellPermissionManager::RegisterPermissionUsage(
    PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin) {
}

int ShellPermissionManager::SubscribePermissionStatusChange(
    PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin,
    const base::Callback<void(blink::mojom::PermissionStatus)>& callback) {
  return kNoPendingOperation;
}

void ShellPermissionManager::UnsubscribePermissionStatusChange(
    int subscription_id) {
}

}  // namespace content