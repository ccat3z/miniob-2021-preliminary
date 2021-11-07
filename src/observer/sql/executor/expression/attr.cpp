#include "attr.h"
#include <string>
using namespace std::literals::string_literals;

AttrExpression::AttrExpression(RelAttr &attr, const TupleSchema &schema) {
  index = schema.index_of_field(attr.relation_name, attr.attribute_name);
  if (index < 0) {
    throw std::invalid_argument("Cannot find attr "
                                " in schema");
  }
  field_type = schema.fields()[index].type();
}
std::shared_ptr<TupleValue> AttrExpression::eval(const Tuple &tuple) {
  return tuple.get_pointer(index);
}
AttrType AttrExpression::type() { return field_type; }