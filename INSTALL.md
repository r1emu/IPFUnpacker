### Build the sources

If you want to build IPFUnpacker, I suggest using GCC - The code is compilable both under Windows and Linux.
Here is a quick guide how to compile the sources on **Windows** (for Linux, install the corresponding packages) : 

- Download MSYS2 (http://sourceforge.net/projects/msys2/files/latest/download)
- Open the mingw64_shell.bat in the MSYS2 installation folder
- Launch the command `pacman -S gcc make zlib-devel`.
- You should now be able to compile the code using `make clean && make release` next to the Makefile in the root folder.