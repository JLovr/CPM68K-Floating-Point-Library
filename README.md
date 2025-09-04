# CPM68K-Floating-Point-Library
Scripts and code to build a single precision floating point library for the CP/M-68K default compiler.
The default C compiler had an ability to link in a math library, but it wasn't included in the default package.
Because of those bindings, we can't employ the float type directly.

To that effect, I have created a new typedef - spf ("Single Precision Floating point") which is format compatable with the IEEE-754 single precision standard ( < sign >+<exponent 8 bits>+<mantissa 23 bits>). The mantissa is actually 24 bits, but the most significant bit is not stored and assumed to be 1 unless all bits are zero then it equates to zero. It is masked back in by all the functions as needed. How rounding, errors and exceptions are not done in the same fashion, but are close enough for low priority/reliability/precision work.  In general, we get at least 5 digits of accuracy from all non-core functions.  Some actually extend to 6 or 7.  The spflib.h header file contains simple definitions for each function.  All source code is included.

Because we are not employing the float type, function overloading and type conversion is not automated - you are responsibile for all your work.  Eventually, I intend to have a version that gives you a runtime debug option, but that is down the road.  It would tell you in which function things went wonky or not - your choice.  I have as a follow-up project a pre-pre processor phase which hopefully will address the overloading and type conversion automation.  But that is for later.

spf is definded as long.  That simplifies many of the type transition issues.  Other types employed internally to the package are q2.30, q4.28 and q2.62.  Those are employed for CORDIC algorithms (euler and trig).  We also employ angle folding to minimize CORDIC error issues.  For those looking to try contructing these yourself, keep in mind that you need to employ accumulators that contain twice as many bits as your desired result.  Took me a long time to figure that out.

The code in the initial release is messy, intentionally as it is a work in progress.  I will address that, eventually releasing a neatened up release, but I wanted to get this out so others could poke holes in it with a minimum effort. You will have to build the library from source.  My version of CP/M68K won't allow me to use cpmtools to copy files back to my linux workstation.

Instructions:  BY default we employ the M drive (for math) as our spflib repository.  I recommend you do all your work on the memory drive - for the rosco, this is typically B:  You need a minimum of 1Meg for it to be useful, but I recommend you size it at 1.5 Meg.

1. Save the original versions of stdio.h, stdlib.h adn string.h to another location.  Copy stdio.h, stdlib.h, and string.h from the M: drive to your a drive.
2. Build memory Drive:
   a. FLOAT M:  (if you put the files on another drive use that - this copys all the requisite files to the B:         drive which by default is the memory drive.
3. Add functions to clib:
   a. archive clib for safety!  pip clib.bak=clib[o]
   a. spfasm bdos.s
   b. c clibadds
   c. ar rav clib bdos.o clibadds.o
   d. save clib back to the base os drive (typically A)
4. Save everything back to your m: drive.
5. Build the object files:
   a. spfasm spfmul
   b. c spfcore
   c. c spfefs
   d. c spfefn
   e. c spft1
   f. c spft2
   g. c spfioi
   h. c spfioo
   i. cspf (builds the spflib library)
   j. save everything.
6. Add include of spflib into program (  #include "spflib.h"  )
   Minimum Includes are, in this order:
   a. ctype.h
   b. stdio.h
   c. spflib.h
7. compile program:  c <program name>
8. link program: spfl <program name>
9. Execute program.


Scripts:
1. c.sub - (modified) compile a c program - will often crash the system when an error is found.
2. ch.sub - (new) test compile a program - will generally not crash the system when finding an error.
3. cspf.sub - (new) builds the floating point library from scratch - spflib
4. save.sub - (new) saves the B memory drive files to the permanent storage drive
5. spfasm.sub - (new) compiles an assembler file for the floating point library
6. spfl.sub - (new) builds executable code (relocatable by default) that employs
           the spflib for a program.

spflib source files:
1.  spflib.h - header file for use in programs.
2.  spfmul.s - assembler routine for a single precision multiply.
3. spfcore.c - contains core functions.
4. spfefs.c - contains support routines for elementary functions.
5. spfefn.c - contains routines for base euler functions.
6. spft1.c - basic trig functions.
7. spft2.c - additional trig functions and some additional functions.
8. spfioi.c - ascii to aspf function.
9. spfioo.c - spf to ascii function.

Includes:
1. stdio.h - updated stdio.h file
2. stdlib.h - updated stdlib.h file.
3. string.h - updated string.h file
4. spflib.h - new spflib header file.



Code for support functions to be added to clib:
1. bdos.s - calls bdos functions from C code.
2. clibadds.c - adds the strchr and strrchr functions
