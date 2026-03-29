#include"../include/json.hpp"
#include<iostream>
int main(){
//    std::string s = R"|(
// {
//     "key1": "only one obj in this",
//     "key2": 3
// }
// )|";


   std::string s = R"JSON(
{
  "name": "Aditya",
  "age": 21,
  "isStudent": true,
  "skills": ["C++", "Go", "Python"],
  "projects": [
    {
      "title": "JSON Parser",
      "completed": true,
      "lines": 1200
    },
    {
      "title": "CLI Todo",
      "completed": false,
      "lines": 300
    }
  ],
  "details": {
    "cgpa": 8.5,
    "active": true,
    "notes": "Loves systems programming\nand low-level work"
  },
  "emptyArray": [],
  "emptyObject": {},
  "nullField": null
}
)JSON";


   json::JSON j =  json::Parse(s);
   // auto projects = j["projects"].as_array();
   // std::cout<<projects[0]["lines"].as_number()<<"\n";

   json::PrettyFormatter fmt(4);
   std::cout<<fmt.format(j);
}