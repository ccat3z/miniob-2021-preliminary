// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_ORDER_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_ORDER_H_
#include "base.h"
#include "session/session.h"
#include "sql/parser/parse.h"
#include <vector>

class OrderNode : public SetExecutionNode {
public:
  OrderNode(Session *session, std::unique_ptr<ExecutionNode> child,
            OrderBy *order_by, size_t order_by_num);
  virtual ~OrderNode();
  const TupleSchema &schema() override;

  RC execute(TupleSet &tuple_set) override;
  std::unique_ptr<ExecutionNode>
  push_down_predicate(std::list<Condition *> &predicate) override;

private:
  std::unique_ptr<ExecutionNode> child;
  std::vector<std::pair<int, OrderDir>> fields;
  Session *session;
};

#endif // __OBSERVER_SQL_EXECUTOR_NODES_ORDER_H_