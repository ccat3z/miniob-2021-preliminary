#include "order.h"
#include "filter.h"

OrderNode::OrderNode(std::unique_ptr<ExecutionNode> child, OrderBy *order_by,
                     size_t order_by_num) {
  for (int i = order_by_num - 1; i >= 0; i--) {
    auto &ob = order_by[i];

    int field_idx = child->schema().index_of_field(ob.attr->relation_name,
                                                   ob.attr->attribute_name);
    if (field_idx < 0) {
      std::stringstream ss;
      ss << "Cannot find field " << ob.attr->relation_name << "."
         << ob.attr->relation_name;
      throw std::invalid_argument(ss.str());
    }

    fields.emplace_back(field_idx, ob.dir);
  }

  this->child = std::move(child);
}

OrderNode::~OrderNode() {}
const TupleSchema &OrderNode::schema() { return child->schema(); };

RC OrderNode::execute(TupleSet &tuple_set) {
  child->execute(tuple_set);
  tuple_set.sort([&](const Tuple &a, const Tuple &b) {
    for (auto &field : fields) {
      int res = a.get(field.first).compare(&b.get(field.first));
      if (res == 0) {
        continue;
      }

      if (field.second == DIR_DESC) {
        return res > 0;
      } else {
        return res < 0;
      }
    }

    return false;
  });

  return RC::SUCCESS;
};

std::unique_ptr<ExecutionNode>
OrderNode::push_down_predicate(std::list<Condition *> &predicate) {
  auto new_child = child->push_down_predicate(predicate);
  if (new_child != nullptr) {
    child = std::move(new_child);
  }
  if (predicate.size() == 0) {
    child = std::make_unique<FilterNode>(std::move(child), predicate.begin(),
                                         predicate.end());
  }
  return nullptr;
}