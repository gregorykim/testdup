// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_pref_service.h"

#include <utility>

#include "components/content_settings/core/browser/cookie_settings.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_registry.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_service_factory.h"
#include "components/syncable_prefs/pref_service_syncable.h"
#include "components/syncable_prefs/pref_service_syncable_factory.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/shell/browser/shell_browser_context.h"
#include "content/shell/browser/shell_content_browser_client.h"
#include "content/shell/browser/shell_host_content_settings_map_factory.h"

namespace {

// The GCM channel's enabled state.
const char kGCMChannelStatus[] = "gcm.channel_status";

// The GCM channel's polling interval (in seconds).
const char kGCMChannelPollIntervalSeconds[] = "gcm.poll_interval";

// Last time when checking with the GCM channel status server is done.
const char kGCMChannelLastCheckTime[] = "gcm.check_time";

const int kDefaultPollIntervalSeconds = 60 * 60;  // 60 minutes.

}

ShellPrefService::ShellPrefService() {
  base::SequencedWorkerPool* worker_pool =
      content::BrowserThread::GetBlockingPool();
  scoped_refptr<base::SequencedTaskRunner> blocking_task_runner(
      worker_pool->GetSequencedTaskRunnerWithShutdownBehavior(
          worker_pool->GetSequenceToken(),
          base::SequencedWorkerPool::SKIP_ON_SHUTDOWN));
  content::ShellBrowserContext* context = content::ShellContentBrowserClient::Get()->browser_context();
  base::FilePath path = context->GetPath();
  path = path.Append("Prefs");
  scoped_refptr<user_prefs::PrefRegistrySyncable> registry(
        new user_prefs::PrefRegistrySyncable);

  registry->RegisterBooleanPref(kGCMChannelStatus, true);
  registry->RegisterIntegerPref(kGCMChannelPollIntervalSeconds, kDefaultPollIntervalSeconds);
  registry->RegisterInt64Pref(kGCMChannelLastCheckTime, 0);

  ContentSettingsPattern::SetNonWildcardDomainNonPortScheme(content_settings::kChromeUIScheme);
  HostContentSettingsMap::RegisterProfilePrefs(registry.get());

  syncable_prefs::PrefServiceSyncableFactory factory_syncable;
  factory_syncable.SetUserPrefsFile(path, blocking_task_runner.get());
  prefs_syncable_ = factory_syncable.CreateSyncable(registry.get());
}

ShellPrefService::~ShellPrefService() {
  if (prefs_syncable_)
    prefs_syncable_.release();
}

// static
PrefService* ShellPrefService::Get() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  PrefService* prefs = GetInstance()->prefs_syncable_.get();
  return const_cast<PrefService*>(prefs);
}

// static
syncable_prefs::PrefServiceSyncable* ShellPrefService::GetSyncable() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  return GetInstance()->prefs_syncable_.get();
}

// static
ShellPrefService* ShellPrefService::GetInstance() {
  return base::Singleton<ShellPrefService>::get();
}
