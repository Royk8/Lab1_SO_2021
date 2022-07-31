#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include "bridge.h"
#include <linux/random.h>

int bridge_major =   BRIDGE_MAJOR;
int bridge_minor =   0;
int bridge_nr_devs = BRIDGE_NR_DEVS;	/* number of bare bridge devices */

module_param(bridge_major, int, S_IRUGO);
module_param(bridge_minor, int, S_IRUGO);
module_param(bridge_nr_devs, int, S_IRUGO);

MODULE_AUTHOR("Jheisson Argiro Lopez Restrepo");
MODULE_LICENSE("Dual BSD/GPL");

LIST_HEAD(stack);
LIST_HEAD(simple_list);
LIST_HEAD(simple_list_2);
LIST_HEAD(hight_Q);
LIST_HEAD(low_Q);
LIST_HEAD(middle_Q);

static void add_element_to_stack(char *node_element_msg){
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);
	list_add(&(tmp_element->list), &stack);
}

static void add_element_to_list(char *node_element_msg){
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);
	list_add_tail(&(tmp_element->list), &simple_list);
}
static void add_element_to_list_2(char *node_element_msg){
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);
	list_add_tail(&(tmp_element->list), &simple_list_2);
}

static void add_to_hight_prior_q(char *node_element_msg){
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);
	list_add_tail(&(tmp_element->list), &hight_Q);
}

static void add_to_low_prior_q(char *node_element_msg){
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);
	list_add_tail(&(tmp_element->list), &low_Q);
}

static void add_to_middle_prior_q(char *node_element_msg){
	struct string_node *tmp_element;
	tmp_element = kmalloc(sizeof(struct string_node), GFP_KERNEL);
	strcpy(tmp_element->message, node_element_msg);
	INIT_LIST_HEAD(&tmp_element->list);
	list_add_tail(&(tmp_element->list), &middle_Q);
}

static void invert_list(void)
{
	struct string_node *tmp_element;
	struct list_head *watch, *prev;
	list_for_each_prev_safe(watch, prev, &simple_list)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		printk(KERN_INFO "%s", tmp_element->message);
		list_del(&(tmp_element->list));
		list_add_tail(&(tmp_element->list), &simple_list);
	}
}

void rotate_list(void){
	struct string_node *tmp_element;
	char message[100];	
	tmp_element = list_last_entry(&simple_list, struct string_node, list);	
	list_del(&(tmp_element->list));	
	list_add(&(tmp_element->list), &simple_list);
}

static int search_duplicated(char *node_element_msg){
	if(list_empty(&simple_list) == 0){
		struct string_node *tmp_element;
		struct list_head *watch, *next;
		int cont = 0;
		list_for_each_safe(watch, next, &simple_list){
        	tmp_element = list_entry(watch, struct string_node, list);
			if(strcmp(tmp_element->message, node_element_msg) == 0){
				printk(KERN_INFO "\nENCONTRÉ IGUALES\n");
				cont = cont + 1;
			}
    	} if(cont > 1){
			return 1;
		}
	}
	return 0;
}

static void concat_lists(void) {
	struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &hight_Q)
	{
		tmp_element = list_entry(watch, struct string_node, list);
		list_del(&(tmp_element->list));
		list_add_tail(&(tmp_element->list), &simple_list);
	}
}

void delete_list(void){
	struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &simple_list){
        	tmp_element = list_entry(watch, struct string_node, list);
        	list_del(&(tmp_element->list));
		kfree(tmp_element);
    }
    kfree(&simple_list);
}

static void delete_repeated_entries_list(void){
	struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &simple_list){
		tmp_element = list_entry(watch, struct string_node, list);
		if(search_duplicated(tmp_element->message) == 1){
			list_del(&(tmp_element->list));
			kfree(tmp_element);
		}
	}
}

static void concat_two_list(void){
	list_splice(&simple_list_2, &simple_list);
}

void mystack_exit(void){
    struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &stack){
        tmp_element = list_entry(watch, struct string_node, list);
        list_del(&(tmp_element->list));
		kfree(tmp_element);
    }
	printk(KERN_INFO "\nSTACK VACIADA\n");
    //kfree(&stack);
}

