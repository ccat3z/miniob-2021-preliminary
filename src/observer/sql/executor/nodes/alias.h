// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_ALIAS_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_ALIAS_H_

#include "base.h"
#include "session/session.h"

class AliasNode : public VolcanoExecutionNode {
public:
  AliasNode(Session *session, std::unique_ptr<ExecutionNode> child,
            const char *table_name);
  virtual ~AliasNode();
  const TupleSchema &schema() override;
  RC next(Tuple &tuple) override;
  void reset() override;
  std::unique_ptr<ExecutionNode>
  push_down_predicate(std::list<Condition *> &predicate) override;

private:
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> child;
  Session *session;
};

#endif // __OBSERVER_SQL_EXECUTOR_NODES_ALIAS_H_