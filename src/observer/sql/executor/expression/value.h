// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_EXPRESSION_VALUE_H_
#define __OBSERVER_SQL_EXECUTOR_EXPRESSION_VALUE_H_

#include "base.h"

class ValueExpression : public Expression {
public:
  ValueExpression(Value *value, AttrType prefer_type = UNDEFINED);
  std::shared_ptr<TupleValue> eval(const Tuple &tuple) override;
  AttrType type() override;

private:
  std::shared_ptr<TupleValue> value;
  AttrType value_type;
};

#endif // __OBSERVER_SQL_EXECUTOR_EXPRESSION_VALUE_H_