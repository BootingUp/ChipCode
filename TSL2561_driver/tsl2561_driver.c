#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/of_device.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include "tsl2561_driver.h"

/***********************************************************
  TSL2561 configure reg
 ************************************************************/
typedef enum TSL2561_REG_COMMAND_TYPE_BIT {
	TSL2561_REG_COMMAND_BIT_CMD                 = (0x01 << 7),
	TSL2561_REG_COMMAND_BIT_CLEAR               = (0x01 << 6),
	TSL2561_REG_COMMAND_BIT_WORD                = (0x01 << 5),
	TSL2561_REG_COMMAND_BIT_BLOCK               = (0x01 << 4),
	TSL2561_REG_COMMAND_BIT_ADDRESS             = (0x00),
}__TSL2561_REG_COMMAND_TYPE_BIT;


typedef enum TSL2561_REG_CONTROL_REGVALUE {
	TSL2561_REG_CONTROL_POWERED_UP          = 0x03,
	TSL2561_REG_CONTROL_POWERED_DOWN        = 0x00,
}__TSL2561_REG_CONTROL_REGVALUE;

typedef enum TSL2561_REG_INTCTL_INTR_SEL {
	TSL2561_REG_INTCTL_INTR_INTERRUPT_DISABLE		= 0x00 << 4,
	TSL2561_REG_INTCTL_INTR_LEVEL_INT               = 0x01 << 4,
	TSL2561_REG_INTCTL_INTR_SMB_COMP                = 0x02 << 4,
	TSL2561_REG_INTCTL_INTR_TEST_MODE               = 0x03 << 4,
}__TSL2561_REG_INT_CTL_INTCONT_SEL;

typedef enum TSL2561_REG_INTCTL_PERSIST_SEL {
	TSL2561_REG_INTCTL_PERSIST_EVERY_ADC            = 0x00,
	TSL2561_REG_INTCTL_PERSIST_ANY_RANGE            = 0x01,
	TSL2561_REG_INTCTL_PERSIST_2RANGE               = 0x02,
	TSL2561_REG_INTCTL_PERSIST_3RANGE               = 0x03,
	TSL2561_REG_INTCTL_PERSIST_4RANGE               = 0x04,
	TSL2561_REG_INTCTL_PERSIST_5RANGE               = 0x05,
	TSL2561_REG_INTCTL_PERSIST_6RANGE               = 0x06,
	TSL2561_REG_INTCTL_PERSIST_7RANGE               = 0x07,
	TSL2561_REG_INTCTL_PERSIST_8RANGE               = 0x08,
	TSL2561_REG_INTCTL_PERSIST_9RANGE               = 0x09,
	TSL2561_REG_INTCTL_PERSIST_10RANGE              = 0x0A,
	TSL2561_REG_INTCTL_PERSIST_11RANGE              = 0x0B,
	TSL2561_REG_INTCTL_PERSIST_12RANGE              = 0x0C,
	TSL2561_REG_INTCTL_PERSIST_13RANGE              = 0x0D,
	TSL2561_REG_INTCTL_PERSIST_14RANGE              = 0x0E,
	TSL2561_REG_INTCTL_PERSIST_15RANGE              = 0x0F,
}__TSL2561_REG_INTCTL_PERSIST_SEL;


typedef enum TSL2561_REG {
	TSL2561_REG_CONTROL         = 0x00,
	TSL2561_REG_TIMING          = 0x01,
	TSL2561_REG_THRESHLOWLOW    = 0x02,
	TSL2561_REG_THRESHLOWHIGH   = 0x03,
	TSL2561_REG_THRESHHIGHLOW   = 0x04,
	TSL2561_REG_THRESHHIGHHIGH  = 0x05,
	TSL2561_REG_INTCTL          = 0x06,
	TSL2561_REG_CRC             = 0x08,
	TSL2561_REG_ID              = 0x0A,
	TSL2561_REG_DATA_0          = 0x0C, // DATA0 ~ DATA3
}__TSL2561_REG;

#define		TSL2561_BASE_REG	(TSL2561_REG_COMMAND_BIT_CMD | \
		TSL2561_REG_COMMAND_BIT_CLEAR)

/////////////////////////////////////////////////////

