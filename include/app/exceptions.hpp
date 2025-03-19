//
// exception handling
//

#pragma once

#include <spdlog/spdlog.h>

#include <app/types.hpp>

#include "precompiled.hpp"

namespace app {

    // Base exception class
    class ServiceException : public std::exception {
      public:
        explicit ServiceException(const Str& message) : msg_(message) {}
        const char* what() const noexcept override { return msg_.c_str(); }

      private:
        Str msg_;
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

    class ValidationException : public ServiceException {
      public:
        using ServiceException::ServiceException;
    };

    class KeyException : public ServiceException {
      public:
        using ServiceException::ServiceException;
    };

}  // namespace app
