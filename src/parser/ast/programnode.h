#pragma once
#include "node.h"
#include <vector>
#include <memory>

struct RunBlockNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    RunBlockNode() {
        type = NodeType::RUN_BLOCK;
    }
};

struct ProgramNode :public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> globals;
    std::unique_ptr<RunBlockNode> runBlock;

    ProgramNode() {
        type = NodeType::PROGRAM;
    }
};