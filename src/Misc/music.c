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

#include "music.h"

#define VELOCITY_DEFAULT 200

struct note_data music_startup[] = {
	{71, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{70, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{71, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	{73, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	{74, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	SCORE_STOP
	};

struct note_data music_fatal_error[] = {
	{71, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	{80, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	{0, VELOCITY_SILENCE, NOTE_DURATION(8)},
	SCORE_REPEAT
	};

struct note_data music_enter_calibration[] = {
	{60, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{60, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{0, VELOCITY_SILENCE, NOTE_DURATION(16)},
	{71, VELOCITY_DEFAULT, NOTE_DURATION_DOT(8)},
	SCORE_STOP
	};

struct note_data music_calibration_measure1[] = {
	{71, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	SCORE_STOP
	};

struct note_data music_calibration_measure2[] = {
	{71, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{0, VELOCITY_SILENCE, NOTE_DURATION(16)},
	{71, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	SCORE_STOP
	};

struct note_data music_calibration_excellent[] = {
	{72, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{79, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{84, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	SCORE_STOP
	};

struct note_data music_calibration_good[] = {
	{72, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{79, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{79, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	SCORE_STOP
	};

struct note_data music_calibration_ugly[] = {
	{72, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{79, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	SCORE_STOP
	};

struct note_data music_calibration_bad[] = {
	{72, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	SCORE_STOP
	};

struct note_data music_calibration_cancel[] = {
	{79, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{75, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	{72, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	SCORE_STOP
	};

struct note_data music_calibration_accept[] = {
	{72, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{0, VELOCITY_SILENCE, NOTE_DURATION(32)},
	{72, VELOCITY_DEFAULT, NOTE_DURATION(32)},

	{76, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{0, VELOCITY_SILENCE, NOTE_DURATION(32)},
	{76, VELOCITY_DEFAULT, NOTE_DURATION(32)}
	,
	{0, VELOCITY_SILENCE, NOTE_DURATION(32)},

	{79, VELOCITY_DEFAULT, NOTE_DURATION(16)},
	SCORE_STOP
	};

struct note_data music_calibration_escs_enter[] = {
	{64, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{65, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{66, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{67, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{68, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{69, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	SCORE_STOP
	};

struct note_data music_calibration_escs_step[] = {
	{69, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{68, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{67, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{66, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{65, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{64, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	SCORE_STOP
	};

struct note_data music_armed[] = {
	{67, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{74, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{67, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{74, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{67, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{74, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{67, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	{74, VELOCITY_DEFAULT, NOTE_DURATION(32)},
	SCORE_STOP
	};

struct note_data music_unarmed[] = {
	{67, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	{74, VELOCITY_DEFAULT, NOTE_DURATION(8)},
	{71, VELOCITY_DEFAULT, NOTE_DURATION_DOT(8)},
	SCORE_STOP
	};
