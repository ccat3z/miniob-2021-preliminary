# miniob

## Introduction
miniob设计的目标是让不熟悉数据库设计和实现的同学能够快速的了解与深入学习数据库内核，期望通过miniob相关训练之后，能够对各个数据库内核模块的功能与它们之间的关联有所了解，并能够在
使用时，设计出高效的SQL。面向的对象主要是在校学生，并且诸多模块做了简化，比如不考虑并发操作。
注意：此代码仅供学习使用，不考虑任何安全特性。

## How to build
please refer to docs/how_to_build.md

## TODO

See details [here](./docs/lectures/miniob-topics.md).

- [ ] RC stringify

### 必做题

- [X] 优化buffer pool
- [X] drop table
- [ ] 实现update功能
  - [ ] Support trx
- [ ] 增加date字段
- [ ] 查询元数据校验
- [ ] 多表查询
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