void mylist_exit(void){
    struct string_node *tmp_element;
	struct list_head *watch, *next;
	list_for_each_safe(watch, next, &simple_list){
        tmp_element = list_entry(watch, struct string_node, list);
        list_del(&(tmp_element->list));
		kfree(tmp_element);
    }
	printk(KERN_INFO "\nLISTA VACIADA\n");
    //kfree(&stack);
}

struct bridge_dev *bridge_devices;	/* allocated in bridge_init_module */

static long bridge_ioctl(struct file *f, unsigned int cmd, unsigned long arg){
    int return_value = 0;
    int data;
    char message[100];
    struct string_node *tmp_element;
    struct complex_struct tmp;
    switch(cmd){
	case BRIDGE_CREATE_Q:
            printk(KERN_INFO "message %s\n", "bla");
	    //Return a posituve value indicating the state of the queue
	    return_value = 1;
	    break;
	case BRIDGE_W_HIGH_PRIOR_Q:
        raw_copy_from_user(message, (char *)arg, 100);
	    add_to_hight_prior_q(message);
        printk(KERN_INFO "message %s\n", "High Prior Queue");
	    break;
	case BRIDGE_W_MIDDLE_PRIOR_Q:
	    raw_copy_from_user(message, (char *)arg, 100);
	    add_to_middle_prior_q(message);
	    printk(KERN_INFO "message %s\n", "Middle Prior Queue");
	    break;
	case BRIDGE_W_LOW_PRIOR_Q:
	    raw_copy_from_user(message, (char *)arg, 100);
	    add_to_low_prior_q(message);
	    printk(KERN_INFO "message %s\n", "Low Prior Queue");
	    break;
	case BRIDGE_R_HIGH_PRIOR_Q:
        tmp_element = list_first_entry(&hight_Q, struct string_node, list);
		list_del(&(tmp_element->list));
		raw_copy_to_user((char *)arg, tmp_element->message, 100);
		kfree(tmp_element);
	    break;
	case BRIDGE_R_MIDDLE_PRIOR_Q:
        tmp_element = list_first_entry(&middle_Q, struct string_node, list);
		list_del(&(tmp_element->list));
		raw_copy_to_user((char *)arg, tmp_element->message, 100);
		kfree(tmp_element);
	    break;
	case BRIDGE_R_LOW_PRIOR_Q:
        tmp_element = list_first_entry(&low_Q, struct string_node, list);
		list_del(&(tmp_element->list));
		raw_copy_to_user((char *)arg, tmp_element->message, 100);
		kfree(tmp_element);
	    break;
	case BRIDGE_STATE_Q:
        /**
		* Retorna 1 si la cola de alta prioridad tiene elementos
		* Retorna 2 si la cola de media prioridad tiene elementos y no la alta
		* Retorna 3 si la cola de baja prioridad tiene elementos pero no la alta o media.
		* Retorna 0 si ninguna cola tiene elementos
		*/
		if(list_empty(&hight_Q) != 1){
			return_value = 1;
		}else if(list_empty(&middle_Q) != 1){
			return_value = 2;
		}else if(list_empty(&low_Q) != 1){
			return_value = 3;
		}else{
			return_value = 0;
	    }
	    printk(KERN_INFO "Simple list state succesfully sended!!!\n");
	    break;
	case BRIDGE_DESTROY_Q:
            printk(KERN_INFO "message %s\n", "bla7");
	    break;

	case BRIDGE_CREATE_S:
	    printk(KERN_INFO "Stack succesfully created\n");
	    break;
	case BRIDGE_W_S:
		raw_copy_from_user(message, (char *)arg, 100);
	    add_element_to_stack(message);
		printk(KERN_INFO "Element succesfully added to the stack\n");
	    break;
	case BRIDGE_R_S:
	    tmp_element = list_first_entry(&stack, struct string_node, list);
		list_del(&(tmp_element->list));
	    raw_copy_to_user((char *)arg, tmp_element->message, 100);
	    kfree(tmp_element);
	    break;
	case BRIDGE_STATE_S:
        if(list_empty(&stack) != 0){
		return_value = 0;
	    }else{
		return_value = 1;
	    }
	    printk(KERN_INFO "Stack state succesfully sended!!!\n");
	    break;
	case BRIDGE_DESTROY_S:
            mystack_exit();
	    break;
	case BRIDGE_CREATE_L:
             printk(KERN_INFO "message %s\n", "bla13");
	     break;
	case BRIDGE_W_L:
		raw_copy_from_user(message, (char *)arg, 100);
	    add_element_to_list(message);
		printk(KERN_INFO "Element succesfully added to the simple list\n");
	     break;
	case BRIDGE_W_L2:
		raw_copy_from_user(message, (char *)arg, 100);
		add_element_to_list_2(message);
		printk(KERN_INFO "Element succesfully added to the simple list\n");
		break;
	case BRIDGE_R_L:
        tmp_element = list_first_entry(&simple_list, struct string_node, list);
		list_del(&(tmp_element->list));
	    raw_copy_to_user((char *)arg, tmp_element->message, 100);
	    kfree(tmp_element);
	    break;
	case BRIDGE_INVERT_L:
		invert_list();
		break;
	case BRIDGE_ROTATE_L:
        rotate_list();
		printk(KERN_INFO "message %d\n", data);
		break;
	case BRIDGE_CLEAN_L:
		delete_repeated_entries_list();
		printk(KERN_INFO "Lista limpia \n");
	case BRIDGE_GREATER_VAL_L:
	     //strcpy((char *)arg, "MensajePrueba");
             printk(KERN_INFO "message %s\n", "bla19");
	     break;
	case BRIDGE_END_L:
             printk(KERN_INFO "message %s\n", "bla21");
	     break;
	case BRIDGE_CONCAT_L:
        concat_two_list();
		break;
	case BRIDGE_STATE_L:
            if(list_empty(&simple_list) != 0){
		return_value = 0;
	    }else{
		return_value = 1;
	    }
	    printk(KERN_INFO "Simple list state succesfully sended!!!\n");
	    break;
	case BRIDGE_DESTROY_L:
		delete_list();
		break;
	case BRIDGE_W_CS:
	    raw_copy_from_user(&tmp, (struct complex_struct *)arg, sizeof(struct complex_struct));
	    printk(KERN_INFO "Message1 %s\n", tmp.messages[0]);
            printk(KERN_INFO "Message2 %s\n", tmp.messages[1]);
            printk(KERN_INFO "Message3 %s\n", tmp.messages[2]);
    }
    return return_value;
}