struct tsl2561_core {
	struct cdev 			cdev;
	dev_t					dev_t;
	struct i2c_client 		*client;
	struct fasync_struct 	*tsl2561_async_queue;	
	struct device 			*dev;
	struct gpio_desc 		*int_gpio;
	int						irq;
	spinlock_t				spinlock;
	int						flags;
};

static DECLARE_WAIT_QUEUE_HEAD(tsl2561_wq);

static struct class 	*tsl2561_class;
struct device 			*this_device;

#define					TSL2561_DEV_COUNT		1

static inline struct tsl2561_core * to_tsl2561(struct cdev *cdev)
{
	return container_of(cdev, struct tsl2561_core, cdev);
}

/*******************************************************
  I2C continuous data operation
 *******************************************************/
static int tsl2561_regs_write(struct i2c_client *client, u8 *val, int len)
{
	int ret;

	struct i2c_msg msg = {
		.addr   = client->addr,
		.flags  = 0,
		.len    = len,
		.buf    = val,
	};

	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret < 0) {
		dev_err(&client->dev, "Failed writing register 0x%04x!\n", val[0]);
		return ret;
	}

	return 0;
}

static int tsl2561_regs_read(struct i2c_client *client, u8 reg, u8 *val, int len)
{
	int ret;

	struct i2c_msg msg[2] = {
		[0] = {
			.addr   = client->addr,
			.flags  = 0,
			.buf    = &reg,
			.len    = 1,
		},

		[1] = {
			.addr   = client->addr,
			.flags  = I2C_M_RD,
			.buf    = val,
			.len    = len,
		},
	};

	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret < 0)
		goto err;

	return len;

err:
	dev_err(&client->dev, "Failed reading register 0x%02x \n", reg);
	return ret;
}

static int inline tsl2561_reg_read(struct i2c_client *client, u8 addr)
{
	int rc;

	rc = i2c_smbus_read_byte_data(client, addr);
	if (rc < 0) { 
		dev_err(&client->dev, "i2c i/o error: rc == %d\n", rc); 
		return rc;
	}    

	dev_warn(&client->dev, "tsl2561: read 0x%02x = 0x%02x\n", addr, rc); 

	return rc;
}

static int inline tsl2561_reg_write(struct i2c_client *client, u8 reg, u8 value)
{
	int rc;

	rc = i2c_smbus_write_byte_data(client, reg, value);
	if (rc < 0) 
		dev_err(&client->dev, "tsl2561: write 0x%02x = 0x%02x\n", reg, value);

	return rc;
}

static void inline tsl2561_poweron(struct i2c_client *client, u8 reg)
{
	tsl2561_reg_write(client, reg, TSL2561_REG_CONTROL_POWERED_UP);
}

static void inline tsl2561_poweroff(struct i2c_client *client, u8 reg)
{
	tsl2561_reg_write(client, reg, TSL2561_REG_CONTROL_POWERED_DOWN);
}

static int tsl2561_get_device_id(struct i2c_client *client, u8 reg)
{
	int rc;
	rc = tsl2561_reg_read(client, reg);
	if ( rc < 0)
		return rc;

	printk(KERN_INFO"TSL2561 device id %02x\n", rc);

	return rc;
}


static int tsl2561_set_threshold_value(struct i2c_client *client, \
		u8 reg, u8 *value, int len)
{
	int rc;
	u8 regs[10];

	regs[0] = reg;
	memcpy(&regs[1], value, len);

	rc = tsl2561_regs_write(client, regs, len + 1);
	if (rc < 0)
		dev_err(&client->dev, "set threshold value is failed\n");

	return 0;
}

static int tsl2561_get_threshold_value(struct i2c_client *client, \
		u8 reg, u8 *value, int len)
{
	int rc;

	rc = tsl2561_regs_read(client, reg, value, len);
	if (rc < 0)
		dev_err(&client->dev, "get threshold value is failed\n");

	return rc;
}


static irqreturn_t tsl2561_irq(int irq, void *data)
{
	struct tsl2561_core *core = (struct tsl2561_core *)data;

	spin_lock_irq(&core->spinlock);
	core->flags = 1;
	spin_unlock_irq(&core->spinlock);

	wake_up_interruptible(&tsl2561_wq);
	kill_fasync(&core->tsl2561_async_queue, SIGIO, POLL_IN);
    
    printk("%s Run\n", __func__);
	return IRQ_HANDLED;
}


