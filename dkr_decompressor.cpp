#include <iostream> 
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string> 
#include <vector> 
#include <cstdlib> 
#include "DKRCompression.h" 

void printHelp()
{
    std::cout 
    << "---- Usage ----" << std::endl
    << " Compress file: ./dkr_decompressor -c <input_filename> <output_filename>" << std::endl
    << " Decompress file: ./dkr_decompressor -d <input_filename> <output_filename>" << std::endl
    << " Decompress section from a ROM file: ./dkr_decompressor -r <rom_filename> <output_filename> <rom_offset> <length>" << std::endl;
}

int main(int argc, char *argv[]) 
{
    if(argc == 1) 
    {
        printHelp();
        return 1;
    }
    
    
    std::string option = std::string(argv[1]);
    
    bool fileOptionCheck = ((option == "-d" || option == "-c") && argc != 4);
    bool romOptionCheck = ((option == "-r") && argc != 6);
    bool invalidOptionCheck = (option != "-d" && option != "-c" && option != "-r");
    
    if(fileOptionCheck || romOptionCheck || invalidOptionCheck) 
    {
        printHelp();
        return 1;
    }
    
    DKRCompression compression;
    
    std::string inputFilename = std::string(argv[2]);
    std::string outputFilename = std::string(argv[3]);
    std::vector<uint8_t> inputBinary, outputBinary;
    
    if(compression.readBinaryFile(inputBinary, inputFilename))
    {
        if (option == "-c") 
        {
            outputBinary = compression.compressBuffer(inputBinary);
        } 
        else if (option == "-d") 
        {
            outputBinary = compression.decompressBuffer(inputBinary);
        }
        else if (option == "-r") 
        {
            int romOffset = std::stol(std::string(argv[4]), 0, 16);
            int length = std::stol(std::string(argv[5]), 0, 16);
            
            std::vector<uint8_t> subsection(inputBinary.begin() + romOffset, inputBinary.begin() + romOffset + length);
            
            outputBinary = compression.decompressBuffer(subsection);
        }
        
        compression.writeBinaryFile(outputBinary, outputFilename);
    }
    else 
    {
        std::cout << "Could not read file: " << inputFilename << std::endl;
        return 1;
    }
    
    return 0;
}




