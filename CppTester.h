#include <iostream>
#include <regex>
#include <vector>
#include "json.hpp"

#define get_or_zero(JSON, VALUE) ((JSON.is_null() || (JSON.is_number() && JSON.get<int>()  == 0) || (JSON.is_boolean() && JSON.get<bool>() == false)) ? 0 : VALUE)

using namespace std;
using namespace regex_constants;
using json = nlohmann::json;

class CppTester {
public:
    json test_regex(json config, json test_strings) {
        json result;
        string test_type;
        string regex_string;


        if(config["test_type"] == nullptr)
            throw "test_type parameter doesn't exists.";

        if(config["regex"] == nullptr)
            throw "regex parameter doesn't exists.";
        
        result["result"] = json();
        result["result"]["resultList"] = json();

        unsigned int match_flags = 0;
        match_flags = match_flags | get_or_zero(config["match_not_bol"], match_not_bol);
        match_flags = match_flags | get_or_zero(config["match_not_eol"], match_not_eol);
        match_flags = match_flags | get_or_zero(config["match_not_bow"], match_not_bow);
        match_flags = match_flags | get_or_zero(config["match_not_eow"], match_not_eow);
        match_flags = match_flags | get_or_zero(config["match_any"], match_any);
        match_flags = match_flags | get_or_zero(config["match_not_null"], match_not_null);
        match_flags = match_flags | get_or_zero(config["match_continuous"], match_continuous);
        match_flags = match_flags | get_or_zero(config["match_prev_avail"], match_prev_avail);
        match_flags = match_flags | get_or_zero(config["format_default"], format_default);
        match_flags = match_flags | get_or_zero(config["format_sed"], format_sed);
        match_flags = match_flags | get_or_zero(config["format_first_only"], format_first_only);

        unsigned int syntax_flags = 0;
        syntax_flags = syntax_flags | get_or_zero(config["icase"], icase);
        syntax_flags = syntax_flags | get_or_zero(config["nosubs"], nosubs);
        syntax_flags = syntax_flags | get_or_zero(config["optimize"], optimize);
        syntax_flags = syntax_flags | get_or_zero(config["collate"], regex_constants::collate);

        string syntax_type;
        if(config["syntax_type"] != nullptr) {
            syntax_type = config["syntax_type"];
        }

        if(syntax_type == "ECMAScript") {
            syntax_flags = syntax_flags | ECMAScript;
        } else if(syntax_type == "basic") {
            syntax_flags = syntax_flags | basic;
        } else if(syntax_type == "extended") {
            syntax_flags = syntax_flags | extended;
        } else if(syntax_type == "awk") {
            syntax_flags = syntax_flags | awk;
        } else if(syntax_type == "grep") {
            syntax_flags = syntax_flags | grep;
        } else if(syntax_type == "egrep") {
            syntax_flags = syntax_flags | egrep;
        }

        test_type = config["test_type"];
        regex_string = config["regex"];
        regex pattern(regex_string, (syntax_option_type) syntax_flags);

        if(test_type == "match") {
            string subtype = config["test_subtype"];
            if(subtype == "match") {
                result["type"] = "MATCH";
            } else if(subtype == "extract") {
                result["type"] = "GROUP";
                result["result"]["columns"] = json();
            }
            smatch match;
            bool first = true;
            for(string test_string: test_strings) {
                if(subtype == "match") {
                    if(regex_match(test_string, match, pattern, (match_flag_type) match_flags)) {
                        result["result"]["resultList"].push_back(true);
                    } else {
                        result["result"]["resultList"].push_back(false);
                    }
                } else if(subtype == "extract") {
                    if(regex_match(test_string, match, pattern, (match_flag_type) match_flags)) {
                        json groups_list, groups;
                        groups_list["list"] = json();

                        for(int i = 0; i < match.size(); i++) {
                            if(first) {
                                result["result"]["columns"].push_back(string("Group #") + to_string(i));
                            }

                            groups.push_back(string(match[i]));
                        }

                        first = false;
                        groups_list["list"].push_back(groups);
                        result["result"]["resultList"].push_back(groups_list);
                    } else {
                        result["result"]["resultList"].push_back(nullptr);
                    }
                }
            }
        } else if(test_type == "search") {
            result["type"] = "GROUP";
            result["result"]["columns"] = json();
            smatch match;
            bool first = true;

            for(string test_string: test_strings) {
                json groups_list;
                groups_list["list"] = json::array();
                
                string temp_string = test_string;
                while(regex_search(temp_string, match, pattern, (match_flag_type) match_flags)) {
                    json groups;
                    for(int i = 0; i < match.size(); i++) {
                        if(first) {
                            result["result"]["columns"].push_back(string("Group #") + to_string(i));
                        }

                        groups.push_back(string(match[i]));
                    }

                    first = false;
                    groups_list["list"].push_back(groups);
                    temp_string = match.suffix().str();   
                }
                
                if(groups_list["list"].size() > 0) {
                    result["result"]["resultList"].push_back(groups_list);
                } else {
                    result["result"]["resultList"].push_back(nullptr);
                }
            }
        } else if(test_type == "replace") {
            result["type"] = "STRING";
            string replace = config["replace"].is_null() ? "" : config["replace"];

            for(string test_string: test_strings) {
                result["result"]["resultList"].push_back(regex_replace(test_string, pattern, replace));
            }
        }

        return result;
    }
};