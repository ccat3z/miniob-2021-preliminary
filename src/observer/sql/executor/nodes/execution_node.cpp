#include "execution_node.h"
#include "storage/default/default_handler.h"
#include <string>
using namespace std::literals::string_literals;

void rewite_sub_query(
    ConditionExpr &expr, const char *db, Trx *trx,
    std::vector<std::unique_ptr<ExecutionNode>> &table_scaners,
    int &virtual_table_idx) {
  if (expr.type != COND_EXPR_SELECT) {
    return;
  }

  auto virtual_table_name = "."s + std::to_string(virtual_table_idx++);

  std::unique_ptr<ExecutionNode> sub_select =
      build_select_executor_node(db, trx, *expr.value.selects);
  sub_select = std::make_unique<AliasNode>(std::move(sub_select),
                                           virtual_table_name.c_str());
  // TODO: Ensure only on tuple will be produced by sub_select if comp is not IN

  // TODO: Extract exec node: ValueNode
  if (sub_select->schema().fields().size() != 1) {
    throw std::invalid_argument("Attribute num in sub select must be 1");
  }
  auto &sub_select_field = sub_select->schema().fields().at(0);

  selects_destroy(expr.value.selects);
  expr.type = COND_EXPR_ATTR;
  expr.value.attr.relation_name = strdup(sub_select_field.table_name());
  expr.value.attr.attribute_name = strdup(sub_select_field.field_name());

  table_scaners.push_back(std::move(sub_select));
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
    rewite_sub_query(condition.left_expr, db, trx, table_scaners,
                     virtual_table_idx);
    rewite_sub_query(condition.right_expr, db, trx, table_scaners,
                     virtual_table_idx);

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
