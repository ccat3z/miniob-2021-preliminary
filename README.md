# miniob

## Introduction
miniob设计的目标是让不熟悉数据库设计和实现的同学能够快速的了解与深入学习数据库内核，期望通过miniob相关训练之后，能够对各个数据库内核模块的功能与它们之间的关联有所了解，并能够在
使用时，设计出高效的SQL。面向的对象主要是在校学生，并且诸多模块做了简化，比如不考虑并发操作。
注意：此代码仅供学习使用，不考虑任何安全特性。

## How to build
please refer to docs/how_to_build.md

## 相关连接

[日测结果](https://open.oceanbase.com/answer)

[榜单](https://open.oceanbase.com/competition/index)

## TODO

See details [here](./docs/lectures/miniob-topics.md).

- [ ] Build executor tree in parser stage
- [ ] <del>RC stringify</del>
- [ ] Colorful log
- [ ] Memory leak of Value
- [ ] Parallel sql test

### 必做题

- [X] 优化buffer pool
- [X] 查询元数据校验
- [X] drop table
- [X] 实现update功能
  - [ ] Support trx
  - [X] Check meta
- [X] 增加date字段
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
- [X] 多表查询
- [ ] 聚合运算

### 选做题

- [ ] 多表join操作      
- [ ] 一次插入多条数据  
- [ ] 唯一索引          
- [ ] 支持NULL类型      
- [ ] 简单子查询        
- [ ] 多列索引          
- [ ] 超长字段          
- [ ] 查询条件支持表达式
- [ ] 复杂子查询        
- [ ] 排序              
- [ ] 分组              