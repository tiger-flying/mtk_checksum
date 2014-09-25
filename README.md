mtk_checksum
============

This is a tool to generate Checksum.ini for MTK factory files(you may can use it elsewhere since it is a really simple checksum tool).

I used to use the precompiled Windows Binary CheckSum_Generate.exe(http://ge.tt/7MDBGal/v/0), but it seems not work properly for me:
* It uses another uboot file name other than mine(mine is lk.bin), so it can't generate checksum for UBOOT in my case.
* It just checks the prefix of the file name, if there are two files named system.img and system.img.bk, either is valid, then I have no idear which is used to checksum.
* I don't have its source code, so can't make change to it.

I write this tool so it can fit my need.

Compilation:
This tool is designed to be portable(but you need a C99 compiler), you can compile it in any OS in theory. I'm only familiar with GNU make, so I write a Makefile only(in a Unix-like system, simply run "make" or "make src" in the terminal). In Windows, you can use MSVS or any other compilers to compile.

Available options(assume the compiled binary is named mtkchecksum/mtkchecksum.exe):
* -c	Indicate the path of the pattern config file. If you don't specify this option, the program will search current directory to find a "pattern.ini" file. The program will abort if it can't find a config file.

* -t	Specify the folder that contains the factory files. If you don't specify the path, then the program will check current working directory(Note: not necessary where the mtkchecksum resides).

Usage:
mtkchecksum [-c pattern_config_file] [-t images_folder]

Look at the src/pattern.ini to know the basic format of the pattern_config_file. The generated Checksum.ini will be placed in the factory images folder(either current directory or the path you specified with "-t" option).

Note:
Since the Checksum.ini is used by the MTK flash tool, which is a Windows program, it uses CRLF("\r\n", or 0d0a in hex mode) as line break. Be careful to edit the Checksum.ini in a Non-Windows environment in order not to change the line break to "\n"(which is used in most Unix-like systems).
