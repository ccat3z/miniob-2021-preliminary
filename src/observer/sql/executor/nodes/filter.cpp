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

FilterNode::FilterNode(std::unique_ptr<ExecutionNode> child,
                       std::vector<std::unique_ptr<TupleFilter>> &&filters)
    : child(std::move(child)), filters(std::move(filters)) {
  tuple_schema_.append(this->child->schema());
}
FilterNode::~FilterNode() {}
const TupleSchema &FilterNode::schema() { return tuple_schema_; };
RC FilterNode::execute(TupleSet &tuple_set) {
  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);

  TupleSet buf;
  RC rc = child->execute(buf);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  for (auto &tuple : buf.tuples()) {
    bool add = true;
    for (auto &filter : filters) {
      if (!filter->filter(tuple)) {
        add = false;
        break;
      }
    }

    if (add) {
      Tuple t(tuple);
      tuple_set.add(std::move(t));
    }
  }

  return RC::SUCCESS;
}

std::unique_ptr<FilterNode>
FilterNode::create(std::unique_ptr<ExecutionNode> child,
                   std::vector<Condition *> &conditions) {
  std::vector<std::unique_ptr<TupleFilter>> filters;
  for (auto &cond : conditions) {
    auto filter = DefaultTupleFilter::create(child->schema(), *cond);
    if (filter == nullptr) {
      return nullptr;
    }

    filters.push_back(std::move(filter));
  }

  return std::unique_ptr<FilterNode>(
      new FilterNode(std::move(child), std::move(filters)));
}