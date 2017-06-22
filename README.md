# FlyX5 Multirotor Controller firmware #

## Firmware ##

This is the firmware that runs on the [FlyX5 controller board](https://github.com/jcarrano/flyx5-avionics).

This code was written between 2013 and 2015 by:

* Andrés Calcabrini
* Juan Ignacio Carrano
* Juan Ignacio Ubeira
* Nicolás Venturo

## Other code ##

Unless otherwise stated, all code is released under version 3 of the GNU General Public License.

### Tiva C Drivers ###

This program includes the "Tiva Peripheral Driver Library". Copyright (c) 2005-2014 Texas Instruments Incorporated.

Released under the 4-clause BSD license

### fixed_point_arith ###

Even though the Tiva C MCU includes a floating point unit, control system calculations are carried out in fixed point. The repo includes a copy of the [Fixed point arithmetic library](https://github.com/jcarrano/fixed_point_arith).

### Tools ###

All the files under the /tools directory are released under the 2-clause BSD license.

In the /tools directory you will find the necessary configurations for building the firmware and libraries and for programming the board using OpenOCD. The Makefiles are written in a modular fashion and can be adapted to other projects, even other microcontrollers, with minimal configuration. You are encouraged to copy and reuse them.
