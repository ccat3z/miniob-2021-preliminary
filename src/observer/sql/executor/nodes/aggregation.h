// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_AGGREGATION_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_AGGREGATION_H_
#include "base.h"
#include <memory>
#include <unordered_map>
#include <vector>

class Aggregator {
public:
  virtual ~Aggregator() = default;
  void set_input_type(AttrType t) { in_type = t; }
  virtual AttrType out_type() { return in_type; }
  virtual void add(const TupleValue &v) = 0;
  virtual TupleValue *value() = 0;

protected:
  AttrType in_type;
};

class CountAggregator : public Aggregator {
public:
  AttrType out_type() override;
  void add(const TupleValue &v) override;
  TupleValue *value() override;

private:
  int count = 0;
};

class AggregationNode : public VolcanoExecutionNode {
public:
  AggregationNode(std::unique_ptr<ExecutionNode> child, SelectExpr *exprs,
                  int expr_num);
  virtual ~AggregationNode();
  const TupleSchema &schema() override;
  RC next(Tuple &tuple) override;
  void reset() override;

private:
  TupleSchema tuple_schema_;
  bool end = false;
  Aggregator &add_aggregator(const char *func);
  std::vector<std::unique_ptr<Aggregator>> aggregators;
  std::unique_ptr<ExecutionNode> child;

  std::vector<int> fields_map;
  std::vector<std::unique_ptr<TupleValue>> values;
};

#endif // __OBSERVER_SQL_EXECUTOR_NODES_AGGREGATION_H_