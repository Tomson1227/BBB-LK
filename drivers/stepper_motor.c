#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>
#include "stepper_motor.h"


#define GPIO_NUMB 4

struct stepper_s {
    struct platform_device *pdev;
    struct task_struct *thread;
    u32 gpio[GPIO_NUMB];
    u32 speed;
    u32 steps;
    u8 busy;
};

static void small_steps(struct stepper_s *motor);

struct stepper_s motor;

/*
static void gpio_test(struct stepper_s *motor)
{
    int index = 0;

    while(index < GPIO_NUMB) {
        printk(KERN_INFO "Test GPIO%d pin %d\n", index, motor->gpio[index]);
        gpio_set_value(motor->gpio[index], 1);
        mdelay(1000);
        gpio_set_value(motor->gpio[index], 0);
        ++index;
    }
}
*/

static int thread_func(void *data)
{
    struct stepper_s *motor = (struct stepper_s *) data;
    motor->busy = 1;
    small_steps(motor);
    motor->busy = 0;

	return 0;
}

static int gpio_init(struct stepper_s *motor)
{
    u8 err;
    u8 gpio_init = 0;
    char buffer[12];
    int index;
    
    motor->speed = 1; // default
    motor->busy = 0;

    index = 0;
    while(index < GPIO_NUMB) {
        sprintf(buffer, "gpio_out_%d", index);
    	
        err = gpio_request(motor->gpio[index], buffer);
        
        printk(KERN_INFO "PIN[%d] ( %d ): request %s\n",
            index, motor->gpio[index], err ? "fail" : "success"); 
        
        gpio_direction_output(motor->gpio[index], 0);

        gpio_init ^= !err << index;
        ++index;
    }

    if(!(gpio_init ^ 0xF)) {
        printk(KERN_WARNING "STEPPER: GPIO init success\n");
    } else {
        index = 0;
        while(index < GPIO_NUMB) {
            if(gpio_init ^ BIT(index))
    	        gpio_free(motor->gpio[index]);
            
            ++index;
        }
        
        printk(KERN_ERR "STEPPER: GPIO init fail\n");
        return -ENODEV;
    }
    // gpio_test(motor);

    return 0;
}

static inline void gpio_deinit(struct stepper_s *motor)
{
    int index = 0;
    
    while(index < GPIO_NUMB) {
        gpio_free(motor->gpio[index]);
        ++index;
    }
}

/*
static void big_steps(struct stepper_s *motor)
{
    int dl = motor->speed * 1500;

    if(motor->steps > 0) {
        while(motor->steps) {
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            --motor->steps;
        }
    } else {
        while(motor->steps) {
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            ++motor->steps;
        }
    }

    gpio_set_value(motor->gpio[0], 0);
    gpio_set_value(motor->gpio[1], 0);
    gpio_set_value(motor->gpio[2], 0);
    gpio_set_value(motor->gpio[3], 0);
    udelay(dl);
}
*/

static void small_steps(struct stepper_s *motor) 
{    
    int dl = motor->speed * 750;

    if(motor->steps > 0) {
        while(motor->steps) {
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[0], 1);
            udelay(dl);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            gpio_set_value(motor->gpio[0], 1);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            --motor->steps;
        }
    } else {
        while(motor->steps) {
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 1);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 0);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 1);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 0);
            udelay(dl);
            gpio_set_value(motor->gpio[0], 1);
            gpio_set_value(motor->gpio[1], 0);
            gpio_set_value(motor->gpio[2], 0);
            gpio_set_value(motor->gpio[3], 1);
            udelay(dl);
            ++motor->steps;
        }
    }

    gpio_set_value(motor->gpio[0], 0);
    gpio_set_value(motor->gpio[1], 0);
    gpio_set_value(motor->gpio[2], 0);
    gpio_set_value(motor->gpio[3], 0);
    udelay(dl);
}

static ssize_t steps_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) {
    motor.steps = simple_strtol(buf, NULL, 0);

    motor.thread = kthread_run(thread_func, (void *)&motor, "motor_thread");
    if(IS_ERR(motor.thread)) {
		pr_err("kthread_run() failed\n");
		return -ENODEV;
	}
    
	return count;
}

