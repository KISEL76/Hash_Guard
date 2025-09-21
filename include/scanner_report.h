#pragma once 

#include <cstddef>
#include <chrono>
#include <openssl/evp.h>

class ScanReportCpp {
public:
    using clock = std::chrono::steady_clock;

    void start();
    void stop();

    void incTotal(std::size_t n = 1);
    void incBad(std::size_t n = 1);
    void incErrors(std::size_t n = 1);

    std::size_t total() const;
    std::size_t bad() const;
    std::size_t errors() const;
    double durationMs() const;

    void merge(const ScanReportCpp& other);

private:
    std::size_t totalFiles = 0;
    std::size_t maliciousFiles = 0;
    std::size_t errorFiles = 0;
    double ms = 0;
  
    clock::time_point startTime{};
    clock::time_point endTime{};
};