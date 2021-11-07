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
    return std::make_unique<SubQueryExpression>(session,
                                                cond_expr->value.selects);
    break;
  default:
    throw std::logic_error("Unreachable code");
    break;
  }
}