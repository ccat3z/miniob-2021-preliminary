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
#include <algorithm>

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

      bool skip = false;
      for (auto &filter : filters) {
        if (!filter->filter(tuple)) {
          skip = true;
          break;
        }
      }

      if (!skip)
        tuple_set.add(std::move(tuple));
    }
  }

  return RC::SUCCESS;
};

std::unique_ptr<CartesianSelectNode>
CartesianSelectNode::create(std::vector<std::unique_ptr<ExecutionNode>> &nodes,
                            std::vector<Condition *> &conditions) {
  if (nodes.size() < 2)
    return nullptr;

  std::unique_ptr<CartesianSelectNode> root(
      new CartesianSelectNode(std::move(nodes[0]), std::move(nodes[1])));
  root->extract_bridge_conditions(conditions);

  auto it = nodes.begin();
  std::advance(it, 2);
  for (; it != nodes.end(); it++) {
    root = std::unique_ptr<CartesianSelectNode>(
        new CartesianSelectNode(std::move(root), std::move(*it)));
    root->extract_bridge_conditions(conditions);
  }

  return root;
}

bool is_condition_bridge_schemas(Condition &condition,
                                 const TupleSchema &lschema,
                                 const TupleSchema &rschema) {
  if (!condition.left_is_attr || !condition.right_is_attr) {
    return false;
  }

  bool lschema_has_lattr =
      lschema.index_of_field(condition.left_attr.relation_name,
                             condition.left_attr.attribute_name) >= 0;
  bool lschema_has_rattr =
      lschema.index_of_field(condition.right_attr.relation_name,
                             condition.right_attr.attribute_name) >= 0;
  bool rschema_has_lattr =
      rschema.index_of_field(condition.left_attr.relation_name,
                             condition.left_attr.attribute_name) >= 0;
  bool rschema_has_rattr =
      rschema.index_of_field(condition.right_attr.relation_name,
                             condition.right_attr.attribute_name) >= 0;

  return ((lschema_has_lattr && !lschema_has_rattr) &&
          (!rschema_has_lattr && rschema_has_rattr)) ||
         ((!lschema_has_lattr && lschema_has_rattr) &&
          (rschema_has_lattr && !rschema_has_rattr));
}

void CartesianSelectNode::extract_bridge_conditions(
    std::vector<Condition *> &conditions) {
  for (auto it = conditions.begin(); it != conditions.end();) {
    if (is_condition_bridge_schemas(**it, left_node->schema(),
                                    right_node->schema())) {
      auto filter = DefaultTupleFilter::create(schema(), **it);
      if (filter == nullptr) {
        LOG_WARN(
            "Cannot create tuple filter for cartesian select, just ignore");
      } else {
        filters.push_back(std::move(filter));
        it = conditions.erase(it);
        continue;
      }
    }
    it++;
  }
}