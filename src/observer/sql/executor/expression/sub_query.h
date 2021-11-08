// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_EXPRESSION_SUB_QUERY_H_
#define __OBSERVER_SQL_EXECUTOR_EXPRESSION_SUB_QUERY_H_

#include "../nodes/execution_node.h"
#include "base.h"

class Session;
class ExecutionNode;

class SubQueryExpression : public Expression {
public:
  SubQueryExpression(Session *session, Selects *select,
                     const TupleSchema &schema);
  std::shared_ptr<TupleValue> eval(const Tuple &tuple) override;
  void evals(std::vector<std::shared_ptr<TupleValue>> &values,
             const Tuple &tuple) override;
  AttrType type() override;
  friend class SubQueryContextNode;

private:
  void retrieve(const Tuple &tuple);
  TupleSet tuple_set;
  std::unique_ptr<ExecutionNode> exec_node;

  const Tuple *current_tuple;
};

#endif // __OBSERVER_SQL_EXECUTOR_EXPRESSION_SUB_QUERY_H_