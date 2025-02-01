//
// g++ -std=c++20 -o client-fetch client-fetch.cpp
//

#include <httplib.h>
#include <iostream>

int main() {
    // Create a client pointing to the host and port
    httplib::Client client("http://10.0.1.197:2030");

    // Send a GET request to the desired endpoint
    if (auto res = client.Get("/temps")) {  // Example API returning JSON
        if (res->status == 200) {
            std::cout << res->body << std::flush;
        } else {
            std::cerr << "Request failed with status: " << res->status << std::endl;
        }
    } else {
        // Handle connection errors
        auto err = res.error();
        std::cerr << "Connection error: " << httplib::to_string(err) << std::endl;
    }
    //
    // Send a GET request to the desired endpoint
    if (auto res = client.Get("/status")) {  // Example API returning JSON
        if (res->status == 200) {
            std::cout << res->body << std::flush;
        } else {
            std::cerr << "Request failed with status: " << res->status << std::endl;
        }
    } else {
        // Handle connection errors
        auto err = res.error();
        std::cerr << "Connection error: " << httplib::to_string(err) << std::endl;
    }

    return 0;
}

