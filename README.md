# DKR Decompressor

This is a command-line tool used for compressing and decompressing assets from the N64 game "Diddy Kong Racing". This tool was designed to be used in the DKR decompilation project, but feel free to use it for whatever you want.

---

## Tool Usage

#### Decompressing a file

`./dkr_decompressor -d <input_filename> <output_filename>`

#### Compressing a file

`./dkr_decompressor -c <input_filename> <output_filename>`
  
### What does a compressed file look like?

In DKR, a compressed block has a 5 byte header. The first 4 bytes is the uncompressed size in little-endian and the fifth byte is `0x09`, which I think is the gzip compression level.

---

## Library Usage

If you want to use this as part of another C++ tool, you can simply add the files into your own project.

Required files:  
* GECompression.h
* GECompression.cpp
* DKRCompression.h
* DKRCompression.cpp
* DKRGZip.h
* DKRGZip.cpp

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


