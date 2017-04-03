// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_SHELL_PREF_SERVICE_H_
#define CONTENT_SHELL_BROWSER_SHELL_PREF_SERVICE_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/singleton.h"

class PrefService;
class HostContentSettingsMap;
class PrefServiceSyncable;

namespace syncable_prefs {
class PrefServiceSyncable;
}

class ShellPrefService {
 public:
  static PrefService* Get();
  static syncable_prefs::PrefServiceSyncable* GetSyncable();
  static ShellPrefService* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<ShellPrefService>;

  ShellPrefService();
  ~ShellPrefService();

  std::unique_ptr<syncable_prefs::PrefServiceSyncable> prefs_syncable_;

  DISALLOW_COPY_AND_ASSIGN(ShellPrefService);
};

#endif // CONTENT_SHELL_BROWSER_SHELL_PREF_SERVICE_H_
