/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Longda on 2021/4/13.
//

#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "event/execution_plan_event.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "event/storage_event.h"
#include "session/session.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/condition_filter.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/trx/trx.h"

using namespace common;
using namespace std::literals::string_literals;

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag) {
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties() {
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SQLStageEvent *sql_event = exe_event->sql_event();
  sql_event->done_immediate();

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event) {
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SessionEvent *session_event = exe_event->sql_event()->session_event();
  Query *sql = exe_event->sqls();
  const char *current_db =
      session_event->get_client()->session->get_current_db().c_str();

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for ExecutionPlanEvent");
    exe_event->done_immediate();
    return;
  }
  exe_event->push_callback(cb);

  switch (sql->flag) {
  case SCF_SELECT: { // select
    RC rc = do_select(current_db, sql, exe_event->sql_event()->session_event());
    if (rc != RC::SUCCESS) {
      session_event->set_response("FAILURE\n");
    }
    exe_event->done_immediate();
  } break;

  case SCF_INSERT:
  case SCF_UPDATE:
  case SCF_DELETE:
  case SCF_CREATE_TABLE:
  case SCF_SHOW_TABLES:
  case SCF_DESC_TABLE:
  case SCF_DROP_TABLE:
  case SCF_CREATE_INDEX:
  case SCF_DROP_INDEX:
  case SCF_LOAD_DATA: {
    StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
    if (storage_event == nullptr) {
      LOG_ERROR("Failed to new StorageEvent");
      event->done_immediate();
      return;
    }

    default_storage_stage_->handle_event(storage_event);
  } break;
  case SCF_SYNC: {
    RC rc = DefaultHandler::get_default().sync();
    session_event->set_response(strrc(rc));
    exe_event->done_immediate();
  } break;
  case SCF_BEGIN: {
    session_event->get_client()->session->set_trx_multi_operation_mode(true);
    session_event->set_response(strrc(RC::SUCCESS));
    exe_event->done_immediate();
  } break;
  case SCF_COMMIT: {
    Trx *trx = session_event->get_client()->session->current_trx();
    RC rc = trx->commit();
    session_event->get_client()->session->set_trx_multi_operation_mode(false);
    session_event->set_response(strrc(rc));
    exe_event->done_immediate();
  } break;
  case SCF_ROLLBACK: {
    Trx *trx = session_event->get_client()->session->current_trx();
    RC rc = trx->rollback();
    session_event->get_client()->session->set_trx_multi_operation_mode(false);
    session_event->set_response(strrc(rc));
    exe_event->done_immediate();
  } break;
  case SCF_HELP: {
    const char *response =
        "show tables;\n"
        "desc `table name`;\n"
        "create table `table name` (`column name` `column type`, ...);\n"
        "create index `index name` on `table` (`column`);\n"
        "insert into `table` values(`value1`,`value2`);\n"
        "update `table` set column=value [where `column`=`value`];\n"
        "delete from `table` [where `column`=`value`];\n"
        "select [ * | `columns` ] from `table`;\n";
    session_event->set_response(response);
    exe_event->done_immediate();
  } break;
  case SCF_EXIT: {
    // do nothing
    const char *response = "Unsupported\n";
    session_event->set_response(response);
    exe_event->done_immediate();
  } break;
  default: {
    exe_event->done_immediate();
    LOG_ERROR("Unsupported command=%d\n", sql->flag);
  }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right) {
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}

std::unique_ptr<ExecutionNode>
build_select_executor_node(const char *db, Trx *trx, Selects &selects) {
  // Build table scanners
  std::vector<std::unique_ptr<ExecutionNode>> table_scaners;
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    Table *table = DefaultHandler::get_default().find_table(db, table_name);
    auto table_scaner = std::make_unique<TableScaner>(trx, table);
    table_scaner->select_all_fields();
    table_scaners.push_back(std::move(table_scaner));
  }

  // Flatten sub query in conditions
  // TODO: We need a better method to implement sub query
  int virtual_table_idx = 0;
  for (size_t i = 0; i < selects.condition_num; i++) {
    Condition &condition = selects.conditions[i];
    if (!condition.left_is_attr && condition.left_selects != nullptr) {
      auto virtual_table_name = "."s + std::to_string(virtual_table_idx++);

      std::unique_ptr<ExecutionNode> sub_select =
          build_select_executor_node(db, trx, *condition.left_selects);
      sub_select = std::make_unique<AliasNode>(std::move(sub_select),
                                               virtual_table_name.c_str());
      // TODO: Ensure only on tuple will be produced by sub_select if comp is not IN

      // TODO: Extract exec node: ValueNode
      if (sub_select->schema().fields().size() != 1) {
        throw std::invalid_argument("Attribute num in sub select must be 1");
      }
      auto &sub_select_field = sub_select->schema().fields().at(0);

      condition.left_is_attr = true;
      condition.left_attr.relation_name = strdup(sub_select_field.table_name());
      condition.left_attr.attribute_name =
          strdup(sub_select_field.field_name());

      table_scaners.push_back(std::move(sub_select));
    }

    if (!condition.right_is_attr && condition.right_selects != nullptr) {
      auto virtual_table_name = "."s + std::to_string(virtual_table_idx++);

      std::unique_ptr<ExecutionNode> sub_select =
          build_select_executor_node(db, trx, *condition.right_selects);
      sub_select = std::make_unique<AliasNode>(std::move(sub_select),
                                               virtual_table_name.c_str());

      // TODO: Extract exec node: ValueNode
      if (sub_select->schema().fields().size() != 1) {
        throw std::invalid_argument("Attribute num in sub select must be 1");
      }
      auto &sub_select_field = sub_select->schema().fields().at(0);

      condition.right_is_attr = true;
      condition.right_attr.relation_name =
          strdup(sub_select_field.table_name());
      condition.right_attr.attribute_name =
          strdup(sub_select_field.field_name());

      table_scaners.push_back(std::move(sub_select));
    }

    if (condition.comp == IN_SET) {
      condition.comp = EQUAL_TO;
    }
  }

  // Build root execution node
  std::unique_ptr<ExecutionNode> exec_node;
  if (table_scaners.size() == 1) {
    exec_node = std::move(*table_scaners.begin());
  } else {
    exec_node = CartesianSelectNode::create(table_scaners);
  }

  // Filter
  if (selects.condition_num > 0) {
    exec_node = std::make_unique<FilterNode>(
        std::move(exec_node), selects.conditions + 0,
        selects.conditions + selects.condition_num);
  }

  // Aggeration
  bool need_agg = false;
  for (size_t i = 0; i < selects.attr_num; i++) {
    auto &expr = selects.attributes[i];
    if (expr.agg != nullptr) {
      need_agg = true;
      break;
    }
  }
  if (need_agg) {
    exec_node = std::make_unique<AggregationNode>(
        std::move(exec_node), selects.attributes, selects.attr_num);
  }

  // Order
  if (selects.order_by_num > 0) {
    exec_node = std::make_unique<OrderNode>(
        std::move(exec_node), selects.order_by, selects.order_by_num);
  }

  // Projection
  exec_node = std::make_unique<ProjectionNode>(
      std::move(exec_node), selects.attributes, selects.attr_num);

  // Simple rule-based optimizer
  auto new_node = exec_node->push_down_predicate();
  if (new_node != nullptr) {
    exec_node = std::move(new_node);
  }

  return exec_node;
}

RC ExecuteStage::do_select(const char *db, Query *sql,
                           SessionEvent *session_event) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  Selects &selects = sql->sstr.selection;

  std::unique_ptr<ExecutionNode> exec_node;
  try {
    exec_node = build_select_executor_node(db, trx, selects);
  } catch (const std::exception &e) {
    LOG_ERROR(e.what());
    return RC::SQL_SYNTAX;
  }

  // Execute node
  TupleSet tuple_set;
  rc = exec_node->execute(tuple_set);
  if (rc != RC::SUCCESS) {
    end_trx_if_need(session, trx, false);
    return rc;
  }

  // Response
  std::stringstream ss;
  tuple_set.print(ss, selects.relation_num > 1);
  session_event->set_response(ss.str());

  end_trx_if_need(session, trx, true);
  return rc;
}
