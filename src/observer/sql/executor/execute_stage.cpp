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

bool ensure_and_complete_relattr(
    std::map<std::string, std::unique_ptr<TableScaner>> &table_scaners,
    RelAttr &attr) {
  // If relation is specified
  if (attr.relation_name != nullptr) {
    auto node = table_scaners.find(attr.relation_name);
    if (node == table_scaners.end()) {
      LOG_ERROR("Table %s not exists", attr.relation_name);
      return false;
    }

    return node->second->can_filter_by(attr);
  }

  // If relation is not specified
  char *matched_relation_name = nullptr;
  for (auto &it : table_scaners) {
    if (it.second->can_filter_by(attr)) {
      if (matched_relation_name != nullptr) {
        LOG_ERROR("Ambiguous column: %s", attr.attribute_name);
        return false;
      }

      matched_relation_name = strdup(it.first.c_str());
    }
  }

  if (matched_relation_name == nullptr) {
    LOG_ERROR("Column %s not exist in any table", attr.attribute_name);
    return false;
  }

  attr.relation_name = matched_relation_name;
  return true;
}

RC ExecuteStage::do_select(const char *db, Query *sql,
                           SessionEvent *session_event) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  Selects &selects = sql->sstr.selection;

  // Build table scanners
  std::map<std::string, std::unique_ptr<TableScaner>> table_scaners;
  for (size_t i = 0; i < selects.relation_num; i++) {
    const char *table_name = selects.relations[i];
    Table *table = DefaultHandler::get_default().find_table(db, table_name);
    if (table == nullptr) {
      LOG_ERROR("Invalid table: %s", table_name);
      return RC::SQL_SYNTAX;
    }

    auto table_scanner = std::make_unique<TableScaner>(trx, table);
    if (!table_scaners.insert({table_name, std::move(table_scanner)}).second) {
      LOG_WARN("Duplicate table is ignored");
    }
    end_trx_if_need(session, trx, false);
  }

  if (table_scaners.empty()) {
    LOG_ERROR("No table given");
    end_trx_if_need(session, trx, false);
    return RC::SQL_SYNTAX;
  }

  // Check and apply attrs to table scanners
  for (int i = selects.attr_num - 1; i >= 0; i--) {
    RelAttr &attr = selects.attributes[i];

    if (strcmp(attr.attribute_name, "*") == 0) {
      if (attr.relation_name == nullptr) {
        attr.relation_name = strdup("*");
        for (auto &it : table_scaners) {
          it.second->select_all_fields();
        }
      } else {
        auto node = table_scaners.find(attr.relation_name);
        if (node == table_scaners.end()) {
          LOG_ERROR("Invalid relation: %s", attr.relation_name);
          return RC::SQL_SYNTAX;
        }

        node->second->select_all_fields();
      }
      continue;
    }

    if (!ensure_and_complete_relattr(table_scaners, attr)) {
      return RC::SQL_SYNTAX;
    }
    rc = table_scaners[attr.relation_name]->select_field(attr.attribute_name);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Invalid attribute: %s.%s", attr.relation_name,
                attr.attribute_name);
      return rc;
    }
  }

  // Check and apply conditions to table scaners
  std::vector<Condition *> unused_conditions;
  for (size_t i = 0; i < selects.condition_num; i++) {
    Condition &condition = selects.conditions[i];

    // Apply static condition to all nodes
    if (!condition.left_is_attr && !condition.right_is_attr) {
      for (auto &it : table_scaners) {
        if (!it.second->add_filter(condition)) {
          return RC::SQL_SYNTAX;
        }
      }
      continue;
    }

    // Fill relation name in condition
    if (condition.left_is_attr &&
        !ensure_and_complete_relattr(table_scaners, condition.left_attr)) {
      return RC::SQL_SYNTAX;
    }
    if (condition.right_is_attr &&
        !ensure_and_complete_relattr(table_scaners, condition.right_attr)) {
      return RC::SQL_SYNTAX;
    }

    // Apply conditions in specific node
    const char *rel_name = nullptr;
    if (condition.left_is_attr && !condition.right_is_attr) {
      rel_name = condition.left_attr.relation_name;
    } else if (!condition.left_is_attr && condition.right_is_attr) {
      rel_name = condition.right_attr.relation_name;
    } else if (strcmp(condition.left_attr.relation_name,
                      condition.right_attr.relation_name) == 0) {
      rel_name = condition.left_attr.relation_name;
    }
    if (rel_name != nullptr &&
        !table_scaners[rel_name]->add_filter(condition)) {
      return RC::SQL_SYNTAX;
    }
    if (rel_name == nullptr) {
      unused_conditions.push_back(&condition);
    }
  }

  // Build root execution node
  std::unique_ptr<ExecutionNode> exec_node;
  if (table_scaners.size() == 1) {
    exec_node = std::move(table_scaners.begin()->second);
  } else {
    std::vector<std::unique_ptr<ExecutionNode>> nodes;
    for (auto &it : table_scaners) {
      nodes.push_back(std::move(it.second));
    }

    exec_node = CartesianSelectNode::create(nodes, unused_conditions);
    if (exec_node == nullptr) {
      return RC::SQL_SYNTAX;
    }

    if (unused_conditions.size() > 0) {
      exec_node = FilterNode::create(std::move(exec_node), unused_conditions);
      if (exec_node == nullptr) {
        return RC::SQL_SYNTAX;
      }
    }

    exec_node = ProjectionNode::create(std::move(exec_node), selects.attributes,
                                       selects.attr_num);
    if (exec_node == nullptr) {
      return RC::SQL_SYNTAX;
    }
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
