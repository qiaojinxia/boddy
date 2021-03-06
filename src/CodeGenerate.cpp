//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cassert>
#include "CodeGenerate.h"
#include "AstNode.h"
#include <string>
#include "Common.h"
#include "Scope.h"

using namespace BDD;


void BDD::CodeGenerate::Visitor(BDD::BinaryNode *node) {
    if (node->BinOp == BinaryOperator::Assign){
        GenerateAddress(node ->Lhs.get());
        Push(Type::Pointer);
        node -> Rhs -> Accept(this);
        Pop(Type::Pointer, GetRdi(Type::Pointer).data());
        Store(node -> Rhs);
        return;
    }else if (node -> BinOp == BinaryOperator::FloatAdd){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  addss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatSub){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  subss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatMul){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  mulss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatDiv){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  divss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleAdd){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  addsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleSub){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  subsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleMul){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  mulsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleDiv){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  divsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatAssign || node -> BinOp == BinaryOperator::DoubleAssign){
        GenerateAddress(node ->Lhs.get());
        Push(Type::Pointer);
        node -> Rhs -> Accept(this);
        Pop(Type::Pointer, GetRdi(Type::Pointer).data());
        Store(node -> Lhs );
        Depth --;
        return;
    }else if (node -> BinOp == BinaryOperator::IMod){  // lhs % rhs
        auto exprNode = std::dynamic_pointer_cast<ExprVarNode>(node ->Rhs);
        if (exprNode){
            node -> Lhs -> Accept(this);
            if (node -> Lhs ->Type ->Size == 4){
                printf("\t  cdq\n");
            }else if(node -> Lhs ->Type ->Size == 8){
                printf("\t  cqo\n");
            }
            printf("\t  %s %d(%%rbp)\n", GetIDivCode(exprNode->VarObj ->Type).data(), exprNode -> VarObj ->Offset);
        }else{
            node -> Lhs -> Accept(this);
            Push(node -> Lhs ->Type);
            node -> Rhs -> Accept(this);
            printf("\t  mov %s,%s\n", GetRax(node ->Rhs ->Type).data(), GetRdi(node ->Rhs ->Type).data());
            Pop(node ->Lhs ->Type);
            if (node -> Lhs ->Type ->Size == 4){
                printf("\t  cdq\n");
            }else if(node -> Lhs ->Type ->Size == 8){
                printf("\t  cqo\n");
            }
            printf("\t  idiv %s\n",GetRdi(node ->Rhs ->Type).data());
        }
        if (node -> Lhs -> Type ->Size == 4 ){
            printf("\t  mov %%edx,%%eax\n");
        }else if (node -> Lhs -> Type ->Size == 8){
            printf("\t  mov %%rdx,%%rax\n");
        }
        return;
    }else if (node -> BinOp == BinaryOperator::Mod){  // lhs % rhs
        auto exprNode = std::dynamic_pointer_cast<ExprVarNode>(node ->Rhs);
        if (exprNode){
            node -> Lhs -> Accept(this);
            if (node -> Lhs ->Type ->Size == 4){
                printf("\t  xor %%edx,%%edx\n");
            }else if(node -> Lhs ->Type ->Size == 8){
                printf("\t  xor %%rdx,%%rdx\n");
            }
            printf("\t  %s %d(%%rbp)\n", GetDivCode(exprNode->VarObj ->Type).data(), exprNode -> VarObj ->Offset);
        }else{
            node -> Lhs -> Accept(this);
            Push(node -> Lhs ->Type);
            node -> Rhs -> Accept(this);
            printf("\t  mov %s,%s\n", GetRax(node ->Rhs ->Type).data(), GetRdi(node ->Rhs ->Type).data());
            Pop(node ->Lhs ->Type);
            if (node -> Lhs ->Type ->Size == 4){
                printf("\t xor %%edx,%%edx\n");
            }else if(node -> Lhs ->Type ->Size == 8){
                printf("\t  xor %%rdx,%%rdx\n");
            }
            printf("\t  div %s\n",GetRdi(node ->Rhs ->Type).data());
        }
        if (node -> Lhs -> Type ->Size == 4 ){
            printf("\t  mov %%edx,%%eax\n");
        }else if (node -> Lhs -> Type ->Size == 8){
            printf("\t  mov %%rdx,%%rax\n");
        }
        return;
    }else if (node -> BinOp == BinaryOperator::Incr){
        node -> Lhs -> Accept(this);
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        printf("\t  mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
        printf("\t  add $%s,%s\n",constNode->GetValue().data(), GetRcx(varNode ->Type).data());
        printf("\t  mov %s,%d(%%rbp)\n", GetRcx(varNode ->Type).data(), varNode->VarObj -> Offset);
        return;
    }else if (node -> BinOp == BinaryOperator::Decr){
        node -> Lhs -> Accept(this);
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        printf("\t mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
        printf("\t  sub $%s,%s\n",constNode->GetValue().data(), GetRcx(node -> Lhs ->Type).data());
        printf("\t  mov %s,%d(%%rbp)\n", GetRcx(varNode ->Type).data(), varNode-> VarObj -> Offset);
        return;
    }
    node -> Rhs ->Accept(this);
    Push(node -> Rhs ->Type);
    node -> Lhs -> Accept(this);
    Pop(node -> Rhs ->Type, GetRdi(node -> Rhs ->Type).data());
    switch (node -> BinOp) {
        case BinaryOperator::Add:
            printf("\t  add %s,%s\n", GetRdi(node -> Lhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Sub:
            printf("\t  sub %s,%s\n", GetRdi(node -> Lhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Mul:
            printf("\t  imul %s,%s\n", GetRdi(node -> Lhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::IDiv:
            printf("\t  xor %%rdx,%%rdx\n");
            printf("\t  idiv %s\n", GetRdi(node -> Rhs->Type).data());
            break;
        case BinaryOperator::Div:
            printf("\t  xor %%rdx,%%rdx\n");
            printf("\t  div %%rdi\n");
            break;
        case BinaryOperator::Greater:
            printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  setg %%al\n");
            printf("\t  movzb %%al,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::GreaterEqual:
            printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  setge %%al\n");
            printf("\t  movzb %%al,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Lesser:
            printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  setl %%al\n");
            printf("\t  movzb %%al,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::LesserEqual:
            printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  setle %%al\n");
            printf("\t  movzb %%al,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Equal:
            printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  sete %%al\n");
            printf("\t  movzb %%al,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::NotEqual:
            printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  setne %%al\n");
            printf("\t  movzb %%al,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::And:
            printf("\t  and %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Or:
            printf("\t  or %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Sar:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  sar %%cl,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Sal:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  sal %%cl,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::Xor:
            printf("\t  xor %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::PointerAdd:
        {
            printf("\t  imul $%d,%s\n", node -> Type ->GetBaseType() -> Size , GetRdi(node -> Rhs->Type).data());
//            printf("\t  movslq %%edi, %%rdi\n");
            printf("\t  add %%rdi,%%rax\n");
            break;
        }
        case BinaryOperator::PointerSub:
        {
            printf("\t  imul $%d,%s\n", node -> Type ->GetBaseType() -> Size , GetRdi(node -> Rhs->Type).data());
            printf("\t  sub %%rdi,%%rax\n");
            break;
        }
        case BinaryOperator::PointerDiff:
        {
            printf("\t  sub %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  mov $%d, %s\n", node -> Lhs -> Type -> GetBaseType() ->Size, GetRdi(node -> Rhs->Type).data());
            printf("\t  cqo\n");
            printf("\t  idiv %s\n", GetRdi(node -> Rhs->Type).data());
            break;
        }
        case BinaryOperator::FloatGreater:
            printf("\t  ucomiss %s, %s\n", Xmm[Depth - 2],Xmm[Depth - 1]);
            printf("\t  seta    %%al\n");
            Depth -=2;
            break;
        case BinaryOperator::FloatLesser:
            printf("\t  ucomiss %s, %s\n",Xmm[Depth - 1], Xmm[Depth - 2]);
            printf("\t  seta    %%al\n");
            Depth -=2;
            break;
        case BinaryOperator::FloatGreaterEqual:
            printf("\t  ucomiss %s, %s\n",Xmm[Depth - 2], Xmm[Depth - 1]);
            printf("\t  setae    %%al\n");
            Depth -=2;
            break;
        case BinaryOperator::FloatLesserEqual:
            printf("\t  ucomiss %s, %s\n",Xmm[Depth - 1], Xmm[Depth - 2]);
            printf("\t  setae    %%al\n");
            Depth -=2;
            break;
        default:
            assert(0);
    }
}

void BDD::CodeGenerate::Visitor(BDD::ConstantNode *node) {
    if (node->Type->IsFloatPointNum()) {
        printf("\t  %s %s(%%rip), %s        #FloatConstant %s \n", GetMoveCode(node->Type).data(), node->Name.data(),
               Xmm[Depth++], node->Name.data());
        return;
    }else if (node ->isStore || node -> Type ->IsPtrCharType()){
        std::string constName = std::string(node->Name);
        printf("\t  lea %s(%%rip),%%rax     #Constant %s\n",constName.data(),constName.data());
        return;
    }
    if (!node -> Type -> IsStringType() && !node -> Type -> IsStructType() && !node -> Type -> IsArrayType())
        printf("\t  mov $%s, %%rax   #Constant %s\n",node->GetValue().c_str(),node->GetValue().c_str());
}

void BDD::CodeGenerate::Visitor(BDD::ExprStmtNode *node) {
    if (node->Lhs){
        node->Lhs ->Accept(this);
    }
}

void CodeGenerate::Visitor(ExprVarNode *node) {
    if (node -> Type -> IsPointerType()){
        printf("\t  mov %d(%%rbp),%%rax\n",node -> VarObj -> Offset);
    }else if(node -> Type -> IsStructType()){
        GenerateAddress(node);
    }else{
        GenerateAddress(node);
        Load(node);
    }
}

void printConstant(int size, bool isFloat,bool isString,std::shared_ptr<Token> token)
{
    if (isString){
        auto s_string =  std::string(token->Content).c_str();
        printf("\t.string  %s\n", s_string);
    }else if (isFloat){
        if (size == 4 ) {
                auto s_num = std::string(token->Content).c_str();
                float d_num = atof(s_num);
                int *lp_num = (int *) &d_num;
                printf("\t.long  %s\n", convert_to_hex(*lp_num).data());
        } else if (size == 8) {
                auto s_num = std::string(token->Content).c_str();
                double d_num = atof(s_num);
                long *lp_num = (long *) &d_num;
                printf("\t.quad  %s\n", convert_to_hex(*lp_num).data());
        }
    }else{
        if (size == 1 ) {
            printf("\t.byte  %d\n", (unsigned int)token->Value);
        }else if (size == 2 ) {
            printf("\t.short  %d\n", (unsigned int)token->Value);
        }else if (size == 4 ) {
            printf("\t.long  %d\n", (unsigned int)token->Value);
        } else if (size == 8) {
            printf("\t.quad  %ld\n", token ->Value);
        }
    }
    return;
}

void ParseInit(std::shared_ptr<ConstantNode> node){
    int size = 0 ;
    bool isFloat = false;
    bool isString = false;
    int offset = 0;
    bool skip = false;
    auto root = node;
    if ( node ->Type  ->IsStringType() || node -> Type -> IsArrayType() || node -> Type -> IsStructType()) {
        if (node->Type->Size <= 48 ){
            skip = true;
        }
    }
    if (node->isRoot){
        node = node ->Next;
    }
    while (node) {
        if(!(node -> Type ->IsPtrCharType() || !skip)){
            node = node ->Next;
            continue;
        }
        //single value direct loading mov $num,%rax
        isFloat = node->Type->IsFloatPointNum();
        size = node ->Type ->Size;
        isString = node->Type->IsPtrCharType();
        offset += node ->Type ->Size;
        int gap = AlignTo(offset,node->Type->Align) - offset;
        offset += gap;
        if (gap != 0)
            printf("\t.zero  %d\n",gap);
        if (node -> Sub){
            ParseInit(node ->Sub);
        }else{
            printConstant(size,isFloat , isString,node ->Token);
        }
        node -> isStore = true;
        node = node ->Next;
        root ->isStore = true;
    }
}

void CodeGenerate::Visitor(ProgramNode *node) {
    for (auto &v: scope->Scope::GetInstance()->GetConstantTable()) {
        if (v .second ->Next == nullptr && v .second ->Type->IsIntegerNum()){
            continue;
        }
        //if arry or struct | array | string size <= 48 direct mov instance value to init not store in data
        printf("%s:\n", v .second -> Name.data());
        ParseInit(v.second);
    }
    for (auto &s: node->Funcs)
        s->Accept(this);
}

void CodeGenerate::Visitor(IfStmtNode *node) {
    int n = Sequence ++;
    node -> Cond ->Accept(this);
    printf("\t  cmp $0,%%rax\n");
    if (node -> Else){
        printf("\t  je .L.else_%d\n",n);
    }else{
        printf("\t  je .L.end_%d\n",n);
    }
    node -> Then->Accept(this);
    printf("\t jmp .L.end_%d\n",n);
    if (node -> Else){
        printf(".L.else_%d:\n",n);
        node ->Else->Accept(this);
        printf("\t jmp .L.end_%d\n",n);
    }
    printf(".L.end_%d:\n",n);
}

void CodeGenerate::Visitor(BlockStmtNode *node) {
    for (auto &s:node->Stmts) {
        s ->Accept(this);
    }
}

void CodeGenerate::Visitor(WhileStmtNode *node) {
    int n = Sequence++;
    std::string EndLabel= string_format(".L.end_%d",n);
    std::string BeginLabel = string_format(".L.begin_%d",n);
    PushBreak(EndLabel);
    PushContinue(BeginLabel);
    printf("\t.L.begin_%d:\n",n);
    node -> Cond ->Accept(this);
    printf("\t  cmp $0,%%rax\n");
    printf("\t  je .L.end_%d\n",n);
    node -> Then ->Accept(this);
    printf("\t  jmp .L.begin_%d\n",n);
    printf("\t.L.end_%d:\n",n);
    PopContinue();
    PopBreak();
}

void CodeGenerate::Visitor(DoWhileStmtNode *node) {
    int n = Sequence ++;
    std::string EndLabel= string_format(".L.end_%d",n);
    std::string CondLabel = string_format(".L.cond_%d",n);
    PushBreak(EndLabel);
    PushContinue(CondLabel);
    printf(".L.begin_%d:\n",n);
    node -> Stmt -> Accept(this);
    printf(".L.cond_%d:\n",n);
    node -> Cond ->Accept(this);
    printf("\t  cmp $0, %%rax\n");
    printf("\t  je .L.end_%d\n",n);
    printf("\t  jmp .L.begin_%d\n",n);
    printf(".L.end_%d:\n",n);
    PopContinue();
    PopBreak();
}

void CodeGenerate::Visitor(ForStmtNode *node) {
    int n  = Sequence++;
    std::string EndLabel = string_format(".L.end_%d",n);
    std::string IncrLabel = string_format(".L.incr_%d",n);
    PushBreak(EndLabel);
    PushContinue(IncrLabel);
    if (node -> Init)
        node -> Init ->Accept(this);
    printf(".L.begin_%d:\n",n);
    if (node -> Cond) {
        node->Cond->Accept(this);
        printf("\t  cmp $0,%%rax\n");
        printf("\t  je .L.end_%d\n", n);
    }

    node -> Stmt ->Accept(this);
    printf(".L.incr_%d:\n",n);
    if (node -> Inc){
        node -> Inc ->Accept(this);
    }
    printf("\t  jmp .L.begin_%d\n",n);
    printf(".L.end_%d:\n",n);
    PopContinue();
    PopBreak();
}

void CodeGenerate::Visitor(FunctionNode *node) {
    CurrentFuncName = node -> FuncName;
    printf(".text\n");
#ifdef __linux__
    printf(".globl %s\n",name.data());
    printf("%s:\n",name.data());
#else
    ///macos
    printf(".globl _%s\n",CurrentFuncName.data());
    printf("_%s:\n",CurrentFuncName.data());
#endif
    int offset = 0;
    for (auto &v: node -> Locals) {
        offset += v ->Type ->Size;
        offset = AlignTo(offset,v -> Type -> Align);
        v -> Offset -= offset;
    }
    offset = AlignTo(offset,16);

    printf("\t  push %%rbp\n");
    printf("\t  mov %%rsp, %%rbp\n");
    if (offset > 0 ){
        printf("\t  sub $%d, %%rsp\n",offset); //set stack top
    }
    auto index = 0;
    for (auto &var: node->Params){
        if (var->Type->IsFloatPointNum()){
            printf("\t %s %s, %d(%%rbp)\n", GetMoveCode(var->Type).data(), Xmm[Depth++], var -> Offset);
        }else if (var->Type->IsIntegerNum()){
                printf("\t  mov %s, %d(%%rbp)\n",Regx64[var -> Type -> Size / 2][index++],var -> Offset );
        }else{
            assert(0);
        }
    }
    for (auto &s:node->Stmts) {
        s ->Accept(this);
        assert(StackLevel == 0);
    }
    printf(".LReturn_%s:\n",CurrentFuncName.data());
    printf("\t  mov %%rbp,%%rsp\n");
    printf("\t  pop %%rbp\n");
    printf("\t  ret \n");
}


void CodeGenerate::Visitor(FuncCallNode *node) {
    for(auto &arg:node -> Args){
        arg ->Accept(this);
        if (!arg->Type->IsFloatPointNum()){
            Push(arg->Type);
        }
    }
    for (int i = node-> Args.size() -1; i >= 0; --i) {
        if (node->Args[i]->Type->IsFloatPointNum()){}

        else if (node->Args[i]->Type->IsPointerType() || node->Args[i]->Type->IsStructType() || node->Args[i]->Type->IsArrayType()){
            Pop(node ->Args[i]->Type, Regx64[4][i]);
        }else{
            Pop(node ->Args[i]->Type, Regx64[node ->Args[i]->Type->GetBaseType()->Size / 2][i]);
            //todo push float to stack
        }

    }
    Depth = 0;
    std::string FuncName(node->FuncName);
#ifdef __linux__
    printf("\t  call %s\n",FuncName.data());
#else
    printf("\t  call _%s\n",FuncName.data());
#endif
}

void CodeGenerate::Visitor(ReturnStmtNode *node) {
    node -> Lhs -> Accept(this);
    printf("\t  jmp .LReturn_%s\n",CurrentFuncName.data());
}

void CodeGenerate::Visitor(DeclarationStmtNode *node) {}



void CodeGenerate::Visitor(StmtExprNode *node) {
    for (auto &s : node ->Stmts) {
        s ->Accept(this);
    }
}

void CodeGenerate::Visitor(UnaryNode *node) {
    switch (node ->Uop) {
        case UnaryOperator::Plus:
            node -> Lhs ->Accept(this);
            break;
        case UnaryOperator::Minus:
            node -> Lhs ->Accept(this);
            printf("\t  neg %s\n", GetRax(node -> Lhs -> Type).data());
            break;
        case UnaryOperator::Deref:
            node -> Lhs ->Accept(this);
            Load(node -> Lhs);
            break;
        case UnaryOperator::Addr:
            GenerateAddress(node -> Lhs.get());
            break;
    }
}

void CodeGenerate::GenerateAddress(AstNode *node) {
    while (auto castNode = dynamic_cast<CastNode *>(node)){
        node = castNode->CstNode.get();
    }
    if (auto varNode = dynamic_cast<ExprVarNode *>(node)){
        printf("\t  lea %d(%%rbp),%%rax\n",varNode -> VarObj -> Offset);
    }else if(auto constNode = dynamic_cast<ConstantNode *>(node)){
        std::string constName =  std::string(constNode->Name);
        printf("\t  lea %s(%%rip),%%rax\n",constName.data());
    }else if (auto unaryNode = dynamic_cast<UnaryNode *>(node)){
        if (unaryNode -> Uop == UnaryOperator::Deref){
            unaryNode ->Lhs->Accept(this);
        }else{
            printf("unaryNode must be defer!\n");
            assert(0);
        }
    }else if (auto memberAccessNode = dynamic_cast<MemberAccessNode *>(node)){
        auto record = std::dynamic_pointer_cast<RecordType>(memberAccessNode ->Lhs -> Type);
        memberAccessNode -> Lhs ->Accept(this);
        auto field = record -> GetField(memberAccessNode -> fieldName);
        printf("\t  add  $%d,%%rax\n", field ->Offset);
    }else if (auto arefNode = dynamic_cast<ArefNode *>(node)){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(arefNode ->Lhs);
        arefNode -> Offset ->Accept(this);
        if (arefNode ->Offset ->Type ->Size == Type::IntType ->Size){
            printf("\t  cdqe\n");
        }
        if (arefNode ->Lhs ->Type ->IsPointerType()){
            Push(Type::LongType);
            arefNode ->Lhs ->Accept(this);
            Pop(Type::LongType, "%rcx");
            printf("\t  lea (%%rax,%%rcx,%d),%%rax\n",node-> Type->GetBaseType()->Size);
        }else{
            printf("\t  lea %d(%%rbp,%%rax,%d),%%rax\n",varNode ->VarObj ->Offset,node-> Type->GetBaseType()->Size);
        }
    }else{

        printf("not a value\n");
        assert(0);
    }
}

void CodeGenerate::Visitor(SizeOfExprNode *node) {
    printf("\t  mov $%d,%%rax   #sizeof %d\n",node -> Lhs -> Type  -> Size,node -> Lhs -> Type  -> Size);
}

void CodeGenerate::Visitor(DeclarationAssignmentStmtNode *node) {
    for (auto &n:node ->AssignNodes) {
        n ->Accept(this);
    }
}

void CodeGenerate::Load(AstNode *node) {
    auto type = node -> Type;
    if (auto castNode = dynamic_cast<CastNode *>(node)){
        type = castNode ->CstNode ->Type;
    }
    Load(type);
}

void CodeGenerate::Load(std::shared_ptr<AstNode> node) {
    auto type = node -> Type;
    auto cursor = node;
    while (auto castNode = std::dynamic_pointer_cast<CastNode>(cursor)){ //int a = 0; in b = 3;(long) a + long(b)  load from memory a is int
        cursor = castNode ->CstNode;
    }
    Load(cursor ->Type);
}

void CodeGenerate::Load(std::shared_ptr<Type> type){
    if (type->IsFloatPointNum() || (type->GetBaseType() != nullptr && type->GetBaseType()->IsFloatPointNum())){
        printf("\t  %s (%%rax),%s\n", GetMoveCode(type ->GetBaseType()).data(),Xmm[Depth++]);
        return;
    }else if(type -> IsPointerType()){
        printf("\t  mov (%%rax),%%rax\n");
        return;
    }else{
        printf("\t  mov (%%rax),%s\n", GetRax(type ->GetBaseType()->Size).data());
    }
}


void CodeGenerate::Store(std::shared_ptr<AstNode> node) {
    std::shared_ptr<AstNode> cursor = node;
    std::shared_ptr<Type> type;
    while (auto castNode = std::dynamic_pointer_cast<CastNode>(cursor)){
        cursor = castNode ->CstNode;
    }
    if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(cursor)){
        if (constNode -> isRoot){
            constNode = constNode ->Next;
        }
        while(constNode){
            if (constNode -> isStore){
                if (constNode-> Type-> IsPtrCharType()){
                    printf("\t  mov %%rax,%d(%%rdi)\n",constNode -> Offset);
                }else{
                    printf("\t  mov $%d,%%rcx\n",cursor->Type->Size);
                    printf("\t  call _mempcy\n");
                    return;
                }
            }else{
                if (constNode-> Type-> IsPointerType()){
                    assert(0);
                }else if (constNode->Type->IsFloatPointNum()){
                    printf("\t  %s %s,(%%rdi)\n", GetMoveCode(constNode-> Type).data(),Xmm[Depth-1]);
                }else if(constNode->Type->IsStringType()){
                    auto iter =  Str2IntArrayIterator(constNode->Token->Content);
                    auto offset = 0;
                    while(iter.has_next()){
                        auto outPutIntNode = iter.next();
                        printf("\t  %s $%lu,%s\n", GetMoveCode(outPutIntNode.Size).data(),outPutIntNode.Value, GetRax(outPutIntNode.Size).data());
                        printf("\t  %s %s,%d(%%rdi)\n",  GetMoveCode(outPutIntNode.Size).data(), GetRax(outPutIntNode.Size).data(), offset);
                        offset += outPutIntNode .Size;
                    }
                }else if (!cursor -> Type -> IsStringType() && !cursor -> Type -> IsStructType() && !cursor -> Type -> IsArrayType()){
                    printf("\t  mov %s,(%%rdi)\n",GetRax(constNode-> Type).data());
                }else{
                    printf("\t  %s  $%s,%d(%%rdi)\n", GetMoveCode2(constNode->Type).data(), constNode->GetValue().c_str(), constNode -> Offset);
                }
            }
            constNode = constNode->Next;
        }
        return;
    }else if (auto exprNode = std::dynamic_pointer_cast<ExprVarNode>(cursor)){
        type = exprNode->Type;
    }else if(auto binaryNode = std::dynamic_pointer_cast<BinaryNode>(cursor)){
        type = binaryNode->Type;
    }else if(auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(cursor)){
        type = unaryNode->Type;
    }else if(auto funcCallNode = std::dynamic_pointer_cast<FuncCallNode>(cursor)){
        type = funcCallNode->Type;
    }
    if (type ->  IsStructType() || type ->  IsArrayType()){
        printf("\t  mov $%d,%%rcx\n",type ->Size);
        printf("\t  call _mempcy\n");
        return;
    }else if(type->IsFloatPointNum()){
        printf("\t  %s %s,(%%rdi)\n", GetMoveCode(type).data(),Xmm[Depth-1]);
        return;
    }else if (type -> IsPointerType()){
        printf("\t  mov %%rax,(%%rdi)\n");
        return;
    }else{
        printf("\t  mov %s,(%%rdi)\n",GetRax(type).data());
        return;
    }
    assert(0);
}

void CodeGenerate::Visitor(MemberAccessNode *node) {
    auto record = std::dynamic_pointer_cast<RecordType>(node ->Lhs ->Type ->GetBaseType());
    GenerateAddress(node->Lhs.get());
    auto field = record -> GetField(node -> fieldName);
    printf("\t  add  $%d,%%rax\n", field ->Offset);
    Load(field ->type);
}

void CodeGenerate::Visitor(BreakStmtNode *node) {
    std::string BreakLabel = std::string(currentBreakTarget());
    printf("\t  jmp %s \n",BreakLabel.data());
}

void CodeGenerate::Visitor(ContinueStmtNode *node) {
    std::string ContinueLabel = std::string(currentContinueTarget());
    printf("\t  jmp %s \n",ContinueLabel.data());
}


void CodeGenerate::PushBreak(std::string_view label) {
    BreakStack.push_back(label);
}

void CodeGenerate::PopBreak() {
    BreakStack.pop_back();
}

std::string_view CodeGenerate::currentBreakTarget() {
    return BreakStack.back();
}

void CodeGenerate::PushContinue(std::string_view label) {
    ContinueStack.push_back(label);
}

void CodeGenerate::PopContinue() {
    ContinueStack.pop_back();
}

std::string_view CodeGenerate::currentContinueTarget() {
    return ContinueStack.back();
}

const std::string CodeGenerate::GetMoveCode(int size) {
    if (size == 1){
        return "movb";
    }else if (size == 2){
        return "movw";
    }else if (size == 4){
        return "movl";
    }else if (size  == 8){
        return "movq";
    }
    assert(0);
}

const std::string CodeGenerate::GetMoveCode(std::shared_ptr<Type>  type) {
    if (type->IsFloatPointNum()){
        if (type -> Size == 4){
            return "movss";
        }else if (type -> Size == 8){
            return "movsd";
        }
    }else if (type->IsIntegerNum()){
        if (type -> Size == 1){
            return "movsb";
        }else if (type -> Size == 2){
            return "movsw";
        }else if (type -> Size == 4){
            return "movsl";
        }else if (type -> Size == 8){
            return "movsq";
        }
    }
    assert(0);
}

const std::string CodeGenerate::GetMoveCode2(std::shared_ptr<Type>  type) {
    if (type->IsFloatPointNum()){
        if (type -> Size == 4){
            return "movss";
        }else if (type -> Size == 8){
            return "movsd";
        }
    }else if (type->IsIntegerNum()){
        if (type -> Size == 1){
            return "movb";
        }else if (type -> Size == 2){
            return "movw";
        }else if (type -> Size == 4){
            return "movl";
        }else if (type -> Size == 8){
            return "movq";
        }
    }
    assert(0);
}

void CodeGenerate::Visitor(CastNode *node) {
    node->CstNode->Accept(this);
    if (node ->Type == node ->CstNode ->Type || node ->Type ->Alias == node -> CstNode ->Type ->Alias ){
        return;
    }
    auto fromTo = string_format("%s->%s", node->CstNode->Type->Alias, node->Type->Alias);
    auto castCode = GetCastCode(fromTo);
    if (castCode == "NULL"){
        return;
    }else if (castCode == ""){
        printf("%s code not exists!\n",fromTo.data());
        return;
    }
    if (is_contains_str(castCode,"%xmm0")){
        string_replace(castCode,"%xmm0",Xmm[Depth++]);
    }
    printf("\t  %s \n",castCode.data());
}


const std::string CodeGenerate::GetIDivCode(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "idivb";
    }else if (type -> Size == 2){
        return "idivw";
    }else if (type -> Size == 4){
        return "idivw";
    }else if (type -> Size == 8){
        return "idivq";
    } else{
        assert(0);
    }
}


const std::string CodeGenerate::GetDivCode(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "divb";
    }else if (type -> Size == 2){
        return "divw";
    }else if (type -> Size == 4){
        return "divw";
    }else if (type -> Size == 8){
        return "divq";
    } else{
        assert(0);
    }
}


void CodeGenerate::Push(std::shared_ptr<Type> ty) {
    auto size = ty ->Size;
    if (ty ->IsAliasType()){
        ty = std::dynamic_pointer_cast<AliasType>(ty)->Base;
    }
    if (ty ->IsStructType() || ty ->IsPointerType() || ty ->IsArrayType() || ty->IsPtrCharType() ){
        size = Type::VoidType->Size;
    }
    printf("\t  sub $%d, %%rsp          #Push %s\n",size,GetRax(ty).data());
    printf("\t  mov %s,(%%rsp)\n",GetRax(ty).data());
    StackLevel ++;

}

void CodeGenerate::Pop(std::shared_ptr<Type> ty) {
    printf("\t  mov(%%rsp),%s           #Pop %s\n",GetRax(ty).data(),GetRax(ty).data());
    StackLevel --;
    if (ty->IsStructType() || ty ->IsArrayType() || ty -> IsPointerType()){
        printf("\t  add $%d, %%rsp\n",Type::VoidType->Size);
        return;
    }
    printf("\t  add $%d, %%rsp\n",ty->Size);
}


void CodeGenerate::Pop(std::shared_ptr<Type> ty,const char *reg) {
    printf("\t  mov(%%rsp),%s           #Pop %s\n",std::string(reg).data(),std::string(reg).data());
    StackLevel --;
    if (ty->IsStructType() || ty ->IsArrayType() || ty -> IsPointerType()){
        printf("\t  add $%d, %%rsp\n",Type::VoidType->Size);
        return;
    }
    printf("\t  add $%d, %%rsp\n",ty->Size);
}

const std::string CodeGenerate::GetRax(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%al";
    }else if (type -> Size == 2){
        return "%ax";
    }else if (type -> Size == 4){
        return "%eax";
    }else if (type -> Size>= 8){
        return "%rax";
    } else{
        assert(0);
    }
}

const std::string CodeGenerate::GetRcx(int size) {
    if (size == 1){
        return "%cl";
    }else if (size == 2){
        return "%cx";
    }else if (size == 4){
        return "%ecx";
    }else if (size == 8){
        return "%rcx";
    } else{
        assert(0);
    }
}


const std::string CodeGenerate::GetRcx(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%cl";
    }else if (type -> Size == 2){
        return "%cx";
    }else if (type -> Size == 4){
        return "%ecx";
    }else if (type -> Size == 8){
        return "%rcx";
    } else{
        assert(0);
    }
}


void CodeGenerate::Visitor(ArefNode *node) {
    auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node ->Lhs);
    node -> Offset ->Accept(this);
    if (node ->Offset ->Type ->Size == Type::IntType ->Size){
        printf("\t  cdqe\n");
    }
    if (node ->Lhs ->Type ->IsPointerType()){
        Push(Type::LongType);
        node ->Lhs ->Accept(this);
        Pop(Type::LongType, "%rcx");
        printf("\t  lea (%%rax,%%rcx,%d),%%rax\n",node-> Type->GetBaseType()->Size);
        Load(node);
        return;
    }
    printf("\t  lea %d(%%rbp,%%rax,%d),%%rax\n",varNode ->VarObj ->Offset,node-> Type->GetBaseType()->Size);
    Load(node);
}

const std::string CodeGenerate::GetRax(int size) {
    if (size == 1){
        return "%al";
    }else if (size == 2){
        return "%ax";
    }else if (size == 4){
        return "%eax";
    }else if (size == 8){
        return "%rax";
    } else{
        assert(0);
    }
}

std::string BDD::CodeGenerate::GetCastCode(std::string fromTo) {
    if (CastMap.empty()){
        CastMap["i8->i32"] = "movsx %al, %eax";
        CastMap["u8->i32"] = "movzbl %al, %eax";
        CastMap["bool->i32"] = "movzbl %al, %eax";
        CastMap["u8->u32"] =  "movzbl %al, %eax";

        CastMap["i16->i32"] = "movswl %ax, %eax";
        CastMap["u16->u32"] = "movzwl %ax, %eax";
        CastMap["i32->f32"] = "cvtsi2ssl %eax, %xmm0";
        CastMap["i32->i64"] = "movsx %eax, %rax";

        CastMap["i16->u32"] = "movzwl %ax, %eax";

        CastMap["u32->u64"] = "NULL";


        CastMap["i32->i8"] =  "movsx %al, %rax";
        CastMap["i32->i16"] =  "movsx %ax, %rax";
        CastMap["i32->u32"] =  "NULL";

        CastMap["i32->u64"] =  "NULL";
        CastMap["u64->i32"] =  "NULL";

        CastMap["u16->i32"] =  "movzwl %ax, %eax";

        CastMap["u64->i64"] =  "NULL";
        CastMap["i64->u64"] =  "NULL";

        CastMap["i32->f64"] = "cvtsi2sdl %eax, %xmm0";

        CastMap["u32->f32"] = "mov %eax, %eax; cvtsi2ssq %rax, %xmm0";
        CastMap["u32->i64"] = "movzx %eax, %rax";
        CastMap["u32->f64"] = "movzx %eax, %rax; cvtsi2sdq %rax, %xmm0";

        CastMap["i64->f32"] = "cvtsi2ssq %rax, %xmm0";
        CastMap["i64->f64"] = "cvtsi2sdq %rax, %xmm0";

        CastMap["u64->f32"] = "cvtsi2ssq %rax, %xmm0";

        CastMap["u64->f64"] =
                "test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "
                "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
                "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:";

        CastMap["f32->i8"] = "cvttss2sil %xmm0, %eax; movsbl %al, %eax";
        CastMap["f32->u8"] = "cvttss2sil %xmm0, %eax; movzbl %al, %eax";
        CastMap["f32->i16"] = "cvttss2sil %xmm0, %eax; movswl %ax, %eax";
        CastMap["f32->u16"] = "cvttss2sil %xmm0, %eax; movzwl %ax, %eax";
        CastMap["f32->i32"] = "cvttss2sil %xmm0, %eax";
        CastMap["f32->u32"] = "cvttss2siq %xmm0, %rax";
        CastMap["f32->i64"] = "cvttss2siq %xmm0, %rax";//
        CastMap["f32->u64"] = "cvttss2siq %xmm0, %rax";
        CastMap["f32->f64"] = "cvtss2sd %xmm0, %xmm0";

        CastMap["f64->i8"] = "cvttsd2sil %xmm0, %eax; movsbl %al, %eax";
        CastMap["f64->u8"] = "cvttsd2sil %xmm0, %eax; movzbl %al, %eax";
        CastMap["f64->i16"] = "cvttsd2sil %xmm0, %eax; movswl %ax, %eax";
        CastMap["f64->u16"] = "cvttsd2sil %xmm0, %eax; movzwl %ax, %eax";
        CastMap["f64->i32"] = "cvttsd2sil %xmm0, %eax";//
        CastMap["f64->u32"] = "cvttsd2siq %xmm0, %rax";
        CastMap["f64->f32"] = "cvtsd2ss %xmm0, %xmm0"; //
        CastMap["f64->i64"] = "cvttsd2siq %xmm0, %rax"; //
        CastMap["f64->u64"] = "cvttsd2siq %xmm0, %rax";
    }
    return CastMap[fromTo];
}

const std::string CodeGenerate::GetRdi(std::shared_ptr<Type> type) {
    if (type ->Size == 1){
        return "%dil";
    }else if (type ->Size == 2){
        return "%di";
    }else if (type ->Size == 4){
        return "%edi";
    }else if (type ->Size == 8){
        return "%rdi";
    } else{
        assert(0);
    }
}

void CodeGenerate::Visitor(EmptyNode *node) {
    //do nothing
}
