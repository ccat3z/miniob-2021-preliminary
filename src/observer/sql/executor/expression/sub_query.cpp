#include "sub_query.h"
#include "../nodes/execution_node.h"
#include "session/session.h"

SubQueryExpression::SubQueryExpression(Session *session, Selects *select) {
  this->exec_node = build_select_executor_node(session, *select);

  if (exec_node->schema().fields().size() != 1) {
    throw std::invalid_argument(
        "More then 1 attributes is required in sub query");
  }
}

std::shared_ptr<TupleValue> SubQueryExpression::eval(const Tuple &tuple) {
  retrieve();
  if (tuple_set.size() == 0) {
    throw std::logic_error("Nothing is found in sub query");
  }

  return tuple_set.tuples()[0].get_pointer(0);
}

void SubQueryExpression::evals(std::vector<std::shared_ptr<TupleValue>> &values,
                               const Tuple &tuple) {
  retrieve();
  for (auto &tuple : tuple_set.tuples()) {
    values.push_back(tuple.get_pointer(0));
  }
}

AttrType SubQueryExpression::type() {
  return exec_node->schema().fields()[0].type();
}

void SubQueryExpression::retrieve() {
  if (retrieved) {
    return;
  }
  retrieved = true;

  if (exec_node->execute(tuple_set) != RC::SUCCESS) {
    throw std::logic_error("Failed to retrieve tuple(s) from sub query");
  }
}