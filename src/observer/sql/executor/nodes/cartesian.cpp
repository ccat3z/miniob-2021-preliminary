// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "cartesian.h"
#include "common/log/log.h"
#include "filter.h"

CartesianSelectNode::CartesianSelectNode(
    Session *session, std::unique_ptr<ExecutionNode> left_node,
    std::unique_ptr<ExecutionNode> right_node)
    : left_node(std::move(left_node)), right_node(std::move(right_node)),
      session(session) {
  tuple_schema_.append(this->left_node->schema());
  tuple_schema_.append(this->right_node->schema());
};
CartesianSelectNode::~CartesianSelectNode() {}
const TupleSchema &CartesianSelectNode::schema() { return tuple_schema_; };

RC CartesianSelectNode::next(Tuple &tuple) {
  if (require_next_left) {
    RC rc = left_node->next(tuple_left);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    require_next_left = false;
  }

  Tuple tuple_right;
  RC rc = right_node->next(tuple_right);
  if (rc == RC::RECORD_EOF) {
    require_next_left = true;
    right_node->reset();
    return next(tuple);
  } else if (rc != RC::SUCCESS) {
    return rc;
  }

  tuple.clear();
  for (auto &v : tuple_left.values()) {
    tuple.add(v);
  }
  for (auto &v : tuple_right.values()) {
    tuple.add(v);
  }

  return RC::SUCCESS;
}

void CartesianSelectNode::reset() {
  require_next_left = true;
  left_node->reset();
  right_node->reset();
}

std::unique_ptr<CartesianSelectNode> CartesianSelectNode::create(
    Session *session, std::vector<std::unique_ptr<ExecutionNode>> &nodes) {
  if (nodes.size() < 2) {
    throw std::invalid_argument("At least 2 nodes");
  }

  std::unique_ptr<CartesianSelectNode> root(new CartesianSelectNode(
      session, std::move(nodes[0]), std::move(nodes[1])));

  auto it = nodes.begin();
  std::advance(it, 2);
  for (; it != nodes.end(); it++) {
    root = std::unique_ptr<CartesianSelectNode>(
        new CartesianSelectNode(session, std::move(root), std::move(*it)));
  }

  return root;
}

bool is_expr_involved_schema(ConditionExpr &expr, const TupleSchema &schema) {
  switch (expr.type) {
  case COND_EXPR_VALUE:
    return false;
  case COND_EXPR_ATTR:
    return schema.index_of_field(expr.value.attr.relation_name,
                                 expr.value.attr.attribute_name) >= 0;
  case COND_EXPR_SELECT:
  case COND_EXPR_CALC:
  default:
    LOG_ERROR("Cannot determine if it is involved");
    return true;
  }
}

void extract_condition(Condition &condition, std::list<Condition *> &both,
                       const TupleSchema &lschema,
                       std::list<Condition *> &left_only,
                       const TupleSchema &rschema,
                       std::list<Condition *> &right_only) {
  bool lschema_involved =
           is_expr_involved_schema(condition.left_expr, lschema) ||
           is_expr_involved_schema(condition.right_expr, lschema),
       rschema_involved =
           is_expr_involved_schema(condition.left_expr, rschema) ||
           is_expr_involved_schema(condition.right_expr, rschema);

  if (!lschema_involved && !rschema_involved) {
    left_only.push_back(&condition);
    right_only.push_back(&condition);
  } else if (lschema_involved && !rschema_involved) {
    left_only.push_back(&condition);
  } else if (!lschema_involved && rschema_involved) {
    right_only.push_back(&condition);
  } else {
    both.push_back(&condition);
  }
}

std::unique_ptr<ExecutionNode>
CartesianSelectNode::push_down_predicate(std::list<Condition *> &predicate) {
  std::list<Condition *> both, left, right;

  for (auto &condition : predicate) {
    extract_condition(*condition, both, left_node->schema(), left,
                      right_node->schema(), right);
  }
  predicate.clear();

  if (left.size() > 0) {
    auto new_left = left_node->push_down_predicate(left);
    if (new_left != nullptr) {
      left_node = std::move(new_left);
    }
    if (left.size() > 0) {
      left_node = std::make_unique<FilterNode>(session, std::move(left_node),
                                               left.begin(), left.end());
    }
  }

  if (right.size() > 0) {
    auto new_right = right_node->push_down_predicate(right);
    if (new_right != nullptr) {
      right_node = std::move(new_right);
    }
    if (right.size() > 0) {
      right_node = std::make_unique<FilterNode>(session, std::move(right_node),
                                                right.begin(), right.end());
    }
  }

  predicate.splice(predicate.end(), both);
  return nullptr;
}