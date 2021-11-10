// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "aggregation.h"
#include "../expression/expression.h"
#include "common/log/log.h"
#include <algorithm>

AggregationNode::AggregationNode(std::unique_ptr<ExecutionNode> child,
                                 SelectExpr *exprs, int expr_num,
                                 RelAttr *group_keys, int group_keys_num) {
  for (int i = 0; i < group_keys_num; i++) {
    auto &key_attr = group_keys[i];
    auto field_idx = child->schema().index_of_field(key_attr.relation_name,
                                                    key_attr.attribute_name);
    if (field_idx < 0) {
      std::stringstream ss;
      ss << "Cannot find group key " << group_keys->relation_name << '.'
         << group_keys->attribute_name;
      throw std::invalid_argument(ss.str());
    }

    keys.push_back(field_idx);

    auto &field = child->schema().field(field_idx);
    tuple_schema_.add(field.type(), field.table_name(), field.field_name());
  }

  for (int i = 0; i < expr_num; i++) {
    auto &expr = exprs[i];
    if (expr.agg == nullptr) {
      continue; // skip non-aggregation exprs
    }

    auto &aggor = add_aggregator(expr.agg->agg_func);
    aggor.set_input_expr(expr.agg->expr, child->schema());

    std::stringstream ss;
    ss << expr;
    tuple_schema_.add(aggor.out_type(), "", ss.str().c_str());
  }

  this->child = std::move(child);
}

AggregationNode::~AggregationNode() {}
const TupleSchema &AggregationNode::schema() { return tuple_schema_; }

bool operator==(const Tuple &a, const Tuple &b) {
  if (a.size() != b.size())
    return false;

  for (int i = 0; i < a.size(); i++) {
    auto &v_a = a.get(i);
    auto &v_b = b.get(i);

    if (v_a.is_null() && v_b.is_null()) {
      continue;
    } else if (v_a.is_null() || v_b.is_null()) {
      return false;
    } else {
      if (v_a.compare(&v_b) != 0) {
        return false;
      }
    }
  }

  return true;
}

RC AggregationNode::execute(TupleSet &tuple_set) {
  // Reset child node
  child->reset();

  // Perpare aggregators
  typedef std::pair<std::shared_ptr<Tuple>,
                    std::vector<std::unique_ptr<Aggregator>>>
      key_to_aggs_t;
  std::vector<key_to_aggs_t> keys_to_aggs;
  auto get_aggors_of_key = [&keys_to_aggs, &aggs_template = aggregators](
                               std::unique_ptr<Tuple> key_tuple)
      -> std::vector<std::unique_ptr<Aggregator>> & {
    auto key_to_aggs_iter = std::find_if(
        keys_to_aggs.begin(), keys_to_aggs.end(),
        [&](const key_to_aggs_t &a) { return *a.first == *key_tuple; });
    if (key_to_aggs_iter == keys_to_aggs.end()) {
      keys_to_aggs.emplace_back(std::move(key_tuple),
                                std::vector<std::unique_ptr<Aggregator>>());
      auto &aggs = keys_to_aggs.back().second;
      for (auto &agg : aggs_template) {
        aggs.push_back(agg->clone());
      }

      return keys_to_aggs.back().second;
    }

    return key_to_aggs_iter->second;
  };

  // Iter and aggregate data
  Tuple buf;
  RC rc;
  while ((rc = child->next(buf)) != RC::RECORD_EOF) {
    auto &aggors = get_aggors_of_key(build_key_tuple(buf));

    for (size_t i = 0; i < aggors.size(); i++) {
      try {
        aggors[i]->add_tuple(buf);
      } catch (const std::exception &e) {
        LOG_ERROR(e.what());
        return RC::GENERIC_ERROR;
      }
    }
  }

  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
  }
  if (rc != RC::SUCCESS) {
    return rc;
  }

  // Write tuple set
  tuple_set.set_schema(schema());

  if (keys_to_aggs.size() == 0 && keys.size() == 0) {
    // If no group keys and no data, write default values

    Tuple tuple;
    for (auto &agg : aggregators) {
      tuple.add(agg->value());
    }
    tuple_set.add(std::move(tuple));
  } else {
    for (auto &key_to_aggs : keys_to_aggs) {
      auto &keys_tuple = key_to_aggs.first;
      auto &aggors = key_to_aggs.second;

      Tuple tuple(*keys_tuple);
      for (auto &agg : aggors) {
        tuple.add(agg->value());
      }
      tuple_set.add(std::move(tuple));
    }
  }

  return RC::SUCCESS;
}

