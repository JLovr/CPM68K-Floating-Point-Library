#include <ctype.h>
#include <stdio.h>
#include <spflib.h>

void    main()
{
spf     o1, o2, res;

#define pi4p2   0x3f1de9e6
#define pi4p4   0x3ec2d174
#define pi4p6   0x3e7058e9

        o1=pi4p2;
        o2=pi4p4;
        res=spfmul(o1,o2);
        printf("done\n");
}

