#include <iostream> 
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string> 
#include <vector> 
#include <cstdlib> 
#include "GECompression.h" 

// C++17
#include <experimental/filesystem> 
namespace fs = std::experimental::filesystem;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

void printHelp()
{
    std::cout 
    << "---- Usage ----" << std::endl
    << " Decompress: ./dkr_decompressor -c <input_directory> <out_filename>" << std::endl
    << " Compress:   ./dkr_decompressor -d <input_filename> <out_directory>" << std::endl;
}

bool readBinaryFile(std::vector<u8>& input, std::string filename)
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

bool writeBinaryFile(u8* output, int fileLength, std::string filename)
{
    std::ofstream wf(filename.c_str(), std::ios::out | std::ios::binary);
    for(int i = 0; i < fileLength; i++)
        wf.write((char *)&output[i], 1);
    wf.close();
}

u32 bytes_to_uint_be(std::vector<u8>& data, int offset) 
{
    return data[offset + 3] | (data[offset + 2] << 8) | (data[offset + 1] << 16) | (data[offset + 0] << 24);
}

std::vector<u8> getSubsection(std::vector<u8>& source, int start, int size) 
{
    std::vector<u8> newVec(source.begin() + start, source.begin() + start + size);
    return newVec;
}

void DecompressZLibSpot(GECompression& compressed, std::vector<u8>& segment, std::string outputFilename) 
{
    std::cout << "Decompressing " << outputFilename << "..." << std::endl;
    //writeBinaryFile(&segment[0], segment.size(), outputFilename + ".cmp.bin");
    
    compressed.SetCompressedBuffer(&segment[0], segment.size());
    int fileSize = 0, fileSizeCompressed = 0;
    u8* outputDecompressed = compressed.OutputDecompressedBuffer(fileSize, fileSizeCompressed);
    
    if(fileSize > 0) {
        //std::cout << "Done! Filesize = " << std::hex << fileSize << std::dec << std::endl;
        writeBinaryFile(outputDecompressed, fileSize, outputFilename + ".bin");
    } else {
        std::cout << "Error: Something bad happend with input \"" << outputFilename << "\"" << std::endl;
    }
}

struct TableOffsetEntry {
    int start;
    int size;
};

void DecompressZLibFromTable(std::string inputFilename, std::string outputDirectory) 
{
    fs::path filePath = fs::path(inputFilename);
    if(!fs::is_regular_file(filePath)) {
        std::cout << inputFilename << " is not a file!" << std::endl;
        return;
    }
    
    std::vector<u8> input;
    
    if(readBinaryFile(input, inputFilename)) {
        std::vector<TableOffsetEntry> tableOffsets;
        GECompression compressed;
        compressed.SetGame(DKR);
        
        // Build table
        int offset = 0;
        while (true)
        {
            int tableOffset = bytes_to_uint_be(input, offset);
            int nextTableOffset = bytes_to_uint_be(input, offset + 4);
            
            if(nextTableOffset == 0xFFFFFFFF)
            {
                offset += 4;
                break;
            }
            
            int tableSize = nextTableOffset - tableOffset;
            
            TableOffsetEntry newEntry;
            newEntry.start = tableOffset;
            newEntry.size = tableSize;
            tableOffsets.push_back(newEntry);
            
            offset += 4;
        }
        int dataStartOffset = (offset + 0x10) & 0xFFFFFFF0;
        
        for(size_t i = 0; i < tableOffsets.size(); i++) {
            int size = tableOffsets[i].size;
            
            std::vector<u8> dataSegment = getSubsection(input, dataStartOffset + tableOffsets[i].start, tableOffsets[i].size);
            std::ostringstream outputFilename;
            outputFilename << outputDirectory << "/" << std::hex << std::fixed 
                << std::setw(6) << std::setfill('0') << tableOffsets[i].start
                << std::dec;
            DecompressZLibSpot(compressed, dataSegment, outputFilename.str());
        }
        std::cout << "Finished decompressing to directory \"" << outputDirectory << "\"" << std::endl;
    } else {
        std::cout << "Could not read file: " << inputFilename << std::endl;
    }
}

