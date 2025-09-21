#pragma once

#include <cstddef>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h> 

#include "scanner_report.h"

#ifdef __cplusplus
extern "C" {
#endif

// Структура для отчета сканирования
struct ScanReport {
    size_t totalFiles;
    size_t maliciousFiles;
    size_t errorFiles;
    double ms;
};

// Функция, которая будет экспортироваться из .dll
// baseCSV — путь к CSV с хешами
// logPath — путь к файлу лога
// dirPath — путь к директории для сканирования
int ScanDirectory(const char* baseCSV, const char* logPath, const char* dirPath, ScanReport* report);

#ifdef __cplusplus
}
#endif