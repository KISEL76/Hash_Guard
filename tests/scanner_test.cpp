#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../include/scanner_dll.h"

namespace fs = std::filesystem;

TEST(ScannerAPI, FindsMaliciousFile) {
    fs::path tmp = fs::temp_directory_path() / "scanner_test_find";
    std::error_code ec;
    fs::remove_all(tmp, ec);
    fs::create_directories(tmp / "data", ec);
    fs::create_directories(tmp / "samples", ec);

    // base.csv: md5("hello") = 5d41402abc4b2a76b9719d911017c592
    {
        std::ofstream(tmp / "data" / "base.csv")
            << "5d41402abc4b2a76b9719d911017c592;Trojan\n";
    }

    // Файлы: плохой ("hello") и хороший ("WORLD")
    { std::ofstream(tmp / "samples" / "bad.txt",  std::ios::binary) << "hello"; }
    { std::ofstream(tmp / "samples" / "good.txt", std::ios::binary) << "WORLD"; }

    // Запуск сканера
    ScanReport rep{};
    int rc = ScanDirectory(
        (tmp / "data" / "base.csv").string().c_str(),
        (tmp / "report.log").string().c_str(),
        (tmp / "samples").string().c_str(),
        &rep
    );

    // Проверки метрик
    ASSERT_EQ(rc, 0);
    EXPECT_EQ(rep.totalFiles, 2u);
    EXPECT_EQ(rep.maliciousFiles, 1u);
    EXPECT_EQ(rep.errorFiles, 0u);
    EXPECT_GT(rep.ms, 0.0);

    // Проверка, что в логе есть запись о bad.txt + вердикт
    std::ifstream in(tmp / "report.log");
    std::string line; bool found = false;
    while (std::getline(in, line)) {
        if (line.find("bad.txt") != std::string::npos &&
            line.find("5d41402abc4b2a76b9719d911017c592") != std::string::npos &&
            line.find("Trojan") != std::string::npos) {
            found = true; break;
        }
    }
    EXPECT_TRUE(found) << "Лог не содержит вредоносновго файла";

    fs::remove_all(tmp, ec);
}

TEST(ScannerAPI, EmptyBaseNoDetections) {
    fs::path tmp = fs::temp_directory_path() / "scanner_test_empty";
    std::error_code ec;
    fs::remove_all(tmp, ec);
    fs::create_directories(tmp / "data", ec);
    fs::create_directories(tmp / "samples", ec);

    // Пустой base.csv
    { std::ofstream(tmp / "data" / "base.csv") << ""; }

    // Пара обычных файлов
    { std::ofstream(tmp / "samples" / "f1.txt", std::ios::binary) << "AAA"; }
    { std::ofstream(tmp / "samples" / "f2.txt", std::ios::binary) << "BBB"; }

    // Запуск сканера
    ScanReport rep{};
    int rc = ScanDirectory(
        (tmp / "data" / "base.csv").string().c_str(),
        (tmp / "report.log").string().c_str(),
        (tmp / "samples").string().c_str(),
        &rep
    );

    // Проверки метрик
    ASSERT_EQ(rc, 0);
    EXPECT_EQ(rep.totalFiles, 2u);
    EXPECT_EQ(rep.maliciousFiles, 0u);
    EXPECT_EQ(rep.errorFiles, 0u);

    // Лог должен быть пустым
    std::ifstream in(tmp / "report.log");
    EXPECT_TRUE(in.peek() == EOF);

    fs::remove_all(tmp, ec);
}

TEST(ScannerAPI, NonExistingDirectoryCountsAsError) {
    namespace fs = std::filesystem;

    fs::path tmp = fs::temp_directory_path() / "scanner_test_no_dir";
    std::error_code ec;
    fs::remove_all(tmp, ec);
    fs::create_directories(tmp / "data", ec);

    // валидная, но пустая база
    std::ofstream(tmp / "data" / "base.csv") << "";

    // путь к несуществующей директории для сканирования
    fs::path missingDir = tmp / "samples_missing";

    ScanReport rep{};
    int rc = ScanDirectory(
        (tmp / "data" / "base.csv").string().c_str(),
        (tmp / "report.log").string().c_str(),
        missingDir.string().c_str(),
        &rep
    );

    // функция не должна падать
    ASSERT_EQ(rc, 0);
    // файлов не обработано
    EXPECT_EQ(rep.totalFiles, 0u);
    EXPECT_EQ(rep.maliciousFiles, 0u);
    // но ошибка обхода учтена
    EXPECT_GE(rep.errorFiles, 1u);

    fs::remove_all(tmp, ec);
}