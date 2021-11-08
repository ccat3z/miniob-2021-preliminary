#include "attr.h"
#include <string>
using namespace std::literals::string_literals;

AttrExpression::AttrExpression(const RelAttr &attr, const TupleSchema &schema) {
  index = schema.index_of_field(attr.relation_name, attr.attribute_name);
  if (index < 0) {
    std::stringstream ss;
    ss << "Cannot find attr " << attr.relation_name << ":"
       << attr.attribute_name << " in scehma";
    throw std::invalid_argument(ss.str());
  }
  field_type = schema.fields()[index].type();
}
std::shared_ptr<TupleValue> AttrExpression::eval(const Tuple &tuple) {
  return tuple.get_pointer(index);
}
AttrType AttrExpression::type() { return field_type; }