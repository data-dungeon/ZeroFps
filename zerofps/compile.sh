cd src

rm -f zerofps
rm -f *.o

g++ *.cpp -c -O3
g++ *.o -O3 -o ../zerofps  -lGL -lGLU -L/usr/X11R6/lib `sdl-config --cflags --libs`

ls -l *.o

cd ..
ls -l zerofps

