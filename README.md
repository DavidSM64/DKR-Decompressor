# DKR Decompressor

This is a command-line tool used for compressing and decompressing assets from the N64 game "Diddy Kong Racing". This tool was designed to be used in the DKR decompilation project, but feel free to use it for whatever you want.

If you are using linux, then the `gzip` package will need to be installed for compressing data.

---

## Usage

#### Decompressing a data block to a directory

`./dkr_decompressor -c <input_directory> <out_filename>`

#### Compressing a directory into a data block

`./dkr_decompressor -d <input_filename> <out_directory>`
