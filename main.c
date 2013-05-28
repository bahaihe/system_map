/*
 ============================================================================
 Name        : main.c
 Author      : Yufei Gu
 Version     :
 Copyright   : Copyright 2012 by UTD. all rights reserved. This material may
 	 	 	   be freely copied and distributed subject to inclusion of this
 	 	 	   copyright notice and our World Wide Web URL http://www.utdallas.edu
 Description : Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "memload.h"
#include "memory.h"

extern long long timeval_diff(struct timeval *difference,
                              struct timeval *end_time,
                              struct timeval *start_time);
unsigned getPgd(char *mem, int mem_size);

unsigned out_pc;
FILE *out_code;
struct timeval programStart;
char *snapshot;


void usage(char *prog)
{
    printf("%s snapshot\n", prog);
}

Mem *initMem(char *snapshot)
{
    struct timeval earlier;
    struct timeval later;
    if (gettimeofday(&earlier, NULL)) {
        perror("gettimeofday() error");
        exit(1);
    }
    char *mem;
    unsigned long mem_size;

    mem = mem_load(snapshot, &mem_size);
    if (mem == NULL)
        return NULL;
    else
        printf("mem '%s' load success! size is %ld\n", snapshot, mem_size);

    if (gettimeofday(&later, NULL)) {
        perror("gettimeofday() error");
        exit(1);
    }
    int loadTime = timeval_diff(NULL, &later, &earlier) / 1000;
    printf("Load mem time cost is %d milliseconds\n", loadTime);
    FILE *out_data;
    out_data = fopen("LoadMemTime", "a+");
    fprintf(out_data, "%d\t%s\n", loadTime, snapshot);
    fclose(out_data);

    //get pgd
    unsigned pgd = getPgd(mem, mem_size);

    //construct a struct Mem
    Mem *mem1 = (Mem *) malloc(sizeof(Mem));
    mem1->mem = mem;
    mem1->mem_size = mem_size;
    mem1->pgd = pgd;

    return mem1;
}


void get_paddrs(char *snapshot1, char *sys_map)
{
    Mem *mem = initMem(snapshot1);
    if (mem == NULL) {
        puts("The snapshot cannot load successfully!");
        return;
    }

    FILE *sys_map_file;
    sys_map_file = fopen(sys_map, "r");
    char line_buffer[BUFSIZ];   /* BUFSIZ is defined if you include stdio.h */
    int line_number;

    if (!sys_map_file) {
        printf("Couldn't open file %s for reading.\n", sys_map);
        return;
    }

    line_number = 0;
	puts("------------------data--------------------------");
	puts("index: physical_address virtual_address name");
    while (fgets(line_buffer, sizeof(line_buffer), sys_map_file)) {

        /* note that the newline is in the buffer */
        if (line_buffer[9] == 't' || line_buffer[9] == 'T'
            || strcmp(line_buffer + 11, "sys_call_table\n") == 0) {
            ++line_number;
            unsigned vaddr;
            sscanf(line_buffer, "%x", &vaddr);

            unsigned pAddr =
                vtop(mem->mem, mem->mem_size, mem->pgd, vaddr);
			printf("%d: %x %s", line_number, pAddr, line_buffer);
        }

		/*
        if (strcmp(line_buffer + 11, "sys_call_table\n") == 0) {
            unsigned vaddr;
            sscanf(line_buffer, "%x", &vaddr);

            unsigned pAddr =
                vtop(mem->mem, mem->mem_size, mem->pgd, vaddr);

			puts("sys_call_table:");
            int j;
            for (j = 0; j < 1600; j = j + 4) {
                unsigned value1 =
                    *((unsigned *) ((unsigned) mem->mem + pAddr + j));
                printf("%x\n", value1);
            }
        }
		*/

		/*
        if (strcmp(line_buffer + 11, "swapper_pg_dir\n") == 0) {
            unsigned vaddr;
            sscanf(line_buffer, "%x", &vaddr);

            unsigned pAddr =
                vtop(mem->mem, mem->mem_size, mem->pgd, vaddr);

			printf("%d: %x %s", line_number, pAddr, line_buffer);
        }
		*/
    }
    printf("\nTotal number of lines = %d\n", line_number);

	fclose(sys_map_file);
    free_mem(mem);
}

int main(int argc, char *argv[])
{
    if (argc < 1) {
        usage(argv[0]);
        return 1;
    }
    //load memory
    snapshot = argv[1];
    char *sys_map = argv[2];

    struct timeval later;
    if (gettimeofday(&programStart, NULL)) {
        perror("gettimeofday() error");
        exit(1);
    }

    get_paddrs(snapshot, sys_map);

    if (gettimeofday(&later, NULL)) {
        perror("gettimeofday() error");
        exit(1);
    }
    printf("Total time cost is %lld milliseconds\n",
           timeval_diff(NULL, &later, &programStart) / 1000);
    return EXIT_SUCCESS;
}
