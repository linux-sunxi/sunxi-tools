/*
 * Driver for AW USB which is for downloading firmware
 *
 * Cesc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Changelog:
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/hardirq.h>
#include <linux/errno.h>
#include <linux/random.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/usb.h>
#include <linux/wait.h>

/* by Cesc */
#include <linux/ioctl.h>
#include <linux/mutex.h>


/*
 * Version Information
 */
#define DRIVER_VERSION "v1.0"
#define DRIVER_AUTHOR "Cesc"
#define DRIVER_DESC "AW USB driver"

#define AW_MINOR	64

/* stall/wait timeout for AWUSB */
#define NAK_TIMEOUT (HZ)

#define IBUF_SIZE 0x1000

/* Size of the AW buffer */
#define OBUF_SIZE 0x10000

/* Max size of data from ioctl */
#define IOCTL_SIZE 0x10000 /* 16k */

struct aw_usb_data {
	struct usb_device *aw_dev;	/* init: probe_aw */
	unsigned int ifnum;		/* Interface number of the USB device */
	int isopen;			/* nz if open */
	int present;			/* Device is present on the bus */
	char *obuf, *ibuf;		/* transfer buffers */
	char bulk_in_ep, bulk_out_ep;	/* Endpoint assignments */
	wait_queue_head_t wait_q;	/* for timeouts */
	struct mutex lock;		/* general race avoidance */
};

/* by Cesc */
struct usb_param {
	unsigned		test_num;
	unsigned		p1;	/* parameter 1 */
	unsigned		p2;
	unsigned		p3;
};

struct aw_command {
	int value;
	int length;
	void __user *buffer;
};

#define AWUSB_IOC_MAGIC 's'

#define AWUSB_IOCRESET _IO(AWUSB_IOC_MAGIC, 0)
#define AWUSB_IOCSET   _IOW(AWUSB_IOC_MAGIC, 1, struct usb_param)
#define AWUSB_IOCGET   _IOR(AWUSB_IOC_MAGIC, 2, struct usb_param)
#define AWUSB_IOCSEND  _IOW(AWUSB_IOC_MAGIC, 3, struct aw_command)
#define AWUSB_IOCRECV  _IOR(AWUSB_IOC_MAGIC, 4, struct aw_command)
#define AWUSB_IOCSEND_RECV _IOWR(AWUSB_IOC_MAGIC, 5, struct aw_command)
/* AWUSB_IOCSEND_RECV, how to implement it? */



static struct aw_usb_data aw_instance;

static int open_aw(struct inode *inode, struct file *file)
{
	struct aw_usb_data *aw = &aw_instance;

	/* mutex_lock(&(aw->lock)); */

	if (aw->isopen || !aw->present) {
		mutex_unlock(&(aw->lock));
		return -EBUSY;
	}
	aw->isopen = 1;

	init_waitqueue_head(&aw->wait_q);

	/* mutex_unlock(&(aw->lock)); */

	dev_info(&aw->aw_dev->dev, "aw opened.\n");

	return 0;
}

static int close_aw(struct inode *inode, struct file *file)
{
	struct aw_usb_data *aw = &aw_instance;

	aw->isopen = 0;

	dev_info(&aw->aw_dev->dev, "aw closed.\n");
	return 0;
}

