
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
* **-l** - use lowercase mnemonics
* **-na** - do not print adresses
* **-nc** - do not print alternatives in comments
* **-nl** - do not assign labels
* **-v** - print version and exit
* **-h** - print help and exit

