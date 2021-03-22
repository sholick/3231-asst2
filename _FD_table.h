#ifndef _FD_table_H
#define _FD_table_H

#include <kern/errno.h>
#include <current.h>
#include <types.h>
#include <proc.h>
#include <spinlock.h>

struct FD_table {
	int num; // num of files opened
	struct OF_entry **table[128]; // table of file_open_entries
}


// creates FD_table for the process
struct FD_table *create_FD_table(void);

// insert an OF_entry into the FD_table
int insert_FD_entry(struct OF_entry * entry);

// remove an FD entry from FD_table
int remove_FD_entry(int fd_num)

// destroy that FD_table
void destory_FD_table(struct FD_table *table);

#endif