static long ioctl_aw(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct aw_usb_data *aw = &aw_instance;
	int retval = 0;
	struct usb_param param_tmp;

	struct aw_command aw_cmd;
	void __user *data;
	unsigned char *buffer;
	int value = 0;
	int buffer_len = 0;

	int result = 0;
	int actual_len = 0;

	mutex_lock(&(aw->lock));
	if (aw->present == 0 || aw->aw_dev == NULL) {
		retval = -ENODEV;
	goto err_out;
	}

	pr_debug("ioctl_aw--enter\n");

	pr_debug("cmd = %d\n", cmd);

	param_tmp.test_num = 0;
	param_tmp.p1 = 0;
	param_tmp.p2 = 0;
	param_tmp.p3 = 0;

	switch (cmd) {
	case AWUSB_IOCRESET:
		pr_debug("ioctl_aw--AWUSB_IOCRESET\n");
		break;

	case AWUSB_IOCSET:
		pr_debug("ioctl_aw--AWUSB_IOCSET\n");

		if (copy_from_user(
			&param_tmp, (void __user *)arg, sizeof(param_tmp))) {
			retval = -EFAULT;
		}

		pr_debug("param_tmp.test_num = %d\n", param_tmp.test_num);
		pr_debug("param_tmp.p1 = %d\n", param_tmp.p1);
		pr_debug("param_tmp.p2 = %d\n", param_tmp.p2);
		pr_debug("param_tmp.p3 = %d\n", param_tmp.p3);
		break;

	case AWUSB_IOCGET:
		pr_debug("ioctl_aw--AWUSB_IOCGET\n");

		param_tmp.test_num = 3;
		param_tmp.p1 = 4;
		param_tmp.p2 = 5;
		param_tmp.p3 = 6;

		if (copy_to_user(
			(void __user *)arg, &param_tmp, sizeof(param_tmp))) {
			retval = -EFAULT;
		}
		break;

	case AWUSB_IOCSEND:
		pr_debug("ioctl_aw--AWUSB_IOCSEND\n");

		data = (void __user *)arg;
		if (data == NULL)
			break;

		if (copy_from_user(&aw_cmd, data, sizeof(struct aw_command))) {
			retval = -EFAULT;
			goto err_out;
		}

		buffer_len = aw_cmd.length;
		value = aw_cmd.value;
		pr_debug("buffer_len=%d\n", buffer_len);
		pr_debug("value=%d\n", value);
		if (buffer_len > IOCTL_SIZE) {
			retval = -EINVAL;
			goto err_out;
		}
		buffer = kmalloc(buffer_len, GFP_KERNEL);
		if (!(buffer)) {
			dev_err(&aw->aw_dev->dev,
				"AWUSB_IOCSEND: Not enough memory for the send buffer");
			retval = -ENOMEM;
			goto err_out;
		}

		/* stage 1, get data from app */
		if (copy_from_user(buffer, aw_cmd.buffer, aw_cmd.length)) {
			retval = -EFAULT;
			kfree(buffer);
			goto err_out;
		}

#if 0
		int ii = 0;

		for (ii = 0; ii < buffer_len; ii++)
				pr_debug(
				"*(buffer + %d) = %d\n", ii, *(buffer + ii));
		pr_debug("*buffer=%d, *(buffer+1)=%d\n", *buffer, *(buffer+1));
#endif

		/* stage 2, send data to usb device */
		result = usb_bulk_msg(aw->aw_dev,
		usb_sndbulkpipe(aw->aw_dev, 1),
		buffer, buffer_len, &actual_len, 5000);
		if (result) {
			kfree(buffer);

			dev_err(&aw->aw_dev->dev,
				"Write Whoops - %08x", result);
			retval = -EIO;
			goto err_out;
		}

		kfree(buffer);

		pr_debug("ioctl_aw--AWUSB_IOCSEND-exit\n");
		break;

	case AWUSB_IOCRECV:
		pr_debug("ioctl_aw--AWUSB_IOCRECV\n");

		data = (void __user *)arg;
		if (data == NULL)
			break;
		if (copy_from_user(&aw_cmd, data, sizeof(struct aw_command))) {
			retval = -EFAULT;
			goto err_out;
		}
		if (aw_cmd.length < 0) {
			retval = -EINVAL;
			goto err_out;
		}
		buffer_len = aw_cmd.length;
		value = aw_cmd.value;
		pr_debug("buffer_len=%d\n", buffer_len);
		pr_debug("value=%d\n", value);
		buffer = kmalloc(buffer_len, GFP_KERNEL);
		if (!(buffer)) {
			dev_err(&aw->aw_dev->dev,
				"AWUSB_IOCSEND: Not enough memory for the receive buffer");
			retval = -ENOMEM;
			goto err_out;
		}
		memset(buffer, 0x33, buffer_len);

		/* stage 1, get data from usb device */
		result = usb_bulk_msg(aw->aw_dev,
		usb_rcvbulkpipe(aw->aw_dev, 2),
		buffer, buffer_len, &actual_len, 0);/*8000); */

		if (result) {
			kfree(buffer);

			dev_err(&aw->aw_dev->dev, "Read Whoops - %x", result);
			retval = -EIO;
			goto err_out;
		}

		/* stage 2, copy data to app in user space */
		if (copy_to_user(aw_cmd.buffer, buffer, aw_cmd.length)) {
			kfree(buffer);
			retval = -EFAULT;
			goto err_out;
		}

		kfree(buffer);

		break;

	case AWUSB_IOCSEND_RECV:
		pr_debug("ioctl_aw--AWUSB_IOCSEND_RECV\n");

		break;

	default:
		retval = -ENOTTY;
		break;
	}

	pr_debug("ioctl_aw--exit\n");

err_out:
	mutex_unlock(&(aw->lock));
	return retval;
}

