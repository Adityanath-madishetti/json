#include <variant>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include<iostream>

/*
- Json is basically string/int/bool/double/nullptr , array of jsons , map of jsons 

*/


namespace json {

    struct  JSON;

    using JSONArray = std::vector<JSON>;
    using JSONObject = std::map<std::string,JSON>;


    class JSON{

        public:
           using Value = std::variant<
                        std::nullptr_t, // type of nullptr
                        bool,
                        double,
                        std::string,
                        JSONArray,
                        JSONObject
                    >;
            
                
            Value value;

            //constructors are needed for each type
            
            JSON(double number):value(number){}
            JSON(bool v):value(v){}
            JSON(const std::string& s):value(s){}
            JSON(const char* s):value(std::string(s)){}
            JSON() : value(nullptr) {} // default constructor
            JSON(const JSONArray& j) : value(j){}
            JSON(const JSONObject& j):value(j){}

            // overload few operators

            JSON& operator[](const std::string& key); 
            JSON& operator[](size_t index);
            // conversion operators but add later

            size_t size() const;
            bool contains(const std::string& key) const ;


            bool is_null() const;
            bool is_bool() const;
            bool is_number() const;
            bool is_string() const;
            bool is_array() const;
            bool is_object() const;

            bool as_bool() const;
            double as_number() const;
            const std::string& as_string() const;

            JSONArray& as_array();
            JSONObject& as_object();

            const JSONArray& as_array() const;
            const JSONObject& as_object() const;

            JSONObject& JSON::items();
            const JSONObject& items() const;
    
    };


        class Parser{
            private:
            std::string input ;
            size_t pos;
            int lineno;
            int colno; // as of now ignore this

            public:
            Parser(const std::string& s);

            private:
            JSON parse_value() ;
            JSON parse_array() ;
            JSON parse_object()  ;
            JSON parse_string()  ;
            JSON parse_null()  ;
            JSON parse_number() ;
            JSON parse_true() ;
            JSON parse_false() ;
            char peek() ;
            char get();
            void skipWhitespaces() ;
            void expect(char c);
            public:
               JSON parse(); 
             

        };
    

        JSON Parse(const std::string& input);

        
        class Formatter{
            public: 
            virtual std::string format(const JSON&j )=0;
            virtual ~Formatter()=default;
        };

        class PrettyFormatter : public Formatter{
            int indent; // interms of tas
            bool sort_keys;
            std::string space_indent(int depth);
            std::string format_impl(const JSON& j,int depth);
            public:
            PrettyFormatter(int indent=2,bool sort_keys=false);
            std::string format(const JSON&j) override;

        };
    

}