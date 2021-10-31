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
#include "../tuple_filter.h"
#include "base.h"
#include <list>
#include <memory>

class FilterNode : public VolcanoExecutionNode {
public:
  FilterNode(std::unique_ptr<ExecutionNode> child, Condition *begin,
             Condition *end)
      : child(std::move(child)) {
    tuple_schema_ = this->child->schema();
    for (; begin != end; begin++) {
      conditions.push_back(begin);
    }

    build_filters();
  }
  template <class InputIt>
  FilterNode(std::unique_ptr<ExecutionNode> child, InputIt begin, InputIt end)
      : child(std::move(child)) {
    tuple_schema_ = this->child->schema();
    for (; begin != end; begin++) {
      conditions.push_back(*begin);
    }

    build_filters();
  }
  virtual ~FilterNode();
  const TupleSchema &schema() override;
  virtual RC next(Tuple &tuple);
  virtual void reset();

private:
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> child;
  std::list<Condition *> conditions;

  std::vector<std::unique_ptr<TupleFilter>> filters;
  void build_filters();
};
#endif // __OBSERVER_SQL_EXECUTOR_NODES_FILTER_H_