#include <linux/device.h>

#include <linux/notifier.h>
#include <linux/bq2429x_notifier.h>
#include <linux/sec_sysfs.h>

/*
  * The src & dest addresses of the noti.
  * keep the same value defined in ccic_notifier.h
  *     b'0001 : CCIC
  *     b'0010 : MUIC
  *     b'1111 : Broadcasting
  */
#define NOTI_ADDR_SRC (1 << 1)
#define NOTI_ADDR_DST (0xf)

/* ATTACH Noti. ID */
#define NOTI_ID_ATTACH (1)


#define SET_BQ2429X_NOTIFIER_BLOCK(nb, fn, dev) do {	\
		(nb)->notifier_call = (fn);		\
		(nb)->priority = (dev);			\
	} while (0)

#define DESTROY_BQ2429X_NOTIFIER_BLOCK(nb)			\
		SET_BQ2429X_NOTIFIER_BLOCK(nb, NULL, -1)

static struct bq2429x_notifier_struct bq2429x_notifier;

struct device *switch_device;

//static int bq2429x_uses_new_noti;

static void __set_noti_cxt(int attach, int type)
{
	if (type < 0) {
		bq2429x_notifier.cmd = attach;
		return;
	}

	/* Old Interface */
	bq2429x_notifier.cmd = attach;
	bq2429x_notifier.attached_dev = type;

}

int bq2429x_notifier_register(struct notifier_block *nb, notifier_fn_t notifier,
			bq2429x_notifier_device_t listener)
{
	int ret = 0;
	void *pcxt;;

	pr_info("%s: listener=%d register\n", __func__, listener);

	SET_BQ2429X_NOTIFIER_BLOCK(nb, notifier, listener);
	ret = blocking_notifier_chain_register(&(bq2429x_notifier.notifier_call_chain), nb);
	if (ret < 0)
		pr_err("%s: blocking_notifier_chain_register error(%d)\n",
				__func__, ret);

	pcxt =  (void *)&(bq2429x_notifier.attached_dev);

	/* current bq2429x's attached_device status notify */
	nb->notifier_call(nb, bq2429x_notifier.cmd, pcxt);

	return ret;
}

int bq2429x_notifier_unregister(struct notifier_block *nb)
{
	int ret = 0;

	pr_info("%s: listener=%d unregister\n", __func__, nb->priority);

	ret = blocking_notifier_chain_unregister(&(bq2429x_notifier.notifier_call_chain), nb);
	if (ret < 0)
		pr_err("%s: blocking_notifier_chain_unregister error(%d)\n",
				__func__, ret);
	DESTROY_BQ2429X_NOTIFIER_BLOCK(nb);

	return ret;
}

static int bq2429x_notifier_notify(void)
{
	int ret = 0;
	void *pcxt;


	pcxt =  (void *)&(bq2429x_notifier.attached_dev);

	ret = blocking_notifier_call_chain(&(bq2429x_notifier.notifier_call_chain),
			bq2429x_notifier.cmd, pcxt);

	switch (ret) {
	case NOTIFY_STOP_MASK:
	case NOTIFY_BAD:
		pr_err("%s: notify error occur(0x%x)\n", __func__, ret);
		break;
	case NOTIFY_DONE:
	case NOTIFY_OK:
		pr_info("%s: notify done(0x%x)\n", __func__, ret);
		break;
	default:
		pr_info("%s: notify status unknown(0x%x)\n", __func__, ret);
		break;
	}

	return ret;
}

void bq2429x_notifier_attach_attached_dev(bq2429x_attached_dev_t new_dev)
{
	printk("xuecheng %s: (%d)\n", __func__, new_dev);

	__set_noti_cxt(BQ2429X_NOTIFY_CMD_ATTACH, new_dev);

	/* bq2429x's attached_device attach broadcast */
	bq2429x_notifier_notify();
		pr_err("[zyk][E] [%-4d] [%s] ",__LINE__, __func__);
}

void bq2429x_notifier_detach_attached_dev(bq2429x_attached_dev_t cur_dev)
{
	printk("xuecheng %s: (%d)\n", __func__, cur_dev);

	__set_noti_cxt(BQ2429X_NOTIFY_CMD_DETACH, -1);



	bq2429x_notifier_notify();

	__set_noti_cxt(0, ATTACHED_DEV_NONE_BQ2429X);
}

void bq2429x_notifier_logically_attach_attached_dev(bq2429x_attached_dev_t new_dev)
{
	pr_info("%s: (%d)\n", __func__, new_dev);

	__set_noti_cxt(BQ2429X_NOTIFY_CMD_ATTACH, new_dev);

	/* bq2429x's attached_device attach broadcast */
	bq2429x_notifier_notify();
}

void bq2429x_notifier_logically_detach_attached_dev(bq2429x_attached_dev_t cur_dev)
{
	pr_info("%s: (%d)\n", __func__, cur_dev);

	__set_noti_cxt(BQ2429X_NOTIFY_CMD_DETACH, cur_dev);

	/* bq2429x's attached_device detach broadcast */
	bq2429x_notifier_notify();

	__set_noti_cxt(0, ATTACHED_DEV_NONE_BQ2429X);
}


static int __init bq2429x_notifier_init(void)
{
	int ret = 0;

	pr_info("%s\n", __func__);


	switch_device = sec_device_create(NULL, "switch");
	if (IS_ERR(switch_device)) {
		pr_err("%s Failed to create device(switch)!\n", __func__);
		ret = -ENODEV;
		goto out;
	}

	BLOCKING_INIT_NOTIFIER_HEAD(&(bq2429x_notifier.notifier_call_chain));
	__set_noti_cxt(0 ,ATTACHED_DEV_UNKNOWN_BQ2429X);

out:
	return ret;
}

device_initcall(bq2429x_notifier_init);

