#include "expression.h"
#include "attr.h"
#include "sub_query.h"
#include "value.h"

std::unique_ptr<Expression> create_expression(Session *session,
                                              ConditionExpr *cond_expr,
                                              const TupleSchema &schema,
                                              AttrType prefer_type) {
  switch (cond_expr->type) {
  case COND_EXPR_VALUE:
    return std::make_unique<ValueExpression>(&cond_expr->value.value,
                                             prefer_type);
    break;
  case COND_EXPR_ATTR:
    return std::make_unique<AttrExpression>(cond_expr->value.attr, schema);
    break;
  case COND_EXPR_SELECT:
    return std::make_unique<SubQueryExpression>(
        session, cond_expr->value.selects, schema);
    break;
  default:
    throw std::logic_error("Unreachable code");
    break;
  }
}

inline bool try_to_match_values_type(Value &a, Value &b) {
  return value_cast(&a, b.type) || value_cast(&b, a.type);
}

std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>
create_expression_pair(Session *session, const TupleSchema &schema,
                       ConditionExpr *left, ConditionExpr *right) {
  std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>> res;

  // Handle value expressions as far back as possible
  // since the AttrType of the value expression must be determined by
  // the expression on the other side.
  if (left->type == COND_EXPR_VALUE && right->type == COND_EXPR_VALUE) {
    try_to_match_values_type(left->value.value, right->value.value);
  }
  if (left->type != COND_EXPR_VALUE) {
    res.first = create_expression(session, left, schema);
  }
  res.second =
      create_expression(session, right, schema,
                        res.first == nullptr ? UNDEFINED : res.first->type());
  if (left->type == COND_EXPR_VALUE) {
    res.first = create_expression(session, left, schema, res.second->type());
  }

  return res;
}