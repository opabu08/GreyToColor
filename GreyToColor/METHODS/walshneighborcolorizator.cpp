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

#include "walshneighborcolorizator.h"

WalshNeighborColorizator::WalshNeighborColorizator()
{

}

// Start Colorization
// @input:
// - TargetImage - unnull unempty target image which we need to colorize
// - SourceImage - unnull unempty source image
// @output:
// - false - can't colorize
// - true - Target Image colorized
bool WalshNeighborColorizator::Colorize(TargetImage *t_targetImg, SourceImage *t_sourceImg)
{
	if ( (NULL == t_targetImg) ||
		 (NULL == t_sourceImg) ||
		 (false == t_targetImg->HasImage()) ||
		 (false == t_sourceImg->HasImage()) )
	{
		qDebug() << "Colorize(): Error - invalid arguments";
		return false;
	}

	m_target = t_targetImg;
	m_source = t_sourceImg;

	bool imagesPrepared = PrepareImages();
	if ( false == imagesPrepared )
	{
		qDebug() << "Colorize(): Error - can't prepare images for colorization";
		return false;
	}

	bool targetColorized = ColorizeImage();
	if ( false == targetColorized )
	{
		qDebug() << "Colorize(): Error - can't colorize Target image";
		return false;
	}

	bool paramsResored = PostColorization();
	if ( false == paramsResored )
	{
		qDebug() << "Colorize(): Error - can't restore images parameters";
		return false;
	}

	return true;
}

// Prepare images to colorization
// @input:
// @output:
bool WalshNeighborColorizator::PrepareImages()
{
	if ( (NULL == m_target) ||
		 (NULL == m_source) ||
		 (false == m_target->HasImage()) ||
		 (false == m_source->HasImage()) )
	{
		qDebug() << "PrepareImages(): Error - invalid arguments";
		return false;
	}

	m_target->SetPixelsUncoloured();

	m_target->TransformImgRGB2LAB();
	m_source->TransformImgRGB2LAB();

	const double targMaxLum = m_target->GetMaxLABLum();
	const double sourceMaxLum = m_source->GetMaxLABLum();
	const double scaleFactor = sourceMaxLum / targMaxLum;
	bool lumScaled = m_target->ScaleLABLum(scaleFactor);
	if ( false == lumScaled )
	{
		qDebug() << "PrepareImages(): Error - can't scale luminance of Target image";
		return false;
	}

	m_target->CalcPixelsSKO();
	m_source->CalcPixelsSKO();

	return true;
}

// Colorize Target image using color information from Source image
// @input:
// @output:
bool WalshNeighborColorizator::ColorizeImage()
{
	qDebug() << "Start colorization!";
	QElapsedTimer timer;
	timer.start();

	if ( (NULL == m_target) ||
		 (NULL == m_source) ||
		 (false == m_target->HasImage()) ||
		 (false == m_source->HasImage()) )
	{
		qDebug() << "ColorizeImage(): Error - invalid arguments";
		return false;
	}

	const unsigned int targetWdt = m_target->GetImageWidth();
	const unsigned int targetHgt = m_target->GetImageHeight();
	const unsigned int sourceWdt = m_source->GetImageWidth();
	const unsigned int sourceHgt = m_source->GetImageHeight();

	for ( unsigned int width = 0; width < targetWdt; width++ )
	{
		qDebug() << "ColorizeImage(): row =" << width;
		for ( unsigned int height = 0; height < targetHgt; height++ )
		{
			if ( true == m_target->IsPixColoured(width, height) )
			{
				continue;
			}

			srand(time(NULL));

			double bestDiffLum = DEFAULT_LUM;
			double bestDiffSKO = DEFAULT_SKO;
			unsigned int bestSourcePixWdt = 0;
			unsigned int bestSourcePixHgt = 0;

			double targPixLum = m_target->PixelChLum(width, height);
			double targPixSKO = m_target->GetPixelsSKO(width, height);
			if ( (targPixLum <= NO_INFO) || (targPixSKO <= ERROR) )
			{
				qDebug() << "ColorizeImage(): Warning - failed to colorize pixel" << width << height;
				continue;
			}

			for ( int pix = 0; pix < NUM_OF_ATTEMPTS; pix++ )
			{
				const unsigned int sourceRandWdt = rand() % sourceWdt;
				const unsigned int sourceRandHgt = rand() % sourceHgt;

				const double sourcePixLum = m_source->PixelChLum(sourceRandWdt, sourceRandHgt);
				const double sourcePixSKO = m_source->GetPixelsSKO(sourceRandWdt, sourceRandHgt);

				const double diffLum = fabs( targPixLum - sourcePixLum );
				const double diffSKO = fabs( targPixSKO - sourcePixSKO );

				if ( ( (diffLum < bestDiffLum) && (diffSKO < (bestDiffSKO + SKO_TRESHOLD)) ) ||
					 ( (diffLum < (bestDiffLum + LUM_TRESHOLD)) && (diffSKO < bestDiffSKO) ) )
				{
					bestDiffLum = diffLum;
					bestDiffSKO = diffSKO;
					bestSourcePixWdt = sourceRandWdt;
					bestSourcePixHgt = sourceRandHgt;
				}
			}

			double sourceChA = m_source->PixelChA(bestSourcePixWdt, bestSourcePixHgt);
			double sourceChB = m_source->PixelChB(bestSourcePixWdt, bestSourcePixHgt);
			m_target->SetPixelChAB(width,
								   height,
								   sourceChA,
								   sourceChB);

			m_target->SetPixColoured(width, height);

			ColorizeNeighbor(width, height);
		}
	}

	qDebug() << "Elapsed time in nanosec:" << timer.nsecsElapsed();

	return true;
}

