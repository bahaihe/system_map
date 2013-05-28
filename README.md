system_map
==========

Get Physical Address Of System Call And Function Pointer

This module read the system call table, and other function pointer specified in system.map

Input: physcial memory snapshot and system.map

Output: physical addresses of the function pointers and system call table specified in system.map 


Usage
---------
    ./signa [memory snapshot] [system.map]

example:
    ./signa  ~/qemu/mem-2.6.28a  ~/data_layout/system.map/System.map-2.6.28 > 2.6.28

