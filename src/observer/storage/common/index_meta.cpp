/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_UNIQUE("unique");

RC IndexMeta::init(const char *name,
                   const std::vector<const FieldMeta *> &fields, bool unique) {
  if (nullptr == name || common::is_blank(name)) {
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  for (auto &fm : fields) {
    fields_.emplace_back(fm->name());
  }
  unique_ = unique;
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const {
  json_value[FIELD_NAME] = name_;
  for (auto field : fields_) {
    json_value[FIELD_FIELD_NAME].append(field);
  }
  json_value[FIELD_UNIQUE] = unique_;
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value,
                        IndexMeta &index) {
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &fields_value = json_value[FIELD_FIELD_NAME];
  const Json::Value &unique_value = json_value[FIELD_UNIQUE];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s",
              name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  if (!fields_value.isArray()) {
    LOG_ERROR("Field name of index [%s] is not a valid array. json value=%s",
              name_value.asCString(), fields_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  if (!unique_value.isBool()) {
    LOG_ERROR("`unique` is not a bool. json value=%s", name_value.asCString(),
              unique_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  std::vector<const FieldMeta *> fields;
  for (int i = 0; i < fields_value.size(); i++) {
    auto field_name = fields_value[i].asCString();
    auto field = table.field(field_name);
    if (field == nullptr) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s",
                name_value.asCString(), field_name);
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    fields.push_back(field);
  }

  if (fields.size() == 0) {
    LOG_ERROR("Deserialize index [%s]: fields is empty",
              name_value.asCString());
    return RC::SCHEMA_FIELD_MISSING;
  }

  return index.init(name_value.asCString(), fields, unique_value.asBool());
}

const char *IndexMeta::name() const { return name_.c_str(); }

const std::vector<std::string> &IndexMeta::fields() const { return fields_; }

bool IndexMeta::unique() const { return unique_; };

void IndexMeta::desc(std::ostream &os) const {
  os << "index name=" << name_ << ", fields=";
  for (auto field : fields_) {
    os << field << ',';
  }

  os << " unique=" << (unique_ ? "yes" : "no");
}