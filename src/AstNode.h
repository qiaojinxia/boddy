//
// Created by a on 2022/3/9.
//

#ifndef BODDY_ASTNODE_H
#define BODDY_ASTNODE_H

#include <memory>
#include <list>
#include <string_view>

namespace BDD{
    class AstVisitor;
    class AstNode {
    public:
        virtual ~AstNode(){};
        virtual void Accept(AstVisitor *visitor) {};

    };
    class Var{
    public:
        std::string_view Name;
        int Offset;
    };
    class ProgramNode:public  AstNode{
    public:
        void Accept(AstVisitor *visitor) override;
        std::list<std::shared_ptr<AstNode>> Statements;
        std::list<std::shared_ptr<Var>> LocalVariables;
    };
    enum class BinaryOperator{
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Assign,
        Equal,
        NotEqual,
        Greater,
        GreaterEqual,
        Lesser,
        LesserEqual,
    };

    class BinaryNode :public  AstNode{
    public:
        BinaryOperator BinOp;
        std::shared_ptr<AstNode> Lhs;
        std::shared_ptr<AstNode> Rhs;
        void Accept(AstVisitor *visitor) override;
    };

    class ConstantNode : public AstNode{
    public:
        int Value;
        void Accept(AstVisitor *visitor) override;
    };

    class ExprStmtNode: public AstNode{
    public:
        std::shared_ptr<AstNode> Lhs;

        void Accept(AstVisitor *visitor) override;
    };

    class ExprVarNode:public AstNode{
    public:
        std::string_view Name;
        std::shared_ptr<Var> VarObj;
        int Offset;
        void Accept(AstVisitor *visitor) override;
    };

    class IfStmtNode :public AstNode{
    public:
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Then{nullptr};
        std::shared_ptr<AstNode> Else{nullptr};

        void Accept(AstVisitor *visitor) override;
    };

    class BlockStmtNode :public AstNode{
    public:
        std::list<std::shared_ptr<AstNode>> Stmts;
        void Accept(AstVisitor *visitor) override;
    };
    class AstVisitor{
    public:
        virtual ~AstVisitor(){};
        virtual void Visitor(ProgramNode *node){};
        virtual void Visitor(BinaryNode *node){};
        virtual void Visitor(ConstantNode *node){};
        virtual void Visitor(ExprStmtNode *node){};
        virtual void Visitor(ExprVarNode *node){};
        virtual void Visitor(IfStmtNode *node){};
        virtual void Visitor(BlockStmtNode *node){};


    };

}

#endif //BODDY_ASTNODE_H
