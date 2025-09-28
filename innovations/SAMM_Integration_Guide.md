# **SAMM Integration Guide: Compiler Implementation**

## **Overview**

This guide provides step-by-step instructions for integrating SAMM (Scope Aware Memory Management) into the NewBCPL compiler. The HeapManager SAMM implementation is complete and tested - this guide focuses on the compiler-side integration.

---

## **Phase 1: CFGBuilderPass Integration**

### **1.1 Locate CFGBuilderPass**

Find the CFGBuilderPass in your compiler codebase:
```bash
find . -name "*CFG*" -o -name "*cfg*" | grep -E "\.(cpp|h)$"
```

### **1.2 Modify BlockStatement Handling**

In `CFGBuilderPass::visit(BlockStatement&)`, replace the existing per-variable cleanup injection with scope boundary calls:

#### **REMOVE: Old per-variable cleanup**
```cpp
// OLD CODE - Remove this section
for (auto& var : local_variables) {
    if (var.needs_cleanup) {
        injectCleanupCall(var.name + ".RELEASE()");
    }
}
```

#### **ADD: SAMM scope boundary injection**
```cpp
// NEW CODE - Add at beginning of visit(BlockStatement&)
void CFGBuilderPass::visit(BlockStatement& stmt) {
    // SAMM: Inject scope entry
    injectRuntimeCall("HeapManager_enter_scope");
    
    // Process block contents...
    processBlockStatements(stmt);
    
    // SAMM: Inject scope exit on ALL control flow exits
    injectScopeExitOnAllPaths();
}

private:
void CFGBuilderPass::injectRuntimeCall(const std::string& function_name) {
    // Generate call instruction for the runtime function
    auto call_instr = createCallInstruction(function_name, {});
    current_block_->addInstruction(call_instr);
}

void CFGBuilderPass::injectScopeExitOnAllPaths() {
    // Inject HeapManager_exit_scope() on every control flow exit:
    
    // 1. Normal fall-through
    injectRuntimeCall("HeapManager_exit_scope");
    
    // 2. Early RETURN/RESULTIS - inject before branch to epilogue
    for (auto& edge : current_block_->getExitEdges()) {
        if (edge.isReturnEdge()) {
            insertBeforeEdge(edge, "HeapManager_exit_scope");
        }
    }
    
    // 3. BREAK/LOOP statements - inject before branch
    for (auto& edge : current_block_->getExitEdges()) {
        if (edge.isLoopExitEdge()) {
            insertBeforeEdge(edge, "HeapManager_exit_scope");
        }
    }
    
    // 4. GOTO statements - handle nested scope exits
    for (auto& edge : current_block_->getExitEdges()) {
        if (edge.isGotoEdge()) {
            int scope_levels = calculateScopeLevelsToExit(edge);
            for (int i = 0; i < scope_levels; i++) {
                insertBeforeEdge(edge, "HeapManager_exit_scope");
            }
        }
    }
}
```

### **1.3 Handle Complex Control Flow**

#### **GOTO Scope Exit Calculation**
```cpp
int CFGBuilderPass::calculateScopeLevelsToExit(const CFGEdge& goto_edge) {
    BasicBlock* target = goto_edge.getTargetBlock();
    BasicBlock* source = goto_edge.getSourceBlock();
    
    // Calculate how many nested scopes the GOTO is jumping out of
    int source_depth = getScopeDepth(source);
    int target_depth = getScopeDepth(target);
    
    return std::max(0, source_depth - target_depth);
}

int CFGBuilderPass::getScopeDepth(BasicBlock* block) {
    // Walk up the dominator tree counting BlockStatement nodes
    int depth = 0;
    auto current = block;
    while (current && current->getParent()) {
        if (current->representsBlockStatement()) {
            depth++;
        }
        current = current->getParent();
    }
    return depth;
}
```

---

## **Phase 2: Code Generation Integration**

### **2.1 Update NewCodeGenerator**

Modify the code generator to handle SAMM allocation calls:

