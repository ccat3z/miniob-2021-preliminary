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

  virtual std::unique_ptr<Aggregator> clone() = 0;

protected:
  // clone_t create a TAggregator and copy properties of class Aggregator.
  // Then reset() the new TAggregator.
  template <class TAggregator> std::unique_ptr<TAggregator> clone_t() {
    auto agg = std::make_unique<TAggregator>();
    agg->expression = expression;
    agg->in_type = in_type;
    agg->reset();
    return agg;
  }
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
  std::unique_ptr<Aggregator> clone() override {
    return clone_t<CountAggregator>();
  };

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
  std::unique_ptr<Aggregator> clone() override {
    return clone_t<MaxAggregator>();
  };

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
  std::unique_ptr<Aggregator> clone() override {
    return clone_t<MinAggregator>();
  };

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
  std::unique_ptr<Aggregator> clone() override {
    return clone_t<AvgAggregator>();
  };

protected:
  void set_input_type(AttrType t) override;
  void add(std::shared_ptr<TupleValue> v) override;

private:
  uint32_t count = 0;
  double avg;
};

class AggregationNode : public SetExecutionNode {
public:
  AggregationNode(std::unique_ptr<ExecutionNode> child, SelectExpr *exprs,
                  int expr_num, RelAttr *group_keys, int group_keys_num);
  virtual ~AggregationNode();
  const TupleSchema &schema() override;

  RC execute(TupleSet &tuple_set) override;
  // TODO: predicate push down is required for trigger push down in child node

private:
  TupleSchema tuple_schema_;
  Aggregator &add_aggregator(const char *func);
  std::unique_ptr<Tuple> build_key_tuple(const Tuple &tuple);
  std::vector<int> keys;
  std::vector<std::unique_ptr<Aggregator>> aggregators;
  std::unique_ptr<ExecutionNode> child;
};

#endif // __OBSERVER_SQL_EXECUTOR_NODES_AGGREGATION_H_