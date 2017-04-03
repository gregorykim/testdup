// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_SHELL_PERMISSION_MANAGER_H_
#define CONTENT_SHELL_BROWSER_SHELL_PERMISSION_MANAGER_H_

#include "base/callback_forward.h"
#include "base/macros.h"
#include "content/public/browser/permission_manager.h"

#include "content/browser/notifications/notification_database.h"
#include "content/shell/browser/notifications/permission_prompt_impl.h"

namespace content {

class ShellPermissionManager : public PermissionManager ,
										Permission_prompt::Delegate {
 public:
  ShellPermissionManager();
  ~ShellPermissionManager() override;

  // PermissionManager implementation.
  int RequestPermission(
      PermissionType permission,
      RenderFrameHost* render_frame_host,
      const GURL& requesting_origin,
      bool user_gesture,
      const base::Callback<void(blink::mojom::PermissionStatus)>& callback)
      override;
  int RequestPermissions(
      const std::vector<PermissionType>& permission,
      RenderFrameHost* render_frame_host,
      const GURL& requesting_origin,
      bool user_gesture,
      const base::Callback<
          void(const std::vector<blink::mojom::PermissionStatus>&)>& callback)
      override;
  void CancelPermissionRequest(int request_id) override;
  void ResetPermission(PermissionType permission,
                       const GURL& requesting_origin,
                       const GURL& embedding_origin) override;
  blink::mojom::PermissionStatus GetPermissionStatus(
      PermissionType permission,
      const GURL& requesting_origin,
      const GURL& embedding_origin) override;
  void RegisterPermissionUsage(PermissionType permission,
                               const GURL& requesting_origin,
                               const GURL& embedding_origin) override;
  int SubscribePermissionStatusChange(
      PermissionType permission,
      const GURL& requesting_origin,
      const GURL& embedding_origin,
      const base::Callback<void(blink::mojom::PermissionStatus)>& callback)
      override;
  void UnsubscribePermissionStatusChange(int subscription_id) override;

///delegate
  void Accept(const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
                                         const GURL req_url) override;
  void Deny(const base::Callback<void(blink::mojom::PermissionStatus)>& callback,
                                         const GURL req_url) override;
  void Closing() override;

 private:
  std::unique_ptr<Permission_prompt::PermissionPromptImpl> ask_popup_;
  std::unique_ptr<NotificationDatabase> database_;
  std::map<std::string, std::string> notification_permission_vector_;

  DISALLOW_COPY_AND_ASSIGN(ShellPermissionManager);
};

}  // namespace content

#endif // CONTENT_SHELL_BROWSER_SHELL_PERMISSION_MANAGER_H
