#include "sub_query.h"
#include "../nodes/execution_node.h"
#include "session/session.h"
#include <memory>

class SubQueryContextNode : public SetExecutionNode {
public:
  SubQueryContextNode(SubQueryExpression *expression,
                      const TupleSchema &ctx_schema)
      : expression(expression) {
    this->ctx_schema.append(ctx_schema);
  };
  const TupleSchema &schema() override { return ctx_schema; }
  RC execute(TupleSet &tuple_set) override {
    tuple_set.clear();
    tuple_set.set_schema(schema());
    Tuple tuple = *expression->current_tuple;

    tuple_set.add(std::move(tuple));
    return RC::SUCCESS;
  }

private:
  SubQueryExpression *expression;
  TupleSchema ctx_schema;
};

SubQueryExpression::SubQueryExpression(Session *session, Selects *select,
                                       const TupleSchema &schema) {
  this->exec_node = build_select_executor_node(
      session, *select, std::make_unique<SubQueryContextNode>(this, schema));

  if (exec_node->schema().fields().size() != 1) {
    throw std::invalid_argument(
        "More then 1 attributes is required in sub query");
  }
}

std::shared_ptr<TupleValue> SubQueryExpression::eval(const Tuple &tuple) {
  retrieve(tuple);
  if (tuple_set.size() != 1) {
    throw std::logic_error(
        "Nothing or more than one tuple is retrieved from sub query");
  }

  return tuple_set.tuples()[0].get_pointer(0);
}

void SubQueryExpression::evals(std::vector<std::shared_ptr<TupleValue>> &values,
                               const Tuple &tuple) {
  retrieve(tuple);
  for (auto &tuple : tuple_set.tuples()) {
    values.push_back(tuple.get_pointer(0));
  }
}

AttrType SubQueryExpression::type() {
  return exec_node->schema().fields()[0].type();
}

void SubQueryExpression::retrieve(const Tuple &tuple) {
  current_tuple = &tuple;

  tuple_set.clear();
  if (exec_node->execute(tuple_set) != RC::SUCCESS) {
    throw std::logic_error("Failed to retrieve tuple(s) from sub query");
  }
}