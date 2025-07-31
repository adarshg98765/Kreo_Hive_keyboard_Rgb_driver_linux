#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hid.h>
#include <linux/slab.h>

static ssize_t rgb_color_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct hid_device *hdev = to_hid_device(dev);

    unsigned char *rgb_data;
    int r, g, b;
    int ret;
    const int data_len = 64;
    const u8 report_id = 4;

    if (!hdev) {
        pr_err("rgb_driver: Could not get HID device from store function\n");
        return -EINVAL;
    }

    if (sscanf(buf, "%d,%d,%d", &r, &g, &b) != 3) {
        hid_err(hdev, "Invalid input format. Expected R,G,B\n");
        return -EINVAL;
    }

    pr_info("Received color: R=%d, G=%d, B=%d\n", r, g, b);

    rgb_data = kzalloc(data_len, GFP_KERNEL);
    if (!rgb_data) {
        return -ENOMEM;
    }
    
    rgb_data[0] = report_id; 
    rgb_data[1] = 0x3b;
    rgb_data[2] = 0x02;
    rgb_data[3] = 0x06;
    rgb_data[4] = 0x22;
    rgb_data[9] = 0x06;
    rgb_data[10] = 0x04;
    rgb_data[11] = 0x02;
    rgb_data[14] = (unsigned char)r;
    rgb_data[15] = (unsigned char)g;
    rgb_data[16] = (unsigned char)b;
    rgb_data[17] = 0x08;
    rgb_data[28] = 0x01;

    ret = hid_hw_raw_request(hdev, report_id, rgb_data, data_len, HID_OUTPUT_REPORT, HID_REQ_SET_REPORT);

    kfree(rgb_data); 

    if (ret < 0) {
        hid_err(hdev, "Failed to send raw RGB command. Error: %d\n", ret);
        return ret; 
    }

    return count; 
}

static DEVICE_ATTR_WO(rgb_color);

static int my_rgb_probe(struct hid_device *hdev, const struct hid_device_id *id);
static void my_rgb_remove(struct hid_device *hdev);

static const struct hid_device_id my_rgb_devices[] = {
    { HID_USB_DEVICE(0x320f, 0x5055) }, 
    { } 
};
MODULE_DEVICE_TABLE(hid, my_rgb_devices);

static struct hid_driver my_rgb_hid_driver = {
    .name = "my_keyboard_rgb",
    .id_table = my_rgb_devices,
    .probe = my_rgb_probe,
    .remove = my_rgb_remove,
};

static int __init my_rgb_init(void) {
    pr_info("My RGB Keyboard Driver: Loading...\n");
    return hid_register_driver(&my_rgb_hid_driver);
}

static void __exit my_rgb_exit(void) {
    pr_info("My RGB Keyboard Driver: Unloading...\n");
    hid_unregister_driver(&my_rgb_hid_driver);
}

module_init(my_rgb_init);
module_exit(my_rgb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adarsh");
MODULE_DESCRIPTION("A driver to control my keyboard's RGB");


static int my_rgb_probe(struct hid_device *hdev, const struct hid_device_id *id) {
    int ret;

    pr_info("My RGB Keyboard (%s) has been connected\n", hdev->name);

    ret = hid_parse(hdev);
    if (ret) {
        hid_err(hdev, "failed to parse HID reports\n");
        return ret;
    }

    ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
    if (ret) {
        hid_err(hdev, "failed to start hardware I/O\n");
        return ret;
    }

    ret = device_create_file(&hdev->dev, &dev_attr_rgb_color);
    if (ret) {
        hid_err(hdev, "Can't create sysfs attribute rgb_color\n");
    }

    pr_info("My RGB driver loaded. Sysfs attribute 'rgb_color' created.\n");

    return 0; 
}

static void my_rgb_remove(struct hid_device *hdev) {
    device_remove_file(&hdev->dev, &dev_attr_rgb_color);

    hid_hw_stop(hdev);
    pr_info("My RGB Keyboard (%s) has been disconnected\n", hdev->name);
}