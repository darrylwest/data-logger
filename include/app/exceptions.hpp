//
// exception handling
//

#include <spdlog/spdlog.h>
#include <exception>
#include <string>

namespace app {

    // Base exception class
    class ServiceException : public std::exception {
    public:
        explicit ServiceException(const std::string& message) : msg_(message) {}
        const char* what() const noexcept override { return msg_.c_str(); }
    private:
        std::string msg_;
    };

    // Specific exceptions
    class NetworkException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class ParseException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class DatabaseException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class FileException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

} // namespace app
