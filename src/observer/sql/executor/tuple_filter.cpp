#include "tuple_filter.h"
#include "common/log/log.h"

DefaultTupleFilter::DefaultTupleFilter() {}
DefaultTupleFilter::~DefaultTupleFilter() {}

bool DefaultTupleFilter::filter(const Tuple &tuple) const {
  int cmp = tuple.get(left_index).compare(tuple.get(right_index));

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

std::unique_ptr<DefaultTupleFilter>
DefaultTupleFilter::create(const TupleSchema &schema,
                           const Condition &condition) {
  if (!condition.left_is_attr || !condition.right_is_attr) {
    LOG_ERROR("DefaultTupleFilter only support condition between attrs");
    return nullptr;
  }

  assert(condition.left_attr.relation_name != nullptr);
  assert(condition.left_attr.attribute_name != nullptr);
  assert(condition.right_attr.relation_name != nullptr);
  assert(condition.right_attr.attribute_name != nullptr);
  auto &left_table = condition.left_attr.relation_name;
  auto &left_attr = condition.left_attr.attribute_name;
  auto &right_table = condition.right_attr.relation_name;
  auto &right_attr = condition.right_attr.attribute_name;

  auto filter = std::unique_ptr<DefaultTupleFilter>(new DefaultTupleFilter());
  filter->op = condition.comp;

  auto &fields = schema.fields();
  for (size_t i = 0; i < fields.size(); i++) {
    auto &field = fields[i];

    if (strcmp(field.table_name(), left_table) == 0 &&
        strcmp(field.field_name(), left_attr) == 0) {
      filter->left_index = i;
    }

    if (strcmp(field.table_name(), right_table) == 0 &&
        strcmp(field.field_name(), right_attr) == 0) {
      filter->right_index = i;
    }
  }

  if (filter->left_index == -1 || filter->right_index == -1) {
    LOG_ERROR("Cannot find fields in schema");
    return nullptr;
  }

  if (fields[filter->left_index].type() != fields[filter->right_index].type()) {
    LOG_ERROR("The type of the two fields does not match");
    return nullptr;
  }

  return filter;
}