#include "../include/scanner_dll.h"

namespace fs = std::filesystem;

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

// Загрузка CSV базы 
static std::unordered_map<std::string, std::string> LoadBase(const fs::path& filePath) {
    std::unordered_map<std::string, std::string> db; // hash -> verdidict 
    std::ifstream ifs(filePath);

    if (!ifs) throw std::runtime_error("Не удалось открыть CSV: " + filePath.string());
    std::string line;

    while (std::getline(ifs, line)) {
        auto pos = line.find(';');
        if (pos != std::string::npos) {
            std::string hash = line.substr(0, pos);
            std::string verdict = line.substr(pos + 1);
            db[hash] = verdict;
        }
    }
    return db;
}

// Подсчет хэша MD5
static std::string CalcMD5(const fs::path& filePath) {
    std::ifstream ifs(filePath, std::ios::binary);
    if (!ifs) throw std::runtime_error("Не удалось открыть: " + filePath.string());

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");
    const EVP_MD* md = EVP_md5();
    if (EVP_DigestInit_ex(ctx, md, nullptr) != 1)
        throw std::runtime_error("EVP_DigestInit_ex failed");

    char buffer[8192];
    while (ifs.read(buffer, sizeof(buffer)) || ifs.gcount() > 0) {
        if (EVP_DigestUpdate(ctx, buffer, ifs.gcount()) != 1)
            throw std::runtime_error("EVP_DigestUpdate failed");
    }

    unsigned char out[EVP_MAX_MD_SIZE];
    unsigned int outlen = 0;
    if (EVP_DigestFinal_ex(ctx, out, &outlen) != 1)
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    EVP_MD_CTX_free(ctx);

    std::ostringstream oss;
    for (unsigned i = 0; i < outlen; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)out[i];
    return oss.str();
}


int ScanDirectory(const char* baseCSV, const char* logPath, const char* dirPath, ScanReport* outReport) {
    if (!outReport) return 2;

    ScanReportCpp rep;
    rep.start();

    try {
        fs::path base(baseCSV);
        fs::path log(logPath);
        fs::path dir(dirPath);

        auto db = LoadBase(base);
        std::ofstream logFile(log, std::ios::app);
        if (!logFile) throw std::runtime_error("Не удалось открыть лог: " + log.string());

        for (const auto& entry : fs::recursive_directory_iterator(std::string(dirPath))) {
            if (!entry.is_regular_file()) continue;
            rep.incTotal();
            try {
                std::string md5 = CalcMD5(entry.path());
                if (db.find(md5) != db.end()) {
                    logFile << std::string(entry.path()) << ";" << md5 << ";" << db[md5] << std::endl;
                    rep.incBad();
                }
            }
            catch(...) {
                rep.incErrors();
            }
        }
        rep.stop();


        outReport->totalFiles     = rep.total();
        outReport->maliciousFiles = rep.bad();
        outReport->errorFiles     = rep.errors();
        outReport->ms     = rep.durationMs();
        
        return 0;
    }
    catch (...) {
        return 1;
    }
}

