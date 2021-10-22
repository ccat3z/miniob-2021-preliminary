// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "value.h"
#include "common/time/datetime.h"
#include <string.h>

bool value_cast(Value *value, AttrType type) {
  if (value->type == type) return true;

  if (value->type == CHARS && type == DATE) {
    common::Date date;
    if (!date.parse((char *) value->data)) {
      return false;
    }

    free(value->data);

    value->type = DATE;
    value->data = malloc(sizeof(int));
    int julian = date.julian();
    memcpy(value->data, &julian, sizeof(int));
    return true;
  }

  return false;
}