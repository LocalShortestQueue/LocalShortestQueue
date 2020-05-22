# Creating the  executables
all: lsq

lsq:  ./*   
	g++ -Wall -o2 -march=native *.cpp -o LSQ.exe -std=c++11

		
# Cleaning old files before new make
clean:
	rm -f LSQ.exe 

