#pragma once

#include <string>
#include <vector>
#include <regex>

namespace ncl {
    bool startsWith(std::string source, std::string query) {
        return source.find(query) == 0;
    }

    std::vector<std::string> split(std::string source, std::string delim) {
        std::vector<std::string> res;
        std::string::size_type n = 0;

        std::string wordpad = source;
        while (n != std::string::npos) {
            std::string::size_type start = 0;
            n = wordpad.find(delim);
            res.push_back(wordpad.substr(start, n));

            wordpad = wordpad.substr(n + 1, wordpad.size());
        }

        return res;
    }

    std::string trim(std::string str) {
        const std::regex pattern("^\\s*(.+)\\s*$");
        std::smatch matches;

        if (std::regex_search(str, matches, pattern)) {
            return matches[1];
        }
        return str;
    }
}