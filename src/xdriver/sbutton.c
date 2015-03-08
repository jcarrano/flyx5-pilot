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
#include "sbutton.h"
#include "rti.h"

void sbutton_init(sbutton *s)
{
	s->activation_valid = false;
}

bool s_hold(sbutton *s, unsigned int hold_time_ms, bool raw_status)
{
	bool r;

	if (raw_status) {
		unsigned int curr_time = rti_GetTimeMs();

		if (s->activation_valid
			&& curr_time - s->activation_ms > hold_time_ms) {
			r = true;
		} else {
			s->activation_valid = true;
			s->activation_ms = curr_time;
			r = false;
		}
	} else {
		s->activation_valid = false;
		r = false;
	}

	return r;
}

bool s_unbounce(sbutton *s, unsigned int unbounce_time_ms, bool raw_status)
{
	bool r;

	if (raw_status) {
		unsigned int curr_time = rti_GetTimeMs();

		if (s->activation_valid) {
			r = curr_time - s->activation_ms > unbounce_time_ms;
		} else {
			s->activation_valid = true;
			r = true;
		}

		if (r)
			s->activation_ms = curr_time;
	} else {
		r = false;
	}

	return r;
}

