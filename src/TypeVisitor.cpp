//
// Created by qiaojinxia on 2022/3/24.
//

#include "TypeVisitor.h"
#include "Diag.h"
#include "Type.h"
using namespace BDD;

void TypeVisitor::Visitor(ExprStmtNode *node) {
    if (node -> Lhs){
        node -> Lhs ->Accept(this);
        node -> Type = node -> Lhs ->Type;
    }
}

void TypeVisitor::Visitor(BinaryNode *node) {
    CurAssignType = nullptr;
    node ->Lhs->Accept(this);
    node ->Rhs ->Accept(this);
    //if lhs is 4 bytes rhs is 8 bytes convert lhs to 8bytes , always  convert to maxSize bytes size between lhs and rhs
    auto maxBitSize = node -> Lhs ->Type ->Size;
    if (node -> Rhs ->Type ->Size > maxBitSize){
        node->Rhs->Type->IsFloatPointNum();
    }
    //if express lhs or rhs is floatType ,convert  express to floatPoint operation
    bool hasFloatPoint = false;
    if (node->Lhs->Type->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        hasFloatPoint = true;
    }

    //if express lhs or rhs is unsigned ,convert express  to unsigned  operation
    bool hasUnsigned = false;
    if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()){
        hasUnsigned = true;
    }

    //auto convert if lhs type not equal to rhs type,set type to same
    if (node -> Lhs ->Type != node ->Rhs ->Type){
        if (hasFloatPoint){
            if (!node->Lhs->Type->IsFloatPointNum()){
                auto castNode = std::make_shared<CastNode>();
                castNode ->CstNode = node->Lhs;
                castNode ->Type = node->Rhs->Type;
                node->Lhs  = castNode;
            }else if (!node->Rhs->Type->IsFloatPointNum()){
                auto castNode = std::make_shared<CastNode>();
                castNode ->CstNode = node->Rhs;
                castNode ->Type = node->Lhs->Type;
                node->Rhs  = castNode;
            }
        }else if(hasUnsigned){
        }else {
                if (node->Lhs->Type->Size < maxBitSize){
                    auto castNode = std::make_shared<CastNode>();
                    castNode ->CstNode = node->Lhs;
                    castNode ->Type = node->Rhs->Type;
                    node->Lhs  = castNode;
                }else if (node->Rhs->Type->Size < maxBitSize) {
                    auto castNode = std::make_shared<CastNode>();
                    castNode->CstNode = node->Rhs;
                    castNode->Type = node->Lhs->Type;
                    node->Rhs = castNode;
                }
        }
    }
    switch (node ->BinOp) {
        case BinaryOperator::Assign:
            if (node->Lhs->Type->IsFloatPointNum() && node->Rhs->Type->IsFloatPointNum()){
                if (node -> Lhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleAssign;
                    if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatAssign;
                    if (node -> Rhs -> Type -> Size == 8){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::FloatType;
                        node -> Rhs = castNode;
                    }
                }
            }
            //to set the assign expr lhs node and rhs node with same type
            CurAssignType = node->Lhs->Type;
            if (CurAssignType ->IsAliasType()){
                CurAssignType = std::dynamic_pointer_cast<AliasType>(CurAssignType)->Base;
            }
            node ->Rhs ->Accept(this);
            break;
        case BinaryOperator::Add:
            if (node -> Lhs -> Type ->IsPointerType() && node->Rhs->Type->IsIntegerNum()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = std::make_shared<PointerType>(node->Lhs->Type);
            }else if (node->Type->IsIntegerNum() && node->Rhs->Type->IsIntegerNum()){
            }else if (node -> Lhs -> Type -> IsArrayType() && node->Rhs->Type->IsIntegerNum()){
                node -> BinOp = BinaryOperator::PointerAdd;
            }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
                auto temp = node->Lhs;
                node->Lhs = node->Rhs;
                node->Rhs = temp;
                node->BinOp = BinaryOperator::PointerAdd;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsFloatPointNum() && node->Rhs->Type->IsFloatPointNum()){
                if (node -> Lhs -> Type -> Size == 8 || node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleAdd;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatAdd;
                }else{
                    assert(0);
                }
            }else if (node->Type->IsFloatPointNum() && node->Rhs->Type->IsFloatPointNum()){
                node -> BinOp = BinaryOperator::FloatAdd;
            }else if (node-> Lhs-> Type->IsULongType() || node->Rhs->Type->IsULongType()){
                if (!node -> Lhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::ULongType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::ULongType;
                    node -> Rhs = castNode;
                }
            }else if (node-> Lhs-> Type->IsUIntType() || node->Rhs->Type->IsUIntType()){
                if (!node -> Lhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::UIntType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::UIntType;
                    node -> Rhs = castNode;
                }
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                printf("invalid add operation");
                assert(0);
            }
            break;
        case BinaryOperator::Sub:
            if (node -> Lhs -> Type ->IsPointerType() && node->Rhs->Type->IsIntegerNum()){
                node -> BinOp = BinaryOperator::PointerSub;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsIntegerNum() && node->Rhs->Type->IsIntegerNum()){
            }else if(node -> Lhs -> Type -> IsPointerType() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerDiff;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
                auto temp = node->Lhs;
                node->Lhs = node->Rhs;
                node->Rhs = temp;
                node->BinOp = BinaryOperator::PointerSub;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsFloatPointNum() && node->Rhs->Type->IsFloatPointNum()){
                if (node -> Lhs -> Type -> Size == 8 || node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleSub;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }
            }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                node ->BinOp = BinaryOperator::FloatSub;
            }else if (node-> Lhs-> Type->IsULongType() || node->Rhs->Type->IsULongType()){
                if (!node -> Lhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::ULongType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::ULongType;
                    node -> Rhs = castNode;
                }
            }else if (node-> Lhs-> Type->IsUIntType() || node->Rhs->Type->IsUIntType()){
                if (!node -> Lhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::UIntType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::UIntType;
                    node -> Rhs = castNode;
                }
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                printf("invalid sub operation");
                assert(0);
            }
            break;
        case BinaryOperator::Incr:
        case BinaryOperator::Decr:
        {
            //i++ when i is char* pointer incr 1 int * incr 4  when i is struct incr struct size
            // when i is array incr array element size
            auto ConstNode =  std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
            auto  size = 1;
            if (node -> Lhs -> Type ->IsPointerType() || node -> Lhs -> Type ->IsStructType() || node -> Lhs -> Type ->IsArrayType()){
                size = node -> Lhs -> Type ->GetBaseType() ->Size;
            }
            ConstNode -> isChange = true;
            ConstNode ->Value *= size;
        }
            break;
        case BinaryOperator::PointerAdd:
            node ->Type = std::make_shared<PointerType>(node->Rhs->Type);;
        case BinaryOperator::PointerSub:
            node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
        case BinaryOperator::Mul:
            if (node->Lhs->Type->IsFloatPointNum() && node->Rhs->Type->IsFloatPointNum()){
                if (node -> Lhs -> Type -> Size == 8 || node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleMul;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatMul;
                }
            }else if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()){
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                assert(0);
            }
            break;
        case BinaryOperator::IDiv:
            if (node->Lhs->Type->IsFloatPointNum() && node->Rhs->Type->IsFloatPointNum()){
                if (node -> Lhs -> Type -> Size == 8 && node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleDiv;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatDiv;
                }
            }else if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()){
                node ->BinOp = BinaryOperator::Div;
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                assert(0);
            }
            break;
        case BinaryOperator::IMod:
        case BinaryOperator::Mod: {
            bool hasUnsigned = false;
            if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()) {
                hasUnsigned = true;
            }
            //find max size type
            auto toConvertType = node->Lhs->Type;
            if (node->Lhs->Type->Size < node->Rhs->Type->Size) {
                toConvertType = node->Rhs->Type;
            }
            if (toConvertType->Size < 4) {
                toConvertType = Type::IntType;
            }

            //if toConvert Type is maxSize Type and is unsigned and and Express has Unsigned number  not need to convert type
            //if toConvert Type is maxSize Type and isnot Unsigned and Express has Unsigned number we need to cast it to Unsigned
            //if toConvert Type is maxSize Type and Express no Unsigned number we don't nedd to cast type
            if (!toConvertType->IsUnsignedNum() && hasUnsigned) {
                toConvertType = BuildInType::GetBuildInType(
                        static_cast<BuildInType::Kind>(std::dynamic_pointer_cast<BuildInType>(toConvertType)->Knd |
                                                       BuildInType::Kind::UnSigned));
            }
            if (node->Lhs->Type != toConvertType) {
                auto castNodeLhs = std::make_shared<CastNode>();
                castNodeLhs->CstNode = node->Lhs;
                castNodeLhs->Type = toConvertType;
                node->Lhs = castNodeLhs;
            }
            if (node->Rhs->Type != toConvertType) {
                auto castNodeLhs = std::make_shared<CastNode>();
                castNodeLhs->CstNode = node->Rhs;
                castNodeLhs->Type = toConvertType;
                node->Rhs = castNodeLhs;
            }
            if (hasUnsigned){
                node ->BinOp = BinaryOperator::Mod;
            }else{
                node ->BinOp = BinaryOperator::IMod;
            }
            node->Type = toConvertType;
        }
        break;
        case BinaryOperator::Greater:
            if (hasFloatPoint){
                node ->BinOp = BinaryOperator::FloatGreater;
            }
            node ->Type = Type::BoolType;
            break;
        case BinaryOperator::GreaterEqual:
            if (hasFloatPoint){
                node ->BinOp = BinaryOperator::FloatGreaterEqual;
            }
            node ->Type = Type::BoolType;
            break;
        case BinaryOperator::Lesser:
            if (hasFloatPoint){
                node ->BinOp = BinaryOperator::FloatLesser;
            }
            node ->Type = Type::BoolType;
            break;
        case BinaryOperator::LesserEqual:
            if (hasFloatPoint){
                node ->BinOp = BinaryOperator::FloatLesserEqual;
            }
            node ->Type = Type::BoolType;
            break;
        default:
            break;
    }
}

