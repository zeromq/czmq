//http://godandme.wordpress.com/2011/04/05/how-to-make-a-sysfs-entry/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>

struct my_attr {
    struct attribute attr;
    int value;
};

static struct my_attr notify = {
    .attr.name="notify",
    .attr.mode = 0644,
    .value = 0,
};

static struct my_attr trigger = {
    .attr.name="trigger",
    .attr.mode = 0644,
    .value = 0,
};

static struct attribute * myattr[] = {
    &notify.attr,
    &trigger.attr,
    NULL
};

static ssize_t default_show(struct kobject *kobj, struct attribute *attr,
        char *buf)
{
    struct my_attr *a = container_of(attr, struct my_attr, attr);
    return scnprintf(buf, PAGE_SIZE, "%d\n", a->value);
}
static struct kobject *mykobj;

static ssize_t default_store(struct kobject *kobj, struct attribute *attr,
        const char *buf, size_t len)
{
    struct my_attr *a = container_of(attr, struct my_attr, attr);

    sscanf(buf, "%d", &a->value);
    notify.value = a->value;
    printk("sysfs_notify store %s = %d\n", a->attr.name, a->value);
    sysfs_notify(mykobj, NULL, "notify");
    return sizeof(int);
}

static struct sysfs_ops myops = {
    .show = default_show,
    .store = default_store,
};

static struct kobj_type mytype = {
    .sysfs_ops = &myops,
    .default_attrs = myattr,
};

static struct kobject *mykobj;
static int __init sysfsexample_module_init(void)
{
    int err = -1;
    printk("sysfs_notify init\n");
    mykobj = kzalloc(sizeof(*mykobj), GFP_KERNEL);
    if (mykobj) {
        kobject_init(mykobj, &mytype);
        if (kobject_add(mykobj, NULL, "%s", "sysfs_sample")) {
             err = -1;
             printk("sysfs_notify: kobject_add() failed\n");
             kobject_put(mykobj);
             mykobj = NULL;
        }
        err = 0;
    }
    return err;
}

static void __exit sysfsexample_module_exit(void)
{
    if (mykobj) {
        kobject_put(mykobj);
        kfree(mykobj);
    }
    printk("sysfs_notify exit\n");
}

module_init(sysfsexample_module_init);
module_exit(sysfsexample_module_exit);
MODULE_LICENSE("GPL");