#### **Standard Allocation (Current Scope)**
```cpp
// In NewCodeGenerator::visit(LetDeclaration& decl)
void NewCodeGenerator::visit(LetDeclaration& decl) {
    if (decl.initializer && decl.initializer->isNewExpression()) {
        NewExpression* new_expr = static_cast<NewExpression*>(decl.initializer.get());
        
        if (decl.is_retained) {
            // SAMM: Generate retained allocation call
            generateRetainedAllocation(decl, new_expr);
        } else {
            // SAMM: Generate standard allocation (current scope)
            generateStandardAllocation(decl, new_expr);
        }
    }
}

void NewCodeGenerator::generateStandardAllocation(LetDeclaration& decl, NewExpression* new_expr) {
    // Generate call to appropriate allocation function
    if (new_expr->type_name == "Object") {
        emitCall("OBJECT_HEAP_ALLOC", {getClassPointer(new_expr)});
    } else if (new_expr->type_name == "Vec") {
        emitCall("Heap_allocVec", {getVecSize(new_expr)});
    } else if (new_expr->type_name == "String") {
        emitCall("Heap_allocString", {getStringSize(new_expr)});
    }
    // Result automatically tracked in current scope by HeapManager
}
```

#### **Retained Allocation (Parent Scope)**
```cpp
void NewCodeGenerator::generateRetainedAllocation(LetDeclaration& decl, NewExpression* new_expr) {
    // Generate call to retained allocation variant
    if (new_expr->type_name == "Object") {
        emitCall("OBJECT_HEAP_ALLOC_RETAINED", {getClassPointer(new_expr)});
    } else if (new_expr->type_name == "Vec") {
        emitCall("Heap_allocVecRetained", {getVecSize(new_expr)});
    } else if (new_expr->type_name == "String") {
        emitCall("Heap_allocStringRetained", {getStringSize(new_expr)});
    }
    // Result automatically tracked in parent scope by HeapManager
}
```

### **2.2 RETAIN Statement Code Generation**

```cpp
// In NewCodeGenerator::visit(RetainStatement& stmt)
void NewCodeGenerator::visit(RetainStatement& stmt) {
    // Load the pointer value into a register
    Register ptr_reg = loadVariable(stmt.variable_name);
    
    // Generate call to retain function
    emitCall("HeapManager_retain_pointer", {ptr_reg});
    
    // Variable ownership is now transferred to parent scope
}
```

---

## **Phase 3: Parser Integration**

### **3.1 Add RETAIN Keyword to Lexer**

In your lexer token definitions:
```cpp
// Add to token enumeration
TOKEN_RETAIN,

// Add to keyword map
{"RETAIN", TOKEN_RETAIN},
```

### **3.2 Extend Parser Grammar**

#### **RETAIN Assignment Syntax**
```cpp
// RETAIN p = NEW Class
// This creates a LetDeclaration with is_retained = true
LetDeclaration* Parser::parseLetDeclaration() {
    bool is_retained = false;
    
    if (current_token.type == TOKEN_RETAIN) {
        is_retained = true;
        consumeToken(); // consume RETAIN
    } else {
        expect(TOKEN_LET);
    }
    
    std::string var_name = expectIdentifier();
    expect(TOKEN_ASSIGN);
    
    auto initializer = parseExpression();
    
    return new LetDeclaration(var_name, std::move(initializer), is_retained);
}
```

#### **RETAIN Statement Syntax**
```cpp
// RETAIN p
// This creates a RetainStatement
Statement* Parser::parseRetainStatement() {
    expect(TOKEN_RETAIN);
    std::string var_name = expectIdentifier();
    return new RetainStatement(var_name);
}
```

### **3.3 Update AST Node Definitions**

```cpp
// In AST.h - Update LetDeclaration
class LetDeclaration : public Statement {
public:
    std::string variable_name;
    std::unique_ptr<Expression> initializer;
    bool is_retained;  // NEW: SAMM retained allocation flag
    
    LetDeclaration(const std::string& name, 
                   std::unique_ptr<Expression> init,
                   bool retained = false)
        : variable_name(name), initializer(std::move(init)), is_retained(retained) {}
};

// In AST.h - Add RetainStatement
class RetainStatement : public Statement {
public:
    std::string variable_name;
    
    RetainStatement(const std::string& name) : variable_name(name) {}
    
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};
```

---

## **Phase 4: Runtime Integration**

### **4.1 Register SAMM Functions with RuntimeManager**

