#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "hmc5883l.h"

struct hmc5883l {
	struct i2c_client *client;
    int X_axis;
    int Y_axis;
    int Z_axis;
};

static struct hmc5883l hmc5883l;

static int hmc5883l_read_data(void)
{
	struct i2c_client *drv_client = hmc5883l.client;

	if (drv_client == 0)
		return -ENODEV;

	i2c_smbus_write_byte_data(drv_client, MODE_REG, SINGLE_MEASURE_MODE);
	mdelay(6);
	hmc5883l.X_axis = i2c_smbus_read_word_data(drv_client, REG_AXIS_X_MSB);
	hmc5883l.Y_axis = i2c_smbus_read_word_data(drv_client, REG_AXIS_Y_MSB);
	hmc5883l.Z_axis = i2c_smbus_read_word_data(drv_client, REG_AXIS_Z_MSB);

	dev_info(&drv_client->dev, "read data:\n");
	dev_info(&drv_client->dev, "X_axis: %06d\n", hmc5883l.X_axis);
	dev_info(&drv_client->dev, "Y_axis: %06d\n", hmc5883l.Y_axis);
	dev_info(&drv_client->dev, "Z_axis: %06d\n", hmc5883l.Z_axis);

	return 0;
}

static ssize_t axis_X_show(struct class *class, struct class_attribute *attr, char *buf) {
	hmc5883l_read_data();

	sprintf(buf, "X_axis: %d\n", hmc5883l.X_axis);
	return strlen(buf);
}

static ssize_t axis_Y_show(struct class *class, struct class_attribute *attr, char *buf) {
	hmc5883l_read_data();

	sprintf(buf, "Y_axis: %d\n", hmc5883l.Y_axis);
	return strlen(buf);
}

static ssize_t axis_Z_show(struct class *class, struct class_attribute *attr, char *buf) {
	hmc5883l_read_data();

	sprintf(buf, "Z_axis: %d\n", hmc5883l.Z_axis);
	return strlen(buf);
}

static u8 verify_i2c_device(struct i2c_client *drv_client)
{
	if(i2c_smbus_read_byte_data(drv_client, IDENT_REG_A) == 'H' &&
	   i2c_smbus_read_byte_data(drv_client, IDENT_REG_B) == '4' &&
	   i2c_smbus_read_byte_data(drv_client, IDENT_REG_C) == '3')
	   return 1;
	
	return 0;
}

static void set_device_mode(struct i2c_client *drv_client)
{
	i2c_smbus_write_byte_data(drv_client, CONFIG_REG_A, DEFAULT_REG_A);
	i2c_smbus_write_byte_data(drv_client, CONFIG_REG_B, DEFAULT_REG_B);
	i2c_smbus_write_byte_data(drv_client, MODE_REG, 	DEFAULT_MODE);
}

CLASS_ATTR_RO(axis_X);
CLASS_ATTR_RO(axis_Y);
CLASS_ATTR_RO(axis_Z);

static struct class *attr_class;

static int hmc5883l_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
	int ret = 0;
	
	dev_info(&drv_client->dev, "i2c client address is 0x%X\n", drv_client->addr);
	dev_info(&drv_client->dev, "i2c driver probed\n");

	if(!verify_i2c_device(drv_client))
		return -ENODEV;

	hmc5883l.client = drv_client;
	set_device_mode(drv_client);
	hmc5883l_read_data();

	attr_class = class_create(THIS_MODULE, "hmc5883l");
	if (IS_ERR(attr_class)) {
		ret = PTR_ERR(attr_class);
		dev_info(&drv_client->dev, "HMC5883L: failed to create sysfs class: %d\n", ret);
		goto err1;
	}

	ret = class_create_file(attr_class, &class_attr_axis_X);
    if (ret) {
		dev_info(&drv_client->dev, "HMC5883L: failed to create sysfs class attribute axis_X: %d\n", ret);
        goto err2;
	}

    ret = class_create_file(attr_class, &class_attr_axis_Y);
	if (ret) {
		dev_info(&drv_client->dev, "HMC5883L: failed to create sysfs class attribute axis_Y: %d\n", ret);
        goto err3;
	}

    ret = class_create_file(attr_class, &class_attr_axis_Z);
	if (ret) {
		dev_info(&drv_client->dev, "HMC5883L: failed to create sysfs class attribute axis_Z: %d\n", ret);
        goto err4;
	}

	dev_info(&drv_client->dev, "HMC5883L: driver initialized.\n");
	return 0;

err4:   
	class_remove_file(attr_class, &class_attr_axis_Y);
err3:   
	class_remove_file(attr_class, &class_attr_axis_X);
err2:	
	class_destroy(attr_class);
err1:   
	dev_info(&drv_client->dev, "HMC5883L: module probe fail.\n");

    return ret;
}

static int hmc5883l_remove(struct i2c_client *drv_client)
{
	hmc5883l.client = 0;
    if (attr_class) {
		class_remove_file(attr_class, &class_attr_axis_Z);
		class_remove_file(attr_class, &class_attr_axis_Y);
		class_remove_file(attr_class, &class_attr_axis_X);
		dev_info(&drv_client->dev, "sysfs class attributes removed\n");

		class_destroy(attr_class);
		dev_info(&drv_client->dev, "HMC5883L: sysfs class destroyed\n");
	}

	dev_info(&drv_client->dev, "HMC5883L: device disabled\n");	
	return 0;
}

static const struct i2c_device_id hmc5883l_idtable [] = {
    { "hmc5883l", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, hmc5883l_idtable);

static struct i2c_driver hmc5883l_i2c_driver = {
    .driver = {
   	 	.name = "hmc5883l",
    },

    .probe = hmc5883l_probe,
    .remove = hmc5883l_remove,
    .id_table = hmc5883l_idtable,
};
module_i2c_driver(hmc5883l_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Povshenko");
MODULE_DESCRIPTION("Driver for I2C magnetometer module HMC5883L");

/****************************************************************************************/
