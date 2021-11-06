// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_BASE_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_BASE_H_
#include "../tuple.h"
#include <list>

class ExecutionNode {
public:
  ExecutionNode() = default;
  virtual ~ExecutionNode() = default;

  // Set model
  virtual const TupleSchema &schema() = 0;
  virtual RC execute(TupleSet &tuple_set) = 0;

  // Volcano model
  virtual RC next(Tuple &tuple) = 0;
  virtual void reset() = 0;

  // Optimizer
  std::unique_ptr<ExecutionNode> push_down_predicate();
  virtual std::unique_ptr<ExecutionNode>
  push_down_predicate(std::list<Condition *> &predicate);
};

class SetExecutionNode : public ExecutionNode {
public:
  virtual ~SetExecutionNode() = default;

  virtual RC next(Tuple &tuple);
  virtual void reset();

private:
  int next_record_idx = -1;
  TupleSet tuple_set_buf;
};

class VolcanoExecutionNode : public ExecutionNode {
public:
  virtual ~VolcanoExecutionNode() = default;

  // Set model
  virtual RC execute(TupleSet &tuple_set);
};
#endif // __OBSERVER_SQL_EXECUTOR_NODES_BASE_H_