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

#include <unistd.h>
#include <stdio.h>
#include <wasmedge/wasmedge.h>

WasmEdge_Result print(int ch) {
  if (ch <= 0x7F) {
    putchar(ch);
  } else if (ch <= 0x7FF) {
    putchar(ch >> 6 & 0x1F | 0xC0);
    putchar(ch & 0x3F | 0x80);
  } else if (ch <= 0xFFFF) {
    putchar(ch >> 12 & 0x0F | 0xE0);
    putchar(ch >> 6 & 0x3F | 0x80);
    putchar(ch & 0x3F | 0x80);
  } else if (ch <= 0x10FFFF) {
    putchar(ch >> 18 & 0x07 | 0xF0);
    putchar(ch >> 12 & 0x3F | 0x80);
    putchar(ch >> 6 & 0x3F | 0x80);
    putchar(ch & 0x3F | 0x80);
  } else {
    return WasmEdge_Result_Fail;
  }
  return WasmEdge_Result_Success;
}

/* The host function to print a utf16 char. */
WasmEdge_Result HostFuncPrint(void *Data,
                            const WasmEdge_CallingFrameContext *CallFrameCxt,
                            const WasmEdge_Value *In, WasmEdge_Value *Out) {
  static int high_surrogate = -1;
  int Val = WasmEdge_ValueGetI32(In[0]);
  if (Val >= 0xD800 && Val <= 0xDBFF) {
    if (high_surrogate != -1) {
        return WasmEdge_Result_Fail;
    }
    high_surrogate = Val - 0xD800;
    return WasmEdge_Result_Success;
  } else {
    if (high_surrogate != -1) {
      Val = (high_surrogate << 10) + Val - 0xDC00 + 0x10000;
      high_surrogate = -1;
    }
  }
  return print(Val);
}

WasmEdge_ModuleInstanceContext *
CreateTestModule(const struct WasmEdge_ModuleDescriptor *Desc) {

  WasmEdge_String ModuleName =
      WasmEdge_StringCreateByCString("spectest");
  WasmEdge_ModuleInstanceContext *Mod =
      WasmEdge_ModuleInstanceCreate(ModuleName);
  WasmEdge_StringDelete(ModuleName);

  WasmEdge_String FuncName;
  WasmEdge_FunctionTypeContext *FType;
  WasmEdge_FunctionInstanceContext *FuncCxt;
  enum WasmEdge_ValType ParamTypes[2], ReturnTypes[1];
  ParamTypes[0] = WasmEdge_ValType_I32;

  FType = WasmEdge_FunctionTypeCreate(ParamTypes, 1, NULL, 0);
  FuncName = WasmEdge_StringCreateByCString("print_char");
  FuncCxt = WasmEdge_FunctionInstanceCreate(FType, HostFuncPrint, NULL, 0);
  WasmEdge_ModuleInstanceAddFunction(Mod, FuncName, FuncCxt);
  WasmEdge_StringDelete(FuncName);
  WasmEdge_FunctionTypeDelete(FType);

  return Mod;
}

static WasmEdge_ModuleDescriptor ModuleDesc[] = {{
    .Name = "spectest",
    .Description = "FFI for using print in MoonBit",,
    .Create = CreateTestModule,
}};

static WasmEdge_PluginDescriptor Desc[] = {{
    .Name = "moonrun",
    .Description = "FFI for using print in MoonBit",
    .APIVersion = WasmEdge_Plugin_CurrentAPIVersion,
    .Version =
        {
            .Major = 0,
            .Minor = 1,
            .Patch = 0,
            .Build = 0,
        },
    .ModuleCount = 1,
    .ProgramOptionCount = 0,
    .ModuleDescriptions = ModuleDesc,
    .ProgramOptions = NULL,
}};

WASMEDGE_CAPI_PLUGIN_EXPORT const WasmEdge_PluginDescriptor *
WasmEdge_Plugin_GetDescriptor() {
    return Desc;
}