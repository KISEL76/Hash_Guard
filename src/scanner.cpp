#include "scanner.h"

Scanner::Scanner() noexcept : path(""), log(""), base("") {}

void Scanner::Scan(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--base" || arg == "-b") {
            if (i + 1 < argc) { this->base = argv[++i]; }
            else std::cerr << "Ошибка: отсутствует значение для аргумента " << arg << std::endl; 
        }
        else if (arg == "--log" || arg == "-l") {
            if (i + 1 < argc) this->log = argv[++i];
            else std::cerr << "Ошибка: отсутствует значение для аргумента " << arg << std::endl; 
        }
        else if (arg == "--path" || arg == "-p") {
            if (i + 1 < argc) this->path = argv[++i]; 
            else std::cerr << "Ошибка: отсутствует значение для аргумента " << arg << std::endl;
        }
        else {
            std::cerr << "Неизвестный аргумент: " << arg << std::endl;
        }
    }
}

void Scanner::Validate() const {
    namespace fs = std::filesystem;

    if (base.empty() || log.empty() || path.empty()) {
        throw std::invalid_argument("Все параметры должны быть использованы");
    }

    fs::path basePath(base);
    fs::path logPath(log);
    fs::path dirPath(path);

    // 1) Проверка базы 
    if (!fs::exists(basePath) || !fs::is_regular_file(basePath)) {
        throw std::runtime_error("Файл базы не найден или не является обычным файлом: " + basePath.string());
    }

    // 2) Проверка/создание лога
    if (fs::exists(logPath)) {
        if (!fs::is_regular_file(logPath)) {
            throw std::runtime_error("Указанный лог не является файлом: " + logPath.string());
        }
    } 
    else {
        std::ofstream ofs(logPath);
        if (!ofs) {
            throw std::runtime_error("Не удалось создать файл лога: " + logPath.string());
        }
    }

    // 3) Проверка пути директории
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        throw std::runtime_error("Указанный путь не существует или не является директорией: " + dirPath.string());
    }
    
}








