#include "tuple_filter.h"
#include "common/log/log.h"

DefaultTupleFilter::DefaultTupleFilter(const TupleSchema &schema,
                                       Condition &condition) {
  op = condition.comp;

  AttrType ltype = UNDEFINED, rtype = UNDEFINED;

  if (condition.left_is_attr) {
    left_index = schema.index_of_field(condition.left_attr.relation_name,
                                       condition.left_attr.attribute_name);
    if (left_index < 0) {
      throw std::invalid_argument("Cannot find left attr in schema");
    }
    ltype = schema.fields().at(left_index).type();
  }

  if (condition.right_is_attr) {
    right_index = schema.index_of_field(condition.right_attr.relation_name,
                                        condition.right_attr.attribute_name);
    if (right_index < 0) {
      throw std::invalid_argument("Cannot find right attr in schema");
    }
    rtype = schema.fields().at(right_index).type();
  }

  if (!condition.left_is_attr) {
    left_value = std::unique_ptr<TupleValue>(
        std::move(TupleValue::from_value(condition.left_value, rtype)));
    ltype = condition.left_value.type;
  }

  if (!condition.right_is_attr) {
    right_value = std::unique_ptr<TupleValue>(
        std::move(TupleValue::from_value(condition.right_value, ltype)));
    rtype = condition.right_value.type;
  }

  if (ltype != rtype) {
    throw std::invalid_argument("Incompareble fields");
  }
}
DefaultTupleFilter::~DefaultTupleFilter() {}

bool DefaultTupleFilter::filter(const Tuple &tuple) const {
  int cmp = left(tuple).compare(right(tuple));

  switch (op) {
  case EQUAL_TO:
    return 0 == cmp;
  case LESS_EQUAL:
    return cmp <= 0;
  case NOT_EQUAL:
    return cmp != 0;
  case LESS_THAN:
    return cmp < 0;
  case GREAT_EQUAL:
    return cmp >= 0;
  case GREAT_THAN:
    return cmp > 0;
  default:
    LOG_ERROR("Unsupport CompOp: %d", op);
    return false;
  }
}

const TupleValue &DefaultTupleFilter::left(const Tuple &tuple) const {
  if (left_index >= 0) {
    return tuple.get(left_index);
  }

  if (left_value == nullptr) {
    throw std::logic_error("unreachable code");
  }

  return *left_value;
}

const TupleValue &DefaultTupleFilter::right(const Tuple &tuple) const {
  if (right_index >= 0) {
    return tuple.get(right_index);
  }

  if (right_value == nullptr) {
    throw std::logic_error("unreachable code");
  }

  return *right_value;
}