#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <cstdio>

#include "../include/hash_utils.h"

namespace fs = std::filesystem;

TEST(CalcMD5, CalculateMD5_1) {
    std::string sP = "temp.txt";
    fs::path p = "temp.txt";
    std::ofstream temp(p, std::ios::out);

    temp << "Hello world!" << std::endl;
    std::string RightHash = "59ca0efa9f5633cb0371bbc0355478d8";
    std::string GotHash = CalcMD5(p);
    std::remove(sP.c_str());
    EXPECT_EQ(RightHash, GotHash);
}

TEST(CalcMD5, CalculateMD5_2) {
    std::string sP = "temp.txt";
    fs::path p = "temp.txt";
    std::ofstream temp(p, std::ios::out);

    temp << "Здарова Мир!" << std::endl;
    std::string RightHash = "7247617fccebe4fc0d180dcd7f89ae0f";
    std::string GotHash = CalcMD5(p);
    std::remove(sP.c_str());
    EXPECT_EQ(RightHash, GotHash);
}

TEST(LoadBase, LoadBase_1) {
    std::string sP = "temp.csv";
    fs::path p = "temp.csv";
    std::ofstream temp(p, std::ios::out);

    temp << "dd9bb9da436f2d19831bcd284c55896b;Backdoor" << std::endl;
    temp << "1b374b4c5d6e7f8091a2b3d;Riskware" << std::endl;
    temp << "2c485b4c5d6e7f8091a2b3e;Grayware" << std::endl;
    auto base = LoadBase(p);
    std::remove(sP.c_str());
    EXPECT_EQ(base["dd9bb9da436f2d19831bcd284c55896b"], "Backdoor");
    EXPECT_EQ(base["1b374b4c5d6e7f8091a2b3d"], "Riskware");
    EXPECT_EQ(base["2c485b4c5d6e7f8091a2b3e"], "Grayware");
}