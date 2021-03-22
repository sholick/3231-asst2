#include <_OF_table.h>

#include <kern/error.h>
#include <lib.h>
#include <types.h>

static struct OF_table *_new_OF_entry();
static struct OF_table *_construct_OF_table();
static void destroy_OF_entry(struct OF_entry * OF_entry);
static void destroy_OF_table(struct OF_table *OF_table);


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

struct OF_table *OF_table = NULL;


int init_OF_table() {

	// check if initialized before
	if (OF_table != NULL)
		return 0;

	new_table = kmalloc(sizeof(*OF_table));
	KASSERT(new_table);
	OF_table = new_table;

	// initialize pointers as NULL
	for (int i=0; i<128; i++) {
		OF_table->table[i] = NULL;
	}

	spinlock_init(&OF_table->lock);
	OF_table->num = 0;

	return 0;
}


int new_file_open(char *path, int flags, mode_t mode, struct OF_entry **entry) {
	// use vfs_open to construct a new OF entry
	struct vnode *vnode = kmalloc(sizeof(struct vnode));
	struct OF_entry *new_entry = kmalloc(sizeof(*OF_entry));
	struct stat inode;

	if (!vnode || !new_entry) {
		kprintf("mem_alloc_OF_entry_failed");
		return -1;
	}

	int r = vfs_open(path, flags, mode, &vnode);

	if (r) {
		kprintf("mem_alloc_OF_entry_failed");
		return r;
	}

	new_entry->vnode = vnode;
	new_entry->flags = flags;
	new_entry->n_ref = 1;

	if (O_APPEND & flags) {
		// get offset
		int r = VOP_STAT(vnode, &inode);
		if (r)
			return r;
		new_entry->offset - inode.st_size;
	}
	else
		new_entry->offset = 0;

	*entry = new_entry;
	return 0;
}

int *new_OF_entry(int flags, mode_t mode, int index){

	// create new entry based on vfs_open params
	struct OF_entry *new_entry;
	char con_path[5] = 'con:\0';

	new_file_open(con_path, flags, mode, &new_entry);

	// Put new file into Open File table
	acquire_OFLock();

	if (OF_table->num >= 128) {
		//too many files opened
		vfs_close(OF_entry->vnode);
		kfree(OF_entry);
		release_OFLock();
		return ENFILE;
	}

	//proceed
	int i = 0;
	while (i < 128){
		if (OF_table->table[i] == NULL) {
			// found
			new_entry->index = i;
			OF_table->table[i] = new_entry;
			OF_table->num ++;
		}
		i++;
	}

	curproc -> FD_table -> table[index] = new_entry;
	curproc -> FD_table -> num ++;

	release_OFLock();
	return 0;
}


// destroy an entry in the OF_table using entry pointer
static void destroy_OF_entry(struct OF_entry * entry) {
	KASSERT(OF_table->table[entry->index] == entry); // check that the entry is valid

	i = entry->index;
	node = entry->vnode;

	vfs_close(node);
	OF_table -> table[i] = NULL;
	OF_table -> num--;
	kfree(entry);
}

void destory_OF_table() {
	//clean up the table
	for (int i = 0; i<128; i++) {
		if (OF_table->table[i] != NULL)
			_destroy_OF_entry(OF_table->table[i]);
	}
	//clean up the lock
	spinlock_cleanup(&OF_table_lock);
	//free the table
	kfree(OF_table);

	return;
}


/*
static struct file_ref *_new_file_ref() {
	struct file_ref *new_file_ref = kmalloc(sizeof(*file_ref))
	KASSERT(new_file_ref);

	new_file_ref -> flags = NULL;
	new_file_ref -> offset = 0;
	new_file_ref -> vnode = NULL;
	new_file_ref -> lock = lock_create("new_file_lock");

	return new_file_ref;
}


struct file_ref *create_file_entry() {
	struct OF_entry *new_OF_entry = _new_OF_entry();
	struct file_ref *new_file_ref = _new_file_ref();

	new_OF_entry->file = new_file_ref;
	// new_file_ref->reference = new_OF_entry;
	return new_OF_entry;
}
*/