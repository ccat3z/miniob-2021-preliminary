// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "base.h"

RC SetExecutionNode::next(Tuple &tuple) {
  if (next_record_idx == -1) {
    RC rc = execute(tuple_set_buf);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    next_record_idx = 0;
  }

  if (next_record_idx == tuple_set_buf.size()) {
    return RC::RECORD_EOF;
  }

  tuple = tuple_set_buf.get(next_record_idx++);

  return RC::SUCCESS;
}

void SetExecutionNode::reset() {
  next_record_idx = -1;
  tuple_set_buf.clear();
}

RC VolcanoExecutionNode::execute(TupleSet &tuple_set) {
  tuple_set.set_schema(schema());
  Tuple tuple;
  RC rc;

  reset();
  while ((rc = next(tuple)) != RC::RECORD_EOF) {
    if (rc != RC::SUCCESS) {
      return rc;
    }

    tuple_set.add(std::move(tuple));
  }

  return RC::SUCCESS;
}

std::unique_ptr<ExecutionNode> ExecutionNode::push_down_predicate() {
  std::list<Condition *> v;
  return push_down_predicate(v);
}

std::unique_ptr<ExecutionNode>
ExecutionNode::push_down_predicate(std::list<Condition *> &candidate) {
  return nullptr;
}