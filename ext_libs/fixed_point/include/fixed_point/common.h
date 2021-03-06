/**
 * @file
 * @author	Juan I Carrano
 * @author	Juan I. Ubeira
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
 * Common macros.
 */

#ifndef FXP_COMMON_H
#define FXP_COMMON_H

#define FXP_DECLARATION_C99_HEADER(x) _FXP_INLINE_KW x

#define FXP_DECLARATION_C99_BODY(x) _FXP_INLINE_PROTO_KW x; \
_FXP_INLINE_KW x

#endif /* FXP_COMMON_H */
