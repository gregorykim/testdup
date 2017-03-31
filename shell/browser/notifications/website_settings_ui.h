// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_NOTIFICATIONS_WEBSITE_SETTINGS_WEBSITE_SETTINGS_UI_H_
#define CONTENT_SHELL_BROWSER_NOTIFICATIONS_WEBSITE_SETTINGS_WEBSITE_SETTINGS_UI_H_

#include <memory>
#include <string>
#include <vector>

#include "base/strings/string16.h"
//#include "chrome/browser/ui/website_settings/website_settings.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "content/public/browser/permission_type.h"
//#include "ui/gfx/native_widget_types.h"

//class GURL;
//class Profile;
//class WebsiteSettings;

namespace base {
class Value;
};

namespace gfx {
class Image;
}
enum NotificationPriority {
  MIN_PRIORITY = -2,
  LOW_PRIORITY = -1,
  DEFAULT_PRIORITY = 0,
  HIGH_PRIORITY = 1,
  MAX_PRIORITY = 2,

  // Top priority for system-level notifications.. This can't be set from
  // kPriorityKey, instead you have to call SetSystemPriority() of
  // Notification object.
  SYSTEM_PRIORITY = 3,
};

enum NotificationType {
  NOTIFICATION_TYPE_SIMPLE = 0,
  NOTIFICATION_TYPE_BASE_FORMAT = 1,
  NOTIFICATION_TYPE_IMAGE = 2,
  NOTIFICATION_TYPE_MULTIPLE = 3,
  NOTIFICATION_TYPE_PROGRESS = 4,  // Notification with progress bar.
  NOTIFICATION_TYPE_CUSTOM = 5,

  // Add new values before this line.
  NOTIFICATION_TYPE_LAST = NOTIFICATION_TYPE_PROGRESS
};

// The class |WebsiteSettingsUI| specifies the platform independent
// interface of the website settings UI. The website settings UI displays
// information and controls for site specific data (local stored objects like
// cookies), site specific permissions (location, popup, plugin, etc.
// permissions) and site specific information (identity, connection status,
// etc.).
class PermissionSettingsUI {
 public:
  // The Website Settings UI contains several tabs. Each tab is associated with
  // a unique tab id. The enum |TabId| contains all the ids for the tabs.
  // |PermissionInfo| contains information about a single permission |type| for
  // the current website.
  struct PermissionInfo {
    PermissionInfo();
    // Site permission |type|.
    ContentSettingsType type;
    // The current value for the permission |type| (e.g. ALLOW or BLOCK).
    ContentSetting setting;
    // The global default settings for this permission |type|.
    ContentSetting default_setting;
    // The settings source e.g. user, extensions, policy, ... .
    content_settings::SettingSource source;
    // Whether the profile is off the record.
    bool is_incognito;
  };
  virtual ~PermissionSettingsUI();




};

#endif  // CONTENT_SHELL_BROWSER_NOTIFICATIONS_WEBSITE_SETTINGS_WEBSITE_SETTINGS_UI_H_