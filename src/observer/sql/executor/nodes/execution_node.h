// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_EXECUTION_NODE_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_EXECUTION_NODE_H_

#include "aggregation.h"
#include "alias.h"
#include "base.h"
#include "cartesian.h"
#include "filter.h"
#include "order.h"
#include "projection.h"
#include "storage/trx/trx.h"
#include "table_scaner.h"
#include <memory>

std::unique_ptr<ExecutionNode>
build_select_executor_node(const char *db, Trx *trx, Selects &selects);

#endif // __OBSERVER_SQL_EXECUTOR_NODES_EXECUTION_NODE_H_