std::vector<u8> compress(std::string inputFile)
{
    //std::cout << inputFile << std::endl;
    std::vector<u8> compressed;
    
    int gzipStatus = 0;
    
#if defined(_WIN32) || defined(_WIN64) // Windows
    gzipStatus = system(("gzip.exe -f -q -9 -k " + inputFile).c_str());
#else // Linux
    gzipStatus = system(("gzip -f -q -9 -k " + inputFile).c_str());
#endif

    if(gzipStatus != 0) {
        std::cout << "Error: Either gzip is not installed or some error has occured" << std::endl;
        throw gzipStatus;
    }
    
    std::vector<u8> compressedData;
    readBinaryFile(compressedData, inputFile + ".gz");
    fs::remove(fs::path(inputFile + ".gz"));
    
    // Find start past the header & filename
    int start = 10;
    while(true) 
    {
        if (compressedData[start] == 0x00) // Check for the null terminator
        {
            start++;
            break;
        }
        start++;
    }
    
    std::vector<u8> dkr_compressed = getSubsection(compressedData, start, compressedData.size() - start - 8);
    
    dkr_compressed.insert(dkr_compressed.begin(), 0x09); // gzip compression level?
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 1]);
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 2]);
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 3]);
    dkr_compressed.insert(dkr_compressed.begin(), compressedData[compressedData.size() - 4]);
    
    while((dkr_compressed.size() & 0xF) != 0) {
        dkr_compressed.push_back(0); // Pad out compressed file to be 16-byte aligned.
    }
    
    /*
    std::string extension = inputFile.substr(inputFile.find_last_of('.'));
    std::string newFilename = inputFile.substr(0, inputFile.find_last_of('.')) + ".cmp2" + extension;
    
    std::cout << newFilename << std::endl;
    
    writeBinaryFile(&dkr_compressed[0], dkr_compressed.size(), newFilename);
    */
    
    return dkr_compressed;
}

void pushInt(std::vector<u8>& vec, int value)
{
    vec.push_back((value & 0xFF000000) >> 24);
    vec.push_back((value & 0xFF0000) >> 16);
    vec.push_back((value & 0xFF00) >> 8);
    vec.push_back((value & 0xFF));
}

void compressDirectory(std::string inputDirectory, std::string outputFilename)
{
    fs::path dirPath = fs::path(inputDirectory);
    if(!fs::is_directory(dirPath)) {
        std::cout << inputDirectory << " is not a directory!" << std::endl;
        return;
    }
    
    std::vector<std::vector<u8>> files;
    
    // TODO: Make this use multiple threads for faster execution.
    for (const auto & entry : fs::directory_iterator(dirPath))
    {
        std::cout << "Compressing file " << entry.path() << "..." << std::endl;
        files.push_back(compress(entry.path()));
    }
    
    std::vector<u8> outFile;
    
    int offset = 0;
    int numFiles = files.size();
    for(int i = 0; i < numFiles; i++) {
        pushInt(outFile, offset);
        offset += files[i].size();
    }
    pushInt(outFile, offset);
    pushInt(outFile, 0xFFFFFFFF);
    
    // Make sure the table is 16-byte aligned.
    while((outFile.size() & 0xF) != 0) {
        pushInt(outFile, 0);
    }
    
    //std::cout << std::hex << outFile.size() << std::dec << std::endl;
    
    for(int i = 0; i < numFiles; i++) {
        outFile.insert(outFile.end(), files[i].begin(), files[i].end());
    }
    
    writeBinaryFile(&outFile[0], outFile.size(), outputFilename);
    std::cout << "Finished compressing to file \"" << outputFilename << "\"" << std::endl;
}

int main(int argc, char *argv[]) 
{
    if(argc != 4) 
    {
        printHelp();
        return 1;
    }
    
    std::string option = std::string(argv[1]);
    
    if (option == "-d") {
        std::string inputFilename = std::string(argv[2]);
        std::string outputDir = std::string(argv[3]);
        fs::create_directories(fs::path(outputDir));
        DecompressZLibFromTable(inputFilename, outputDir);
    } else if (option == "-c") {
        std::string inputDirectory = std::string(argv[2]);
        std::string outputFilename = std::string(argv[3]);
        compressDirectory(inputDirectory, outputFilename);
    } else {
        printHelp();
        return 1;
    }
    
    return 0;
}
