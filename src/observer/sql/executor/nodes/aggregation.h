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
#include "../expression/base.h"
#include "base.h"
#include <memory>
#include <unordered_map>
#include <vector>

class Aggregator {
public:
  virtual ~Aggregator() = default;

  void set_input_expr(SelectExpr *expr, const TupleSchema &schema);
  void add_tuple(const Tuple &tuple);

  virtual AttrType out_type() { return in_type; }
  virtual std::shared_ptr<TupleValue> value() = 0;
  virtual void reset(){};

protected:
  virtual void set_input_type(AttrType t) { in_type = t; }
  virtual void add(std::shared_ptr<TupleValue> v) = 0;
  virtual bool support_any_column() { return false; };

  std::shared_ptr<Expression> expression;
  AttrType in_type;
};

class CountAggregator : public Aggregator {
public:
  AttrType out_type() override;
  std::shared_ptr<TupleValue> value() override;
  void reset() override { count = 0; };

protected:
  void add(std::shared_ptr<TupleValue> v) override;
  bool support_any_column() { return true; };

private:
  int count = 0;
};

class MaxAggregator : public Aggregator {
public:
  MaxAggregator() : max(new NullValue()){};
  std::shared_ptr<TupleValue> value() override;
  void reset() override { max = std::shared_ptr<TupleValue>(new NullValue()); };

protected:
  void add(std::shared_ptr<TupleValue> v) override;

private:
  std::shared_ptr<TupleValue> max;
};

class MinAggregator : public Aggregator {
public:
  MinAggregator() : min(new NullValue()){};
  std::shared_ptr<TupleValue> value() override;
  void reset() override { min = std::shared_ptr<TupleValue>(new NullValue()); };

protected:
  void add(std::shared_ptr<TupleValue> v) override;

private:
  std::shared_ptr<TupleValue> min;
};

class AvgAggregator : public Aggregator {
public:
  AttrType out_type() override;
  std::shared_ptr<TupleValue> value() override;
  void reset() override { count = 0; };

protected:
  void set_input_type(AttrType t) override;
  void add(std::shared_ptr<TupleValue> v) override;

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
  // TODO: predicate push down is required for trigger push down in child node

private:
  TupleSchema tuple_schema_;
  bool end = false;
  Aggregator &add_aggregator(const char *func);
  std::vector<std::unique_ptr<Aggregator>> aggregators;
  std::unique_ptr<ExecutionNode> child;
};

#endif // __OBSERVER_SQL_EXECUTOR_NODES_AGGREGATION_H_