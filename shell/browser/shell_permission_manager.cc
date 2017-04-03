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
#include "content/shell/browser/shell_host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/browser/content_settings_utils.h"

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
}

ShellPermissionManager::~ShellPermissionManager() {
}

void ShellPermissionManager::Accept(const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
									   const GURL req_url)
{
    DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
#if 1
    ContentSetting dummy_result = CONTENT_SETTING_ALLOW;
    // set content setting map
    ShellHostContentSettingsMapFactory::Get()->SetContentSettingDefaultScope(
              GURL(req_url), GURL(), CONTENT_SETTINGS_TYPE_NOTIFICATIONS,
              std::string(), dummy_result);

    std::string setting_str = content_settings::ContentSettingToString(dummy_result);

    callback.Run(blink::mojom::PermissionStatus::GRANTED);
#endif
}

void ShellPermissionManager::Deny(const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
									 const GURL req_url)
{
#if 1
    ContentSetting dummy_result = CONTENT_SETTING_BLOCK;
    // set content setting map
    ShellHostContentSettingsMapFactory::Get()->SetContentSettingDefaultScope(
              GURL(req_url), GURL(), CONTENT_SETTINGS_TYPE_NOTIFICATIONS,
              std::string(), dummy_result);

    std::string setting_str = content_settings::ContentSettingToString(dummy_result);
    // write db
    // task_runner_->PostTask(
    //     FROM_HERE, base::Bind(&ShellPermissionManager::WriteDBOnIO,
    //                           base::Unretained(this), req_url, setting_str));
    callback.Run(blink::mojom::PermissionStatus::DENIED);
#endif
}

void ShellPermissionManager::Closing()
{
    DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
}

int ShellPermissionManager::RequestPermission(
    PermissionType permission,
    RenderFrameHost* render_frame_host,
    const GURL& requesting_origin,
    bool user_gesture,
    const base::Callback<void(blink::mojom::PermissionStatus)>& callback) {
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
#if 0
return RequestPermissions(
    std::vector<PermissionType>(1, permission),
    render_frame_host,
    requesting_origin,
    user_gesture,
    base::Bind(&PermissionRequestResponseCallbackWrapper, callback));
#else
  if (permission == PermissionType::NOTIFICATIONS) {
    // need to popup
    content::WebContents* web_contents =
        content::WebContents::FromRenderFrameHost(render_frame_host);
	const std::vector<bool> tmp;
    ask_popup_.reset(new Permission_prompt::PermissionPromptImpl(this,requesting_origin,callback,web_contents));
    ask_popup_->Show(tmp);
  }
  else {
    callback.Run(IsWhitelistedPermissionType(permission)
                     ? blink::mojom::PermissionStatus::GRANTED
                     : blink::mojom::PermissionStatus::DENIED);
  }
  return kNoPendingOperation;
#endif
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
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
}

void ShellPermissionManager::ResetPermission(
    PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin) {
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " << __FUNCTION__;
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
