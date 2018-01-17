# annos

ANNOS (Arthur's Not Named Operating System) is a OS, powered by a C++ 
monolythic kernel.

It aims to be a relatively easy-to-study system, almost completely object-oriented
and as fast as possible.

## Building

To build annos, you'll need a gcc with the i686-elf and C++14 support (g++). 
You can use other compiler, given that it supports the `__attribute__((packed))` directive.

You'll have to build it. [Here](http://wiki.osdev.org/GCC_Cross-Compiler "link to OSDEV tutorial") you'll find instructions. Download [this binutils](https://ftp.gnu.org/gnu/binutils/binutils-2.29.tar.xz) and [this GCC](https://ftp.gnu.org/gnu/gcc/gcc-7.2.0/gcc-7.2.0.tar.xz) and use them in the tutorial, because it's what I'm 
using now.

Please set the `CXX`, `AS` an `QEMU` variables in the makefile to point to 
the path of your new `i686-elf-g++`, `i686-elf-as` and `qemu-system-i386`.

## Running

Type `make qemu-run` to run it in QEMU. Type `make iso` to generate an ISO
that you can use to boot it on real hardware.

## And...?

Please contribute, I'm going to love it :heart:

Open a pull request if you like, check the issues, help me to improve it, if you 
like. Have fun!

## License

All of this source code is licensed under the MIT license.
