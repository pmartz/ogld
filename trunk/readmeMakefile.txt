README for the makefile system

To build the example code, type "make".

The example source code employs a cross-platform makefile pattern as
described in "Cross-Platform Builds" by John Graham-Cumming in Dr
Dobb's Journal (January 2005). This makefile pattern simplifies the
make system and allows you to build the code for multiple platforms
from a single shared file system. I've tested this system on Intel-
based Linux, Cygwin, and Mac platforms, as well as a Power Macintosh.

To use the makefile system, you'll need two platform-specific files.
The name of the file is derived from the output of the uname command.
For example, on my Linux system, "uname -ms" outputs "Linux i686", so
the two required files are:
    Linux_i686.mak
    Linux_i686-rules.mak

If you're using Linux, Mac OS X, or Cygwin, the example code comes with
makefiles ready to use. Otherwise, type the following at a shell prompt
to see the base name for your platform-specific makefile names:
    uname -ms | sed -e s"/ /_/g"

In the ".mak" platform-specific makefile, define platform-specific
filename extensions for object files, statically-linked library files,
and executables. See, for example:
    Linux_i686.mak
    Darwin_Power_Macintosh.mak
    Darwin_i386.mak
    CYGWIN_NT-5.1_i686.mak.

In the "-rules.mak" platform-specific makefile, specify platform-
specific compiler and linker options. See, for example:
    Linux_i686-rules.mak
    Darwin_Power_Macintosh-rules.mak
    Darwin_i386-rules.mak
    CYGWIN_NT-5.1_i686-rules.mak.

The make system require GNU Make version 3.80 or higher. Check your
version by typing "make -ver". If you need to upgrade to a newer
version, visit http://www.gnu.org/software/make.