// Try to colorize neighbor pixels by the same color
void WalshNeighborColorizator::ColorizeNeighbor(const unsigned int &t_startWidth, const unsigned int &t_startHeight)
{
	const unsigned int targetWdt = m_target->GetImageWidth();
	const unsigned int targetHgt = m_target->GetImageHeight();
	unsigned int targCurrWdt = t_startWidth;
	unsigned int targCurrHgt = t_startHeight;

	// Try to color lower neighbor pixel
	double targHgtTry = targCurrHgt + 1;
	while( (true == m_target->IsPixColoured(targCurrWdt, targCurrHgt)) &&
		   (targHgtTry < targetHgt) &&
		   (false == m_target->IsPixColoured(targCurrWdt, targHgtTry)) )
	{
		// Get characteristics of current pixel
		double targPixLum = m_target->PixelChLum(targCurrWdt, targCurrHgt);
		double targPixSKO = m_target->GetPixelsSKO(targCurrWdt, targCurrHgt);

		// Get characteristics of next lower pixel
		double lowerTargPixLum = m_target->PixelChLum(targCurrWdt, targHgtTry);
		double lowerTargPixSKO = m_target->GetPixelsSKO(targCurrWdt, targHgtTry);

		double lowerNeighborDiffLum = fabs( targPixLum - lowerTargPixLum );
		double lowerNeighborDiffSKO = fabs( targPixSKO - lowerTargPixSKO );
		if ( (lowerNeighborDiffLum < LUM_TRESHOLD) && (lowerNeighborDiffSKO < SKO_TRESHOLD) )
		{
			// If neighbor pixel has quite the same characteristics, apply to it the same color
			double chA = m_target->PixelChA(targCurrWdt, targCurrHgt);
			double chB = m_target->PixelChB(targCurrWdt, targCurrHgt);
			m_target->SetPixelChAB(targCurrWdt,
								   targHgtTry,
								   chA,
								   chB);

			m_target->SetPixColoured(targCurrWdt, targHgtTry);

			// Try to color neighbor right pixel
			double targRightWdt = targCurrWdt;
			double targWdtTry = targCurrWdt + 1;
			while( (true == m_target->IsPixColoured(targRightWdt, targHgtTry)) &&
				   (targWdtTry < targetWdt) &&
				   (false == m_target->IsPixColoured(targWdtTry, targHgtTry)) )
			{
				double rightTargPixLum = m_target->PixelChLum(targWdtTry, targHgtTry);
				double rightTargPixSKO = m_target->GetPixelsSKO(targWdtTry, targHgtTry);

				double rightNeighborDiffLum = fabs( lowerTargPixLum - rightTargPixLum );
				double rightNeighborDiffSKO = fabs( lowerTargPixSKO - rightTargPixSKO );
				if ( (rightNeighborDiffLum < LUM_TRESHOLD) && (rightNeighborDiffSKO < SKO_TRESHOLD) )
				{
					m_target->SetPixelChAB(targWdtTry,
										   targHgtTry,
										   chA,
										   chB);

					m_target->SetPixColoured(targWdtTry, targHgtTry);

					// Recursion!
					ColorizeNeighbor(targWdtTry, targHgtTry);
				}

				targRightWdt = targWdtTry;
				targWdtTry++;
			}
		}

		// Set this just checked pixel as current
		targCurrHgt = targHgtTry;
		targHgtTry++;
	}
}

// Restore images params if needed
// @input:
// @output:
bool WalshNeighborColorizator::PostColorization()
{
	m_target->UnScaleLABLum();
	return true;
}