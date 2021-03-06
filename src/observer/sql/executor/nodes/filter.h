// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_FILTER_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_FILTER_H_
#include "../expression/expression.h"
#include "base.h"
#include "session/session.h"
#include <list>
#include <memory>

class TupleFilter {
public:
  TupleFilter(Session *session, const TupleSchema &schema,
              Condition &condition);
  virtual ~TupleFilter();
  bool filter(const Tuple &tuple) const;

private:
  std::unique_ptr<Expression> left, right;
  CompOp op;
};

class FilterNode : public VolcanoExecutionNode {
public:
  FilterNode(Session *session, std::unique_ptr<ExecutionNode> child,
             Condition *begin, Condition *end);
  template <class InputIt>
  FilterNode(Session *session, std::unique_ptr<ExecutionNode> child,
             InputIt begin, InputIt end)
      : session(session), child(std::move(child)) {
    tuple_schema_ = this->child->schema();
    for (; begin != end; begin++) {
      conditions.push_back(*begin);
    }

    build_filters();
  }
  virtual ~FilterNode();
  const TupleSchema &schema() override;
  RC next(Tuple &tuple) override;
  void reset() override;
  std::unique_ptr<ExecutionNode>
  push_down_predicate(std::list<Condition *> &predicate) override;

private:
  Session *session;

  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> child;
  std::list<Condition *> conditions;

  std::vector<std::unique_ptr<TupleFilter>> filters;
  void build_filters();
};
#endif // __OBSERVER_SQL_EXECUTOR_NODES_FILTER_H_