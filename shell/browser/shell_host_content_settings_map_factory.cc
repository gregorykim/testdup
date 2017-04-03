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
#include "content/shell/browser/shell_browser_context.h"
#include "content/shell/browser/shell_content_browser_client.h"
#include "content/shell/browser/shell_pref_service.h"


ShellHostContentSettingsMapFactory::ShellHostContentSettingsMapFactory()
    // : RefcountedBrowserContextKeyedServiceFactory(
    //     "HostContentSettingsMap",
    //     BrowserContextDependencyManager::GetInstance()) {
{
  settings_map = new HostContentSettingsMap(
    ShellPrefService::Get(),
    false,
    false);

  // syncable_prefs::PrefServiceSyncable* pref_service =
  //     PrefServiceSyncableFromProfile(profile);
  if (ShellPrefService::GetSyncable()) {
    ShellPrefService::GetSyncable()->RegisterMergeDataFinishedCallback(
        base::Bind(&HostContentSettingsMap::MigrateDomainScopedSettings,
                   settings_map->GetWeakPtr(), true /* after_sync */));
  }
}

ShellHostContentSettingsMapFactory::~ShellHostContentSettingsMapFactory() {
}

// static
HostContentSettingsMap* ShellHostContentSettingsMapFactory::Get() {
  // DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  // content::ShellBrowserContext* context = content::ShellContentBrowserClient::Get()->browser_context();
  // return static_cast<HostContentSettingsMap*>(
  //     GetInstance()->GetServiceForBrowserContext(context, true).get());
  return GetInstance()->settings_map.get();
}

// static
ShellHostContentSettingsMapFactory* ShellHostContentSettingsMapFactory::GetInstance() {
  return base::Singleton<ShellHostContentSettingsMapFactory>::get();
}

// scoped_refptr<RefcountedKeyedService>
//     ShellHostContentSettingsMapFactory::BuildServiceInstanceFor(
//     content::BrowserContext* context) const {
//   // DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

//   scoped_refptr<HostContentSettingsMap> settings_map(new HostContentSettingsMap(
//     ShellPrefService::Get(),
//     false,
//     true));

//   // syncable_prefs::PrefServiceSyncable* pref_service =
//   //     PrefServiceSyncableFromProfile(profile);
//   if (ShellPrefService::GetSyncable()) {
//     ShellPrefService::GetSyncable()->RegisterMergeDataFinishedCallback(
//         base::Bind(&HostContentSettingsMap::MigrateDomainScopedSettings,
//                    settings_map->GetWeakPtr(), true /* after_sync */));
//   }

//   return settings_map;
// }

// content::BrowserContext* ShellHostContentSettingsMapFactory::GetBrowserContextToUse(
//     content::BrowserContext* context) const {
//   return context;
// }
