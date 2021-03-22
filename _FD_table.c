#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/unistd.h>
#include <limits.h>
#include <spinlock.h>

#include <_FD_table.h>
#include <_OF_table.h>
/*
	Per-process FD table that gets generated whenever a new process spawns.
	Generates stdin, stdout, stderr in the table when first created.
*/

struct FD_table *create_FD_table() {
	struct FD_table *table = kmalloc(sizeof(*table));
	KASSERT(table);
	memset(table, NULL, sizeof(*table));

	table->num = 0;
	cur_proc -> FD_table = table;

	// open the stdin, stdout, stderr
	int result[3] = {};
	result[0] = new_OF_entry(O_RDONLY, 0, STDIN_FILENO);
	result[1] = new_OF_entry(O_WRONLY, 0, STDOUT_FILENO);
	result[2] = new_OF_entry(O_WRONLY, 0, STDERR_FILENO);

	// check error and rewind if found
	for (int i=0; i<3; i++) {
		if (result[i]) {
			kfree(table);
			return result[i];
		}
	}

	// end
	return 0;
}

// insert into FD_table using a created OF_entry, return 0 if succeed
int insert_FD_entry(struct OF_entry * entry) {

	// check if too much files open by process
	if (cur_proc -> FD_table -> num > 128)
		return EMFILE;

	// insert into FD_table
	for(int i=0; i<128; i++) {
		if (cur_proc -> FD_table -> table[i] == NULL) {
			cur_proc -> FD_table -> table[i] = entry;
		}
	}

	return 0;
}


int remove_FD_entry(int fd_num) {

	//assert the number is valid
	KASSERT(FD_table -> table[fd_num] != NULL);

	// decrement num_ref of FD table and OF table;
	FD_table -> num--;
	FD_table -> table[i] -> n_ref --;

	// Destroy the OF entry if reference count is zero
	if (FD_table -> table[i] -> n_ref <= 0) {
		destroy_OF_entry(FD_table -> table[i]);
	}

	//clear the FD_table entry
	FD_table -> table[i] = NULL;
}


void destroy_FD_table(struct FD_table *FD_table) {

	for(int i=0; i<128; i++) {
		if (FD_table -> table[i] != NULL) {
			remove_FD_entry(i);
		}
	}
	kfree(table);
}
/*
int open_std_FD(int flag, int index) {
	char path[5] = "con:\0";
	struct file_entry *file;
	int res;

	res = create_open_file(path, flag, 0, &file);
	if (res)
		return res;

	acquire_OFLock();
	
int get_file(int fd, struct file_ref **Fd)
}
*/
