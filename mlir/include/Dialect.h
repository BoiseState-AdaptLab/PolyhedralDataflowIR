//===- Dialect.h - Definition of the PolyDF dialect -----------------------===//
//
// Copyright 2019 The MLIR Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#ifndef PDFL_DIALECT_H_
#define PDFL_DIALECT_H_

#include "mlir/IR/Dialect.h"

namespace pdfg {

/// The PolyDF Dialect is not exposed to the outside world. It is registered by
/// linking and accessed via generic MLIR accessors.
class PolyDFDialect : public mlir::Dialect {
public:
  /// Create a new Dialect that is registered on construction and adds the
  /// relevant types and operations.
  explicit PolyDFDialect(mlir::MLIRContext *context);
  static llvm::StringRef getDialectNamespace() { return "pdfg"; }

  /// Parse a type registered to this dialect.
  mlir::Type parseType(llvm::StringRef spec, mlir::Location loc) const override;

  /// Print a type registered to this dialect.
  void printType(mlir::Type type, llvm::raw_ostream &os) const override;
};

} // namespace pdfg

#endif // PDFL_DIALECT_H_

