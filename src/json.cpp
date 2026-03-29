#include"../include/json.hpp"
#include<iostream>
using namespace std;
namespace json{



    
            bool JSON::is_null()   const { return std::holds_alternative<std::nullptr_t>(value); }
            bool JSON::is_bool()   const { return std::holds_alternative<bool>(value); }
            bool JSON::is_number() const { return std::holds_alternative<double>(value); }
            bool JSON::is_string() const { return std::holds_alternative<std::string>(value); }
            bool JSON::is_array()  const { return std::holds_alternative<JSONArray>(value); }
            bool JSON::is_object() const { return std::holds_alternative<JSONObject>(value); }


            bool JSON::as_bool() const {
                return std::get<bool>(value);
            }

            double JSON::as_number() const {
                return std::get<double>(value);
            }

            const std::string&  JSON::as_string() const {
                return std::get<std::string>(value);
            }

            JSONArray& JSON::as_array() {
                return std::get<JSONArray>(value);
            }

            JSONObject& JSON::as_object() {
                return std::get<JSONObject>(value);
            }

            const JSONArray& JSON::as_array() const {
                return std::get<JSONArray>(value);
            }

            const JSONObject& JSON::as_object() const {
                return std::get<JSONObject>(value);
            }


            JSONObject& JSON::items() {
                if (!is_object())
                    throw std::runtime_error("Not an object");

                return std::get<JSONObject>(value);
            }

            const JSONObject& JSON::items() const {
                if (!is_object())
                    throw std::runtime_error("Not an object");

                return std::get<JSONObject>(value);
            }
    

        Parser::Parser(const std::string& str){
            input=str;
            pos=0;
            lineno=1;
        }

     JSON Parse(const std::string& input) {
            Parser p(input);
            return p.parse();
        }


    JSON& JSON::operator[](const std::string& key){
        if(!is_object()){
            throw std::runtime_error("Not an object");
        }
         return std::get<JSONObject>(value)[key]; // creates if not exists
     }

     JSON& JSON::operator[](size_t index){
        if(!is_array()){
            throw std::runtime_error("Not an array");
        }
        auto& arr = std::get<JSONArray>(value);
        if(index>=arr.size() or index<0){
            throw std::runtime_error("index out of bounds");
        }
        return arr[index];
     }
    
     size_t JSON::size() const {
        if (is_array())
            return std::get<JSONArray>(value).size();

        if (is_object())
            return std::get<JSONObject>(value).size();

        throw std::runtime_error("Size not applicable");
    }

    bool JSON::contains(const std::string& key) const {
        if (!is_object())
            return false;

        const auto& obj = std::get<JSONObject>(value);
        return obj.find(key) != obj.end();
    }


    JSON Parser::parse_value() {

        
        
        skipWhitespaces();
        auto ch =peek();


        if(ch=='{'){
            return parse_object();
        }else if(ch=='['){
            return parse_array();
        }else if(ch=='"'){
            return parse_string();              
        }else if(ch=='t'){
            return parse_true();
        }else if (ch=='f'){
            return parse_false();
        }else if(ch=='n'){
            return parse_null();
        }else if(isdigit(ch) or ch=='-'){
            return parse_number();
        }else{
            throw std::runtime_error("invalid TYPE in JSON");
        }
        // imposible to reach  here

        return JSON();

    }

    JSON Parser::parse() {
        auto final_result = parse_value();
        skipWhitespaces();
        if (pos != input.size())
            throw std::runtime_error("Extra data after JSON");
        return final_result;
    }

    char Parser::peek() {
        if (pos >= input.size()) return '\0';
        return input[pos];
    }

    char  Parser::get(){
        if(pos>=input.size()) throw std::runtime_error("no more input available");
        colno++;
        return input[pos++];
    }

    void Parser::expect(char ch) {
        char c = get();

        if (c != ch) {
            throw std::runtime_error(
                std::string("line :"+ std::to_string(lineno)+"column : " +std::to_string(colno)+ " Unexpected character: expected '") +
                ch +
                "', but got '" +
                std::string(1, c) +
                "'"
            );
        }
    }

    // peek just gives current pointing char without increamenting 
    // get gives by incremeanting
        JSON Parser::parse_number() {

            std::string number;

            //  sign
            if (peek() == '-') {
                number += get();
            }

            //  integer part
            if (peek() == '0') {
                number += get();

                if (std::isdigit(peek())) {
                    throw std::runtime_error("Leading zeros not allowed");
                }

            }else if (std::isdigit(peek())) {
                while (std::isdigit(peek())) {
                    number += get();
                }
            }else {
                throw std::runtime_error("Invalid number");
            }

            //  fractional part
            if (peek() == '.') {
                number += get();

                if (!std::isdigit(peek())) {
                    throw std::runtime_error("Expected digits after decimal point");
                }

                while (std::isdigit(peek())) {
                    number += get();
                }
            }

            //  exponent part
            if (peek() == 'e' || peek() == 'E') {
                number += get(); // consume e/E

                if (peek() == '+' || peek() == '-') {
                    number += get();
                }

                if (!std::isdigit(peek())) {
                    throw std::runtime_error("Expected digits in exponent");
                }

                while (std::isdigit(peek())) {
                    number += get();
                }
            }

            return JSON(std::stod(number));
        }