static ssize_t
write_aw(
	struct file *file, const char __user *buffer,
	size_t count, loff_t *ppos)
{
	DEFINE_WAIT(wait);
	struct aw_usb_data *aw = &aw_instance;

	unsigned long copy_size;
	unsigned long bytes_written = 0;
	unsigned int partial;

	int result = 0;
	int maxretry;
	int errn = 0;
	int intr;

	intr = mutex_lock_interruptible(&(aw->lock));
	if (intr)
		return -EINTR;
	/* Sanity check to make sure aw is connected, powered, etc */
	if (aw->present == 0 || aw->aw_dev == NULL) {
		mutex_unlock(&(aw->lock));
		return -ENODEV;
	}

	do {
		unsigned long thistime;
		char *obuf = aw->obuf;

		copy_size = (count >= OBUF_SIZE) ? OBUF_SIZE : count;
		thistime = copy_size;
		if (copy_from_user(aw->obuf, buffer, copy_size)) {
			errn = -EFAULT;
			goto error;
		}
		maxretry = 5;
		while (thistime) {
			if (!aw->aw_dev) {
				errn = -ENODEV;
				goto error;
			}
			if (signal_pending(current)) {
				mutex_unlock(&(aw->lock));
				return bytes_written ? bytes_written : -EINTR;
			}

			result = usb_bulk_msg(aw->aw_dev,
					 usb_sndbulkpipe(aw->aw_dev, 2),
					 obuf, thistime, &partial, 5000);

			pr_debug(
				"write stats: result:%d thistime:%lu partial:%u",
						result, thistime, partial);

			if (result == -ETIMEDOUT) {
				/* NAK - so hold for a while */
				if (!maxretry--) {
					errn = -ETIME;
					goto error;
				}
				prepare_to_wait(
					&aw->wait_q, &wait, TASK_INTERRUPTIBLE);
				schedule_timeout(NAK_TIMEOUT);
				finish_wait(&aw->wait_q, &wait);
				continue;
			} else if (!result && partial) {
				obuf += partial;
				thistime -= partial;
			} else {
				break;
			}
		};
		if (result) {
			dev_err(&aw->aw_dev->dev, "Write Whoops - %x", result);
			errn = -EIO;
			goto error;
		}
		bytes_written += copy_size;
		count -= copy_size;
		buffer += copy_size;
	} while (count > 0);

	mutex_unlock(&(aw->lock));

	return bytes_written ? bytes_written : -EIO;

error:
	mutex_unlock(&(aw->lock));
	return errn;
}

static ssize_t
read_aw(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	DEFINE_WAIT(wait);
	struct aw_usb_data *aw = &aw_instance;
	ssize_t read_count;
	unsigned int partial;
	int this_read;
	int result;
	int maxretry = 10;
	char *ibuf;
	int intr;

	intr = mutex_lock_interruptible(&(aw->lock));
	if (intr)
		return -EINTR;
	/* Sanity check to make sure aw is connected, powered, etc */
	if (aw->present == 0 || aw->aw_dev == NULL) {
		mutex_unlock(&(aw->lock));
		return -ENODEV;
	}

	ibuf = aw->ibuf;

	read_count = 0;


	while (count > 0) {
		if (signal_pending(current)) {
			mutex_unlock(&(aw->lock));
			return read_count ? read_count : -EINTR;
		}
		if (!aw->aw_dev) {
			mutex_unlock(&(aw->lock));
			return -ENODEV;
		}
		this_read = (count >= IBUF_SIZE) ? IBUF_SIZE : count;

		result = usb_bulk_msg(aw->aw_dev,
				      usb_rcvbulkpipe(aw->aw_dev, 1),
				      ibuf, this_read, &partial,
				      8000);

		pr_debug(
			"read stats: result:%d this_read:%u partial:%u",
						result, this_read, partial);

		if (partial) {
			count = partial;
			this_read = partial;
		} else if (result == -ETIMEDOUT || result == 15) {
							/* FIXME: 15 ??? */
			if (!maxretry--) {
				mutex_unlock(&(aw->lock));
				dev_err(&aw->aw_dev->dev, "read_aw: maxretry timeout");
				return -ETIME;
			}
			prepare_to_wait(&aw->wait_q, &wait, TASK_INTERRUPTIBLE);
			schedule_timeout(NAK_TIMEOUT);
			finish_wait(&aw->wait_q, &wait);
			continue;
		} else if (result != -EREMOTEIO) {
			mutex_unlock(&(aw->lock));
			dev_err(&aw->aw_dev->dev,
				"Read Whoops - result:%u partial:%u this_read:%u",
				result, partial, this_read);
			return -EIO;
		} else {
			mutex_unlock(&(aw->lock));
			return 0;
		}

		if (this_read) {
			if (copy_to_user(buffer, ibuf, this_read)) {
				mutex_unlock(&(aw->lock));
				return -EFAULT;
			}
			count -= this_read;
			read_count += this_read;
			buffer += this_read;
		}
	}
	mutex_unlock(&(aw->lock));
	return read_count;
}

