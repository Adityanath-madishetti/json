#include"../include/json.hpp"
#include<string>
namespace json{
    PrettyFormatter::PrettyFormatter(int indent=2,bool sort_keys=false){
        this->indent=indent;
        this->sort_keys=sort_keys;
    }
    std::string PrettyFormatter::format(const JSON& j){
        return format_impl(j,0);
    }

    std::string PrettyFormatter::format_impl(const JSON&j , int depth){
        
    }

    std::string PrettyFormatter::space_indent(int depth){
        return std::string(depth*this->indent,' ');
    }
}