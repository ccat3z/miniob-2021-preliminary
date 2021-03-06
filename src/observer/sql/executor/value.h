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

#ifndef __OBSERVER_SQL_EXECUTOR_VALUE_H_
#define __OBSERVER_SQL_EXECUTOR_VALUE_H_

#include <string.h>

#include "common/time/datetime.h"
#include "sql/parser/parse.h"
#include <ostream>
#include <string>

class TupleValue {
public:
  TupleValue() = default;
  virtual ~TupleValue() = default;

  virtual void to_string(std::ostream &os) const = 0;
  virtual int compare(const TupleValue *other) const = 0;
  // TODO: Comparable table is hard code for now. Needs to be refactored. It may be possible to use an undirected graph to represent comparable and comparative methods.
  static TupleValue *from_value(Value &value, AttrType type = UNDEFINED,
                                bool must = true);

  virtual bool is_null() const;
  virtual void set_null(bool null);

private:
  bool null = false;
};

class IntValue : public TupleValue {
public:
  explicit IntValue(int value) : value_(value) {}

  void to_string(std::ostream &os) const override {
    if (is_null()) {
      os << "NULL";
      return;
    }

    os << value_;
  }

  int compare(const TupleValue *other) const override;

  int value() const { return value_; }

private:
  int value_;
};

class FloatValue : public TupleValue {
public:
  explicit FloatValue(float value) : value_(value) {}

  void to_string(std::ostream &os) const override {
    if (is_null()) {
      os << "NULL";
      return;
    }

    char buf[100];
    char *end = buf + snprintf(buf, 100, "%.2f", value_) - 1;

    while (*end == '0')
      end--;
    if (*end == '.')
      end--;
    *(end + 1) = 0;
    os << buf;
  }

  int compare(const TupleValue *other) const override;

  float value() const { return value_; }

private:
  float value_;
};

class StringValue : public TupleValue {
public:
  StringValue(const char *value, int len) : value_(value, len) {}
  explicit StringValue(const char *value) : value_(value) {}

  void to_string(std::ostream &os) const override {
    if (is_null()) {
      os << "NULL";
      return;
    }
    os << value_;
  }

  int compare(const TupleValue *other) const override;

private:
  std::string value_;
};

class DateValue : public TupleValue {
public:
  explicit DateValue(int julian) : date(julian){};
  void to_string(std::ostream &os) const override;
  int compare(const TupleValue *other) const override;

  int julian() { return date.julian(); }

private:
  const common::Date date;
};

class NullValue : public TupleValue {
public:
  NullValue() = default;
  void to_string(std::ostream &os) const override { os << "NULL"; }
  int compare(const TupleValue *other) const override {
    throw std::invalid_argument("Null is incompareable");
  }
  bool is_null() const override { return true; }
};

#endif //__OBSERVER_SQL_EXECUTOR_VALUE_H_