```cpp
// In RuntimeManager initialization
void RuntimeManager::registerSAMMFunctions() {
    // Core SAMM functions
    registerFunction("HeapManager_enter_scope", (void*)HeapManager_enter_scope);
    registerFunction("HeapManager_exit_scope", (void*)HeapManager_exit_scope);
    registerFunction("HeapManager_retain_pointer", (void*)HeapManager_retain_pointer);
    
    // Retained allocation variants
    registerFunction("OBJECT_HEAP_ALLOC_RETAINED", (void*)OBJECT_HEAP_ALLOC_RETAINED);
    registerFunction("Heap_allocVecRetained", (void*)Heap_allocVecRetained);
    registerFunction("Heap_allocStringRetained", (void*)Heap_allocStringRetained);
    registerFunction("Heap_allocListRetained", (void*)Heap_allocListRetained);
    
    // Control functions
    registerFunction("HeapManager_setSAMMEnabled", (void*)HeapManager_setSAMMEnabled);
    registerFunction("HeapManager_handleMemoryPressure", (void*)HeapManager_handleMemoryPressure);
}
```

### **4.2 Enable SAMM by Default**

```cpp
// In main.cpp or runtime initialization
void initializeNewBCPLRuntime() {
    // Initialize HeapManager
    HeapManager& heap = HeapManager::getInstance();
    
    // Enable SAMM by default for new programs
    heap.setSAMMEnabled(true);
    heap.setTraceEnabled(false); // Disable for performance
    
    // Register signal handlers for graceful shutdown
    registerShutdownHandler([]() {
        HeapManager::getInstance().shutdown();
    });
}
```

---

## **Phase 5: ASTAnalyzer Integration (RELEASE Synthesis)**

### **5.1 Move RELEASE Synthesis to ASTAnalyzer**

Based on the architectural review, RELEASE synthesis should happen in ASTAnalyzer:

```cpp
// In ASTAnalyzer::visit(RoutineDeclaration& routine)
void ASTAnalyzer::visit(RoutineDeclaration& routine) {
    if (routine.name == "RELEASE" && current_class_name_) {
        // Inject cleanup code using _this parameter
        injectSAMMReleaseCode(routine);
    }
    
    // Continue with normal analysis...
}

void ASTAnalyzer::injectSAMMReleaseCode(RoutineDeclaration& release_method) {
    // Handle both BlockStatement and CompoundStatement bodies
    Statement* body = release_method.body.get();
    
    if (auto block = dynamic_cast<BlockStatement*>(body)) {
        injectCleanupIntoBlock(block);
    } else if (auto compound = dynamic_cast<CompoundStatement*>(body)) {
        injectCleanupIntoCompound(compound);
    }
}

void ASTAnalyzer::injectCleanupIntoBlock(BlockStatement* block) {
    // Create cleanup call: OBJECT_HEAP_FREE(_this)
    auto cleanup_call = createCleanupCall("_this");
    
    // Add to end of block
    block->statements.push_back(std::move(cleanup_call));
}
```

---

## **Phase 6: Testing Integration**

### **6.1 Create Integration Tests**

```cpp
// tests/samm_integration_test.bcpl
PROGRAM SAMMIntegrationTest;

CLASS TestClass {
    value: INT;
    
    NEW(v: INT) {
        value := v;
    }
    
    RELEASE() {
        // Will be automatically injected with OBJECT_HEAP_FREE(_this)
    }
}

FUNCTION testBasicSAMM() {
    // This scope will be automatically managed
    {
        LET obj1 = NEW TestClass(42);
        LET obj2 = NEW TestClass(84);
        // Automatic cleanup when scope exits
    }
}

FUNCTION testRetainSyntax() {
    LET retained_obj: ^TestClass;
    
    {
        LET obj1 = NEW TestClass(100);
        RETAIN retained_obj = NEW TestClass(200);  // Allocated to parent scope
        RETAIN obj1;  // Transfer obj1 to parent scope
        // Only locally allocated objects cleaned up here
    }
    
    // retained_obj still valid here
    // Will be cleaned up when this scope exits
}

BEGIN
    testBasicSAMM();
    testRetainSyntax();
END SAMMIntegrationTest.
```

### **6.2 Performance Benchmarks**

```cpp
// tests/samm_performance_test.bcpl
PROGRAM SAMMPerformanceTest;

FUNCTION measureScopePerformance() {
    LET start_time = getCurrentTime();
    
    FOR i := 1 TO 1000 DO {
        // Each iteration creates a new scope
        LET obj1 = NEW TestClass(i);
        LET obj2 = NEW TestClass(i * 2);
        LET obj3 = NEW TestClass(i * 3);
        // Automatic cleanup - should be very fast
    }
    
    LET end_time = getCurrentTime();
    PRINT("SAMM scope performance: ", end_time - start_time, "ms");
}

BEGIN
    measureScopePerformance();
END SAMMPerformanceTest.
```

