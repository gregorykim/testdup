// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_NOTIFICATIONS_PLATFORM_NOTIFICATION_SERVICE_IMPL_H_
#define CONTENT_SHELL_BROWSER_NOTIFICATIONS_PLATFORM_NOTIFICATION_SERVICE_IMPL_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>

#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/memory/singleton.h"
#include "base/strings/string16.h"
#include "content/public/browser/platform_notification_service.h"
#include "content/public/common/persistent_notification_status.h"
#include "third_party/WebKit/public/platform/modules/permissions/permission_status.mojom.h"

#include "content/shell/browser/notifications/notification.h"
#include "content/shell/browser/notifications/web_push_content_views.h"

class NotificationDisplayService;
class ScopedKeepAlive;

namespace content {
class BrowserContext;
struct NotificationResources;
}

namespace gcm {
class PushMessagingBrowserTest;
}

// The platform notification service is the profile-agnostic entry point through
// which Web Notifications can be controlled.
class PlatformNotificationServiceImpl
    : public content::PlatformNotificationService {
 public:
  // Returns the active instance of the service in the browser process. Safe to
  // be called from any thread.
  static PlatformNotificationServiceImpl* GetInstance();

  // To be called when a persistent notification has been clicked on. The
  // Service Worker associated with the registration will be started if
  // needed, on which the event will be fired. Must be called on the UI thread.
  void OnPersistentNotificationClick(content::BrowserContext* browser_context,
                                     const std::string& notification_id,
                                     const GURL& origin,
                                     int action_index);

  // To be called when a persistent notification has been closed. The data
  // associated with the notification has to be pruned from the database in this
  // case, to make sure that it continues to be in sync. Must be called on the
  // UI thread.
  void OnPersistentNotificationClose(content::BrowserContext* browser_context,
                                     const std::string& notification_id,
                                     const GURL& origin,
                                     bool by_user);

  // content::PlatformNotificationService implementation.
  blink::mojom::PermissionStatus CheckPermissionOnUIThread(
      content::BrowserContext* browser_context,
      const GURL& origin,
      int render_process_id) override;
  blink::mojom::PermissionStatus CheckPermissionOnIOThread(
      content::ResourceContext* resource_context,
      const GURL& origin,
      int render_process_id) override;
  void DisplayNotification(
      content::BrowserContext* browser_context,
      const std::string& notification_id,
      const GURL& origin,
      const content::PlatformNotificationData& notification_data,
      const content::NotificationResources& notification_resources,
      std::unique_ptr<content::DesktopNotificationDelegate> delegate,
      base::Closure* cancel_callback) override;
  void DisplayPersistentNotification(
      content::BrowserContext* browser_context,
      const std::string& notification_id,
      const GURL& service_worker_scope,
      const GURL& origin,
      const content::PlatformNotificationData& notification_data,
      const content::NotificationResources& notification_resources) override;
  void ClosePersistentNotification(content::BrowserContext* browser_context,
                                   const std::string& notification_id) override;
  bool GetDisplayedPersistentNotifications(
      content::BrowserContext* browser_context,
      std::set<std::string>* displayed_notifications) override;

 private:
  friend struct base::DefaultSingletonTraits<PlatformNotificationServiceImpl>;

  PlatformNotificationServiceImpl();
  ~PlatformNotificationServiceImpl() override;

  // Persistent notifications fired through the delegate do not care about the
  // lifetime of the Service Worker responsible for executing the event.
  void OnClickEventDispatchComplete(
      content::PersistentNotificationStatus status);
  void OnCloseEventDispatchComplete(
      content::PersistentNotificationStatus status);

  // Returns a display name for an origin, to be used in the context message
  base::string16 DisplayNameForContextMessage(std::string& profile,
                                              const GURL& origin) const;

  // Returns the notification display service to use.
  // This can be overriden in tests.
  // TODO(miguelg): Remove this method in favor of providing a testing factory
  // to the NotificationDisplayServiceFactory.
  NotificationDisplayService* GetNotificationDisplayService(std::string& profile);

  void SetNotificationDisplayServiceForTesting(
      NotificationDisplayService* service);

#if 0 //BUILDFLAG(ENABLE_BACKGROUND)
  // Makes sure we keep the browser alive while the event in being processed.
  // As we have no control on the click handling, the notification could be
  // closed before a browser is brought up, thus terminating Chrome if it was
  // the last KeepAlive. (see https://crbug.com/612815)
  std::unique_ptr<ScopedKeepAlive> click_dispatch_keep_alive_;

  int pending_click_dispatch_events_;
#endif

  // Tracks the id of persistent notifications that have been closed
  // programmatically to avoid dispatching close events for them.
  std::unordered_set<std::string> closed_notifications_;
  std::unique_ptr<WebPushView::WebPushContentViews> web_push_popup_view_;

  DISALLOW_COPY_AND_ASSIGN(PlatformNotificationServiceImpl);
};

#endif  // CONTENT_SHELL_BROWSER_NOTIFICATIONS_PLATFORM_NOTIFICATION_SERVICE_IMPL_H_
