// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_INDEXED_DB_INDEXED_DB_TRACING_H_
#define CONTENT_BROWSER_INDEXED_DB_INDEXED_DB_TRACING_H_

#include "base/trace_event/trace_event.h"
#define IDB_TRACE(a) TRACE_EVENT0("IndexedDB", (a));
#define IDB_TRACE1(a, arg1_name, arg1_val) \
  TRACE_EVENT1("IndexedDB", (a), (arg1_name), (arg1_val));

#define IDB_ASYNC_TRACE_BEGIN(a, id) \
  TRACE_EVENT_ASYNC_BEGIN0("IndexedDB", (a), (id));
#define IDB_ASYNC_TRACE_END(a, id) \
  TRACE_EVENT_ASYNC_END0("IndexedDB", (a), (id));

#endif  // CONTENT_BROWSER_INDEXED_DB_INDEXED_DB_TRACING_H_
