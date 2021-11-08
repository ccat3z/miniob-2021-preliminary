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
  virtual void set_input_type(AttrType t) { in_type = t; }
  virtual AttrType out_type() { return in_type; }
  virtual void add(std::shared_ptr<TupleValue> v) = 0;
  virtual std::shared_ptr<TupleValue> value() = 0;
  virtual bool support_any_column() { return false; };
  // TODO: predicate push down is required for trigger push down in child node

protected:
  AttrType in_type;
};

class CountAggregator : public Aggregator {
public:
  AttrType out_type() override;
  void add(std::shared_ptr<TupleValue> v) override;
  std::shared_ptr<TupleValue> value() override;
  bool support_any_column() { return true; };

private:
  int count = 0;
};

class MaxAggregator : public Aggregator {
public:
  MaxAggregator() : max(new NullValue()){};
  void add(std::shared_ptr<TupleValue> v) override;
  std::shared_ptr<TupleValue> value() override;

private:
  std::shared_ptr<TupleValue> max;
};

class MinAggregator : public Aggregator {
public:
  MinAggregator() : min(new NullValue()){};
  void add(std::shared_ptr<TupleValue> v) override;
  std::shared_ptr<TupleValue> value() override;

private:
  std::shared_ptr<TupleValue> min;
};

class AvgAggregator : public Aggregator {
public:
  void set_input_type(AttrType t) override;
  AttrType out_type() override;

  void add(std::shared_ptr<TupleValue> v) override;
  std::shared_ptr<TupleValue> value() override;

private:
  uint32_t count = 0;
  double avg;
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

  // < 0: value index
  // = 0: any
  // > 0: tuple index
  std::vector<int> fields_map;
  std::vector<std::shared_ptr<TupleValue>> values;
};

#endif // __OBSERVER_SQL_EXECUTOR_NODES_AGGREGATION_H_