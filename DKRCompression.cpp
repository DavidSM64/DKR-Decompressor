#include "DKRCompression.h"

DKRCompression::DKRCompression()
{
    compressed.SetGame(DKR);
}

DKRCompression::~DKRCompression(void)
{
}

std::vector<uint8_t> DKRCompression::compressBuffer(std::vector<uint8_t> data) 
{
    writeBinaryFile(data, "__temp.dkr.bin");
    
    std::vector<uint8_t> compressed;
    
    int gzipStatus = 0;
    
#if defined(_WIN32) || defined(_WIN64) // Windows
    gzipStatus = system(WINDOWS_GZIP + " -fnq9 __temp.dkr.bin");
#else // Linux
    gzipStatus = system("gzip -fnq9 __temp.dkr.bin");
#endif

    if(gzipStatus != 0) {
        std::cout << "Error: Either gzip is not installed or some error has occured" << std::endl;
        throw gzipStatus;
    }
    
    std::vector<uint8_t> compressedData;
    readBinaryFile(compressedData, "__temp.dkr.bin.gz");
    fs::remove(fs::path("__temp.dkr.bin.gz"));
    
    std::vector<uint8_t> dkr_compressed = getSubsection(compressedData, 10, compressedData.size() - 10 - 8);
    
    dkr_compressed.insert(dkr_compressed.begin(), 0x09); // gzip compression level?
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 1]);
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 2]);
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 3]);
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 4]);
    
    while((dkr_compressed.size() & 0xF) != 0) {
        dkr_compressed.push_back(0); // Pad out compressed file to be 16-byte aligned.
    }
    
    return dkr_compressed;
}

std::vector<uint8_t> DKRCompression::decompressBuffer(std::vector<uint8_t> data) 
{
    compressed.SetCompressedBuffer(&data[0], data.size());
    
    int fileSize = 0, fileSizeCompressed = 0;
    uint8_t* decompressed = compressed.OutputDecompressedBuffer(fileSize, fileSizeCompressed);
    
    std::vector<uint8_t> out(decompressed, decompressed + fileSize);
    
    delete[] decompressed;
    
    return out;
}

bool DKRCompression::writeBinaryFile(std::vector<uint8_t>& output, std::string filename)
{
    try 
    {
        std::ofstream wf(filename.c_str(), std::ios::out | std::ios::binary);
        for(int i = 0; i < output.size(); i++)
            wf.write((char *)&output[i], 1);
        wf.close();
    } 
    catch (int e)
    {
        return false;
    }
    return true;
}

bool DKRCompression::readBinaryFile(std::vector<uint8_t>& input, std::string filename)
{
    try 
    {
        std::ifstream is;
        is.open(filename.c_str(), std::ios::binary);
        is.seekg(0, std::ios::end);
        size_t filesize = is.tellg();
        is.seekg(0, std::ios::beg);
        input.resize(filesize);
        is.read((char *)input.data(), filesize);
    } 
    catch (int e)
    {
        return false;
    }
    return true;
}

std::vector<uint8_t> DKRCompression::getSubsection(std::vector<uint8_t>& source, int start, int size) 
{
    std::vector<uint8_t> newVec(source.begin() + start, source.begin() + start + size);
    return newVec;
}
