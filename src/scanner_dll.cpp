#include "../include/scanner_dll.h"

namespace fs = std::filesystem;

int ScanDirectory(const char* baseCSV, const char* logPath, const char* dirPath, ScanReport* outReport) {
    if (!outReport) return 2;

    ScanReportCpp rep;
    rep.start();

    try {
        fs::path base(baseCSV);
        fs::path log(logPath);
        fs::path dir(dirPath);

        auto db = LoadBase(base);

        const unsigned workers = std::max(1u, std::thread::hardware_concurrency());
        std::vector<std::string> threadLogs(workers); // для слияния логов воедино

        std::mutex repMx; // для слияния отчетов
        PathQueue q;

        auto worker = [&](unsigned id) {
            ScanReportCpp local;
            std::ostringstream buf;
            fs::path p;
            while (q.pop(p)) {
                try {
                    if (!fs::is_regular_file(p)) continue;
                    local.incTotal();
                    std::string md5 = CalcMD5(p);
                    if (auto it = db.find(md5); it != db.end()) {
                        buf << p.string() << ';' << md5 << ';' << it->second << std::endl;
                        local.incBad();
                    }
                } catch (...) {
                    local.incErrors();
                }
            }
            // перемещаем логи в массив и соединяем репорты 
            threadLogs[id] = std::move(buf).str();
            {
                std::lock_guard<std::mutex> lk(repMx);
                rep.merge(local);
            }
        };

        // запуск воркеров
        std::vector<std::thread> threads;
        threads.reserve(workers);
        for (unsigned i = 0; i < workers; ++i) threads.emplace_back(worker, i);

        // продюсер заполняет очередь файлами
        try {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                q.push(entry.path());
            }
        } catch (...) {
            rep.incErrors();
        }

        q.close();
        for (auto& t : threads) t.join();
        rep.stop();

        // итоговая запись в лог
        std::ofstream logFile(log, std::ios::app);
        if (!logFile) throw std::runtime_error("Не удалось открыть лог: " + log.string());
        for (const auto& s : threadLogs) logFile << s;

        outReport->totalFiles     = rep.total();
        outReport->maliciousFiles = rep.bad();
        outReport->errorFiles     = rep.errors();
        outReport->ms             = rep.durationMs();
        
        return 0;
    } catch (...) {
        return 1;
    }
}