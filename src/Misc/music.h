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
 * Information tones.
 */

#ifndef MUSIC_H
#define MUSIC_H

#include "../peripheral/buzzer.h"

extern struct note_data music_startup[];
extern struct note_data music_fatal_error[];
extern struct note_data music_enter_calibration[];
extern struct note_data music_calibration_measure1[];
extern struct note_data music_calibration_measure2[];
extern struct note_data music_calibration_excellent[];
extern struct note_data music_calibration_good[];
extern struct note_data music_calibration_ugly[];
extern struct note_data music_calibration_bad[];
extern struct note_data music_calibration_cancel[];
extern struct note_data music_calibration_accept[];
extern struct note_data music_calibration_escs_enter[];
extern struct note_data music_calibration_escs_step[];
extern struct note_data music_armed[];
extern struct note_data music_unarmed[];

#endif /* MUSIC_H */
