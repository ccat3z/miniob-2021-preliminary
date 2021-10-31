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
#include "filter.h"

CartesianSelectNode::CartesianSelectNode(
    std::unique_ptr<ExecutionNode> left_node,
    std::unique_ptr<ExecutionNode> right_node)
    : left_node(std::move(left_node)), right_node(std::move(right_node)) {
  tuple_schema_.append(this->left_node->schema());
  tuple_schema_.append(this->right_node->schema());
};
CartesianSelectNode::~CartesianSelectNode() {}
const TupleSchema &CartesianSelectNode::schema() { return tuple_schema_; };

RC CartesianSelectNode::execute(TupleSet &tuple_set) {
  TupleSet tuple_left, tuple_right;

  RC rc;
  if ((rc = left_node->execute(tuple_left)) != RC::SUCCESS) {
    return rc;
  }
  if ((rc = right_node->execute(tuple_right)) != RC::SUCCESS) {
    return rc;
  }

  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);
  for (auto &tuple_left : tuple_left.tuples()) {
    for (auto &tuple_right : tuple_right.tuples()) {
      Tuple tuple;
      for (auto &v : tuple_left.values()) {
        tuple.add(v);
      }
      for (auto &v : tuple_right.values()) {
        tuple.add(v);
      }

      tuple_set.add(std::move(tuple));
    }
  }

  return RC::SUCCESS;
};

std::unique_ptr<CartesianSelectNode> CartesianSelectNode::create(
    std::vector<std::unique_ptr<ExecutionNode>> &nodes) {
  if (nodes.size() < 2) {
    throw std::invalid_argument("At least 2 nodes");
  }

  std::unique_ptr<CartesianSelectNode> root(
      new CartesianSelectNode(std::move(nodes[0]), std::move(nodes[1])));

  auto it = nodes.begin();
  std::advance(it, 2);
  for (; it != nodes.end(); it++) {
    root = std::unique_ptr<CartesianSelectNode>(
        new CartesianSelectNode(std::move(root), std::move(*it)));
  }

  return root;
}

void extract_condition(Condition &condition, std::list<Condition *> &both,
                       const TupleSchema &lschema,
                       std::list<Condition *> &left_only,
                       const TupleSchema &rschema,
                       std::list<Condition *> &right_only) {
  bool lschema_involved = false, rschema_involved = false;
  if (condition.left_is_attr) {
    lschema_involved =
        lschema_involved ||
        lschema.index_of_field(condition.left_attr.relation_name,
                               condition.left_attr.attribute_name) >= 0;
    rschema_involved =
        rschema_involved ||
        rschema.index_of_field(condition.left_attr.relation_name,
                               condition.left_attr.attribute_name) >= 0;
  }

  if (condition.right_is_attr) {
    lschema_involved =
        lschema_involved ||
        lschema.index_of_field(condition.right_attr.relation_name,
                               condition.right_attr.attribute_name) >= 0;
    rschema_involved =
        rschema_involved ||
        rschema.index_of_field(condition.right_attr.relation_name,
                               condition.right_attr.attribute_name) >= 0;
  }

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
      left_node = std::make_unique<FilterNode>(std::move(left_node),
                                               left.begin(), left.end());
    }
  }

  if (right.size() > 0) {
    auto new_right = right_node->push_down_predicate(right);
    if (new_right != nullptr) {
      right_node = std::move(new_right);
    }
    if (right.size() > 0) {
      right_node = std::make_unique<FilterNode>(std::move(right_node),
                                                right.begin(), right.end());
    }
  }

  predicate.splice(predicate.end(), both);
  return nullptr;
}