/**
 * Extra functions to complement TivaWare's gpio driver.
 *
 * @author Juan I Carrano
 */

#include "gpio_extra.h"
#include <inc/hw_types.h>

lock_status gpio_lock(uint32_t port_base, lock_status locks)
{
	lock_status ret = (HWREG(port_base + GPIO_O_LOCK) == GPIO_LOCK_LOCKED)?
							Lock: Unlock;

	HWREG(port_base + GPIO_O_LOCK) = IS_LOCK(locks)? 0 : GPIO_LOCK_KEY;

	return ret;
}

lock_status gpio_commit_ctrl(uint32_t port_base, int pin_n, lock_status locks)
{
	lock_status ret = (HWREGBITW(port_base + GPIO_O_CR, pin_n))?
							Unlock : Lock;

	HWREGBITW(port_base + GPIO_O_CR, pin_n) = IS_LOCK(locks)? 0 : 1;

	return ret;
}

