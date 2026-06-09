#pragma once
#include "node.h"
#include <string>



struct IntLitNode : public ASTNode {
    long long value;
    IntLitNode(long long v) : value(v) {
        type = NodeType::INT_LIT;
    }
};

struct BoolLitNode: public ASTNode {
    bool value;
    BoolLitNode(bool v) : value(v) {type = NodeType::BOOL_LIT;}
};

struct DecimalLitNode : public ASTNode {
    double value;
    DecimalLitNode(double v) : value(v) {
        type = NodeType::DECIMAL_LIT;
    }
};

struct IdentNode : public ASTNode {
    std::string name;
    std::string varType;

    IdentNode(const std::string& name, const std::string& varType):
        name(name), varType(varType) {
            type = NodeType::IDENT;
        }
};

struct CharLitNode : public ASTNode {
    char value;
    CharLitNode(char v) : value(v) {
        type = NodeType::CHAR_LIT;
    }
};

struct StringLitNode : public ASTNode {
    std::string value;
    StringLitNode(std::string& s) : value(s) {
        type = NodeType::STRING_LIT;
    } 
};

struct NullLitNode : public ASTNode {
    NullLitNode() {
        type = NodeType :: NULL_LIT;
    }
};