static const struct file_operations usb_aw_fops = {
	.owner =	THIS_MODULE,
	.read =		read_aw,
	.write =	write_aw,
	.unlocked_ioctl = ioctl_aw,
	.open =		open_aw,
	.release =	close_aw,
};

static struct usb_class_driver usb_aw_class = {
	.name =		"aw_efex%d",
	.fops =		&usb_aw_fops,
	.minor_base =	AW_MINOR,
};

static int probe_aw(struct usb_interface *intf,
		     const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev(intf);
	struct aw_usb_data *aw = &aw_instance;
	int retval;

	dev_info(&intf->dev, "USB aw found at address %d\n", dev->devnum);

	retval = usb_register_dev(intf, &usb_aw_class);
	if (retval) {
		dev_err(&aw->aw_dev->dev,
			"Not able to get a minor for this device.");
		return -ENOMEM;
	}

	aw->aw_dev = dev;

	aw->obuf = kmalloc(OBUF_SIZE, GFP_KERNEL);
	if (!(aw->obuf)) {
		dev_err(&aw->aw_dev->dev,
			"probe_aw: Not enough memory for the output buffer");
		usb_deregister_dev(intf, &usb_aw_class);
		return -ENOMEM;
	}
	dbg("probe_aw: obuf address:%p", aw->obuf);

	aw->ibuf = kmalloc(IBUF_SIZE, GFP_KERNEL);
	if (!(aw->ibuf)) {
		dev_err(&aw->aw_dev->dev,
			"probe_aw: Not enough memory for the input buffer");
		usb_deregister_dev(intf, &usb_aw_class);
		kfree(aw->obuf);
		return -ENOMEM;
	}
	dbg("probe_aw: ibuf address:%p", aw->ibuf);

	mutex_init(&(aw->lock));

	usb_set_intfdata(intf, aw);
	aw->present = 1;

	return 0;
}

static void disconnect_aw(struct usb_interface *intf)
{
	struct aw_usb_data *aw = usb_get_intfdata(intf);

	usb_set_intfdata(intf, NULL);
	if (aw) {
		usb_deregister_dev(intf, &usb_aw_class);

		mutex_lock(&(aw->lock));
		if (aw->isopen) {
			aw->isopen = 0;
			/* Better let it finish,
			 * the release will do whats needed
			 */
			aw->aw_dev = NULL;
			mutex_unlock(&(aw->lock));
			return;
		}
		kfree(aw->ibuf);
		kfree(aw->obuf);

		dev_info(&intf->dev, "USB aw disconnected.\n");

		aw->present = 0;
		mutex_unlock(&(aw->lock));
	}
}

static struct usb_device_id aw_table[] = {
	{USB_DEVICE(0x1f3a, 0xefe8)},		/* aw usb device */
	{ }					/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, aw_table);

static struct usb_driver aw_driver = {
	.name =		"aw",
	.probe =	probe_aw,
	.disconnect =	disconnect_aw,
	.id_table =	aw_table,
};

static int __init usb_aw_init(void)
{
	int retval;
	retval = usb_register(&aw_driver);
	if (retval)
		goto out;

	pr_info(KBUILD_MODNAME ": " DRIVER_VERSION ":" DRIVER_DESC "\n");

out:
	return retval;
}


static void __exit usb_aw_cleanup(void)
{
	struct aw_usb_data *aw = &aw_instance;

	aw->present = 0;
	usb_deregister(&aw_driver);


}

module_init(usb_aw_init);
module_exit(usb_aw_cleanup);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
