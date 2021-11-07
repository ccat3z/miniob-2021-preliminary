#include "value.h"

ValueExpression::ValueExpression(Value *value, AttrType prefer_type) {
  this->value = std::shared_ptr<TupleValue>(
      TupleValue::from_value(*value, prefer_type, false));
  this->value_type = value->type;
}

std::shared_ptr<TupleValue> ValueExpression::eval(const Tuple &tuple) {
  return this->value;
}

AttrType ValueExpression::type() { return value_type; }