    JSON Parser::parse_string() {
    get(); // consume opening "

    std::string collection;

    // 🔹 hex checker
    auto is_hex = [](char c) {
        return (c >= '0' && c <= '9') ||
               (c >= 'a' && c <= 'f') ||
               (c >= 'A' && c <= 'F');
    };

    // 🔹 convert codepoint → UTF-8
    auto append_utf8 = [&](int codepoint) {
        if (codepoint <= 0x7F) {
            collection += static_cast<char>(codepoint);
        } else if (codepoint <= 0x7FF) {
            collection += static_cast<char>(0xC0 | (codepoint >> 6));
            collection += static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0xFFFF) {
            collection += static_cast<char>(0xE0 | (codepoint >> 12));
            collection += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            collection += static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0x10FFFF) {
            collection += static_cast<char>(0xF0 | (codepoint >> 18));
            collection += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            collection += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            collection += static_cast<char>(0x80 | (codepoint & 0x3F));
        } else {
            throw std::runtime_error("Invalid Unicode codepoint");
        }
    };

    while (true) {
        char ch = get();

        // 🔴 End of string
        if (ch == '"') {
            return JSON(collection);
        }

        // 🔴 Control chars not allowed
        if ((unsigned char)ch <= 0x1F) {
            throw std::runtime_error("Control character in JSON string");
        }

        // 🔴 Escape handling
        if (ch == '\\') {
            char next_ch = get();

            switch (next_ch) {
                case '"':  collection += '"';  break;
                case '\\': collection += '\\'; break;
                case '/':  collection += '/';  break;
                case 'b':  collection += '\b'; break;
                case 'f':  collection += '\f'; break;
                case 'n':  collection += '\n'; break;
                case 'r':  collection += '\r'; break;
                case 't':  collection += '\t'; break;

                case 'u': {
                    // 🔹 read 4 hex digits
                    auto read_hex4 = [&]() {
                        int value = 0;
                        for (int i = 0; i < 4; i++) {
                            if (peek() == '\0')
                                throw std::runtime_error("Unexpected end in \\u escape");

                            char h = get();

                            if (!is_hex(h))
                                throw std::runtime_error("Invalid hex digit in \\u escape");

                            value = value * 16 + std::stoi(std::string(1, h), nullptr, 16);
                        }
                        return value;
                    };

                    int code = read_hex4();

                    // 🔴 Surrogate pair handling
                    if (code >= 0xD800 && code <= 0xDBFF) {
                        // must be followed by \uXXXX
                        if (get() != '\\' || get() != 'u') {
                            throw std::runtime_error("Invalid surrogate pair");
                        }

                        int low = read_hex4();

                        if (low < 0xDC00 || low > 0xDFFF) {
                            throw std::runtime_error("Invalid low surrogate");
                        }

                        // combine
                        int full_code =
                            0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);

                        append_utf8(full_code);
                    }
                    else if (code >= 0xDC00 && code <= 0xDFFF) {
                        throw std::runtime_error("Unexpected low surrogate");
                    }
                    else {
                        append_utf8(code);
                    }

                    break;
                }

                default:
                    throw std::runtime_error(
                        std::string("Invalid escape sequence: \\") + next_ch
                    );
            }
        }
        else {
            collection += ch;
        }
    }
}



    JSON Parser::parse_array(){
        get();// consume [
        
        JSONArray j;

        skipWhitespaces();
        if(peek()==']'){
            get(); // consume ']'
            return JSON(j);
        }

        while(true){
            
            skipWhitespaces();
            auto value = parse_value();
            skipWhitespaces();
            j.push_back(value);
           
            if(peek()==']'){
                get();
                break;
            }

            expect(',');
            
        }

        return j;

    }


    JSON Parser::parse_object(){
        get(); //consume {
        skipWhitespaces();
        JSONObject j;
        // empty object 
        if(peek()=='}'){
                get();
                return JSON(j);
        }

        while(true){
            skipWhitespaces();
            if(peek()!='"'){
                //throw appropriate error
                throw std::runtime_error("Invalid key in object");
            }
            auto str_value = parse_string(); // it returns json pacjed string cause this function is menat to parse and return json(string)
            skipWhitespaces();
            expect(':');
            skipWhitespaces();
            auto value = parse_value();
            skipWhitespaces();
            j[str_value.as_string()]=value;
            if(peek()=='}'){
                get();
                break;
            }
            expect(',');
        }

        return j;
         
    }



    JSON Parser::parse_true(){
        expect('t');expect('r');expect('u');expect('e');
        return JSON(true);
    }

    JSON Parser::parse_false(){
        expect('f');expect('a');expect('l');expect('s');expect('e');
        return JSON(false);
    }

    JSON Parser::parse_null(){
        expect('n');expect('u');expect('l');expect('l');

        return JSON();
    }

    void Parser::skipWhitespaces() {
        while (pos < input.size() && std::isspace(static_cast<unsigned char>(input[pos]))) {
            if(input[pos]=='\n'){
                lineno++;
                colno=0;
            }
            colno++;
            pos++;
        }
    }

       





}