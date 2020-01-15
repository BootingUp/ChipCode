/**================================================================
 *   Copyright (C) 2020  ZhangYiFei. All rights reserved.
 *   
 *   文件名称：times.c
 *   创 建 者：ZhangYiFei
 *   创建日期：2020年01月13日
 *   描    述：
 *
 ================================================================**/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/export.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/list.h>

#include "mult_time.h"

typedef struct timer_set {
    unsigned char data;
    struct list_head next;
}timer_set_t;

static LIST_HEAD(timer_list_head);
static struct kmem_cache *timer_slab;

static DECLARE_WAIT_QUEUE_HEAD(mult_time_queue_head);

static struct timer_list    mult_timer[48];
static spinlock_t           splock;
static spinlock_t           timer_lock_list;
static int                  timer_status = 0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0)
static void mult_timer_process(unsigned long data)
{
    spin_lock_irq(&splock);
    timer_status += 1;
    spin_unlock_irq(&splock);

    wake_up_interruptible(&mult_time_queue_head);

	printk("%s\n", __func__);
}
#else
static void mult_timer_process(struct timer_list *list)
{

    spin_lock_irq(&splock);
    timer_status += 1;
    spin_unlock_irq(&splock);

    wake_up_interruptible(&mult_time_queue_head);

	printk("%s\n", __func__);
}
#endif


//打开函数
static int  misc_open(struct inode *node, struct file *fp)
{
	printk("this dev is open\r\n");
	return 0;
}

//关闭函数
static int misc_close(struct inode *node, struct file *fp)
{
	int i = 0;
    struct list_head    *pos        = NULL;
    timer_set_t         *ptr        = NULL;

	printk("this dev is close\r\n");

	for (i = 0; i < 48; i++) {
		del_timer(&mult_timer[i]);
	}

    while (!list_empty(&timer_list_head)) {
        pos = timer_list_head.next;
        spin_lock_irq(&timer_lock_list);
        list_del_init(pos);
        spin_unlock_irq(&timer_lock_list);

        ptr = (timer_set_t *)list_entry(pos, timer_set_t, next);
        kmem_cache_free(timer_slab, ptr);
    }

	return 0;
}

//读函数
ssize_t misc_read(struct file *fp, char __user *buf, size_t size, loff_t *loff)
{
    ssize_t status      = 0;
    unsigned char data  = 0x00;
    struct list_head    *pos    = NULL;
    timer_set_t         *ptr    = NULL;

	printk("this dev is read\r\n");

	DECLARE_WAITQUEUE(wait, current);

	add_wait_queue(&mult_time_queue_head, &wait);

	__set_current_state(TASK_INTERRUPTIBLE);


	wait_event_interruptible(mult_time_queue_head, timer_status > 0);
    
    spin_lock_irq(&splock);
    timer_status -= 1;
    if (timer_status < 0)
        timer_status = 0;
    spin_unlock_irq(&splock);
    
    spin_lock_irq(&timer_lock_list);
    if (!list_empty(&timer_list_head)) {
        pos = timer_list_head.prev;
        ptr = (timer_set_t *)list_entry(pos, timer_set_t, next);
        data = ptr->data;
        list_del_init(&ptr->next);
        kmem_cache_free(timer_slab, ptr);
        status = 1;
    } else
        status = -1;
    spin_unlock_irq(&timer_lock_list);

    copy_to_user(buf, &data, 1);

err:
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&mult_time_queue_head, &wait);

	return status;
}

