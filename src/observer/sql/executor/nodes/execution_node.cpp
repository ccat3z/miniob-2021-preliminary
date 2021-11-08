#include "execution_node.h"
#include "storage/common/table.h"
#include "storage/common/table_meta.h"
#include "storage/default/default_handler.h"
#include <string>
using namespace std::literals::string_literals;

std::unique_ptr<ExecutionNode> build_select_executor_node(Session *session,
                                                          Selects &selects) {
  auto db = session->get_current_db().c_str();
  auto trx = session->current_trx();

  // Build table scanners
  std::vector<const TableMeta *> table_metas;
  std::vector<std::unique_ptr<ExecutionNode>> table_scaners;
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    Table *table = DefaultHandler::get_default().find_table(db, table_name);
    table_metas.push_back(&table->table_meta());
    auto table_scaner = std::make_unique<TableScaner>(trx, table);
    table_scaner->select_all_fields();
    table_scaners.push_back(std::move(table_scaner));
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
        session, std::move(exec_node), selects.conditions + 0,
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
      std::move(exec_node), table_metas, selects.attributes, selects.attr_num);

  // Simple rule-based optimizer
  auto new_node = exec_node->push_down_predicate();
  if (new_node != nullptr) {
    exec_node = std::move(new_node);
  }

  return exec_node;
}
