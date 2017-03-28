// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/notifications/platform_notification_service_impl.h"

#include <utility>
#include <vector>

#include "base/metrics/histogram_macros.h"
#include "base/metrics/user_metrics_action.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/prefs/pref_service.h"
#include "content/shell/browser/notifications/notification_object_proxy.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/desktop_notification_delegate.h"
#include "content/public/browser/notification_event_dispatcher.h"
#include "content/public/browser/permission_type.h"
#include "content/public/browser/permission_manager.h"
#include "content/public/browser/user_metrics.h"
#include "content/public/common/notification_resources.h"
#include "content/public/common/platform_notification_data.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/message_center/notification.h"
#include "ui/message_center/notification_types.h"
#include "ui/message_center/notifier_settings.h"
#include "url/url_constants.h"

#include "content/shell/browser/shell_host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
using content::BrowserContext;
using content::BrowserThread;
using message_center::NotifierId;

namespace {


void OnCloseNonPersistentNotificationProfileLoaded(
    const std::string& notification_id,
    std::string& profile) {
  NOTIMPLEMENTED();
}

// Callback used to close an non-persistent notification from blink.
void CancelNotification(const std::string& notification_id,
                        std::string profile_id,
                        bool incognito) {
  NOTIMPLEMENTED();
}

}  // namespace

// static
PlatformNotificationServiceImpl*
PlatformNotificationServiceImpl::GetInstance() {
  return base::Singleton<PlatformNotificationServiceImpl>::get();
}

PlatformNotificationServiceImpl::PlatformNotificationServiceImpl() {
}

PlatformNotificationServiceImpl::~PlatformNotificationServiceImpl() {}

void PlatformNotificationServiceImpl::OnPersistentNotificationClick(
    BrowserContext* browser_context,
    const std::string& notification_id,
    const GURL& origin,
    int action_index) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  NOTIMPLEMENTED();
}

void PlatformNotificationServiceImpl::OnPersistentNotificationClose(
    BrowserContext* browser_context,
    const std::string& notification_id,
    const GURL& origin,
    bool by_user) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  NOTIMPLEMENTED();
}

blink::mojom::PermissionStatus
PlatformNotificationServiceImpl::CheckPermissionOnUIThread(
    BrowserContext* browser_context,
    const GURL& origin,
    int render_process_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  // NOTIMPLEMENTED();

  DCHECK(browser_context);
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " <<__FUNCTION__;
  if (!browser_context->GetPermissionManager())
    return blink::mojom::PermissionStatus::DENIED;

  return browser_context->GetPermissionManager()->GetPermissionStatus(
      content::PermissionType::NOTIFICATIONS, origin, origin);

  // return PermissionManager::Get(profile)->GetPermissionStatus(
  //     content::PermissionType::NOTIFICATIONS, origin, origin);
  // return blink::mojom::PermissionStatus::GRANTED;
}

blink::mojom::PermissionStatus
PlatformNotificationServiceImpl::CheckPermissionOnIOThread(
    content::ResourceContext* resource_context,
    const GURL& origin,
    int render_process_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // return browser_context->GetPermissionManager()->GetPermissionStatus(
  //     content::PermissionType::NOTIFICATIONS, origin, origin);
  ContentSetting setting = ShellHostContentSettingsMapFactory::Get()->GetContentSetting(
              origin, GURL(), CONTENT_SETTINGS_TYPE_NOTIFICATIONS, 
              std::string());
  DLOG(WARNING) << __FILE__ <<":"<<__LINE__ << " " <<__FUNCTION__ << " setting: " << setting;
  switch(setting) {
    case CONTENT_SETTING_ALLOW:
      return blink::mojom::PermissionStatus::GRANTED;
    case CONTENT_SETTING_BLOCK:
      return blink::mojom::PermissionStatus::DENIED;
    default:
      break;
  }
  return blink::mojom::PermissionStatus::ASK;
}

void PlatformNotificationServiceImpl::DisplayNotification(
    BrowserContext* browser_context,
    const std::string& notification_id,
    const GURL& origin,
    const content::PlatformNotificationData& notification_data,
    const content::NotificationResources& notification_resources,
    std::unique_ptr<content::DesktopNotificationDelegate> delegate,
    base::Closure* cancel_callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  std::string profile = notification_id;

  NotificationObjectProxy* proxy = new NotificationObjectProxy(
      browser_context, notification_id, origin, std::move(delegate));
  Notification notification = CreateNotificationFromData(
      profile, GURL() /* service_worker_scope */, origin, notification_data,
      notification_resources, proxy);

  DLOG(WARNING) << "notification_data.title : " << notification_data.title;
  DLOG(WARNING) << "notification_data.body : " << notification_data.body;

  NOTIMPLEMENTED();
}

