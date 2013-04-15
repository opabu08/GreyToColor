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

#include "colorpixel.h"

ColorPixel::ColorPixel()
{
	ClearColor();
}

ColorPixel::~ColorPixel()
{
	ClearColor();
}

// Clear color pixel
// @input:
// @output:
void ColorPixel::ClearColor()
{
	this->Clear();

	m_relativeLum = ERROR;
	m_sko = 0.0;
	m_entropy = 0.0;
}

// Calc relative value of LAB luminance
// @input:
// @output:
void ColorPixel::CalcRelativeLum()
{
	m_relativeLum = GetChL() / LAB_MAX_LUM;
}

// Get relative LAB luminance value
// @input:
// @output:
// - double - relative LAB luminance in range [0, 1]
// - double < 0 - error, relative luminance not calc yet
double ColorPixel::GetRelativeLum() const
{
	return m_relativeLum;
}

// Set relative LAB luminance value
// @input:
// - double - value of relative LAB luminace in range [0, 1]
// @output:
// - true - luminance accepted
// - false - luminance is out of range
bool ColorPixel::SetRelativeLum(const double &t_lum)
{
	if ( (t_lum < RELATIVE_MIN) || (RELATIVE_MAX < t_lum) )
	{
		qDebug() << "SetRelativeLum(): Error - invalid arguments, t_lum =" << t_lum;
		qDebug() << "Realtive lumiance not accepted";
		return false;
	}

	m_relativeLum = t_lum;

	return true;
}

// Set SKO for pixel of image
// @input:
// - double - relative SKO value in range [0, 1]
// @output:
// - true - SKO accepted
// - false - SKO is out of range
bool ColorPixel::SetSKO(const double &t_sko)
{
	if ( (t_sko < RELATIVE_MIN) || (RELATIVE_MAX < t_sko) )
	{
		qDebug() << "SetSKO(): Error - invalid arguments, t_sko =" << t_sko;
		qDebug() << "Realtive SKO not accepted";
		return false;
	}

	m_sko = t_sko;

	return true;
}

// Set SKO for pixel of image
// @input:
// @output:
// - double - relative SKO value in range [0, 1]
double ColorPixel::GetSKO() const
{
	return m_sko;
}

// Set entropy for pixel of image
// @input:
// - double - relative entropy value in range [0, 1]
// @output:
// - true - entropy accepted
// - false - entropy is out of range
bool ColorPixel::SetEntropy(const double &t_entropy)
{
	if ( (t_entropy < RELATIVE_MIN) || (RELATIVE_MAX < t_entropy) )
	{
		qDebug() << "SetEntropy(): Error - invalid arguments, t_entropy =" << t_entropy;
		qDebug() << "Realtive Entropy not accepted";
		return false;
	}

	m_entropy = t_entropy;

	return true;
}

// Set entropy for pixel of image
// @input:
// @output:
// - double - entropy value in range [0, 1]
double ColorPixel::GetEntropy() const
{
	return m_entropy;
}
