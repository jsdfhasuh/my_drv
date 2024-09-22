#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x8d42f378, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x931f05d3, __VMLINUX_SYMBOL_STR(ktime_get_raw) },
	{ 0x5363d73f, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xff178f6, __VMLINUX_SYMBOL_STR(__aeabi_idivmod) },
	{ 0x64889e21, __VMLINUX_SYMBOL_STR(gpiod_direction_output) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x5c2e3421, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0x43a53735, __VMLINUX_SYMBOL_STR(__alloc_workqueue_key) },
	{ 0x1974ad52, __VMLINUX_SYMBOL_STR(give_gpio_opr) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
	{ 0x5ee52022, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0x22016404, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x275ef902, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x68c6b359, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x51d559d1, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x8c03d20c, __VMLINUX_SYMBOL_STR(destroy_workqueue) },
	{ 0xbe0b7785, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x57ade692, __VMLINUX_SYMBOL_STR(jsdfhasuh_register_chrdev) },
	{ 0xa38caae0, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x42160169, __VMLINUX_SYMBOL_STR(flush_workqueue) },
	{ 0x622598b1, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0xdab2886f, __VMLINUX_SYMBOL_STR(jsdfhasuh_unregister_chrdev) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0xe28518ed, __VMLINUX_SYMBOL_STR(destroy_device) },
	{ 0x6658c377, __VMLINUX_SYMBOL_STR(create_device_point) },
	{ 0x1bec770f, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x598542b2, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0xd85cd67e, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x344b7739, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x6b67fde, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0xc70990bf, __VMLINUX_SYMBOL_STR(gpiod_get_index) },
	{ 0x3e72fd42, __VMLINUX_SYMBOL_STR(kill_fasync) },
	{ 0x1cfb04fa, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0xb2d48a2e, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x910d9798, __VMLINUX_SYMBOL_STR(gpiod_count) },
	{ 0xb9347d89, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x3e41f41f, __VMLINUX_SYMBOL_STR(gpiod_set_value) },
	{ 0x3ba226d0, __VMLINUX_SYMBOL_STR(gpiod_put) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=jsdfhasuh_cdev";

