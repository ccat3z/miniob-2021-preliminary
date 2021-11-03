# miniob

## Introduction
miniob设计的目标是让不熟悉数据库设计和实现的同学能够快速的了解与深入学习数据库内核，期望通过miniob相关训练之后，能够对各个数据库内核模块的功能与它们之间的关联有所了解，并能够在
使用时，设计出高效的SQL。面向的对象主要是在校学生，并且诸多模块做了简化，比如不考虑并发操作。
注意：此代码仅供学习使用，不考虑任何安全特性。

## How to build
please refer to docs/how_to_build.md

## 相关连接

[日测结果](https://open.oceanbase.com/answer)

[测试请求](https://github.com/hnwyllmm/miniob_test/issues/1#issue-comment-box)

[榜单](https://open.oceanbase.com/competition/index)

## TODO

See details [here](./docs/lectures/miniob-topics.md).

- [ ] Build executor tree in parser stage
- [ ] <del>RC stringify</del>
- [ ] Colorful log
- [ ] Memory leak of Value
- [ ] Parallel sql test
- [X] Refactor exectuor
  - [X] Impl tuple filter
    - [X] Support attr op value
    - [ ] Save *cond
  - [X] Rm hardcode optimizer
  - [X] Complete sql in parser
    - [ ] Expand *
  - [X] Impl cond push down: c = [] -> new_node, unused_c
    - [ ] Unit test?
  - [ ] Impl proj push down: want: schema -> new_node
  - [X] Record based exec node / Volcane Model
  - [X] Agg support

### 必做题

- [X] 基础功能 (basic)
- [X] 优化buffer pool
- [X] 查询元数据校验 (select-meta)
- [X] drop table (drop-table)
- [X] 实现update功能 (update)
  - [ ] Support trx
  - [X] Check meta
- [X] 增加date字段 (date)
  - [X] Parser
    - [X] Lex & Yacc
  - [X] Executor
    - [X] Query
      - [X] Record -> Tuple (see `class TupleRecordConvertor`)
  - [X] Storage
    - [X] Deserialize (see [`default_storage_stage`](https://github.com/ccat3z/miniob/blob/cf1e48282549d46c3c181068d1f2c8604194f314/src/observer/storage/default/default_storage_stage.cpp#L289))
      - [ ] Test case
    - [X] Index (see `bplus_tree.cpp`)
      - [X] Test case
    - [X] Condition (see `condition.cpp`)
- [X] 多表查询 (select-tables)
- [X] 聚合运算 (aggregation-func)

### 选做题

- [X] 多表join操作 (join-tables)
- [X] 一次插入多条数据 (insert)
- [ ] 唯一索引 (unique)
- [ ] 支持NULL类型 (null)
- [ ] 简单子查询 (simple-sub-query)
- [ ] 多列索引 (multi-index)
- [ ] 超长字段 (text)
- [ ] 查询条件支持表达式 (expression)
- [ ] 复杂子查询 (complex-sub-query)
- [X] 排序 (order-by)
- [ ] 分组 (group-by)