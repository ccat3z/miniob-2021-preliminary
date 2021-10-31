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

FilterNode::~FilterNode() {}
const TupleSchema &FilterNode::schema() { return tuple_schema_; };

void FilterNode::reset() { child->reset(); }

RC FilterNode::next(Tuple &tuple) {
  RC rc;
  while ((rc = child->next(tuple)) == RC::SUCCESS) {
    bool add = true;
    for (auto &filter : filters) {
      if (!filter->filter(tuple)) {
        add = false;
        break;
      }
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
    auto filter = std::make_unique<DefaultTupleFilter>(child->schema(), *cond);
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