static int tsl2561_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	struct tsl2561_core *core = to_tsl2561(inode->i_cdev);
	file->private_data = core;

	tsl2561_poweron(core->client, TSL2561_BASE_REG | TSL2561_REG_CONTROL);

	printk("%s RUN\n", __func__);

	return ret;
}

static ssize_t tsl2561_read(struct file *file, char __user *buf,
		size_t count, loff_t *ppos)
{
	int ret = 0;
	u8 data[4] = { 0 };

	DECLARE_WAITQUEUE(wait, current);

	struct tsl2561_core *core = file->private_data;

	if (file->f_flags & O_NONBLOCK) 
	{
		return -EAGAIN;
	}

	add_wait_queue(&tsl2561_wq, &wait);

	__set_current_state(TASK_INTERRUPTIBLE);


	ret = tsl2561_regs_read(core->client, TSL2561_BASE_REG | TSL2561_REG_DATA_0, 
			data, sizeof(data));
	if (ret < 0) 
	{
		ret = -EBADMSG;
		goto err;
	}

	//wait_event_interruptible(tsl2561_wq, core->flags);

	ret = copy_to_user(buf, data, sizeof(data));

	spin_lock_irq(&core->spinlock);
	core->flags = 0;
	spin_unlock_irq(&core->spinlock);

#if 0
	if (signal_pending(current)) 
    {
		ret = -ERESTARTSYS;
		goto err;
	}
#endif

err:
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&tsl2561_wq, &wait);

	return 4;
}

static long tsl2561_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	struct tsl2561_core *core = file->private_data;	

	tsl2561_thr_value __user *tsl2561_thr = (tsl2561_thr_value __user *)arg;

	switch (cmd)
	{
		case TSL_EN_INT:
			ret = tsl2561_reg_write(core->client, 
					TSL2561_BASE_REG | TSL2561_REG_INTCTL,  
					TSL2561_REG_INTCTL_INTR_TEST_MODE | 
					TSL2561_REG_INTCTL_PERSIST_10RANGE);
            printk("%s TSL_EN_INT RUN\n", __func__);
			break;

		case TSL_DIS_INT:
			ret = tsl2561_reg_write(core->client, 
					TSL2561_BASE_REG | TSL2561_REG_INTCTL,  
					TSL2561_REG_INTCTL_INTR_INTERRUPT_DISABLE);

            printk("%s TSL_DIS_INT RUN\n", __func__);

			break;

		case TSL_SET_THR:
			if (tsl2561_thr != NULL )
			{
				ret = tsl2561_set_threshold_value(core->client, 
						TSL2561_BASE_REG | TSL2561_REG_THRESHLOWLOW,
						tsl2561_thr->data, 4);

                printk("%s TSL_SET_THR RUN\n", __func__);
			}
			break;

		case TSL_GET_THR:

			if (tsl2561_thr != NULL )
			{
				ret = tsl2561_get_threshold_value(core->client, 
						TSL2561_BASE_REG | TSL2561_REG_THRESHLOWLOW,
                        tsl2561_thr->data, 4);

                printk("%s TSL_GET_INT RUN\n", __func__);
			}
			break;

		default :
			ret = -1;
			break;
	}

	return ret;
}


/****************************************************
  Registration signal

  Use kill_fasync to send a signal in the interrupt handler

  kill_fasync(&tsl2561_async_queue, SIGIO, POLL_IN);

 *****************************************************/
static int tsl2561_fasync(int fd, struct file *filp, int mode)
{
	struct tsl2561_core *core = filp->private_data;

	return fasync_helper(fd, filp, mode, &core->tsl2561_async_queue);
}


static int tsl2561_release(struct inode *inode, struct file *file)
{
	struct tsl2561_core *core = file->private_data;


	tsl2561_poweroff(core->client, TSL2561_BASE_REG | TSL2561_REG_CONTROL);

	//Remove file handles from the asynchronous notification list
	tsl2561_fasync(-1, file, 0);

	return 0;
}


