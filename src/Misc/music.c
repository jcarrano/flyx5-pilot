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

#include "../peripheral/buzzer.h"

struct note_data music_startup[] = {
	{71, 200, NOTE_DURATION(16)},
	{70, 200, NOTE_DURATION(16)},
	{71, 200, NOTE_DURATION(8)},
	{73, 200, NOTE_DURATION(8)},
	{74, 200, NOTE_DURATION(8)},
	SCORE_STOP
	};

struct note_data music_fatal_error[] = {
	{71, 200, NOTE_DURATION(8)},
	{76, 200, NOTE_DURATION(8)},
	{80, 200, NOTE_DURATION(8)},
	{0, VELOCITY_SILENCE, NOTE_DURATION(8)},
	SCORE_REPEAT
	};
