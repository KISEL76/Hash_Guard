#include <stdexcept>
#include <iostream>

#include "../include/scanner.h"
#include "../include/scanner_dll.h"

int main(int argc, char* argv[]) {
    try {
        Scanner scanner;
        scanner.Scan(argc, argv);
        scanner.Validate();
        std::cout <<  "Валидация параметров прошла успешно" << std::endl;

        ScanReport report {};
        int rc = ScanDirectory(scanner.GetBase().c_str(), scanner.GetLog().c_str(), scanner.GetPath().c_str(), &report);

        if (rc != 0) {
            std::cerr << "Ошибка при сканировании, код: " << rc << std::endl;
            return 1;
        }

        std::cout << "Обработано файлов: " << report.totalFiles << std::endl;
        std::cout << "Количество вредоносных файлов: " << report.maliciousFiles << std::endl;
        std::cout << "Количество ошибок анализа файлов: " << report.errorFiles << std::endl;
        std::cout << "Время выполнения: " << report.ms << std::endl;

        return 0;
    } 
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}