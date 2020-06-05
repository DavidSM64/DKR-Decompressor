#pragma once
#include <iostream> 
#include <fstream> 
#include <vector>
#include <string>
#include <cstdint>
#include "GECompression.h"
#include "DKRGzip.h"

// C++17
#include <experimental/filesystem> 
namespace fs = std::experimental::filesystem;

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_GZIP "gzip.exe"
#endif

class DKRCompression
{
public:
    DKRCompression();
    ~DKRCompression(void);
    
    std::vector<uint8_t> compressBuffer(std::vector<uint8_t> data);
    std::vector<uint8_t> decompressBuffer(std::vector<uint8_t> data);
    bool readBinaryFile(std::vector<uint8_t>& input, std::string filename);
    bool writeBinaryFile(std::vector<uint8_t>& output, std::string filename);
    
private:
    std::vector<uint8_t> getSubsection(std::vector<uint8_t>& source, int start, int size);
    
    GECompression compressed;
};
