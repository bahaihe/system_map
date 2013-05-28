all:
	gcc  -o signa    getPgd.c  vtop.c  main.c  memload.c memory.c  -g 
clean:
	rm -f modhash
