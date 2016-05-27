### Build the sources

If you want to build IPFUnpacker, I suggest using GCC - The code is compilable both under Windows and Linux.
Here is a quick guide how to compile the sources on **Windows** (for Linux, install the corresponding packages) : 

- Download MSYS2 (http://sourceforge.net/projects/msys2/files/latest/download)
- Open the mingw64_shell.bat in the MSYS2 installation folder
- Launch the following commands :
```
pacman -S git gcc make zlib-devel
git clone https://github.com/r1emu/IPFUnpacker.git
cd IPFUnpacker
make clean && make release
```

The executable "ipf_unpack" should have been compiled fine and should be present inside your IPFUnpacker folder.
