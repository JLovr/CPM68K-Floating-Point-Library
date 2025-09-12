rm ./CPMDISKM.IMG
mkfs.cpm -f 4mb-hd CPMDISKM.IMG
cpmcp -T raw -t -f 4mb-hd CPMDISKM.IMG *.c 0:
cpmcp -T raw -t -f 4mb-hd CPMDISKM.IMG *.h 0:
cpmcp -T raw -t -f 4mb-hd CPMDISKM.IMG *.s 0:
cpmcp -T raw -t -f 4mb-hd CPMDISKM.IMG *.sub 0:
cpmcp -T raw -t -f 4mb-hd CPMDISKM.IMG *.ME 0:
cpmls -f 4mb-hd CPMDISKM.IMG

