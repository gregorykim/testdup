// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/notifications/web_notification_delegate.h"

#include "base/feature_list.h"
#include "base/metrics/histogram_macros.h"
#include "content/shell/browser/notifications/notification_common.h"
#include "content/public/browser/web_contents.h"
#include "ui/message_center/notifier_settings.h"

using message_center::NotifierId;

namespace features {

const base::Feature kAllowFullscreenWebNotificationsFeature{
  "FSNotificationsWeb", base::FEATURE_DISABLED_BY_DEFAULT
};

} // namespace features


WebNotificationDelegate::WebNotificationDelegate(
    content::BrowserContext* browser_context,
    const std::string& notification_id,
    const GURL& origin)
    : browser_context_(browser_context),
      notification_id_(notification_id),
      origin_(origin) {}

WebNotificationDelegate::~WebNotificationDelegate() {}

std::string WebNotificationDelegate::id() const {
  return notification_id_;
}

void WebNotificationDelegate::SettingsClick() {
  NotificationCommon::OpenNotificationSettings(browser_context_);
}

bool WebNotificationDelegate::ShouldDisplaySettingsButton() {
  return true;
}

bool WebNotificationDelegate::ShouldDisplayOverFullscreen() const {
  return false;
}
