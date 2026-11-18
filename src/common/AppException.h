#pragma once
#include <stdexcept>
#include <string>

class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string& msg) : std::runtime_error(msg) {}
};

class DatabaseException : public AppException {
public:
    explicit DatabaseException(const std::string& msg) : AppException(msg) {}
};