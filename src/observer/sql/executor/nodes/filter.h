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
#include <memory>

class FilterNode : public ExecutionNode {
public:
  virtual ~FilterNode();
  static std::unique_ptr<FilterNode>
  create(std::unique_ptr<ExecutionNode> child,
         std::vector<Condition *> &conditions);
  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;

private:
  FilterNode(std::unique_ptr<ExecutionNode> child,
             std::vector<std::unique_ptr<TupleFilter>> &&filters);
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> child;
  std::vector<std::unique_ptr<TupleFilter>> filters;
};
#endif // __OBSERVER_SQL_EXECUTOR_NODES_FILTER_H_