#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/file.h>

static int parent_pid = 0;  
module_param(parent_pid, int, 0644);
MODULE_PARM_DESC(parent_pid, "Parent process ID");

static struct proc_dir_entry *proc_entry;
	
void print_memory_map(struct seq_file *m, struct task_struct *task, int level) {
	struct mm_struct *mm = task->mm;
	seq_printf(m, "%*s| PID: %d | Parent: %d | Name: %s\n", level * 4, "", task->pid, task->parent->pid, task->comm);
	
	if(mm) {
		seq_printf(m, "%*s Data:  0x%lx\n", level * 4, "", mm->start_data);
		seq_printf(m, "%*s Heap:  0x%lx\n", level * 4, "", mm->start_brk);
		seq_printf(m, "%*s Stack: 0x%lx\n", level * 4, "", mm->start_stack);
	}
}

void dfs(struct seq_file *m, struct task_struct *task, int level) {
    struct task_struct *child;
    struct list_head *list;

    print_memory_map(m, task, level);

    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        dfs(m, child, level + 1);
    }
}

static int show_deets(struct seq_file *m, void *v) {
	struct task_struct *task;
	seq_printf(m, "Process Tree: ");
	for_each_process(task) {
		if(task->pid == parent_pid) {
			printk(KERN_INFO "Starting traversal from PID %d...\n\n", parent_pid);
			dfs(m, task, 0);
			break;
		}
	}
	
	return 0;
	
}

static int mem_map_open(struct inode *inode, struct file *file) {
	return single_open(file, show_deets, NULL);
}

static const struct proc_ops proc_ops = {
	.proc_open = mem_map_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release
};

static int task_lister_init(void) {
    printk(KERN_INFO "Loading module...\n");

    proc_create("mem_tree", 0644, NULL, &proc_ops);
    
    printk(KERN_INFO "Module loaded.\n");
    return 0;
}

static void task_lister_exit(void) {
	remove_proc_entry("mem_tree", NULL);
    printk(KERN_INFO "Module removed.\n");
}

module_init(task_lister_init);
module_exit(task_lister_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Lists tasks and memory maps in a tree structure");
MODULE_AUTHOR("Shriya Asija");

