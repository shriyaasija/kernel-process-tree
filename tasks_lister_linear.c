#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/signal.h>

int tasks_lister_linear_init(void) {
  printk(KERN_INFO "Loading module...\n");

  struct task_struct *task;

  for_each_process(task){
    printk(KERN_INFO "[LINEAR] pid: %d | pname: %s | state: %ld\n", task->pid, task->comm, task->flags);
  }

  printk(KERN_INFO "Module loaded.\n");
  return 0;
}

void tasks_lister_linear_exit(void) {
  printk(KERN_INFO "Module removed.\n");
}

module_init(tasks_lister_linear_init);
module_exit(tasks_lister_linear_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("List tasks linearly");
MODULE_AUTHOR("Shriya Asija");
