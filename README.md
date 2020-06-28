# DKR Decompressor

This is a command-line tool used for compressing and decompressing assets from the N64 game "Diddy Kong Racing". This tool was designed to be used in the DKR decompilation project, but feel free to use it for whatever you want.

---

## Tool Usage

#### Decompressing a file

`./dkr_decompressor -d <input_filename> <output_filename>`

#### Compressing a file

`./dkr_decompressor -c <input_filename> <output_filename>`

#### Decompressing a section from a ROM file

`./dkr_decompressor -r <rom_filename> <output_filename> <rom_offset> <length>`

---

### What does a compressed file look like?

In DKR, a compressed block has a 5 byte header. The first 4 bytes is the uncompressed size in little-endian and the fifth byte is `0x09`, which I think is the gzip compression level.

Here are the ROM offsets for the compressed data in US version 1.0  
  
| Files | Look-up table | Start offset | End offset |
| ------------- | ------------- | ------------ | ---------- |
| Textures | 0x296CA0 | 0x0ECD50 | 0x296CA0 |
| More Textures | 0x375D70 | 0x298290 | 0x375D70 |
| Object placement maps | 0x383AA0 | 0x383CD0 | 0x3A1440 |
| Track Models | 0x3A5670 | 0x3A5760 | 0x54F4B0 |
| Object Models | 0x54F4B0 | 0x54FAD0 | 0x61FA70 |
| Animations | 0x61FD80 | 0x6204E0 | 0x6C0050 |

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


