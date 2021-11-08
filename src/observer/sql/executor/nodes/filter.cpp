// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "filter.h"
#include "common/log/log.h"
#include <algorithm>

FilterNode::FilterNode(Session *session, std::unique_ptr<ExecutionNode> child,
                       Condition *begin, Condition *end)
    : session(session), child(std::move(child)) {
  tuple_schema_ = this->child->schema();
  for (; begin != end; begin++) {
    conditions.push_back(begin);
  }

  build_filters();
}

FilterNode::~FilterNode() {}
const TupleSchema &FilterNode::schema() { return tuple_schema_; };

void FilterNode::reset() { child->reset(); }

RC FilterNode::next(Tuple &tuple) {
  RC rc;
  while ((rc = child->next(tuple)) == RC::SUCCESS) {
    bool add = true;
    for (auto &filter : filters) {
      try {
        add = filter->filter(tuple);
      } catch (const std::exception &e) {
        LOG_WARN("Filter failed: %s", e.what());
        add = false;
      }

      if (!add)
        break;
    }

    if (add) {
      return RC::SUCCESS;
    }
  }

  return rc;
}

void FilterNode::build_filters() {
  filters.clear();
  for (auto &cond : conditions) {
    auto filter =
        std::make_unique<TupleFilter>(session, child->schema(), *cond);
    filters.push_back(std::move(filter));
  }
}

std::unique_ptr<ExecutionNode>
FilterNode::push_down_predicate(std::list<Condition *> &predicate) {
  conditions.splice(conditions.end(), predicate);
  auto new_child = child->push_down_predicate(conditions);
  if (new_child != nullptr) {
    child = std::move(new_child);
  }

  if (conditions.size() == 0)
    return std::move(child);

  build_filters();
  return nullptr;
}

inline bool try_to_match_values_type(Value &a, Value &b) {
  return value_cast(&a, b.type) || value_cast(&b, a.type);
}

TupleFilter::TupleFilter(Session *session, const TupleSchema &schema,
                         Condition &condition) {
  op = condition.comp;

  // Handle value expressions as far back as possible
  // since the AttrType of the value expression must be determined by
  // the expression on the other side.
  if (condition.left_expr.type == COND_EXPR_VALUE &&
      condition.right_expr.type == COND_EXPR_VALUE) {
    try_to_match_values_type(condition.left_expr.value.value,
                             condition.right_expr.value.value);
  }
  if (condition.left_expr.type != COND_EXPR_VALUE) {
    left = create_expression(session, &condition.left_expr, schema);
  }
  right = create_expression(session, &condition.right_expr, schema,
                            left == nullptr ? UNDEFINED : left->type());
  if (condition.left_expr.type == COND_EXPR_VALUE) {
    left =
        create_expression(session, &condition.left_expr, schema, right->type());
  }

  // TODO: Comparable table is hard code for now. Needs to be refactored. It may be possible to use an undirected graph to represent comparable and comparative methods.
  auto ltype = left->type(), rtype = right->type();
  if (ltype != rtype) {
    if (!((ltype == INTS && rtype == FLOATS) ||
          (ltype == FLOATS && rtype == INTS))) {
      throw std::invalid_argument("Incompareble fields");
    }
  }
}
TupleFilter::~TupleFilter() {}

bool TupleFilter::filter(const Tuple &tuple) const {
  if (EQUAL_TO <= op && op <= GREAT_THAN) {
    int cmp = left->eval(tuple)->compare(right->eval(tuple).get());

    switch (op) {
    case EQUAL_TO:
      return cmp == 0;
    case LESS_EQUAL:
      return cmp <= 0;
    case NOT_EQUAL:
      return cmp != 0;
    case LESS_THAN:
      return cmp < 0;
    case GREAT_EQUAL:
      return cmp >= 0;
    case GREAT_THAN:
      return cmp > 0;
    default:
      throw std::logic_error("Unreachable code: 127");
    }
  }

  if (op == IN_SET || op == NOT_IN_SET) {
    auto lvalue = left->eval(tuple);
    std::vector<std::shared_ptr<TupleValue>> rvalues;
    right->evals(rvalues, tuple);
    auto found = std::find_if(rvalues.begin(), rvalues.end(),
                              [&](std::shared_ptr<TupleValue> &rvalue) {
                                return lvalue->compare(rvalue.get()) == 0;
                              }) != rvalues.end();

    if (op == IN_SET) {
      return found;
    } else {
      return !found;
    }
  }

  if (op == IS_NULL || op == IS_NOT_NULL) {
    bool is_null = left->eval(tuple)->is_null();
    if (op == IS_NULL) {
      return is_null;
    } else {
      return !is_null;
    }
  }

  LOG_ERROR("Unsupport CompOp: %d", op);
  return false;
}