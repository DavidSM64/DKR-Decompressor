# DKR Decompressor

This is a command-line tool used for compressing and decompressing assets from the N64 game "Diddy Kong Racing". This tool was designed to be used in the DKR decompilation project, but feel free to use it for whatever you want.

If you are using linux, then the `gzip` package will need to be installed for compressing data. Windows users use the provided `gzip.exe`.

---

## Tool Usage

#### Decompressing a file

`./dkr_decompressor -d <input_filename> <output_filename>`

#### Compressing a file

`./dkr_decompressor -c <input_filename> <output_filename>`

---

## Library Usage

If you want to use this as part of another C++ tool, you can simply add the files into your own project.

Required files:  
* GECompression.h
* GECompression.cpp
* DKRCompression.h
* DKRCompression.cpp

The main file you want to include is `DKRCompression.h`. It is a class that contains 4 public functions.

```cpp
std::vector<uint8_t> compressBuffer(std::vector<uint8_t> data);
std::vector<uint8_t> decompressBuffer(std::vector<uint8_t> data);
bool readBinaryFile(std::vector<uint8_t>& input, std::string filename);
bool writeBinaryFile(std::vector<uint8_t>& output, std::string filename);
```

#### Example

```cpp
#include <vector>
#include <string>
#include <cstdint>

#include "DKRCompression.h"

// Returns the compressed version of a file.
std::vector<uint8_t> getFileAndReturnCompressed(std::string inputFilename)
{
    DKRCompression compression;

    std::vector<uint8_t> inputFileData;
    if(compression.readBinaryFile(inputFileData, inputFilename)) 
    {
        return compression.compressBuffer(inputFileData);
    }
    
    // Input file could not be read.
    throw 1;
}
```


