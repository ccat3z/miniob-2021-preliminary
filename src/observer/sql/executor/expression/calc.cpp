#include "calc.h"
#include "expression.h"

CalcExpression::CalcExpression(ConditionCalcExpr &calc_expr, Session *session,
                               const TupleSchema &schema) {
  if (calc_expr.left.type == COND_EXPR_SELECT ||
      calc_expr.right.type == COND_EXPR_SELECT) {
    throw std::invalid_argument("Sub query in calc expression is not support");
  }

  std::tie(left, right) = create_expression_pair(
      session, schema, &calc_expr.left, &calc_expr.right);
  op = calc_expr.op;

  if (!((left->type() == INTS || right->type() == FLOATS) &&
        (right->type() == INTS || right->type() == FLOATS))) {
    throw std::invalid_argument("Calc expr only support INTS and FLOATS");
  }
}

CalcExpression::CalcExpression(SelectCalcExpr &calc_expr,
                               const TupleSchema &schema) {
  std::tie(left, right) =
      create_expression_pair(schema, calc_expr.left, calc_expr.right);
  op = calc_expr.op;

  if (!((left->type() == INTS || right->type() == FLOATS) &&
        (right->type() == INTS || right->type() == FLOATS))) {
    throw std::invalid_argument("Calc expr only support INTS and FLOATS");
  }
}

AttrType CalcExpression ::type() {
  if (left->type() == FLOATS || right->type() == FLOATS) {
    return FLOATS;
  }

  if (op == CALC_DIV) {
    return FLOATS;
  }

  return INTS;
}

double get_value(TupleValue *value) {
  if (auto int_tv = dynamic_cast<IntValue *>(value)) {
    return (double)int_tv->value();
  } else if (auto float_tv = dynamic_cast<FloatValue *>(value)) {
    return (double)float_tv->value();
  } else {
    throw std::invalid_argument("Cannot get double from this tuple value");
  }
}

std::shared_ptr<TupleValue> CalcExpression::eval(const Tuple &tuple) {
  auto ltv = left->eval(tuple);
  auto rtv = right->eval(tuple);

  if (ltv->is_null() || rtv->is_null())
    return std::make_shared<NullValue>();

  double lv = get_value(ltv.get()), rv = get_value(rtv.get());
  double result;

  switch (op) {
  case CALC_ADD:
    result = lv + rv;
    break;
  case CALC_MINUS:
    result = lv - rv;
    break;
  case CALC_MULTI:
    result = lv * rv;
    break;
  case CALC_DIV:
    if (rv == 0)
      return std::make_shared<NullValue>();
    result = lv / rv;
    break;
  default:
    throw std::logic_error("Unreacable code: 60");
  }

  switch (type()) {
  case INTS:
    return std::make_shared<IntValue>(result);
    break;
  case FLOATS:
    return std::make_shared<FloatValue>(result);
    break;
  default:
    throw std::logic_error("Unreacable code: 71");
    break;
  }
}