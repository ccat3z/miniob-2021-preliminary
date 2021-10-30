// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_SQL_EXECUTOR_NODES_TABLE_SCANER_H_
#define __OBSERVER_SQL_EXECUTOR_NODES_TABLE_SCANER_H_
#include "base.h"
#include "storage/common/condition_filter.h"

class Trx;
class Table;

class TableScaner : public SetExecutionNode {
public:
  TableScaner(Trx *trx, Table *table);
  virtual ~TableScaner();

  const TupleSchema &schema() override;
  RC execute(TupleSet &tuple_set) override;

  void select_all_fields();
  RC select_field(const char *field_name);
  bool add_filter(Condition &condition);
  bool can_filter_by(RelAttr &attr);
  const Table *table();

private:
  Trx *trx_ = nullptr;
  Table *table_;
  TupleSchema tuple_schema_;
  std::vector<DefaultConditionFilter *> condition_filters_;
};
#endif // __OBSERVER_SQL_EXECUTOR_NODES_TABLE_SCANER_H_