std::unique_ptr<Tuple> AggregationNode::build_key_tuple(const Tuple &tuple) {
  auto key_tuple = std::make_unique<Tuple>();
  for (auto idx : keys) {
    key_tuple->add(tuple.get_pointer(idx));
  }

  return key_tuple;
}

Aggregator &AggregationNode::add_aggregator(const char *func) {
  char *name = strdup(func);
  for (char *n = name; *n != 0; n++)
    *n = tolower(*n);

  if (strcmp(name, "count") == 0) {
    aggregators.emplace_back(new CountAggregator());
  } else if (strcmp(name, "max") == 0) {
    aggregators.emplace_back(new MaxAggregator());
  } else if (strcmp(name, "min") == 0) {
    aggregators.emplace_back(new MinAggregator());
  } else if (strcmp(name, "avg") == 0) {
    aggregators.emplace_back(new AvgAggregator());
  } else {
    std::stringstream ss;
    ss << "Unsupport aggregation func: " << name;
    throw std::invalid_argument(ss.str());
  }

  return *aggregators.back();
}

void Aggregator::set_input_expr(SelectExpr *expr, const TupleSchema &schema) {
  if (expr->attribute != nullptr) {
    if (strcmp(expr->attribute->attribute_name, "*") == 0) {
      if (!support_any_column()) {
        throw std::invalid_argument("* is not support in this aggregator");
      }
      return;
    }
  }

  expression = std::move(create_expression(expr, schema));
  set_input_type(expression->type());
}

void Aggregator::add_tuple(const Tuple &tuple) {
  if (expression == nullptr) {
    add(nullptr);
    return;
  }

  auto value = expression->eval(tuple);
  add(value);
}

AttrType CountAggregator::out_type() { return INTS; }
void CountAggregator::add(std::shared_ptr<TupleValue> v) {
  if (v == nullptr) { // any column
    count++;
  } else if (!v->is_null()) {
    count++;
  }
}
std::shared_ptr<TupleValue> CountAggregator::value() {
  return std::make_shared<IntValue>(count);
}

void MaxAggregator::add(std::shared_ptr<TupleValue> v) {
  if (v->is_null()) {
    return;
  }

  if (max->is_null()) {
    max = v;
    return;
  }

  if (max->compare(v.get()) < 0) {
    max = v;
  }
}
std::shared_ptr<TupleValue> MaxAggregator::value() { return max; };

void MinAggregator::add(std::shared_ptr<TupleValue> v) {
  if (v->is_null()) {
    return;
  }

  if (min->is_null()) {
    min = v;
    return;
  }

  if (min->compare(v.get()) > 0) {
    min = v;
  }
}
std::shared_ptr<TupleValue> MinAggregator::value() { return min; };

void AvgAggregator::set_input_type(AttrType t) {
  if (!(t == INTS || t == FLOATS || t == DATE)) {
    std::stringstream ss;
    ss << "Unsupport type for avg: " << t;
    throw std::invalid_argument(ss.str());
  }

  this->in_type = t;
}
AttrType AvgAggregator::out_type() {
  if (in_type == INTS || in_type == FLOATS) {
    return FLOATS;
  } else if (in_type == DATE) {
    return DATE;
  }

  throw std::logic_error("Unreachable code");
}
void AvgAggregator::add(std::shared_ptr<TupleValue> v) {
  if (v->is_null()) {
    return;
  }

  float value;
  switch (this->in_type) {
  case INTS: {
    auto tv = (IntValue *)v.get();
    value = tv->value();

  } break;
  case FLOATS: {
    auto tv = (FloatValue *)v.get();
    value = tv->value();
  } break;
  case DATE: {
    auto tv = (DateValue *)v.get();
    value = tv->julian();
  } break;
  default:
    throw std::logic_error("Unreachable code");
  }

  this->avg = (this->avg * this->count + value) / (this->count + 1);
  this->count++;
}
std::shared_ptr<TupleValue> AvgAggregator::value() {
  if (count == 0) {
    return std::make_shared<NullValue>();
  }

  switch (out_type()) {
  case FLOATS: {
    auto tv = std::make_shared<FloatValue>(avg);
    return tv;
  } break;
  case DATE: {
    auto tv = std::make_shared<DateValue>(avg);
    return tv;
  } break;
  default:
    throw std::logic_error("Unreachable code");
  }
}