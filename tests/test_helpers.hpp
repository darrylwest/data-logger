
#pragma once

#include <app/types.hpp>
#include <app/cli.hpp>

namespace helpers {

    std::string capture_stdout(Func<void()> func) {
        std::ostringstream oss;
        std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());  // Redirect std::cout

        func();  // Execute function that prints to stdout

        std::cout.rdbuf(old_cout);  // Restore std::cout
        return oss.str();
    }

    app::config::WebConfig default_web_config = {
        .scheme = "http",
        .host = "0.0.0.0",
        .port = 9090,
        .www = "html",
        .cert_file = "cert.pem",
        .key_file = "cert.pem",
        .verbose = false,
    };

}
