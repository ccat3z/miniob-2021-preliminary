#include "alias.h"
#include "filter.h"

AliasNode::AliasNode(Session *session, std::unique_ptr<ExecutionNode> child,
                     const char *table_name)
    : child(std::move(child)), session(session) {
  for (auto field : this->child->schema().fields()) {
    tuple_schema_.add(field.type(), table_name, field.field_name());
  }
}

AliasNode::~AliasNode() {}

const TupleSchema &AliasNode::schema() { return tuple_schema_; }

RC AliasNode::next(Tuple &tuple) { return child->next(tuple); }
void AliasNode::reset() { child->reset(); }

std::unique_ptr<ExecutionNode>
AliasNode::push_down_predicate(std::list<Condition *> &predicate) {
  auto new_child = child->push_down_predicate(predicate);
  if (new_child != nullptr) {
    child = std::move(new_child);
  }
  if (predicate.size() == 0) {
    child = std::make_unique<FilterNode>(session, std::move(child),
                                         predicate.begin(), predicate.end());
  }
  return nullptr;
}