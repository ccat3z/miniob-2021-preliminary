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
#include "common/log/log.h"

AggregationNode::AggregationNode(std::unique_ptr<ExecutionNode> child,
                                 SelectExpr *exprs, int expr_num) {
  for (int i = 0; i < expr_num; i++) {
    auto &expr = exprs[i];
    if (expr.attribute != nullptr) {
      throw std::invalid_argument("Group aggregation is not support yet");
    }

    auto &agg = expr.agg;
    auto &aggor = add_aggregator(agg->agg_func);

    if (agg->value != nullptr) {
      aggor.set_input_type(agg->value->type);

      values.emplace_back(TupleValue::from_value(*agg->value));
      fields_map.push_back(-values.size());
    } else if (agg->attr != nullptr) {
      int field_idx;
      if (strcmp(agg->attr->attribute_name, "*") == 0) {
        if (!aggor.support_any_column()) {
          std::stringstream ss;
          ss << "* is not support in agg func: " << agg->agg_func;
          throw std::invalid_argument(ss.str());
        }
        field_idx = 0;
      } else {
        field_idx = child->schema().index_of_field(agg->attr->relation_name,
                                                   agg->attr->attribute_name);

        if (field_idx < 0) {
          throw std::invalid_argument("cannot find field for aggregation func");
        }

        aggor.set_input_type(child->schema().field(field_idx).type());
        field_idx++;
      }

      fields_map.push_back(field_idx);
    } else {
      throw std::invalid_argument("non value or attr for aggregation func");
    }

    tuple_schema_.add(aggor.out_type(), "", agg->name);
  }

  this->child = std::move(child);
}

AggregationNode::~AggregationNode() {}
const TupleSchema &AggregationNode::schema() { return tuple_schema_; }

RC AggregationNode::next(Tuple &tuple) {
  if (end) {
    return RC::RECORD_EOF;
  }

  Tuple buf;
  RC rc;
  while ((rc = child->next(buf)) != RC::RECORD_EOF) {
    if (rc != RC::SUCCESS) {
      return rc;
    }

    for (size_t i = 0; i < tuple_schema_.fields().size(); i++) {
      try {
        int idx = fields_map[i];
        if (idx < 0) {
          aggregators[i]->add(values[-idx - 1]);
        } else if (idx == 0) {
          aggregators[i]->add(nullptr);
        } else {
          aggregators[i]->add(buf.get_pointer(idx - 1));
        }
      } catch (const std::exception &e) {
        LOG_ERROR(e.what());
        return RC::GENERIC_ERROR;
      }
    }
  }

  for (auto &agg : aggregators) {
    tuple.add(agg->value());
  }

  end = true;
  return RC::SUCCESS;
}

void AggregationNode::reset() {
  end = false;
  child->reset();
  for (auto &agg : aggregators) {
    agg->reset();
  }
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