#pragma once 

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

class Scanner {
public: 
    Scanner() noexcept;

    std::string GetPath() const { return path; }
    std::string GetLog() const { return log; }
    std::string GetBase() const { return base; }

    void SetPath(const std::string newPath) { path = newPath; }
    void SetLog(const std::string newLog) { log = newLog; }
    void SetBase(const std::string newBase) { base = newBase; }

    void Scan(int argc, char *argv[]);

    void Validate() const;

private:
    std::string path;
    std::string log;
    std::string base;
};
