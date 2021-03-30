DIROBJ := obj/
DIREXE := exec/
DIRSRC := src/

CC := mpicc
CR := mpirun

all : dirs toroide 

dirs:
	mkdir -p $(DIROBJ) $(DIREXE) $(DIRSRC)

toroide: 
	$(CC) $(DIRSRC)/toroide.c -o $(DIROBJ)/toroide.o
	$(CC) $(DIRSRC)/toroide.c -o $(DIREXE)/toroide

hipercubo: 
	$(CC) $(DIRSRC)/hipercubo.c -o $(DIROBJ)/hipercubo.o
	$(CC) $(DIRSRC)/hipercubo.c -o $(DIREXE)/hipercubo

testToroide:
	$(CR) -n 16 -oversubscribe ./$(DIREXE)toroide

testHipercubo:
	$(CR) -n 16 -oversubscribe ./$(DIREXE)hipercubo

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE)
