CXX := g++

dkr_decompressor_SOURCES  := dkr_decompressor.cpp DKRCompression.cpp GECompression.cpp
dkr_decompressor_CXXFLAGS := -lstdc++fs

all: dkr_decompressor

dkr_decompressor:
	$(CXX) $(dkr_decompressor_SOURCES) $(dkr_decompressor_CXXFLAGS) -o dkr_decompressor
    
clean:
	$(RM) dkr_decompressor
