/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2021/5/13.
//

#ifndef __OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_
#define __OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_

#include <vector>
#include <memory>
#include "storage/common/condition_filter.h"
#include "sql/executor/tuple.h"

class Table;
class Trx;

class ExecutionNode {
public:
  ExecutionNode() = default;
  virtual ~ExecutionNode() = default;

  virtual const TupleSchema &schema() = 0;
  virtual RC execute(TupleSet &tuple_set) = 0;
};

class SelectExeNode : public ExecutionNode {
public:
  SelectExeNode(Trx *trx, Table *table);
  virtual ~SelectExeNode();

  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;

  void select_all_fields();
  RC select_field(const char *field_name);
  bool add_filter(Condition &condition);
  bool can_filter_by(RelAttr &attr);
private:
  Trx *trx_ = nullptr;
  Table  * table_;
  TupleSchema  tuple_schema_;
  std::vector<DefaultConditionFilter *> condition_filters_;
};

class CartesianSelectExeNode : public ExecutionNode {
public:
  virtual ~CartesianSelectExeNode();
  static std::unique_ptr<CartesianSelectExeNode> create(
    std::vector<std::unique_ptr<ExecutionNode>> &nodes
  );

  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;
private:
  CartesianSelectExeNode(std::unique_ptr<ExecutionNode> left_node, std::unique_ptr<ExecutionNode> right_node);
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> left_node;
  std::unique_ptr<ExecutionNode> right_node;
};
#endif //__OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_