---

## **Phase 7: Build System Integration**

### **7.1 Update Build Scripts**

Ensure SAMM HeapManager files are included in compilation:

```bash
# In build.sh - Add SAMM files to compilation
SAMM_FILES="
HeapManager/HeapManager.cpp
HeapManager/Heap_allocObject.cpp
HeapManager/Heap_allocVec.cpp
HeapManager/Heap_allocString.cpp
HeapManager/Heap_allocList.cpp
HeapManager/heap_c_wrappers.cpp
"

# Add to ALL_SRC_FILES_RAW
ALL_SRC_FILES_RAW="${ALL_SRC_FILES_RAW}\n${SAMM_FILES}"
```

### **7.2 Enable SAMM in Compilation**

```bash
# Add SAMM preprocessor flag
CXXFLAGS="${CXXFLAGS} -DSAMM_ENABLED"
```

---

## **Phase 8: Configuration and Deployment**

### **8.1 Runtime Configuration**

```cpp
// Allow runtime configuration via environment variables
void configureSAMMFromEnvironment() {
    const char* samm_enabled = getenv("NEWBCPL_SAMM_ENABLED");
    if (samm_enabled && strcmp(samm_enabled, "0") == 0) {
        HeapManager::getInstance().setSAMMEnabled(false);
        printf("SAMM disabled via environment variable\n");
    }
    
    const char* samm_trace = getenv("NEWBCPL_SAMM_TRACE");
    if (samm_trace && strcmp(samm_trace, "1") == 0) {
        HeapManager::getInstance().setTraceEnabled(true);
        printf("SAMM tracing enabled\n");
    }
}
```

### **8.2 Deployment Checklist**

- [ ] **CFGBuilderPass updated** with scope boundary injection
- [ ] **Code generator updated** for RETAIN syntax
- [ ] **Parser updated** to handle RETAIN keyword
- [ ] **AST nodes extended** for RETAIN support
- [ ] **Runtime functions registered** with RuntimeManager
- [ ] **SAMM enabled by default** in initialization
- [ ] **Integration tests pass** with real NewBCPL programs
- [ ] **Performance benchmarks** show expected improvements
- [ ] **Build system updated** to include SAMM files
- [ ] **Documentation updated** for new RETAIN syntax

---

## **Troubleshooting Guide**

### **Common Issues**

#### **1. "HeapManager_enter_scope not found"**
- **Cause**: Runtime function not registered
- **Fix**: Add function registration in RuntimeManager initialization

#### **2. "Double-free detected"**
- **Cause**: SAMM and legacy cleanup both running
- **Fix**: Ensure legacy cleanup is disabled when SAMM is enabled

#### **3. "Scope exit not called"**
- **Cause**: Missing scope exit injection on control flow path
- **Fix**: Check CFGBuilderPass covers all exit edges (GOTO, RETURN, etc.)

#### **4. "Memory leak on early return"**
- **Cause**: Scope exit not injected before return statement
- **Fix**: Ensure injectScopeExitOnAllPaths() handles return edges

### **Debug Commands**

```bash
# Enable SAMM tracing for debugging
export NEWBCPL_SAMM_TRACE=1
./NewBCPL your_program.bcpl

# Run with memory debugging
valgrind --leak-check=full ./NewBCPL your_program.bcpl

# Check SAMM statistics
gdb ./NewBCPL
(gdb) call HeapManager::getInstance().getSAMMStats()
```

---

## **Success Metrics**

### **Functional Success**
- [ ] All existing NewBCPL programs compile and run correctly
- [ ] RETAIN syntax works for all allocation types
- [ ] No memory leaks in SAMM-enabled programs
- [ ] Proper cleanup on all control flow paths

### **Performance Success**
- [ ] Scope exit operations complete in <1 microsecond (main thread)
- [ ] Background cleanup keeps pace with allocation rate
- [ ] Memory pressure handling responds within milliseconds
- [ ] Overall application performance maintained or improved

### **Integration Success**
- [ ] Clean compilation with no SAMM-related errors
- [ ] Seamless transition between legacy and SAMM modes
- [ ] All test suites pass with SAMM enabled
- [ ] Development team can use RETAIN syntax effectively

---

**This integration guide provides a complete roadmap for incorporating SAMM into the NewBCPL compiler. Follow the phases sequentially for a smooth, validated integration process.**