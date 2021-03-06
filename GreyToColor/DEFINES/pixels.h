/* === This file is part of GreyToColor ===
 *
 *	Copyright 2012-2013, Antony Cherepanov <antony.cherepanov@gmail.com>
 *
 *	GreyToColor is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	GreyToColor is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with GreyToColor. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PIXELS_H
#define PIXELS_H

#define DEFAULT_MAX_RGB_LUM 0
#define DEFAULT_MIN_RGB_LUM 255

#define DEFAULT_MAX_LAB_LUM 0
#define DEFAULT_MIN_LAB_LUM 10000

#define SMALL_MASK_RECT_SIDE 3
#define MEDIUM_MASK_RECT_SIDE 5
#define BIG_MASK_RECT_SIDE 11

#define SKO_MASK_RECT_SIDE 5
#define ENTROPY_MASK_RECT_SIDE 11

#define LAB_MIN_LUM 0.0
#define LAB_MAX_LUM 4.2
#define LAB_LUM_HIST_DIVIDER 0.001

// For mask 5x5
#define SKO_MAX 1.0
#define SKO_DIVIDER 0.001

// For mask 11x11
#define ENTROPY_MAX -7.0
#define SKEW_OFFSET 15.0
#define KURT_OFFSET 121.0

#define RELATIVE_MIN 0.0
#define RELATIVE_MAX 1.0
#define RELATIVE_DIVIDER 0.001

#endif // PIXELS_H
