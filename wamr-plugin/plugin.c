// Copyright 2024 International Digital Economy Academy
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include <stdio.h>
#include <wasm_export.h>

void print(int ch) {
  if (ch <= 0x7F) {
    printf("%c", ch);
  } else if (ch <= 0x7FF) {
    printf("%c%c", ch >> 6 & 0x1F | 0xC0, ch & 0x3F | 0x80);
  } else if (ch <= 0xFFFF) {
    printf("%c%c%c", ch >> 12 & 0x0F | 0xE0, ch >> 6 & 0x3F | 0x80, ch & 0x3F | 0x80);
  } else if (ch <= 0x10FFFF) {
    printf("%c%c%c%c", ch >> 18 & 0x07 | 0xF0, ch >> 12 & 0x3F | 0x80, ch >> 6 & 0x3F | 0x80, ch & 0x3F | 0x80);
  }
}

static void 
print_char_wrapper(wasm_exec_env_t exec_env, int ch)
{
    static int high_surrogate = -1;
    if (ch >= 0xD800 && xch <= 0xDBFF) {
        high_surrogate = ch - 0xD800;
        return;
    } else {
        if (high_surrogate != -1) {
            ch = (high_surrogate << 10) + ch - 0xDC00 + 0x10000;
            high_surrogate = -1;
        }
    }
    print("%c", ch);
}

#define REG_NATIVE_FUNC(func_name, signature) \
    { #func_name, func_name##_wrapper, signature, NULL }

static NativeSymbol native_symbols[] = {
    REG_NATIVE_FUNC(print_char, "(i)")
};

uint32_t
get_native_lib(char **p_module_name, NativeSymbol **p_native_symbols)
{
    *p_module_name = "spectest";
    *p_native_symbols = native_symbols;
    return sizeof(native_symbols) / sizeof(NativeSymbol);
}