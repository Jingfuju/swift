//===--- Types.cpp - Driver input & temporary type information ------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "swift/Driver/Types.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"

using namespace swift;
using namespace swift::driver;
using namespace swift::driver::types;

struct TypeInfo {
  const char *Name;
  const char *Flags;
  const char *TempSuffix;
};

static const TypeInfo TypeInfos[] = {
#define TYPE(NAME, ID, TEMP_SUFFIX, FLAGS) \
  { NAME, FLAGS, TEMP_SUFFIX },
#include "swift/Driver/Types.def"
#undef TYPE
};
static const unsigned numTypes = llvm::array_lengthof(TypeInfos);

static const TypeInfo &getInfo(unsigned Id) {
  assert(Id > 0 && Id - 1 < numTypes && "Invalid Type ID.");
  return TypeInfos[Id - 1];
}

StringRef types::getTypeName(ID Id) {
  return getInfo(Id).Name;
}

StringRef types::getTypeTempSuffix(ID Id) {
  return getInfo(Id).TempSuffix;
}

ID types::lookupTypeForExtension(StringRef Ext) {
  return llvm::StringSwitch<types::ID>(Ext)
           .Case("swift", TY_Swift)
           .Case("sil", TY_SIL)
           .Case("swiftmodule", TY_SwiftModuleFile)
           .Case("pcm", TY_ClangModuleFile)
           .Case("o", TY_Object)
           .Default(TY_INVALID);
}

ID types::lookupTypeForName(StringRef Name) {
  for (int i = 0, e = numTypes; i != e; ++i) {
    const struct TypeInfo &TI = TypeInfos[i];
    if (Name == TI.Name)
      return (ID)(i + 1);
  }
  return TY_INVALID;
}

bool types::isTextual(ID Id) {
  switch (Id) {
  case types::TY_Swift:
  case types::TY_SIL:
  case types::TY_Dependencies:
  case types::TY_Assembly:
  case types::TY_RawSIL:
  case types::TY_LLVM_IR:
  case types::TY_ObjCHeader:
    return true;
  case types::TY_Image:
  case types::TY_Object:
  case types::TY_dSYM:
  case types::TY_SwiftModuleFile:
  case types::TY_SwiftModuleDocFile:
  case types::TY_LLVM_BC:
  case types::TY_SerializedDiagnostics:
  case types::TY_ClangModuleFile:
  case types::TY_Nothing:
    return false;
  case types::TY_INVALID:
  case types::TY_LAST:
    llvm_unreachable("Invalid type ID.");
  }
}
