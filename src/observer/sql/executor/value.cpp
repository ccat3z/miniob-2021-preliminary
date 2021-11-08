/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "value.h"

void DateValue::to_string(std::ostream &os) const {
  if (is_null()) {
    os << "NULL";
    return;
  }

  os << date.format();
}
int DateValue::compare(const TupleValue *other) const {
  if (is_null() || other->is_null()) {
    throw std::invalid_argument("Null is incompareable");
  }

  if (auto date_other = dynamic_cast<const DateValue *>(other)) {
    return date.julian() - date_other->date.julian();
  } else {
    throw std::logic_error("Uncomparable values");
  }
}

TupleValue *TupleValue::from_value(Value &value, AttrType type, bool must) {
  if (type != UNDEFINED) {
    if (!value_cast(&value, type) && must) {
      std::stringstream msg;
      msg << "Cannot convert value's type from " << value.type << " to "
          << type;
      throw std::invalid_argument(msg.str());
    }
  }

  TupleValue *res;
  switch (value.type) {
  case INTS:
    res = new IntValue(*((int *)value.data));
    break;
  case FLOATS:
    res = new FloatValue(*((float *)value.data));
    break;
  case CHARS: {
    auto s = (char *)value.data;
    res = new StringValue(s, strlen(s));
    break;
  }
  case DATE:
    res = new DateValue(*((int *)value.data));
    break;
  case TYPE_NULL:
    res = new NullValue();
    break;
  default:
    std::stringstream msg;
    msg << "Unsupport value type " << value.type;
    throw std::invalid_argument(msg.str());
  }

  res->set_null(value.is_null);

  return res;
}

bool TupleValue::is_null() const { return null; }
void TupleValue::set_null(bool null) { this->null = null; }

int IntValue::compare(const TupleValue *other) const {
  if (is_null() || other->is_null()) {
    throw std::invalid_argument("Null is incompareable");
  }

  if (auto int_value = dynamic_cast<const IntValue *>(other)) {
    return value_ - int_value->value();
  } else if (auto float_value = dynamic_cast<const FloatValue *>(other)) {
    float result = value_ - float_value->value();

    if (-0.001 < result && result < 0.001) {
      return 0;
    }

    if (result > 0) {
      return 1;
    }
    if (result < 0) {
      return -1;
    }

    throw std::logic_error("Unreachable code: 97");
  }

  throw std::logic_error("Uncomparable values");
}

int FloatValue::compare(const TupleValue *other) const {
  if (is_null() || other->is_null()) {
    throw std::invalid_argument("Null is incompareable");
  }

  float result;
  if (auto int_value = dynamic_cast<const IntValue *>(other)) {
    result = value_ - int_value->value();
  } else if (auto float_value = dynamic_cast<const FloatValue *>(other)) {
    result = value_ - float_value->value();
  } else {
    throw std::logic_error("Uncomparable values");
  }

  if (-0.001 < result && result < 0.001) {
    return 0;
  }

  if (result > 0) {
    return 1;
  }
  if (result < 0) {
    return -1;
  }

  throw std::logic_error("Unreachable code: 128");
}

int StringValue::compare(const TupleValue *other) const {
  if (is_null() || other->is_null()) {
    throw std::invalid_argument("Null is incompareable");
  }

  if (auto string_other = dynamic_cast<const StringValue *>(other)) {
    return strcmp(value_.c_str(), string_other->value_.c_str());
  } else {
    throw std::logic_error("Uncomparable values");
  }
}
