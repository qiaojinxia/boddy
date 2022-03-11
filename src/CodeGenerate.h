//
// Created by a on 2022/3/8.
//

#ifndef BODDY_CODEGENERATE_H
#define BODDY_CODEGENERATE_H

#include "AstNode.h"

namespace BDD{
    class CodeGenerate:public AstVisitor{
    private:
        int StackLevel{0};
    public:
        CodeGenerate(){}
        void Visitor(ProgramNode *node) override;
    private:
        void Visitor(BinaryNode *node) override;
        void Visitor(ConstantNode *node) override;
        void Push();
        void Pop(const char *reg);
    };
}

#endif //BODDY_CODEGENERATE_H