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

void DateValue::to_string(std::ostream &os) const { os << date.format(); }
int DateValue::compare(const TupleValue &other) const {
  const DateValue &date_other = (const DateValue &)other;
  return date.julian() - date_other.date.julian();
}

TupleValue *TupleValue::from_value(Value &value, AttrType type) {
  if (type != UNDEFINED) {
    if (!value_cast(&value, type)) {
      std::stringstream msg;
      msg << "Cannot convert value's type from " << value.type << " to "
          << type;
      throw std::invalid_argument(msg.str());
    }
  }

  switch (value.type) {
  case INTS:
    return new IntValue(*((int *)value.data));
    break;
  case FLOATS:
    return new FloatValue(*((float *)value.data));
    break;
  case CHARS: {
    auto s = (char *)value.data;
    return new StringValue(s, strlen(s));
  }
  case DATE:
    return new DateValue(*((int *)value.data));
  default:
    std::stringstream msg;
    msg << "Unsupport value type " << value.type;
    throw std::invalid_argument(msg.str());
  }
}