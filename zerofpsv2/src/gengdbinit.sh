find * -type d -print | grep -v CVS | awk '{print "dir ../src/"$1}' > ../bin/.gdbinit
