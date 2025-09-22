#pragma once 

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <openssl/md5.h>
#include <openssl/evp.h> 

std::string CalcMD5(const std::filesystem::path& filePath);

std::unordered_map<std::string, std::string> LoadBase(const std::filesystem::path& filePath);