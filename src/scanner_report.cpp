#include "../include/scanner_report.h"

void ScanReportCpp::start() { startTime = clock::now(); }

void ScanReportCpp::stop()  { endTime  = clock::now(); }

void ScanReportCpp::incTotal(std::size_t n) { totalFiles += n; }

void ScanReportCpp::incBad(std::size_t n) { maliciousFiles += n; }

void ScanReportCpp::incErrors(std::size_t n) { errorFiles += n; }

std::size_t ScanReportCpp::total() const { return totalFiles; }

std::size_t ScanReportCpp::bad() const { return maliciousFiles; }

std::size_t ScanReportCpp::errors() const { return errorFiles; }

double ScanReportCpp::durationMs() const {
    return std::chrono::duration<double, std::milli>(endTime - startTime).count();
}  

void ScanReportCpp::merge(const ScanReportCpp &other) {
    totalFiles += other.totalFiles;
    maliciousFiles += other.maliciousFiles;
    errorFiles += other.errorFiles;
}



