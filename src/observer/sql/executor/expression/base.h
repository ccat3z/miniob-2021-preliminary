// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_EXPRESSION_BASE_H_
#define __OBSERVER_SQL_EXECUTOR_EXPRESSION_BASE_H_

#include "../tuple.h"
#include "sql/parser/parse.h"
#include <vector>

class Expression {
public:
  virtual ~Expression() = default;
  virtual std::shared_ptr<TupleValue> eval(const Tuple &tuple) = 0;
  virtual void evals(std::vector<std::shared_ptr<TupleValue>> &values,
                     const Tuple &tuple) {
    values.push_back(eval(tuple));
  };

  virtual AttrType type() = 0;
};

#endif // __OBSERVER_SQL_EXECUTOR_EXPRESSION_BASE_H_