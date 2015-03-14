/**
 * @file
 * @author Juan I Carrano
 * @copyright	Copyright 2013 by Juan I Carrano, Andrés Calcabrini, Juan I. Ubeira and
 * 		Nicolás Venturo. All rights reserved. \n
 * 		\n
 *		This program is free software: you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation, either version 3 of the License, or
 *		(at your option) any later version. \n
 * 		\n
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details. \n
 * 		\n
 *		You should have received a copy of the GNU General Public License
 *		along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Anti-bounce and hold functionality for buttons.
 */

#include <stdbool.h>

/**
 * Super-button.
 *
 * Implements anti-bounce or hold.
 */
typedef struct sbutton {
	unsigned int activation_ms;
	bool activation_valid;
} sbutton;

/**
 * Initialize (or reset) super-button.
 */
void sbutton_init(sbutton *s);

/**
 * Evaluate the hold condition for a button.
 *
 * Returns true if the buttons has been pressed for at least hold_time_ms
 * milliseconds.
 *
 * @param	s	Super-button
 * @param	hold_time_ms	Minumum hold time in milliseconds.
 * @param	raw_status	The current status of the button.
 */
bool s_hold(sbutton *s, unsigned int hold_time_ms, bool raw_status);

/**
 * Evaluate the current condition of the button, with as dead-time.
 *
 * Returns true if the buttons is pressed and was not pressed in the preceeding
 * unbounce_time_ms milliseconds
 *
 *
 * @param	s	Super-button
 * @param	unbounce_time_ms	Anti-bounce time in milliseconds
 * @param	raw_status	The current status of the button.
 */
bool s_unbounce(sbutton *s, unsigned int unbounce_time_ms, bool raw_status);