static struct file_operations tsl2561_fops = {
	.owner			= THIS_MODULE,
	.open			= tsl2561_open,
	.release		= tsl2561_release,
	.read       	= tsl2561_read,
	.unlocked_ioctl = tsl2561_ioctl,
	.fasync     	= tsl2561_fasync,
};


/*************************************************
  I2C Client & Driver 
 **************************************************/
static int tsl2561_probe(struct i2c_client *c,
		const struct i2c_device_id *id)
{
	int ret = -1;
	struct tsl2561_core *core;

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(c->adapter,
				I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE_DATA))
		return -EIO;


	core = devm_kzalloc(&c->dev, sizeof(struct tsl2561_core), GFP_KERNEL);
	if (!core)
		return -ENOMEM;

	core->client = c;
	core->dev = &c->dev;

	i2c_set_clientdata(c, core);

	core->int_gpio = devm_gpiod_get_optional(&c->dev, "int", GPIOD_IN);
	if (core->int_gpio != NULL)
	{
		core->irq = gpiod_to_irq(core->int_gpio);
		if (core->irq  == 0)
			return -EINVAL;

		printk("irq number:%d\n", core->irq);

		ret = devm_request_threaded_irq(core->dev, 
				core->irq,  NULL, tsl2561_irq, 
				IRQF_TRIGGER_FALLING | IRQF_ONESHOT, 
				"tsl2561_int", core);
		if (ret < 0)
		{
			dev_err(&c->dev, "Application for device interruption failed\n");
			goto err_request_irq;
		}
	}

	ret = alloc_chrdev_region(&core->dev_t, 0, TSL2561_DEV_COUNT, "tsl2561_cdev");
	if ( ret < 0)
	{
		printk("register cdev is error\n");
		goto err_alloc_chrdev;
	}

	spin_lock_init(&core->spinlock);

	cdev_init(&core->cdev, &tsl2561_fops);

	ret = cdev_add(&core->cdev, core->dev_t, TSL2561_DEV_COUNT);
	if ( ret < 0)
	{
		printk("Registered character device failed\n");
		goto err_cdev;
	}

	tsl2561_class = class_create(THIS_MODULE, "tsl2561_chrdev");
	ret = PTR_ERR(tsl2561_class);
	if (IS_ERR(tsl2561_class))
		goto err_class;

	this_device = device_create(tsl2561_class, NULL, core->dev_t, 
			NULL, "tsl_dev", NULL);

	if (this_device == NULL)
	{
		ret = PTR_ERR(this_device);
		goto err_device;
	}

	printk("Init TSL2561 device is OK\n");

	return 0;

err_device:
	class_destroy(tsl2561_class);

err_class:
	cdev_del(&core->cdev);

err_cdev:
	unregister_chrdev_region(core->dev_t, TSL2561_DEV_COUNT);

err_alloc_chrdev:
	if ((core->int_gpio != NULL) && (core->irq > 0))
		devm_free_irq(core->dev, core->irq, core);

err_request_irq:
	devm_kfree(&c->dev, core);

	return ret;
}

static int tsl2561_remove(struct i2c_client *c)
{
	struct tsl2561_core *core = i2c_get_clientdata(c);


	device_destroy(tsl2561_class, core->dev_t);
	class_destroy(tsl2561_class);

	cdev_del(&core->cdev);

	unregister_chrdev_region(core->dev_t, TSL2561_DEV_COUNT);

	if ((core->int_gpio != NULL) && (core->irq > 0))
		devm_free_irq(core->dev, core->irq, core);

	devm_kfree(&c->dev, core);

	return 0;
}


/*******************************************************/

static const struct i2c_device_id tsl2561_id[] = {
	{ "tsl2561", 0 }, 
	{ }  
};
MODULE_DEVICE_TABLE(i2c, tsl2561_id);

static struct i2c_driver tsl2561_driver = {
	.driver = {
		.name   = "tsl2561",
	},   
	.probe      = tsl2561_probe,
	.remove     = tsl2561_remove,
	.id_table   = tsl2561_id,
};

module_i2c_driver(tsl2561_driver);

MODULE_AUTHOR("zhang_yong_jies163.com");
MODULE_DESCRIPTION("This is the TSL2561 I2C Driver");
MODULE_LICENSE("GPL");