void TypeVisitor::Visitor(ConstantNode *node) {
    auto cursor = node;
    if (CurAssignType){
        if (node->isRoot){
            node ->Type = CurAssignType;
            cursor = cursor->Next.get();
        }
        if (auto structType = std::dynamic_pointer_cast<RecordType>(CurAssignType)){
            for (auto &filed:structType->fields) {
                cursor -> Offset = filed ->Offset;
                cursor ->Type = filed ->type;
                if (cursor ->Sub != nullptr){
                    auto bak = CurAssignType;
                    CurAssignType = cursor ->Type;
                    cursor -> Sub ->Accept(this);
                    CurAssignType = bak;
                }
                cursor  = cursor ->Next.get();
                if (cursor == nullptr){
                    break;
                }
            }
            return;
        }else if(auto arrType = std::dynamic_pointer_cast<ArrayType>(CurAssignType)){
            if (arrType ->IsStringType()){
                cursor = cursor->Next.get();
            }
            int offset = 0;
            while (cursor) {
                cursor -> Offset = offset;
                offset += arrType->ElementType->Size;
                cursor ->Type = arrType->ElementType;
                if (cursor ->Next == nullptr){
                    break;
                }
                cursor  = cursor ->Next.get();
                if (cursor ->Sub != nullptr){
                    auto bak = CurAssignType;
                    cursor -> Sub ->Accept(this);
                    CurAssignType = bak;
                }

            }
            return;
        }
        node ->Type = CurAssignType;
    }

}

