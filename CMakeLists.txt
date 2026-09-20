cmake_minimum_required(VERSION 3.20)
project(BeryCompiler)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BRE_SOURCES
    src/bre/src/bery_runtime.cpp
    src/bre/src/bery_alloc.cpp
    src/bre/src/bery_gc_roots.cpp
    src/bre/src/bery_type_registry.cpp
    src/bre/src/bery_gc.cpp
    src/bre/src/bery_string.cpp
    src/bre/src/bery_array.cpp
    src/bre/src/bery_native.cpp
    src/bre/src/bery_stdlib_core.cpp
    src/bre/src/bery_stdlib_time.cpp
    src/bre/src/bery_stdlib_os.cpp
)

set(PARSER_SOURCES
    src/parser/parser.cpp
    src/parser/parser_conditional.cpp
    src/parser/parser_controlflow.cpp
    src/parser/parser_errors.cpp
    src/parser/parser_functions.cpp
    src/parser/parser_expression.cpp
    src/parser/parser_loops.cpp
    src/parser/parser_oop.cpp
    src/parser/parser_statements.cpp
)

set(SEMA_SOURCES
    src/sema/sema.cpp
    src/sema/sema_declarations.cpp
    src/sema/sema_controlflow.cpp
    src/sema/symboltable.cpp
    src/sema/typechecker.cpp
    src/sema/typechecker_binary.cpp
    src/sema/typechecker_unary.cpp
    src/sema/typechecker_ternary.cpp
    src/sema/check_calls.cpp
    src/sema/check_access.cpp

    src/sema/typechecker_oop.cpp
)

set(CODEGEN_SOURCES
    src/codegen/codegen.cpp
    src/codegen/irgen/declarations.cpp
    src/codegen/irgen/expressions.cpp
    src/codegen/irgen/controlflow.cpp
    src/codegen/irgen/helpers.cpp
    src/codegen/irgen/gc.cpp
)

set(LLVM_EMIT_SOURCES
    src/llvm/emit/core.cpp
    src/llvm/emit/constants.cpp
    src/llvm/emit/controlflow.cpp
    src/llvm/emit/functions.cpp
    src/llvm/emit/memory.cpp
    src/llvm/emit/operator.cpp
    src/llvm/emit/types.cpp
    src/llvm/emit/naming.cpp
    src/llvm/emit/struct.cpp
    src/llvm/emit/externs.cpp
    src/llvm/emit/gc.cpp
    src/llvm/emit/calls.cpp
)

set(CLI_SOURCES
    cli/bery_cli.cpp
    cli/commands/cmd_compile.cpp
    cli/commands/cmd_run.cpp
    cli/commands/cmd_version.cpp
)

set(IMPORTER_SOURCES
    src/importer/mangler.cpp
    src/importer/importer.cpp
)

add_library(bre STATIC ${BRE_SOURCES})
target_include_directories(bre PRIVATE src)

add_executable(bery
    src/main.cpp
    src/diagnostic/diagnostic_engine.cpp
    src/lexer/lexer.cpp
    ${PARSER_SOURCES}
    ${SEMA_SOURCES}
    ${CODEGEN_SOURCES}
    ${LLVM_EMIT_SOURCES}
    ${IMPORTER_SOURCES}
    ${CLI_SOURCES}
)

target_include_directories(bery PRIVATE src cli)
target_link_libraries(bery PRIVATE bre)

install(TARGETS bre ARCHIVE DESTINATION lib)
install(TARGETS bery RUNTIME DESTINATION bin)