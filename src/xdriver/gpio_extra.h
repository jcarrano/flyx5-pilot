/**
 * Extra functions to complement TivaWare's gpio driver.
 *
 * @author Juan I Carrano
 */

#ifndef GPIO_EXTRA_H
#define GPIO_EXTRA_H

#include "../common.h"
#include <inc/hw_gpio.h>

typedef bool lock_status;

static const lock_status Lock = true;
static const lock_status Unlock = false;

#define IS_LOCK(x) BOOL(x)

/**
 * Unlock the GPIO commit register.
 *
 * @param	port_base	The port to lock/unlock.
 * @param	lock	If true, the port is locked,if false it is locked.
 *
 * @return	Previous lock status.
 */
lock_status gpio_lock(uint32_t port_base, lock_status locks);

/**
 * Modify the GPIO Commit Control register.
 *
 * @param	port_base	The port to lock/unlock.
 * @param	pin_n		The pin number
 * @param	locks		If true, commit is disabled,
 * 				if false it is enabled.
 *
 * @return	The previous commit status.
 */
lock_status gpio_commit_ctrl(uint32_t port_base, int pin_n, lock_status locks);

#endif /* GPIO_EXTRA_H */
