#!/bin/bash

# Enhanced script to remove bcpl:: namespace references from the project
# This will address the 10,000 error cascade across hundreds of files

echo "Starting enhanced namespace removal..."

# Step 1: First properly edit DataTypes.h to fix the core issue
cat > DataTypes.h.new << 'EOF'
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <optional>

// Forward declare Expression class to avoid circular dependency
class Expression;

// Define ExprPtr using the forward declaration
using ExprPtr = std::unique_ptr<Expression>;

// Define VarType here so it's available to all files that include DataTypes.h
enum class VarType { UNKNOWN, INTEGER, FLOAT };

enum class FunctionType {
    STANDARD, // For functions using X registers (int/pointer)
    FLOAT     // For functions using D registers (float/double)
};

// Structure to hold information about a string literal
struct StringLiteralInfo {
    std::string label;
    std::u32string value;
};

// Structure to hold information about a float literal
struct FloatLiteralInfo {
    std::string label;
    double value;
};

// Structure to hold information about a static variable
struct StaticVariableInfo {
    std::string label;
    ExprPtr initializer;
};

#endif // DATA_TYPES_H
EOF

mv DataTypes.h.new DataTypes.h

# Step 2: Fix AST.h to not redefine ExprPtr
cat > AST.h.new << 'EOF'
#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <iostream>

// Custom nullable type since std::optional might not be available
template<typename T>
class OptionalValue {
    bool has_value_;
    T value_;
public:
    OptionalValue() : has_value_(false) {}
    OptionalValue(T value) : has_value_(true), value_(value) {}
    bool has_value() const { return has_value_; }
    T value() const { return value_; }
    void reset() { has_value_ = false; }
};

// Include DataTypes.h for ExprPtr definition
#include "DataTypes.h"

// Smart pointer typedefs for AST nodes
typedef std::unique_ptr<class ASTNode> ASTNodePtr;
typedef std::unique_ptr<class Program> ProgramPtr;
typedef std::unique_ptr<class Declaration> DeclPtr;
// ExprPtr is already defined in DataTypes.h
typedef std::unique_ptr<class Statement> StmtPtr;

// Forward declarations for smart pointers
class ASTNode;
class Program;
class Declaration;
class Expression;
class Statement;
class LabelTargetStatement;
class ConditionalBranchStatement;
class CaseStatement;
class DefaultStatement;
class TableExpression;

// Forward declaration of the ASTVisitor interface
class ASTVisitor;

// Helper function to clone a single unique_ptr (defined in AST.cpp)
EOF

# Get the rest of AST.h (after the first 50 lines) and append
tail -n +50 AST.h >> AST.h.new
mv AST.h.new AST.h

# Step 3: Remove namespace blocks from all .h and .cpp files
find . -name "*.h" -o -name "*.cpp" | xargs grep -l "namespace bcpl" | while read file; do
  echo "Removing namespace block from $file"
  # Use awk to process the file and remove namespace blocks
  awk '
    BEGIN { in_namespace = 0; skip_next_line = 0; }
    /namespace bcpl {/ { in_namespace = 1; next; }
    /} \/\/ namespace bcpl/ { in_namespace = 0; next; }
    { print; }
  ' "$file" > "$file.new"
  mv "$file.new" "$file"
done

# Step 4: Now find and remove bcpl:: references from all files
find . -name "*.h" -o -name "*.cpp" | while read file; do
  if grep -q "bcpl::" "$file"; then
    echo "Removing bcpl:: references from $file"
    sed -i '' -e 's/bcpl:://g' "$file"
  fi
done

# Step 5: Fix using statements
find . -name "*.h" -o -name "*.cpp" | while read file; do
  if grep -q "using namespace bcpl" "$file"; then
    echo "Removing 'using namespace bcpl' from $file"
    sed -i '' -e 's/using namespace bcpl;//g' "$file"
  fi
  if grep -q "using bcpl::" "$file"; then
    echo "Fixing 'using bcpl::' in $file"
    sed -i '' -e 's/using bcpl::/using /g' "$file"
  fi
done

# Step 6: Fix Symbol.h and Symbol.cpp specifically
if [ -f "Symbol.h" ]; then
  echo "Fixing Symbol.h"
  sed -i '' -e 's/namespace bcpl {//' -e 's/} \/\/ namespace bcpl//' "Symbol.h"
fi

if [ -f "Symbol.cpp" ]; then
  echo "Fixing Symbol.cpp"
  sed -i '' -e 's/namespace bcpl {//' -e 's/} \/\/ namespace bcpl//' "Symbol.cpp"
fi

# Step 7: Fix SymbolTable.h and SymbolTable.cpp
if [ -f "SymbolTable.h" ]; then
  echo "Fixing SymbolTable.h"
  sed -i '' -e 's/namespace bcpl {//' -e 's/} \/\/ namespace bcpl//' "SymbolTable.h"
fi

if [ -f "SymbolTable.cpp" ]; then
  echo "Fixing SymbolTable.cpp"
  sed -i '' -e 's/namespace bcpl {//' -e 's/} \/\/ namespace bcpl//' "SymbolTable.cpp"
fi

# Step 8: Fix forward references in runtime code
for file in runtime/*.h runtime/*.cpp; do
  if [ -f "$file" ]; then
    echo "Fixing runtime references in $file"
    sed -i '' -e 's/bcpl:://g' "$file"
  fi
done

# Count the remaining references to double-check
remaining=$(grep -r "bcpl::" --include="*.h" --include="*.cpp" . | wc -l)
echo "Finished processing files. Remaining references to check: $remaining"

echo "Done! You may need to manually fix any remaining issues."
