// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/notifications/notification_common.h"

#include "content/public/browser/browser_context.h"

// static
void NotificationCommon::OpenNotificationSettings(
    content::BrowserContext* browser_context) {
#if defined(OS_ANDROID)
  NOTIMPLEMENTED();
#else
  NOTIMPLEMENTED();
#endif  // defined(OS_ANDROID)
}
