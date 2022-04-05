//
// Created by qiaojinxia on 2022/3/20.
//

#include "Type.h"
#include <memory>


using namespace BDD;

std::shared_ptr<BuildInType> Type::IntType = std::make_shared<BuildInType>(BuildInType::Kind::Int,4,4 );
std::shared_ptr<BuildInType> Type::CharType = std::make_shared<BuildInType>(BuildInType::Kind::Char,1,1 );
std::shared_ptr<BuildInType> Type::ShortType = std::make_shared<BuildInType>(BuildInType::Kind::Short ,2,2 );
std::shared_ptr<BuildInType> Type::LongType = std::make_shared<BuildInType>(BuildInType::Kind::Long,8,8 );

std::shared_ptr<PointerType> Type::Pointer = std::make_shared<PointerType>(LongType);



bool Type::IsIntegerType() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Int
        ||  build_in_type -> GetKind() == BuildInType::Kind::Char
        ||  build_in_type -> GetKind() == BuildInType::Kind::Short
        ||  build_in_type -> GetKind() == BuildInType::Kind::Long;
    }
    return false;
}

bool Type::IsFunctionType() const {
    return TypeC == TypeClass::FuncType;
}

bool Type::IsPointerType() const {
    return TypeC == TypeClass::PtrType;
}

bool Type::IsArrayType() const {
    return TypeC == TypeClass::AryType;
}

bool Type::IsStructType() const {
    if (TypeC == TypeClass::RecordType){
        auto ry = dynamic_cast<const RecordType *>(this);
        return ry -> Kind  == RecordType::TagKind::Struct;
    }
    return false;
}

bool Type::IsUnionType() const {
    if (TypeC == TypeClass::RecordType){
        auto ry = dynamic_cast<const RecordType *>(this);
        return ry -> Kind  == RecordType::TagKind::Union;
    }
    return false;
}


std::shared_ptr<Field> RecordType::GetField(std::string_view fieldName) {
    for(auto &field:fields){
        if (field->token->Content == fieldName){
            return field;
        }
    }
    return nullptr;
}
