//
// Created by qiaojinxia on 2022/4/4.
//

#ifndef BODDY_SCOPE_H
#define BODDY_SCOPE_H
#include <stdio.h>
#include "AstNode.h"
#include "Type.h"
#include <unordered_map>

namespace BDD{
    class Scope {
    private:
        class ScopeItem{
        public:
            std::unordered_map<std::string_view,std::shared_ptr<Var>> VarScope;
            std::unordered_map<std::string_view,std::shared_ptr<Type>> TypeScope;
        };
    private:
        std::list<std::shared_ptr<ScopeItem>> scopes{};
    public:
        void EnterScope();
        void LeaveScope();

        void PushVar(std::shared_ptr<Var> var);
        std::shared_ptr<Var> FindVar(std::string_view varName);
        std::shared_ptr<Var> FindVarInCurrentScope(std::string_view name);


        void PushTag(std::string_view tagName, std::shared_ptr<Type> tag);
        std::shared_ptr<Type> FindTag(std::string_view typeName);
        std::shared_ptr<Type> FindTagInCurrentScope(std::string_view tag);

        bool CheckScopeDepthZero();

    };

}


#endif //BODDY_SCOPE_H