static ssize_t speed_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count) 
{
    motor.speed = simple_strtol(buf, NULL, 0);
    if(motor.speed > 18) {
        motor.speed = 18;
        printk(KERN_WARNING "STEPPER: MAX spped is 18\n");
    } else if(motor.speed < 1) {
        motor.speed = 1;
        printk(KERN_WARNING "STEPPER: MIN spped is 1\n");
    }

	return count;
}

static ssize_t speed_show(struct class *class, struct class_attribute *attr, char *buf)
{
    sprintf(buf, "Current speed: %d rpm\n", motor.speed);

	return strlen(buf);
}

static ssize_t busy_show(struct class *class, struct class_attribute *attr, char *buf)
{
    sprintf(buf, "Stepper motor: %s\n", motor.busy ? "busy" : "redy");

	return strlen(buf);
}

CLASS_ATTR_RW(speed);
CLASS_ATTR_WO(steps);
CLASS_ATTR_RO(busy);

static struct class *attr_class;

static int stepper_probe(struct platform_device *pdev)
{
	int ret;
	struct device_node *node = pdev->dev.of_node;

    motor.pdev = pdev;

	ret = of_property_read_u32_array(node, "pins", motor.gpio, 4);
	if (!ret){
		printk(KERN_INFO "STEPPER: Used pins: %d %d %d %d\n", 
            motor.gpio[0], motor.gpio[1], motor.gpio[2], motor.gpio[3]);
	} else {
		printk(KERN_WARNING "STEPPER: Used default pins\n");
        motor.gpio[0] = STEPPER_OUT_0;
        motor.gpio[1] = STEPPER_OUT_1;
        motor.gpio[2] = STEPPER_OUT_2;
        motor.gpio[3] = STEPPER_OUT_3;
	}
    
    ret = gpio_init(&motor); 
	if (ret) {
	    goto err_gpio_init;
    }

	attr_class = class_create(THIS_MODULE, "stepper");
	if (IS_ERR(attr_class)) {
		ret = PTR_ERR(attr_class);
		printk(KERN_ERR "stepper: failed to create sysfs class: %d\n", ret);
		goto err_class_create;
	}

	ret = class_create_file(attr_class, &class_attr_steps);
    if (ret) {
		printk(KERN_ERR "stepper: failed to create sysfs class attribute steps: %d\n", ret);
        goto err_class_file_steps;
	}

	ret = class_create_file(attr_class, &class_attr_speed);
    if (ret) {
		printk(KERN_ERR "stepper: failed to create sysfs class attribute speed: %d\n", ret);
        goto err_class_file_speed;
	}

	ret = class_create_file(attr_class, &class_attr_busy);
    if (ret) {
		printk(KERN_ERR "stepper: failed to create sysfs class attribute busy: %d\n", ret);
        goto err_class_file_busy;
	}

	dev_info(&pdev->dev, "device probed\n");

	return 0;

err_class_file_busy:
    class_remove_file(attr_class, &class_attr_speed);
err_class_file_speed:
    class_remove_file(attr_class, &class_attr_steps);
err_class_file_steps:
    class_destroy(attr_class);
err_class_create: 
    gpio_deinit(&motor);
err_gpio_init:
    dev_info(&pdev->dev, "device probed failed\n");

    return -EAGAIN;
}

static int stepper_remove(struct platform_device* pdev)
{
    if(motor.thread)
		kthread_stop(motor.thread);
    
    class_remove_file(attr_class, &class_attr_busy);
    class_remove_file(attr_class, &class_attr_steps);
    class_remove_file(attr_class, &class_attr_speed);
    class_destroy(attr_class);
    gpio_deinit(&motor);

	printk(KERN_INFO "STEPPER: driver removed");
	return 0;
}

static const struct platform_device_id stepper_keys[] = {
    { "stepper", 0},
    {},
};
MODULE_DEVICE_TABLE(platform, stepper_keys);

static const struct of_device_id stepper_of_table[] = {
    { .compatible = "stepper" },
    { },
};
MODULE_DEVICE_TABLE(of, stepper_of_table);

static struct platform_driver stepper_driver = {
    .probe = stepper_probe,
    .remove = stepper_remove,
    .id_table = stepper_keys,
    .driver = {
        .name = "stepper",
        .of_match_table = stepper_of_table,
    }
};
module_platform_driver(stepper_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Povshenko");
MODULE_DESCRIPTION("Driver for stepper motor");