void TypeVisitor::Visitor(ExprVarNode *node) {
    node -> Type = node ->VarObj ->Type;
    node ->Offset = node ->VarObj ->Offset;
}

void TypeVisitor::Visitor(ProgramNode *node) {
    for(auto &fn:node ->Funcs){
        fn ->Accept(this);
    }
}

void TypeVisitor::Visitor(IfStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Then ->Accept(this);
    if (node -> Else){
        node ->Else->Accept(this);
    }
}

void TypeVisitor::Visitor(BlockStmtNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
}

void TypeVisitor::Visitor(WhileStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Then ->Accept(this);
}

void TypeVisitor::Visitor(DoWhileStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Stmt ->Accept(this);
}

void TypeVisitor::Visitor(ForStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Stmt ->Accept(this);
    node ->Inc ->Accept(this);
    node ->Init ->Accept(this);
}

void TypeVisitor::Visitor(FunctionNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
}

void TypeVisitor::Visitor(FuncCallNode *node) {
    CurAssignType = nullptr;
    for(auto &arg:node ->Args){
        arg ->Accept(this);
    }
}

void TypeVisitor::Visitor(ReturnStmtNode *node) {
   node ->Lhs ->Accept(this);
}

void TypeVisitor::Visitor(DeclarationStmtNode *node) {
    for(auto &n:node ->declarationNodes){
        n ->Accept(this);
    }
}

