#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x93e67167, "device_destroy" },
	{ 0xcbedc829, "class_destroy" },
	{ 0xffc6fd5, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xfe990052, "gpio_free" },
	{ 0x122c3a7e, "_printk" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x1581d08d, "gpio_to_desc" },
	{ 0x1519c0cc, "gpiod_direction_input" },
	{ 0x1c691234, "gpiod_to_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xe6f9d1e8, "cdev_init" },
	{ 0xe8f65a0d, "cdev_add" },
	{ 0x6f33fed0, "class_create" },
	{ 0x7188b409, "device_create" },
	{ 0x62d1bc06, "gpiod_get_raw_value" },
	{ 0xe478ef45, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D071DFC5D1071568FF03F6F");