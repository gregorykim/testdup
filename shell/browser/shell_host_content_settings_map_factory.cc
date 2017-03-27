// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_host_content_settings_map_factory.h"

#include <utility>

#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/prefs/pref_service.h"
#include "components/syncable_prefs/pref_service_syncable.h"
#include "content/public/browser/browser_thread.h"
#include "content/shell/browser/shell_pref_service.h"


ShellHostContentSettingsMapFactory::ShellHostContentSettingsMapFactory() {
  pref_service_.reset(ShellPrefService::Get());
  // pref_service_ = ShellPrefService::Get();
  settings_map_ = new HostContentSettingsMap(
    pref_service_.get(),
    false,
    true);

  // if (pref_service_) {
    ShellPrefService::GetSyncable()->RegisterMergeDataFinishedCallback(
        base::Bind(&HostContentSettingsMap::MigrateDomainScopedSettings,
                   settings_map_->GetWeakPtr(), true /* after_sync */));
  // }
}

ShellHostContentSettingsMapFactory::~ShellHostContentSettingsMapFactory() {
}

// static
HostContentSettingsMap* ShellHostContentSettingsMapFactory::Get() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  return GetInstance()->settings_map_.get();
}

// static
ShellHostContentSettingsMapFactory* ShellHostContentSettingsMapFactory::GetInstance() {
  return base::Singleton<ShellHostContentSettingsMapFactory>::get();
}
