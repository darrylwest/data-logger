//
// 2025-01-10 10:28:16 dpw
//

#pragma once

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace app {
    namespace fs = std::filesystem;

    struct Database {
        std::string datapath = "./data";

        std::string getInfo() const {
            const auto filename = datapath + "/info.json";
            return read_file(filename);
        }

        std::string read_file(const std::string &filename) const {
            std::ifstream file(filename);
            if (!file) {
                spdlog::error("could not open db file: {}", filename);
                throw std::runtime_error("Error opening Db file:  " + filename);
            }

            return std::string((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
        }
    };

}  // namespace app