long misc_ioctl(struct file *fp, unsigned int cmd, unsigned long cmd_data)
{
	int status = -1;	
	int timeout = 2;
    unsigned char data = 0x00;

    struct list_head    *pos = NULL;
    timer_set_t         *ptr_timer = NULL;

	mult_time_t *timer = (mult_time_t *)cmd_data;

	if (timer == NULL)
		return -EINVAL;

	if (timer->time_id < 0 || timer->time_id > 48)
		return -EINVAL;

	if (timer->timeout_s < 1 || timer->timeout_s > 25)
		timeout = 1;
	else
		timeout = timer->timeout_s;

	__set_current_state(TASK_INTERRUPTIBLE);

	switch (cmd) {
		case MULT_TIME_START:
			//mult_timer[timer->time_id].function = &mult_timer_process;
			mult_timer[timer->time_id].expires = jiffies + (HZ * timeout);
		#if LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0)
			mult_timer[timer->time_id].data = 0;
		#endif

            ptr_timer = (timer_set_t *)kmem_cache_alloc(timer_slab, GFP_KERNEL);
            if (ptr_timer == NULL) {
                status = -EAGAIN;
                break;
            }
            
            ptr_timer->data = timer->time_id;
            INIT_LIST_HEAD(&ptr_timer->next);

            spin_lock_irq(&timer_lock_list);
            list_add(&ptr_timer->next, &timer_list_head);
            spin_unlock_irq(&timer_lock_list);

			add_timer(&mult_timer[timer->time_id]);
            status = 0;

			break;

		case MULT_TIME_STOP:
			del_timer(&mult_timer[timer->time_id]);
            spin_lock_irq(&timer_lock_list);
            if(!list_empty(&timer_list_head)) {
                list_for_each(pos, &timer_list_head) {
                    ptr_timer = (timer_set_t *)list_entry(pos, timer_set_t, next);
                    if (ptr_timer->data == timer->time_id) {
                        list_del_init(&ptr_timer->next);    
                        kmem_cache_free(timer_slab, ptr_timer);
                        break;
                    }
                } 
            }
            spin_unlock_irq(&timer_lock_list);

            status = 0;
			break;
		default:
			status = -1;
			break;
	}

	return status;
}

//文件操作集合
static struct file_operations mult_time_fops={
	.owner			= THIS_MODULE,
	.open			= misc_open,
	.read			= misc_read,
	.unlocked_ioctl	= misc_ioctl,
	.release		= misc_close,
};


//设备相关信息
static struct miscdevice mult_time_misc={
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mult_time",
	.fops = &mult_time_fops,
};

//驱动初始化
static int __init misc_init(void)
{
	int i = 0;
	for (i = 0; i < 48; i++) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0)
		__init_timer(&mult_timer[i], mult_timer_process, TIMER_TRACE_FLAGMASK);
#else
		timer_setup(&mult_timer[i], mult_timer_process, 0);
#endif
	}

	if (misc_register(&mult_time_misc)) {
		printk("this module is insmod fail\r\n");
		return -1;
	}
    
    spin_lock_init(&splock);
    spin_lock_init(&timer_lock_list);

    timer_slab = kmem_cache_create("timer_cache", sizeof(timer_set_t) * 128, \
            0, SLAB_HWCACHE_ALIGN, NULL);
    
    if (timer_slab == NULL)
        goto err;


	printk("this module is success\r\n");
	return 0;

err:
	misc_deregister(&mult_time_misc);

    return -1;
}

//驱动卸载
static void __exit misc_exit(void)
{
	int i = 0;
    struct list_head    *pos = NULL;
    timer_set_t         *ptr = NULL;

	printk("this module is exit\r\n");

	misc_deregister(&mult_time_misc);

	for (i = 0; i < 48; i++) {
		del_timer(&mult_timer[i]);
	}
    

    while (!list_empty(&timer_list_head)) {
        pos = timer_list_head.next;
        spin_lock_irq(&timer_lock_list);
        list_del_init(pos);
        spin_unlock_irq(&timer_lock_list);

        ptr = (timer_set_t *)list_entry(pos, timer_set_t, next);
        kmem_cache_free(timer_slab, ptr);
    }

    kmem_cache_destroy(timer_slab);
}

module_init(misc_init);
module_exit(misc_exit);
MODULE_LICENSE("GPL");


