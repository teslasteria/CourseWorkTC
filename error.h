#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <string>

[[noreturn]] inline void throwError(const std::string& message, size_t line) {
    throw std::runtime_error("Error at line " + std::to_string(line) + ": " + message);

}

#endif // ERROR_H
