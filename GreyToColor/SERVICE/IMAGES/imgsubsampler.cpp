/* === This file is part of GreyToColor ===
 *
 *	Copyright 2012, Antony Cherepanov <antony.cherepanov@gmail.com>
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

#include "imgsubsampler.h"

ImgSubsampler::ImgSubsampler()
{

}

// Subsample image and get RGB luminances of each sample
// @input:
// - Image - unnul image (color/grey)
// - unsigned int - num of samples we should divide input image
// @output:
// - empty QList<double> - failed to subsample image
// - QList<double> - RGB luminances of each sample of subsampled image
QList<double> ImgSubsampler::SubsampleImg(const Image &t_img, const int &t_numOfSamplesOnFacet)
{
	if ( (true == t_img.IsNull()) || (t_numOfSamplesOnFacet <= 0) )
	{
		qDebug() << "SubsampleImg(): Error - invalid arguments";
		QList<double> empty;
		return empty;
	}

	QImage greyImg = GetGreyImg(t_img);
	if ( true == greyImg.isNull() )
	{
		qDebug() << "SubsampleImg(): Error - no grey image";
		QList<double> empty;
		return empty;
	}

	QList<double> empty;
	return empty;
}

// Get greyscale image
// @input:
// - Image - unnul image
// @output:
// - null QImage - failed to greyscale image
// - QImage - grey image
QImage ImgSubsampler::GetGreyImg(const Image &t_img)
{
	if ( true == t_img.IsNull() )
	{
		qDebug() << "GetGreyImg(): Error - invalid arguments";
		QImage empty;
		return empty;
	}

	ImgTransform imgTransformer;
	Image greyImg = imgTransformer.ToGrey(t_img);
	if ( true == greyImg.IsNull() )
	{
		qDebug() << "GetGreyImg(): Error - can't greyscale image";
		QImage empty;
		return empty;
	}

	QImage greyscaledImg = greyImg.GetImg();
	if ( true == greyscaledImg.isNull() )
	{
		qDebug() << "GetGreyImg(): Error - can't get grey image";
		QImage empty;
		return empty;
	}

	return greyscaledImg;
}

// Get images samples rectangulars coordinates
// @input:
// - int - num of samples on each of rects facets
// - int - positive image width
// - int - positive image height
// @output:
// - empty QList<QRect> - failed calc rects of samples
// - QList<QRect> - samples coords
QList<QRect> ImgSubsampler::GetSamplesRects(const int &t_numOfSamplesOnFacet,
											const int &t_imgWidth,
											const int &t_imgHeight)
{
	if ( (t_numOfSamplesOnFacet <= 0) ||
		 (t_imgWidth <= 0) ||
		 (t_imgHeight <= 0) )
	{
		qDebug() << "GetSamplesRects(): Error - invalid arguments";
		QList<QRect> empty;
		return empty;
	}

	QList< QPair<int, int> > wdtSamples = DivideFacet(t_imgWidth, t_numOfSamplesOnFacet);
	QList< QPair<int, int> > hgtSamples = DivideFacet(t_imgHeight, t_numOfSamplesOnFacet);
	if ( (true == wdtSamples.isEmpty()) ||
		 (t_numOfSamplesOnFacet != wdtSamples.size()) ||
		 (true == hgtSamples.isEmpty()) ||
		 (t_numOfSamplesOnFacet != hgtSamples.size()))
	{
		qDebug() << "GetSamplesRects(): Error - can't divide to samples";
		QList<QRect> empty;
		return empty;
	}

	const int lastWdtSample = wdtSamples.size();
	const int lastHgtSample = hgtSamples.size();
	QList<QRect> samplesRects;
	for ( int wSamp = 0; wSamp < lastWdtSample; wSamp++ )
	{
		int coordX = wdtSamples.at(wSamp).first;
		int width = wdtSamples.at(wSamp).second;

		for ( int hSamp = 0; hSamp < lastHgtSample; hSamp++ )
		{
			int coordY = hgtSamples.at(hSamp).first;
			int height = hgtSamples.at(hSamp).second;

			QRect sample(coordX, coordY, width, height);
			samplesRects.append(sample);
		}
	}

	return samplesRects;
}

// Devide facet of image to samples
// @input:
// - int - length of facet to divide
// - int - number of samples
// @output:
// - empty QList< QPair<int, int> > - can't divide facet to samples
// - QList< QPair<int, int> > - samples params (start coord and length)
QList< QPair<int, int> > ImgSubsampler::DivideFacet(const int &t_length, const int &t_samplesNum)
{
	if ( (t_length <= 0) || ( t_samplesNum <= 0) )
	{
		qDebug() << "DivideFacet(): Error - invalid arguments";
		QList< QPair<int, int> > empty;
		return empty;
	}

	QList< QPair<int, int> > resultSamples;

	if ( t_length < t_samplesNum )
	{
		// If facet is too small, all samples will be the same
		int startCoord = 0;
		int sampleLength = t_length;
		QPair<int, int> sample(startCoord, sampleLength);
		for ( int samp = 0; samp < t_samplesNum; samp++ )
		{
			resultSamples.append(sample);
		}
	}
	else
	{
		int pixPerSample = t_length / t_samplesNum;
		int pixInEnd = t_length % t_samplesNum;
		int pixToAdd = pixInEnd / t_samplesNum;
		pixPerSample += pixToAdd;

		// Create samples with same length. Last sample will have different length
		for ( int samp = 0; samp < t_samplesNum - 1; samp++ )
		{
			int startCoord = pixPerSample * samp;
			QPair<int, int> sample(startCoord, pixPerSample);
			resultSamples.append(sample);
		}

		// Calc last sample coord and length
		int coord = pixPerSample * (t_samplesNum - 1);
		int length = t_length - coord;
		QPair<int, int> lastSample(coord, length);
		resultSamples.append(lastSample);
	}

	return resultSamples;
}

// Test image dividing to samples
void ImgSubsampler::TestDivide()
{
	QList<QRect> resultRecs = GetSamplesRects(16, 100, 80);
	if ( 16 * 16 != resultRecs.size() )
	{
		qDebug() << "First failed";
		return;
	}

	if ( 6 != resultRecs.at(0).width() )
	{
		qDebug() << "First length" << resultRecs.at(0).width();
		return;
	}

	resultRecs.clear();
	resultRecs = GetSamplesRects(16, 16, 16);
	if ( 16 * 16 != resultRecs.size() )
	{
		qDebug() << "Second failed";
		return;
	}

	if ( 1 != resultRecs.at(0).width() )
	{
		qDebug() << "Second length" << resultRecs.at(0).width();
		return;
	}

	resultRecs.clear();
	resultRecs = GetSamplesRects(16, 10, 6);
	if ( 16 * 16 != resultRecs.size() )
	{
		qDebug() << "Third failed";
		return;
	}

	if ( 10 != resultRecs.at(0).width() )
	{
		qDebug() << "Third length" << resultRecs.at(0).width();
		return;
	}
}
