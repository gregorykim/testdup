// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_BROWSER_SHELL_HOST_CONTENT_SETTINGS_MAP_FACTORY_H_
#define CONTENT_SHELL_BROWSER_SHELL_HOST_CONTENT_SETTINGS_MAP_FACTORY_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/singleton.h"
#include "components/keyed_service/content/refcounted_browser_context_keyed_service_factory.h"

class HostContentSettingsMap;

class ShellHostContentSettingsMapFactory {
 public:
  static HostContentSettingsMap* Get();
  static ShellHostContentSettingsMapFactory* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<ShellHostContentSettingsMapFactory>;

  ShellHostContentSettingsMapFactory();
  ~ShellHostContentSettingsMapFactory();

  std::unique_ptr<PrefService> pref_service_;
  scoped_refptr<HostContentSettingsMap> settings_map_;

  DISALLOW_COPY_AND_ASSIGN(ShellHostContentSettingsMapFactory);
};

#endif // CONTENT_SHELL_BROWSER_SHELL_HOST_CONTENT_SETTINGS_MAP_FACTORY_H_
