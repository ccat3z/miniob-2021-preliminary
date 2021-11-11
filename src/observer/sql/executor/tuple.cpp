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
// Created by Wangyunlai on 2021/5/14.
//

#include "sql/executor/tuple.h"
#include "common/log/log.h"
#include "common/time/datetime.h"
#include "storage/common/table.h"

Tuple::Tuple(const Tuple &other) {
  for (auto &v : other.values()) {
    add(v);
  }
}
Tuple &Tuple::operator=(const Tuple &other) {
  values_.clear();
  for (auto &v : other.values()) {
    add(v);
  }
  return *this;
}

Tuple::Tuple(Tuple &&other) noexcept : values_(std::move(other.values_)) {}

Tuple &Tuple::operator=(Tuple &&other) noexcept {
  if (&other == this) {
    return *this;
  }

  values_.clear();
  values_.swap(other.values_);
  return *this;
}

Tuple::~Tuple() {}

// add (Value && value)
void Tuple::add(TupleValue *value) { values_.emplace_back(value); }
void Tuple::add(const std::shared_ptr<TupleValue> &other) {
  values_.emplace_back(other);
}
void Tuple::add(int value) { add(new IntValue(value)); }

void Tuple::add(float value) { add(new FloatValue(value)); }

void Tuple::add(const char *s, int len) { add(new StringValue(s, len)); }

////////////////////////////////////////////////////////////////////////////////

std::string TupleField::to_string() const {
  return std::string(table_name_) + "." + field_name_ + std::to_string(type_);
}

////////////////////////////////////////////////////////////////////////////////
void TupleSchema::add_field_from_table(const Table *table) {
  const char *table_name = table->name();
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = 0; i < field_num; i++) {
    const FieldMeta *field_meta = table_meta.field(i);
    if (field_meta->visible()) {
      add(field_meta->type(), table_name, field_meta->name());
    }
  }
}

RC TupleSchema::add_field_from_table(const Table *table,
                                     const char *field_name) {
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  add_if_not_exists(field_meta->type(), table->name(), field_meta->name());
  return RC::SUCCESS;
}

void TupleSchema::add(AttrType type, const char *table_name,
                      const char *field_name) {
  fields_.emplace_back(type, table_name, field_name);
}

void TupleSchema::add_if_not_exists(AttrType type, const char *table_name,
                                    const char *field_name) {
  for (const auto &field : fields_) {
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name)) {
      return;
    }
  }

  add(type, table_name, field_name);
}

void TupleSchema::append(const TupleSchema &other) {
  fields_.reserve(fields_.size() + other.fields_.size());
  for (const auto &field : other.fields_) {
    fields_.emplace_back(field);
  }
}

int TupleSchema::index_of_field(const char *table_name,
                                const char *field_name) const {
  const int size = fields_.size();
  for (int i = 0; i < size; i++) {
    const TupleField &field = fields_[i];
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name)) {
      return i;
    }
  }
  return -1;
}

void TupleSchema::print(std::ostream &os, bool show_table) const {
  if (fields_.empty()) {
    os << "No schema";
    return;
  }

  for (std::vector<TupleField>::const_iterator iter = fields_.begin(),
                                               end = --fields_.end();
       iter != end; ++iter) {
    if (show_table && iter->table_name()[0] != '\0') {
      os << iter->table_name() << ".";
    }
    os << iter->field_name() << " | ";
  }

  if (show_table && fields_.back().table_name()[0] != '\0') {
    os << fields_.back().table_name() << ".";
  }
  os << fields_.back().field_name() << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
TupleSet::TupleSet(TupleSet &&other)
    : tuples_(std::move(other.tuples_)), schema_(other.schema_) {
  other.schema_.clear();
}

TupleSet &TupleSet::operator=(TupleSet &&other) {
  if (this == &other) {
    return *this;
  }

  schema_.clear();
  schema_.append(other.schema_);
  other.schema_.clear();

  tuples_.clear();
  tuples_.swap(other.tuples_);
  return *this;
}

void TupleSet::add(Tuple &&tuple) { tuples_.emplace_back(std::move(tuple)); }

void TupleSet::clear() {
  tuples_.clear();
  schema_.clear();
}

void TupleSet::print(std::ostream &os, bool show_table) const {
  if (schema_.fields().empty()) {
    LOG_WARN("Got empty schema");
    return;
  }

  schema_.print(os, show_table);

  for (const Tuple &item : tuples_) {
    const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
    for (std::vector<std::shared_ptr<TupleValue>>::const_iterator
             iter = values.begin(),
             end = --values.end();
         iter != end; ++iter) {
      (*iter)->to_string(os);
      os << " | ";
    }
    values.back()->to_string(os);
    os << std::endl;
  }
}

void TupleSet::set_schema(const TupleSchema &schema) { schema_ = schema; }

const TupleSchema &TupleSet::get_schema() const { return schema_; }

bool TupleSet::is_empty() const { return tuples_.empty(); }

int TupleSet::size() const { return tuples_.size(); }

const Tuple &TupleSet::get(int index) const { return tuples_[index]; }

const std::vector<Tuple> &TupleSet::tuples() const { return tuples_; }

/////////////////////////////////////////////////////////////////////////////
TupleRecordConverter::TupleRecordConverter(Table *table, TupleSet &tuple_set)
    : table_(table), tuple_set_(tuple_set) {}

void TupleRecordConverter::add_record(const char *record) {
  const TupleSchema &schema = tuple_set_.schema();
  Tuple tuple;
  const TableMeta &table_meta = table_->table_meta();
  uint32_t null = *(uint32_t *)(record + table_meta.null_field()->offset());
  for (const TupleField &field : schema.fields()) {
    int field_idx = -1;
    for (int i = 0; i < table_meta.field_num(); i++) {
      if (strcmp(table_meta.field(i)->name(), field.field_name()) == 0) {
        field_idx = i;
        break;
      }
    }
    assert(field_idx != -1);
    const FieldMeta *field_meta = table_meta.field(field_idx);

    switch (field_meta->type()) {
    case INTS: {
      int value = *(int *)(record + field_meta->offset());
      tuple.add(value);
    } break;
    case FLOATS: {
      float value = *(float *)(record + field_meta->offset());
      tuple.add(value);
    } break;
    case CHARS: {
      const char *s = record + field_meta->offset(); // 现在当做Cstring来处理
      tuple.add(s, strlen(s));
    } break;
    case DATE:
      tuple.add(new DateValue(*(int *)(record + field_meta->offset())));
      break;
    case TEXT: {
      auto data =
          table_->get_block(*(uint32_t *)(record + field_meta->offset()));
      const char *s = data->data;
      tuple.add(new StringValue(s, strlen(s)));
      break;
    }
    default: {
      LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
    }
    }

    if ((null & ((uint32_t)1 << field_idx)) == ((uint32_t)1 << field_idx)) {
      tuple.values().back()->set_null(true);
    }
  }

  tuple_set_.add(std::move(tuple));
}

void Tuple::clear() { values_.clear(); }