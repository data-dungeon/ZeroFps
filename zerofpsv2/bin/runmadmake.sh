#sh

echo Running in workdirectory, and setting LD_LIBRARY_PATH to curent direcotry
export templdpath=$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH

cd `dirname $0`
./madmake.bin $@ 

export LD_LIBRARY_PATH=$templdpath

