// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_TUPLE_FILTER_H_
#define __OBSERVER_SQL_EXECUTOR_TUPLE_FILTER_H_

#include "tuple.h"
#include <memory>
#include <string>
#include <vector>

class TupleFilter {
public:
  virtual ~TupleFilter() = default;

  virtual bool filter(const Tuple &tuple) const = 0;
};

class DefaultTupleFilter : public TupleFilter {
public:
  virtual ~DefaultTupleFilter();
  bool filter(const Tuple &tuple) const;
  static std::unique_ptr<DefaultTupleFilter> create(const TupleSchema &schema,
                                                    const Condition &condition);

private:
  DefaultTupleFilter();
  int left_index = -1, right_index = -1;
  CompOp op;
};

#endif // __OBSERVER_SQL_EXECUTOR_TUPLE_FILTER_H_