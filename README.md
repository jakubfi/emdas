
About EMDAS
==========================================================================

EMDAS is a disassembler for MERA-400, available as a standalone commandline tool and a library.

Requirements
==========================================================================

To build and run emas you need:

* cmake
* GNU make

Build instructions
==========================================================================

Do the following in the directory where this README lives:

```
cmake .
make
make install
```

Usage
==========================================================================

```
emdas [options] input
```

Where *options* are:

* **-o output** - specify output file (*stdout* otherwise)
* **-c cpu** - set CPU type: *mera400*, *mx16* (default is *mera400*)
* **-a addr** - set base address
* **-s file** - load a symbol table (lines: `<addr> <name>`) and use the names as labels
* **-u** - use uppercase mnemonics
* **-na** - do not print adresses
* **-nb** - do not break labels onto their own line
* **-nc** - do not print alternatives in comments
* **-nl** - do not assign labels
* **-v** - print version and exit
* **-h** - print help and exit

