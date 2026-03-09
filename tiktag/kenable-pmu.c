#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/smp.h>

#ifdef MODULE
#ifndef fs_initcall
#define fs_initcall(fn) module_init(fn)
#endif
#endif


/* PMU related */
#define ARMV8_PMCR_E            (1 << 0) /* Enable all counters */
#define ARMV8_PMCR_P            (1 << 1) /* Reset all counters */
#define ARMV8_PMCR_C            (1 << 2) /* Cycle counter reset */
#define ARMV8_PMCR_N_SHIFT      11
#define ARMV8_PMCR_N_MASK       0x1f
#define ARMV8_PMCNTENSET_CCNT   (1UL << 31) /* Cycle counter enable bit */
#define ARMV8_PMUUSERNR_ALL     0x1f        /* Enable EL0 access to PMU controls/counters */

void pmu_test_init(void);

static int kpmu_proc_show(struct seq_file *m, void *v) {
	return 0;
}

static int kpmu_proc_open(struct inode *inode, struct file *file) {
	pmu_test_init();
	return single_open(file, kpmu_proc_show, NULL);
}

static ssize_t kpmu_write(struct file *file, const char __user *buf,
                                  size_t count, loff_t *ppos)
{
  return count;
}

static const struct proc_ops kpmu_proc_fops = {
	.proc_flags = PROC_ENTRY_PERMANENT,
	.proc_open = kpmu_proc_open,
	.proc_write = kpmu_write,
	.proc_release = single_release,
};

int __init kpmu_init(void)
{
	printk("Enable user PMU init\n");

	proc_create("enable-pmu", 0, NULL, &kpmu_proc_fops);
	pmu_test_init();
	printk("Enable user PMU: initialized on all online CPUs\n");

	return 0;
}

fs_initcall(kpmu_init);

static void init_pmu_on_cpu(void *info) {
     (void)info;
     unsigned long value = 0;
     unsigned long pmcr = 0;
     unsigned long counters = 0;
     unsigned long event_cnt = 0;

     asm volatile("MRS %0, PMCR_EL0" : "=r" (pmcr));
     event_cnt = (pmcr >> ARMV8_PMCR_N_SHIFT) & ARMV8_PMCR_N_MASK;
     value = pmcr | ARMV8_PMCR_E | ARMV8_PMCR_C | ARMV8_PMCR_P;
     asm volatile("MSR PMCR_EL0, %0" : : "r" (value));

     if (event_cnt >= 31)
         counters = ~0UL;
     else
         counters = (1UL << event_cnt) - 1;
     counters |= ARMV8_PMCNTENSET_CCNT;
     asm volatile("MSR PMCNTENSET_EL0, %0" : : "r" (counters));
 
     /* user enable */
     asm volatile("MRS %0, PMUSERENR_EL0" : "=r" (value));
     value |= ARMV8_PMUUSERNR_ALL;
     asm volatile("MSR PMUSERENR_EL0, %0" :: "r" (value));
     asm volatile("isb");
}

void pmu_test_init(void)
{
    on_each_cpu(init_pmu_on_cpu, NULL, 1);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Enable EL0 PMU access via /proc/enable-pmu");
