// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_STORAGE_DEFAULT_LARGE_BLOCK_POOL_H_
#define __OBSERVER_STORAGE_DEFAULT_LARGE_BLOCK_POOL_H_
#include "rc.h"
#include <memory>
#include <string>

#define LARGE_BLOCK_POOL_BLOCK_SIZE ((size_t)4096)

class LargeBlock {
public:
  char data[LARGE_BLOCK_POOL_BLOCK_SIZE];
};

class LargeBlockPool {
public:
  LargeBlockPool();
  virtual ~LargeBlockPool();

  RC open_file(std::string file_name);
  void close();
  RC remove();

  std::unique_ptr<LargeBlock> get(uint32_t idx) const;
  RC set(uint32_t idx, const char *data, size_t size, bool end_null = true);
  void mark(uint32_t idx, bool used);

  uint32_t find_next_free();

private:
  RC init();
  std::string file_name;
  int fd = -1;
};

#endif // __OBSERVER_STORAGE_DEFAULT_LARGE_BLOCK_POOL_H_