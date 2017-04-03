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
#include "url/url_constants.h"

#include "content/shell/browser/shell_host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "content/shell/browser/notifications/web_push_content_views.h"

using content::BrowserContext;
using content::BrowserThread;

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


	WebPushView::Notification noti(NOTIFICATION_TYPE_SIMPLE,///type
					  "",///id
					  notification_data.title,//title
					  base::ASCIIToUTF16(notification_data.lang.c_str()),//message
					  gfx::Image(),///icon
					  notification_data.body,//display source
					  GURL(notification_data.tag),//orignal url
					  0);

    WebPushView::WebPushViewRequest* req = new WebPushView::WebPushViewRequest(std::move(delegate),notification_data.title,notification_data.body);
    req->DisplayWebPush();
  DLOG(WARNING) << "DisplayNotification notification_data.title : " << notification_data.title;
  DLOG(WARNING) << "notification_data.body : " << notification_data.body;
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

base::string16 PlatformNotificationServiceImpl::DisplayNameForContextMessage(
    std::string& profile,
    const GURL& origin) const {

  return base::string16();
}