void PlatformNotificationServiceImpl::DisplayPersistentNotification(
    BrowserContext* browser_context,
    const std::string& notification_id,
    const GURL& service_worker_scope,
    const GURL& origin,
    const content::PlatformNotificationData& notification_data,
    const content::NotificationResources& notification_resources) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  DLOG(WARNING) << "DisplayPersistentNotification notification_data.title : " << notification_data.title;
  DLOG(WARNING) << "notification_data.body : " << notification_data.body;
  NOTIMPLEMENTED();
}

void PlatformNotificationServiceImpl::ClosePersistentNotification(
    BrowserContext* browser_context,
    const std::string& notification_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  NOTIMPLEMENTED();
}

bool PlatformNotificationServiceImpl::GetDisplayedPersistentNotifications(
    BrowserContext* browser_context,
    std::set<std::string>* displayed_notifications) {
  DCHECK(displayed_notifications);

  NOTIMPLEMENTED();
  return false;
}

void PlatformNotificationServiceImpl::OnCloseEventDispatchComplete(
    content::PersistentNotificationStatus status) {
  NOTIMPLEMENTED();
}

Notification PlatformNotificationServiceImpl::CreateNotificationFromData(
    std::string& profile,
    const GURL& service_worker_scope,
    const GURL& origin,
    const content::PlatformNotificationData& notification_data,
    const content::NotificationResources& notification_resources,
    NotificationDelegate* delegate) const {
  DCHECK_EQ(notification_data.actions.size(),
            notification_resources.action_icons.size());

  // TODO(peter): Handle different screen densities instead of always using the
  // 1x bitmap - crbug.com/585815.
  Notification notification(
      message_center::NOTIFICATION_TYPE_SIMPLE, notification_data.title,
      notification_data.body,
      gfx::Image::CreateFrom1xBitmap(notification_resources.notification_icon),
      NotifierId(origin), base::UTF8ToUTF16(origin.host()), origin,
      notification_data.tag, message_center::RichNotificationData(), delegate);

  notification.set_service_worker_scope(service_worker_scope);
  notification.set_context_message(
      DisplayNameForContextMessage(profile, origin));
  notification.set_vibration_pattern(notification_data.vibration_pattern);
  notification.set_timestamp(notification_data.timestamp);
  notification.set_renotify(notification_data.renotify);
  notification.set_silent(notification_data.silent);

  if (!notification_resources.image.drawsNothing()) {
    notification.set_type(message_center::NOTIFICATION_TYPE_IMAGE);
    notification.set_image(
        gfx::Image::CreateFrom1xBitmap(notification_resources.image));
  }

  // Badges are only supported on Android, primarily because it's the only
  // platform that makes good use of them in the status bar.
#if defined(OS_ANDROID)
  // TODO(peter): Handle different screen densities instead of always using the
  // 1x bitmap - crbug.com/585815.
  notification.set_small_image(
      gfx::Image::CreateFrom1xBitmap(notification_resources.badge));
#endif  // defined(OS_ANDROID)

  // Developer supplied action buttons.
  std::vector<message_center::ButtonInfo> buttons;
  for (size_t i = 0; i < notification_data.actions.size(); ++i) {
    const content::PlatformNotificationAction& action =
        notification_data.actions[i];
    message_center::ButtonInfo button(action.title);
    // TODO(peter): Handle different screen densities instead of always using
    // the 1x bitmap - crbug.com/585815.
    button.icon =
        gfx::Image::CreateFrom1xBitmap(notification_resources.action_icons[i]);
    button.placeholder = action.placeholder.string();
    switch (action.type) {
      case content::PLATFORM_NOTIFICATION_ACTION_TYPE_BUTTON:
        button.type = message_center::ButtonType::BUTTON;
        break;
      case content::PLATFORM_NOTIFICATION_ACTION_TYPE_TEXT:
        button.type = message_center::ButtonType::TEXT;
        break;
    }
    buttons.push_back(button);
  }
  notification.set_buttons(buttons);

  // On desktop, notifications with require_interaction==true stay on-screen
  // rather than minimizing to the notification center after a timeout.
  // On mobile, this is ignored (notifications are minimized at all times).
  if (notification_data.require_interaction)
    notification.set_never_timeout(true);

  return notification;
}

base::string16 PlatformNotificationServiceImpl::DisplayNameForContextMessage(
    std::string& profile,
    const GURL& origin) const {

  return base::string16();
}