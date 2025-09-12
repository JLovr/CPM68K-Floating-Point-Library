cd "/projects/RoscoCPM/FPLib20250425/Float Point Library"
./mkimg.sh
ls -al .
cp ./CPMDISKM.IMG "../C Preprocessor"
cd "/projects/RoscoCPM/FPLib20250425/C Preprocessor"
./SaveToImage.sh
ls -al .
cp ./CPMDISKM.IMG "../DiskImages"
cd "/projects/RoscoCPM/FPLib20250425/DiskImages"
rm /Media/jl/ROSCO-CPM/CPMDISKM.IMG
cp ./CPMDISKM.IMG /Media/jl/ROSCO-CPM
ls -al /Media/jl/ROSCO-CPM
