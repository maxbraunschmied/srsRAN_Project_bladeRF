/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// Scheduler of asynchronous tasks that are not associated to any specific UE.
class common_task_scheduler
{
public:
  virtual ~common_task_scheduler() = default;

  /// Schedule a new common task.
  virtual bool schedule_async_task(async_task<void> task) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran