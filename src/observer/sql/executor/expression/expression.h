// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_EXPRESSION_EXPRESSION_H_
#define __OBSERVER_SQL_EXECUTOR_EXPRESSION_EXPRESSION_H_
#include "base.h"
#include "session/session.h"
#include <memory>

std::unique_ptr<Expression> create_expression(Session *session,
                                              ConditionExpr *cond_expr,
                                              const TupleSchema &schema,
                                              AttrType prefer_type = UNDEFINED);

std::unique_ptr<Expression> create_expression(SelectExpr *expr,
                                              const TupleSchema &schema,
                                              AttrType prefer_type = UNDEFINED);

std::unique_ptr<Expression> create_expression(const RelAttr &attr,
                                              const TupleSchema &schema);

std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>
create_expression_pair(Session *session, const TupleSchema &schema,
                       ConditionExpr *left, ConditionExpr *right);

std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>
create_expression_pair(const TupleSchema &schema, SelectExpr *left,
                       SelectExpr *right);
#endif // __OBSERVER_SQL_EXECUTOR_EXPRESSION_EXPRESSION_H_