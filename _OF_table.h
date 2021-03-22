#ifndef _OF_Table_H
#define _OF_Table_H

#include <types.h>
#include <spinlock.h>
#include <synch.h>
#include <vfs.h>

#define acquire_OFLock() (spinlock_acquire(&OF_table->lock))
#define release_OFLock() (spinlock_release(&OF_table->lock))

struct OF_table {
	struct spinlock lock;
	struct OF_entry table[128];
	int num;
}

struct OF_entry {
	struct vnode * vnode;	// file vnode
	int flags;
	int offset;
	int n_ref;
	int index;
}

// create an OF_table if not initialized.
int init_OF_table();

// destroy OF_table;
void destroy_OF_table(void);

// open file using vfs_open() and flags; return 0 if succeed. Stores result in entry pointer
int new_file_open(char *path, int flags, mode_t mode, struct OF_entry **entry)

// Creates entry and put into the OF_table;
int *new_OF_entry(int flags, mode_t mode, int index);

// destroy the OF_entry
static void destroy_OF_entry(struct OF_entry * entry);

void destory_OF_table();

// int release_file_ref(struct file_ref *file_ref);

extern struct OF_table *OF_table;

#endif