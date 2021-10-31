// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_PROJECTION_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_PROJECTION_H_
#include "base.h"
#include <memory>

class ProjectionNode : public SetExecutionNode {
public:
  ProjectionNode(std::unique_ptr<ExecutionNode> child, RelAttr *attrs,
                 int attr_num);
  virtual ~ProjectionNode();
  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;

private:
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> child;
  std::vector<int> fields_map;
};

#endif // __OBSERVER_SQL_EXECUTOR_NODES_PROJECTION_H_