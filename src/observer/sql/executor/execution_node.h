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
#include "tuple_filter.h"

class Table;
class Trx;

class ExecutionNode {
public:
  ExecutionNode() = default;
  virtual ~ExecutionNode() = default;

  virtual const TupleSchema &schema() = 0;
  virtual RC execute(TupleSet &tuple_set) = 0;
};

class TableScaner : public ExecutionNode {
public:
  TableScaner(Trx *trx, Table *table);
  virtual ~TableScaner();

  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;

  void select_all_fields();
  RC select_field(const char *field_name);
  bool add_filter(Condition &condition);
  bool can_filter_by(RelAttr &attr);
  const Table *table();
private:
  Trx *trx_ = nullptr;
  Table  * table_;
  TupleSchema  tuple_schema_;
  std::vector<DefaultConditionFilter *> condition_filters_;
};

class CartesianSelectNode : public ExecutionNode {
public:
  virtual ~CartesianSelectNode();
  static std::unique_ptr<CartesianSelectNode> create(
    std::vector<std::unique_ptr<ExecutionNode>> &nodes
  );

  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;
private:
  CartesianSelectNode(std::unique_ptr<ExecutionNode> left_node, std::unique_ptr<ExecutionNode> right_node);
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> left_node;
  std::unique_ptr<ExecutionNode> right_node;
};

class ProjectionNode : public ExecutionNode {
public:
  static std::unique_ptr<ProjectionNode> create(std::unique_ptr<ExecutionNode> child, RelAttr *attrs, int attr_num);
  virtual ~ProjectionNode();
  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;

private:
  ProjectionNode(std::unique_ptr<ExecutionNode> child, int fields_size) : child(std::move(child)), fields_map(fields_size, -1) {};
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> child;
  std::vector<int> fields_map;
};

class FilterNode : public ExecutionNode {
public:
  virtual ~FilterNode();
  static std::unique_ptr<FilterNode> create(std::unique_ptr<ExecutionNode> child, std::vector<Condition *> &conditions);
  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;
private:
  FilterNode(std::unique_ptr<ExecutionNode> child, std::vector<std::unique_ptr<TupleFilter>> &&filters);
  TupleSchema tuple_schema_;
  std::unique_ptr<ExecutionNode> child;
  std::vector<std::unique_ptr<TupleFilter>> filters;
};
#endif //__OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_