void TypeVisitor::Visitor(StmtExprNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
    node ->Type = node ->Stmts .back()->Type;
}

void TypeVisitor::Visitor(UnaryNode *node) {
    node ->Lhs ->Accept(this);
    switch (node -> Uop) {
        case UnaryOperator::Plus:
        case UnaryOperator::Minus:
            node -> Type = node -> Lhs -> Type;
            break;
        case UnaryOperator::Deref:
            if (node -> Lhs -> Type ->IsPointerType()){
                node -> Type = std::dynamic_pointer_cast<PointerType>(node -> Lhs->Type)->Base;
            }else if (node -> Lhs -> Type->IsArrayType()){
                node -> Type = std::dynamic_pointer_cast<ArrayType>(node -> Lhs->Type) ->ElementType;
            }else {
                node -> Type = node -> Lhs -> Type;
            }
            break;
        case UnaryOperator::Addr:
            node -> Type = std::make_shared<PointerType>(node -> Lhs ->Type);
            break;
    }
}

void TypeVisitor::Visitor(SizeOfExprNode *node) {
        node -> Lhs ->Accept(this);
        node ->Type = Type::IntType;
}

void TypeVisitor::Visitor(DeclarationAssignmentStmtNode *node) {
    for(auto &n:node ->AssignNodes){
        n ->Accept(this);
    }
    node ->Type = node -> AssignNodes.back()->Type;
}

void TypeVisitor::Visitor(MemberAccessNode *node) {
    node ->Lhs -> Accept(this);
    auto record = std::dynamic_pointer_cast<RecordType>(node ->Lhs  ->Type->GetBaseType());
    auto field = record ->GetField(node ->fieldName);
    node ->Type = field ->type;
}

void TypeVisitor::Visitor(BreakStmtNode *node) {}

void TypeVisitor::Visitor(ContinueStmtNode *node) {}

void TypeVisitor::Visitor(CastNode *node) {
    node -> CstNode ->Accept(this);
}

void TypeVisitor::Visitor(ArefNode *node) {
    node -> Lhs ->Accept(this);
    node -> Offset ->Accept(this);
    //set the varName[index] index must greater eq then 4  to load by eax
    if (node ->Offset ->Type ->Size < Type::IntType->Size){
        auto castNode = std::make_shared<CastNode>();
        castNode ->CstNode = node ->Offset;
        castNode ->Type = Type::IntType;
        node ->Offset= castNode;
    }
    if (auto leftArrayType = std::dynamic_pointer_cast<ArrayType>(node -> Lhs->Type)){
        node -> Type = leftArrayType -> ElementType;
        return;
    }
    auto leftPtrBaseType = std::dynamic_pointer_cast<PointerType>(node -> Lhs->Type)->Base;
    node -> Type  = leftPtrBaseType;
}

void TypeVisitor::Visitor(EmptyNode *node) {
    //do nothing
}