struct file_operations bridge_fops = {
	.owner =    THIS_MODULE,
	.unlocked_ioctl = bridge_ioctl
};

/*
 * Finally, the module stuff
 */

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void bridge_cleanup_module(void)
{
	int i;
	dev_t devno = MKDEV(bridge_major, bridge_minor);
	mystack_exit();
	mylist_exit();
	/* Get rid of our char dev entries */
	if (bridge_devices) {
		for (i = 0; i < bridge_nr_devs; i++) {
			cdev_del(&bridge_devices[i].cdev);
		}
		//Be aware of clean memory completely
		kfree(bridge_devices);
	}

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, bridge_nr_devs);
}

/*
 * Set up the char_dev structure for this device.
 */
static void bridge_setup_cdev(struct bridge_dev *dev, int index)
{
	int err, devno;

	devno = MKDEV(bridge_major, bridge_minor + index);
	cdev_init(&dev->cdev, &bridge_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding bridge%d", err, index);
}


int bridge_init_module(void)
{
	int result, i;
	dev_t dev = 0;

/*
 * Get a range of minor numbers to work with, asking for a dynamic
 * major unless directed otherwise at load time.
 */
	if (bridge_major) {
		dev = MKDEV(bridge_major, bridge_minor);
		result = register_chrdev_region(dev, bridge_nr_devs, "bridge");
	} else {
		result = alloc_chrdev_region(&dev, bridge_minor, bridge_nr_devs, "bridge");
		bridge_major = MAJOR(dev);
        	bridge_devices = kmalloc(bridge_nr_devs * sizeof(struct bridge_dev), GFP_KERNEL);
	}

	if (result < 0) {
		printk(KERN_WARNING "bridge: can't get major %d\n", bridge_major);
		return result;
	}

		/*
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	bridge_devices = kmalloc(bridge_nr_devs * sizeof(struct bridge_dev), GFP_KERNEL);

	if (!bridge_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}

	memset(bridge_devices, 0, bridge_nr_devs * sizeof(struct bridge_dev));

		/* Initialize each device. */
	for (i = 0; i < bridge_nr_devs; i++) {
		bridge_setup_cdev(&bridge_devices[i], i);
	}

	return 0; /* succeed */

  fail:
	bridge_cleanup_module();
	return result;
}

module_init(bridge_init_module);
module_exit(bridge_cleanup_module);