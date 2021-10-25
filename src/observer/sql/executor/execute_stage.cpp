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

#include <string>
#include <sstream>
#include <unordered_map>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "event/execution_plan_event.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
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
  const char *current_db = session_event->get_client()->session->get_current_db().c_str();

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
    }
    break;

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
    }
    break;
    case SCF_SYNC: {
      RC rc = DefaultHandler::get_default().sync();
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_BEGIN: {
      session_event->get_client()->session->set_trx_multi_operation_mode(true);
      session_event->set_response(strrc(RC::SUCCESS));
      exe_event->done_immediate();
    }
    break;
    case SCF_COMMIT: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->commit();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_ROLLBACK: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->rollback();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_HELP: {
      const char *response = "show tables;\n"
          "desc `table name`;\n"
          "create table `table name` (`column name` `column type`, ...);\n"
          "create index `index name` on `table` (`column`);\n"
          "insert into `table` values(`value1`,`value2`);\n"
          "update `table` set column=value [where `column`=`value`];\n"
          "delete from `table` [where `column`=`value`];\n"
          "select [ * | `columns` ] from `table`;\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
    case SCF_EXIT: {
      // do nothing
      const char *response = "Unsupported\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
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

bool check_and_fill_relattr(std::unordered_map<std::string, SelectExeNode *> &select_nodes, RelAttr &attr) {
  // If relation is specified
  if (attr.relation_name != nullptr) {
    auto node = select_nodes.find(attr.relation_name);
    if (node == select_nodes.end()) {
      LOG_ERROR("Table %s not exists", attr.relation_name);
      return false;
    }

    return node->second->can_filter_by(attr);
  }

  // If relation is not specified
  char *matched_relation_name = nullptr;
  for (auto it : select_nodes) {
    if (it.second->can_filter_by(attr)) {
      if (matched_relation_name != nullptr) {
        LOG_ERROR("Ambiguous column in condition: %s", attr.attribute_name);
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

RC ExecuteStage::do_select(const char *db, Query *sql, SessionEvent *session_event) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  Selects &selects = sql->sstr.selection;

  // Build select nodes from relations
  std::unordered_map<std::string, SelectExeNode *> select_nodes;
  for (size_t i = 0; i < selects.relation_num; i++) {
    const char *table_name = selects.relations[i];
    Table * table = DefaultHandler::get_default().find_table(db, table_name);
    if (table == nullptr) {
      LOG_ERROR("Invalid table: %s", table_name);
      return RC::SQL_SYNTAX;
    }

    SelectExeNode *select_node = new SelectExeNode(trx, table);
    if (!select_nodes.insert({table_name, select_node}).second) {
      LOG_WARN("Duplicate table is ignored");
    }
    end_trx_if_need(session, trx, false);
  }

  if (select_nodes.empty()) {
    LOG_ERROR("No table given");
    end_trx_if_need(session, trx, false);
    return RC::SQL_SYNTAX;
  }

  // Config schema of select nodes
  for (int i = selects.attr_num - 1; i >= 0; i--) {
    RelAttr &attr = selects.attributes[i];

    // select t1.a or t1.*
    if (attr.relation_name != nullptr) {
      if (select_nodes.find(attr.relation_name) == select_nodes.end()) {
        LOG_ERROR("Invalid table: %s", attr.relation_name);
        return RC::SQL_SYNTAX;
      }

      // select t1.*
      if (strcmp(attr.attribute_name, "*") == 0) {
        select_nodes[attr.relation_name]->select_all_fields();
        continue;
      }

      // select t1.a
      rc = select_nodes[attr.relation_name]->select_field(attr.attribute_name);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Invalid attribute: %s.%s", attr.relation_name, attr.attribute_name);
        return rc;
      }
      continue;
    }

    // select *
    if (strcmp(attr.attribute_name, "*") == 0) {
      for (auto it : select_nodes) {
        it.second->select_all_fields();
      }
      continue;
    }

    // select a
    size_t target_nodes_num = 0;
    for (auto it : select_nodes) {
      rc = it.second->select_field(attr.attribute_name);
      if (rc == RC::SUCCESS) {
        target_nodes_num++;
      }
    }
    if (target_nodes_num != 1) {
      LOG_ERROR("Attribute %s matched %d tables", attr.attribute_name, target_nodes_num);
      return RC::SQL_SYNTAX;
    }
  }

  // Check and apply conditions to SelectExeNode
  for (size_t i = 0; i < selects.condition_num; i++) {
    Condition &condition = selects.conditions[i];

    // Apply static condition to all nodes
    if (!condition.left_is_attr && !condition.right_is_attr) {
      for (auto it : select_nodes) {
        if (!it.second->add_filter(condition)) {
          return RC::SQL_SYNTAX;
        }
      }
      continue;
    }

    // Fill relation name in condition
    if (condition.left_is_attr && !check_and_fill_relattr(select_nodes, condition.left_attr)) {
      return RC::SQL_SYNTAX;
    }
    if (condition.right_is_attr && !check_and_fill_relattr(select_nodes, condition.right_attr)) {
      return RC::SQL_SYNTAX;
    }

    // Apply conditions in specific node
    const char *rel_name = nullptr;
    if (condition.left_is_attr && !condition.right_is_attr) {
      rel_name = condition.left_attr.relation_name;
    } else if (!condition.left_is_attr && condition.right_is_attr) {
      rel_name = condition.right_attr.relation_name;
    } else if (strcmp(condition.left_attr.relation_name, condition.right_attr.relation_name) == 0) {
      rel_name = condition.left_attr.relation_name;
    }
    if (rel_name != nullptr && !select_nodes[rel_name]->add_filter(condition)) {
      return RC::SQL_SYNTAX;
    }
    if (rel_name == nullptr) {
      // TODO: Save unused conditions
    }
  }

  // Build root select node
  ExecutionNode *exec_node;
  if (select_nodes.size() == 1) {
    exec_node = select_nodes.begin()->second;
  } else {
    // TODO: Build CartesianSelectNode
    exec_node = select_nodes.begin()->second;
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
  tuple_set.print(ss);
  session_event->set_response(ss.str());

  // TODO: defer cleanup
  for (auto it : select_nodes) {
    delete it.second;
  }
  end_trx_if_need(session, trx, true);

  return rc;
}
