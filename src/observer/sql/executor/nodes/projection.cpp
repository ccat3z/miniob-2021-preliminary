// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "projection.h"
#include "common/log/log.h"

std::unique_ptr<ProjectionNode>
ProjectionNode::create(std::unique_ptr<ExecutionNode> child, RelAttr *attrs,
                       int attr_num) {
  auto proj =
      std::unique_ptr<ProjectionNode>(new ProjectionNode(std::move(child)));
  proj->fields_map.reserve(attr_num);

  auto &child_fields = proj->child->schema().fields();

  for (int i = attr_num - 1; i >= 0; i--) {
    auto &attr = attrs[i];
    bool any_table = strcmp(attr.relation_name, "*") == 0;
    bool any_attr = strcmp(attr.attribute_name, "*") == 0;
    bool matched = false;
    for (size_t j = 0; j < child_fields.size(); j++) {
      auto &child_field = child_fields[j];
      if ((any_attr ||
           strcmp(attr.attribute_name, child_field.field_name()) == 0) &&
          (any_table ||
           strcmp(attr.relation_name, child_field.table_name()) == 0)) {
        matched = true;
        proj->fields_map.push_back(j);
        proj->tuple_schema_.add(child_field.type(), child_field.table_name(),
                                child_field.field_name());

        if (!(any_table || any_attr)) {
          break;
        }
      }
    }
    if (!matched) {
      LOG_ERROR("Failed to find field %s.%s in child node", attr.relation_name,
                attr.attribute_name);
      return nullptr;
    }
  }

  return proj;
}

ProjectionNode::~ProjectionNode() {}
const TupleSchema &ProjectionNode::schema() { return tuple_schema_; }

RC ProjectionNode::execute(TupleSet &tuple_set) {
  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);

  TupleSet buf;
  RC rc = child->execute(buf);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  for (auto &v : buf.tuples()) {
    Tuple tuple;
    for (auto &i : fields_map) {
      tuple.add(v.values()[i]);
    }
    tuple_set.add(std::move(tuple));
  }

  return RC::SUCCESS;
}