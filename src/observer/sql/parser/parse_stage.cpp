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

#include <string.h>
#include <string>

#include "parse_stage.h"

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "event/execution_plan_event.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "session/session.h"
#include "sql/parser/parse.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"

using namespace common;

//! Constructor
ParseStage::ParseStage(const char *tag) : Stage(tag) {}

//! Destructor
ParseStage::~ParseStage() {}

//! Parse properties, instantiate a stage object
Stage *ParseStage::make_stage(const std::string &tag) {
  ParseStage *stage = new (std::nothrow) ParseStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ParseStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ParseStage::set_properties() {
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
bool ParseStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  optimize_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ParseStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ParseStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  StageEvent *new_event = handle_request(event);
  if (nullptr == new_event) {
    callback_event(event, nullptr);
    event->done_immediate();
    return;
  }

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for SQLStageEvent");
    callback_event(event, nullptr);
    event->done_immediate();
    return;
  }
  event->push_callback(cb);
  optimize_stage_->handle_event(new_event);

  LOG_TRACE("Exit\n");
  return;
}

void ParseStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");
  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);
  sql_event->session_event()->done_immediate();
  LOG_TRACE("Exit\n");
  return;
}

RC complete_sql(SQLStageEvent *event, Selects &selects);

StageEvent *ParseStage::handle_request(StageEvent *event) {
  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);
  const std::string &sql = sql_event->get_sql();

  Query *result = query_create();
  if (nullptr == result) {
    LOG_ERROR("Failed to create query.");
    return nullptr;
  }

  RC ret = parse(sql.c_str(), result);
  if (ret != RC::SUCCESS) {
    // set error information to event
    const char *error =
        result->sstr.errors != nullptr ? result->sstr.errors : "Unknown error";
    LOG_ERROR("Failed to parse sql: %s, error msg: %s\n", sql.c_str(), error);
    sql_event->session_event()->set_response("FAILURE\n");
    query_destroy(result);
    return nullptr;
  }

  switch (result->flag) {
  case SCF_SELECT:
    ret = complete_sql(sql_event, result->sstr.selection);
    if (ret != RC::SUCCESS) {
      sql_event->session_event()->set_response("FAILURE\n");
      query_destroy(result);
      return nullptr;
    }
    break;
  default:
    break;
  }

  return new ExecutionPlanEvent(sql_event, result);
}

bool ensure_and_complete_relattr(std::map<std::string, Table *> &tables,
                                 RelAttr &attr) {
  // If relation is specified
  if (attr.relation_name != nullptr) {
    auto node = tables.find(attr.relation_name);
    if (node == tables.end()) {
      LOG_ERROR("Table %s not exists", attr.relation_name);
      return false;
    }

    return node->second->table_meta().field(attr.attribute_name) != nullptr;
  }

  // If relation is not specified
  char *matched_relation_name = nullptr;
  for (auto &it : tables) {
    if (it.second->table_meta().field(attr.attribute_name) != nullptr) {
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

RC complete_attr(std::map<std::string, Table *> &tables, RelAttr &attr) {
  if (strcmp(attr.attribute_name, "*") == 0) {
    if (attr.relation_name == nullptr) {
      attr.relation_name = strdup("*");
    }
    return RC::SUCCESS;
  }

  if (!ensure_and_complete_relattr(tables, attr)) {
    return RC::SQL_SYNTAX;
  }

  return RC::SUCCESS;
}

RC complete_sql(SQLStageEvent *event, Selects &selects) {
  const char *db =
      event->session_event()->get_client()->session->get_current_db().c_str();

  // Merge join list into relation list
  if (selects.join_num > 0) {
    for (int i = selects.relation_num - 1; i >= 0; i--) {
      selects.relations[i + selects.join_num] = selects.relations[i];
    }
    for (size_t i = 0; i < selects.join_num; i++) {
      selects.relations[i] = selects.joins[i];
      selects.joins[i] = nullptr;
    }
    selects.relation_num += selects.join_num;
    selects.join_num = 0;
  }

  // Retrive tables
  std::map<std::string, Table *> tables;
  for (size_t i = 0; i < selects.relation_num; i++) {
    const char *table_name = selects.relations[i];
    Table *table = DefaultHandler::get_default().find_table(db, table_name);
    if (table == nullptr) {
      LOG_ERROR("Invalid table %s", table);
      return RC::SQL_SYNTAX;
    }
    if (!tables.insert({table_name, table}).second) {
      LOG_WARN("Duplicate table is ignored");
    }
  }

  if (tables.empty()) {
    LOG_ERROR("No table given");
    return RC::SQL_SYNTAX;
  }

  // Complete attributes
  for (int i = selects.attr_num - 1; i >= 0; i--) {
    auto &expr = selects.attributes[i];
    RC rc;
    if (expr.attribute != nullptr) {
      rc = complete_attr(tables, *expr.attribute);
    } else if (expr.agg != nullptr && expr.agg->attr != nullptr) {
      rc = complete_attr(tables, *expr.agg->attr);
    }

    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  // Complete attributes in condition
  for (size_t i = 0; i < selects.condition_num; i++) {
    Condition &condition = selects.conditions[i];

    // Fill relation name in condition
    if (condition.left_is_attr &&
        !ensure_and_complete_relattr(tables, condition.left_attr)) {
      return RC::SQL_SYNTAX;
    }
    if (condition.right_is_attr &&
        !ensure_and_complete_relattr(tables, condition.right_attr)) {
      return RC::SQL_SYNTAX;
    }
  }

  // Complete attributes in order by
  for (size_t i = 0; i < selects.order_by_num; i++) {
    if (!ensure_and_complete_relattr(tables, *selects.order_by[i].attr)) {
      return RC::SQL_SYNTAX;
    }
  }

  return RC::SUCCESS;
}