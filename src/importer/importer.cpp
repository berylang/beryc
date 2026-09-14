#include "importer.h"

/*

    Bery Importer,

    it resolves imports by - 
        1. finds .bry file on disk
        2. send it to lexer -> parser to create it's own AST
        3. recursively resolves any imports inside that module too.
        4. collects all global names from the module,
        5. runs mangler on them, like add() function becomes moduleName.add() function.
        6. replaces the 'IMPORT_NODE' on main programs AST with these nodes.

*/

#include "mangler.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/arraydeclare.h"
#include "../parser/ast/importer.h"
#include "../parser/ast/functions.h"
#include <iostream>
#include <fstream>
#include <sstream>

const std::vector<std::string> Importer::PRELUDE_MODULES = {
    "io", "core"
};


std::string Importer::resolvePath(const std::string& modName, const std::string& sourceBasePath, const std::string& stdlibPath) {
    std::string userPath = sourceBasePath + modName + ".bry";
    std::ifstream userFile(userPath);
    if (userFile.good()) return userPath;

    std::string stdPath = stdlibPath + modName + ".bry";
    std::ifstream stdFile(stdPath);
    if (stdFile.good()) return stdPath;

    return "";
}

void Importer::resolveImports(ProgramNode* mainProgram, const std::string& sourceBasePath, std::string& stdlibPath, DiagnosticEngine& diag) {
    std::vector<std::unique_ptr<ASTNode>> newGlobals;
    for (const auto& mod : PRELUDE_MODULES) {
        std::string fullPath = stdlibPath + mod + ".bry";
        loadModule(mod, fullPath, sourceBasePath, stdlibPath, newGlobals, diag,true);
    }

    for (auto& node : mainProgram->globals) {
        if (node->type == NodeType::IMPORT_STMT) {
            auto* imp = static_cast<ImportNode*>(node.get());
            std::string fullPath = resolvePath(imp->moduleName, sourceBasePath, stdlibPath);
            if (fullPath.empty()) {
                diag.report("ERROR501", 0, 0, "", imp->moduleName);
                diag.printAll();
                exit(1);
            }
            loadModule(imp->moduleName, fullPath, sourceBasePath, stdlibPath, newGlobals, diag,false);
        } else {
            newGlobals.push_back(std::move(node));
        }
    }
    mainProgram->globals = std::move(newGlobals);
}
void Importer::loadModule(const std::string& modName, const std::string& fullPath, const std::string& sourceBasePath, const std::string& stdlibPath,
    std::vector<std::unique_ptr<ASTNode>>& outGlobals, DiagnosticEngine& diag, bool openImport) {
    
        if (importedFiles.count(fullPath)) return;
    importedFiles.insert(fullPath);

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        diag.report("ERROR501", 0, 0, "", modName);
        diag.printAll();
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string moduleSource = buffer.str();
    DiagnosticEngine moduleDiag(moduleSource, fullPath);

    Lexer lexer(moduleSource, moduleDiag);
    auto tokens = lexer.tokanize();
    Parser parser(tokens, moduleDiag);
    auto ast = parser.parse();
    auto* importedProg = static_cast<ProgramNode*>(ast.get());

    if (moduleDiag.hasErrors() || parser.hasErrors()) {
        moduleDiag.printAll();
        std::cerr << "Bery: Compilation halted due to syntax errors in imported module '" << modName << "'.\n";
        exit(1);
    }

    std::vector<std::unique_ptr<ASTNode>> processedGlobals;
    for (auto& node : importedProg->globals) {
        if (node->type == NodeType::IMPORT_STMT) {
            auto* imp = static_cast<ImportNode*>(node.get());
            std::string nextFullPath = resolvePath(imp->moduleName, sourceBasePath, stdlibPath);
            if (nextFullPath.empty()) {
                diag.report("ERROR501", imp->line, 1, "", imp->moduleName);
                diag.printAll();
                exit(1);
            }
            loadModule(imp->moduleName, nextFullPath, sourceBasePath, stdlibPath, processedGlobals, diag, false);
        } else {
            processedGlobals.push_back(std::move(node));
        }
    }

    std::unordered_set<std::string> globalNames;
    for (auto& g : processedGlobals) {
        if (g->type == NodeType::FUNC_DEF)
            globalNames.insert(static_cast<FunctionDefNode*>(g.get())->name);
        else if (g->type == NodeType::VAR_DECL)
            globalNames.insert(static_cast<VarDeclNode*>(g.get())->name);
        else if (g->type == NodeType::ARRAY_DECL)
            globalNames.insert(static_cast<ArrayDeclNode*>(g.get())->name);
        else if (g->type == NodeType::ENUM_DECL)
            globalNames.insert(static_cast<EnumDeclNode*>(g.get())->name);
    }

    if (!openImport) {
        ASTNameMangler mangler(modName, globalNames);
        for (auto& g : processedGlobals) mangler.mangle(g.get());
    }

    for (auto& g : processedGlobals) {
        outGlobals.push_back(std::move(g));
    }
}