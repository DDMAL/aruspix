/////////////////////////////////////////////////////////////////////////////
// Name:        aximage.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
using std::min;
using std::max;

// IMLIB
#include <im.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_palette.h>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "aximage.h"

// statics
bool AxImage::s_reduceBigImages = true;
bool AxImage::s_zoomInterpolation = true;
int AxImage::s_imageSizeToReduce = 3000;
bool AxImage::s_checkIfNegative = true;


static int *CreateCoeffInt(int nLen, int nNewLen, bool bShrink)
{
	int nSum = 0, nSum2;
	int *pRes = new int[2 * nLen];
	int *pCoeff = pRes;
	int nNorm = (bShrink) ? (nNewLen << 12) / nLen : 0x1000;
	//int nNorm = (bShrink) ? (nNewLen * 4096) / nLen : 0x1000;
	int	nDenom = (bShrink)? nLen : nNewLen;

	memset((void*)pRes,0,2 * nLen * sizeof(int));

	for(int i = 0; i < nLen; i++, pCoeff += 2)
	{
		nSum2 = nSum + nNewLen;
		if(nSum2 > nLen)
		{
			*pCoeff = ((nLen - nSum) << 12) / nDenom;
			//*pCoeff = ((nLen - nSum) * 4096) / nDenom;
			pCoeff[1] = ((nSum2 - nLen) << 12) / nDenom;
			//pCoeff[1] = ((nSum2 - nLen) * 4096) / nDenom;
			nSum2 -= nLen;
		}
		else
		{
			*pCoeff = nNorm;
			if(nSum2 == nLen)
			{
				pCoeff[1] = -1;
				nSum2 = 0;
			}
		}
		nSum = nSum2;
	}
	return pRes;
}


//----------------------------------------------------------------------------
// AxImage
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(AxImage, wxImage)

AxImage::AxImage(const wxImage img) 
						   : wxImage(img)
{

}

AxImage::AxImage(int width, int height) 
						   : wxImage(width,height)
{

}

AxImage::AxImage() : wxImage()
{
}


bool AxImage::LoadFile(const wxString& name, long type, int index)
{
	bool res = wxImage::LoadFile( name, type, index);

	if (AxImage::s_reduceBigImages 
		&& (max(this->GetWidth(),this->GetHeight()) > AxImage::s_imageSizeToReduce))
	{
		imImage *imTmp1 = GetImImage(this, IM_RGB);
		do
		{
			int removeX = imTmp1->width % 2;
			int removeY = imTmp1->height % 2;
			if (removeX || removeY) // controler si width ou height sont impaires -> si oui, crop image
			{
				imImage *imTmp2 = imImageCreate(imTmp1->width - removeX,imTmp1->height - removeY,imTmp1->color_space, imTmp1->data_type);
				imProcessCrop(imTmp1,imTmp2, 0, 0);
				imImageDestroy(imTmp1);
				imTmp1 = imTmp2;
			}
			imImage *imTmp2 = imImageCreate(imTmp1->width/2,imTmp1->height/2,imTmp1->color_space, imTmp1->data_type);
			imProcessReduceBy4(imTmp1,imTmp2);
			imImageDestroy(imTmp1);
			imTmp1 = imTmp2;
		} while (max(imTmp1->width, imTmp1->height) > AxImage::s_imageSizeToReduce);
		SetImImage(imTmp1,this);
		imImageDestroy(imTmp1);
	}
	//wxLogMessage("%d %d", this->GetWidth(), this->GetHeight());
	return res;
	
	/*

	int error;
	imFile* ifile = imFileOpen(name.c_str(), &error);
	imImage* image = imFileLoadBitmap(ifile, index, &error);
	imFileClose(ifile);
	if (!image)
		return false;

	if ( AxImage::s_reduceBigImages )
	{
		while ( max( image->width, image->height ) > AxImage::s_imageSizeToReduce )	
		{
			int removeX = image->width % 2;
			int removeY = image->height % 2;
			if (removeX || removeY) // controler si width ou height sont impaires -> si oui, crop image
			{
				imImage *imTmp = imImageCreate(image->width - removeX,image->height - removeY,image->color_space, image->data_type);
				imProcessCrop(image,imTmp, 0, 0);
				imImageDestroy(image);
				image = imTmp;
			}
			imImage *imTmp = imImageCreate(image->width/2,image->height/2,image->color_space, image->data_type);
			imProcessReduceBy4(image,imTmp);
			imImageDestroy(image);
			image = imTmp;
		};
	}
	//wxLogMessage("%d %d", this->GetWidth(), this->GetHeight());
	SetImImage(image,this);
	imImageDestroy(image);
	return true;*/
}



wxBitmap AxImage::GetAxBitmap(int width, int height)
{
	if ((width != -1) && (height != -1))
	{
		wxBitmap bmp(this->ScaleInterpolate(width,height,AxImage::s_zoomInterpolation));
		//wxBitmap bmp(this->Scale(width,height));
		return bmp;
	}
	else
	{
		//const wxImage img;
		//wxBitmap bmp( img );
		wxBitmap bmp( *this );
		return bmp;
	}
}

AxImage AxImage::ScaleInterpolate(int newWidth, int newHeight, int interpolation)
{
	AxImage scaledImg(newWidth, newHeight);
	int thisWidth = this->GetWidth();
	int thisHeight = this->GetHeight();
	double factor = (double)newWidth/(double)thisWidth;

	if (interpolation && (factor < 1.0))
	{
		ShrinkDataInt(this->GetData(),thisWidth,thisHeight,scaledImg.GetData(),newWidth,newHeight);
	}
	else
	{
		scaledImg = this->Scale(newWidth, newHeight);
	}
	return scaledImg;
}


void AxImage::ShrinkDataInt(unsigned char *pInBuff, int wWidth, int wHeight,
                   unsigned char *pOutBuff, int wNewWidth, int wNewHeight)
{
	int pixelToPickUp = 3;
	if ( imBinCPUByteOrder() == IM_BIGENDIAN )
		pixelToPickUp = 0;


	unsigned char  *pLine = pInBuff, *pPix;
	unsigned char  *pOutLine = pOutBuff;
	//int dwInLn = (3 * wWidth + 3) & ~3;
	//int dwOutLn = (3 * wNewWidth + 3) & ~3;
	int dwInLn = (3 * wWidth);
	int dwOutLn = (3 * wNewWidth);

	int   x, y, i, ii;
	bool  bCrossRow, bCrossCol;
	int   *pRowCoeff = CreateCoeffInt(wWidth, 
                                   wNewWidth,
                                   true);
	int   *pColCoeff = CreateCoeffInt(wHeight, 
                                   wNewHeight, 
                                   true);
	int *pXCoeff, *pYCoeff = pColCoeff;
	int dwBuffLn = 3 * wNewWidth * sizeof(int);
	int *pdwBuff = new int[9 * wNewWidth];
	int *pdwCurrLn = pdwBuff, 
       *pdwCurrPix, 
       *pdwNextLn = pdwBuff + 3 * wNewWidth;
	int dwTmp, *pdwNextPix;

	memset((void*)pdwBuff,0,3 * dwBuffLn);
	y = 0;
	while(y < wNewHeight - 1) 
		// normalement pas -1, mais sinon depasse le buffer (sauf si alloue par 
		// par windows !!!!! avec un bitmap) -> laisse une petite ligne noire en bas ... 
	{
		pPix = pLine;
		pLine += dwInLn;
		pdwCurrPix = pdwCurrLn;
		pdwNextPix = pdwNextLn;

		x = 0;
		pXCoeff = pRowCoeff;
		bCrossRow = pYCoeff[1] > 0;
		while(x < wNewWidth)
		{
			dwTmp = *pXCoeff * *pYCoeff;
			for(i = 0; i < 3; i++)
				pdwCurrPix[i] += dwTmp * pPix[i];
			bCrossCol = pXCoeff[1] > 0;
			if(bCrossCol)
			{
				dwTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0, ii = 3; i < 3; i++, ii++)
					pdwCurrPix[ii] += dwTmp * pPix[i];
			}
			if(bCrossRow)
			{
				dwTmp = *pXCoeff * pYCoeff[1];
				for(i = 0; i < 3; i++)
					pdwNextPix[i] += dwTmp * pPix[i];
				if(bCrossCol)
				{
					dwTmp = pXCoeff[1] * pYCoeff[1];
					for(i = 0, ii = 3; i < 3; i++, ii++)
						pdwNextPix[ii] += dwTmp * pPix[i];
				}
			}
			if(pXCoeff[1])
			{
				x++;
				pdwCurrPix += 3;
				pdwNextPix += 3;
			}
			pXCoeff += 2;
			pPix += 3;
		}
		if(pYCoeff[1])
		{
			// set result line
			pdwCurrPix = pdwCurrLn;
			pPix = pOutLine;
			for(i = 3 * wNewWidth; i > 0; i--, pdwCurrPix++, pPix++)
				*pPix = ((unsigned char *)pdwCurrPix)[pixelToPickUp];
			// prepare line buffers
			pdwCurrPix = pdwNextLn;
			pdwNextLn = pdwCurrLn;
			pdwCurrLn = pdwCurrPix;
			memset(pdwNextLn, 0, dwBuffLn);

			y++;
			pOutLine += dwOutLn;
		}
		pYCoeff += 2;
	}
	delete [] pRowCoeff;
	delete [] pColCoeff;
	delete [] pdwBuff;
} 


/*AxImage AxImage::ScaleInterpolate(int width, int height, int interpolation)
{
	AxImage image;
	vigra::BImage *in = GetVigraBImage(this);
	try
	{

		if (in==NULL) return image;	
		
		vigra::BImage out(width, height);
                      
		switch(interpolation)
		{
			case 0:
				// resize the image, using a bi-cubic spline algorithms
				resizeImageNoInterpolation(srcImageRange(*in), 
					destImageRange(out));
				break;
			case 1:
				// resize the image, using a bi-cubic spline algorithms
				resizeImageLinearInterpolation(srcImageRange(*in), 
					destImageRange(out));
				break;
			default:
				// resize the image, using a bi-cubic spline algorithms
				resizeImageSplineInterpolation(srcImageRange(*in), 
					destImageRange(out));
		}
		delete in;
		SetVigraBImage(&out,&image);
		return image;

	}
	catch (vigra::StdException & e)
	{
		wxLogError(wxString::Format("%s",e.what()));
		delete in;
        return image;
	}

}*/


_imImage* GetImImage(const AxImage *img, const int color_space, const int data_type)
{
	if (!img) return NULL;


	//imImage* imTmp1 = imImageCreate(img->GetWidth(), img->GetHeight(), (IM_RGB | IM_PACKED ) , IM_BYTE);
	imImage* imTmp1 = imImageCreate(img->GetWidth(), img->GetHeight(), ( IM_RGB ) , IM_BYTE);
	memcpy(imTmp1->data[0],img->GetData(),img->GetWidth() * img->GetHeight() * 3);
	//imImageCopyData(im,imTmp1);


	if (!imColorModeIsPacked(color_space))
	{
		imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height,( IM_RGB ), IM_BYTE);
		imConvertPacking(imTmp1->data[0],imTmp2->data[0],imTmp1->width,imTmp1->height,imTmp1->depth,imTmp2->depth,imTmp1->data_type,1);
		imImageDestroy(imTmp1);
		imTmp1 = imTmp2;
	}

	if (data_type != IM_BYTE)
	{
		imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, ( IM_RGB ) , data_type);
		imConvertDataType(imTmp1,imTmp2,0,0,0,0);
		imImageDestroy(imTmp1);
		imTmp1 = imTmp2;
	}

	if (!imColorModeMatch(color_space,IM_RGB))
	{
		imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, color_space, imTmp1->data_type);
		imConvertColorSpace(imTmp1,imTmp2);
		imImageDestroy(imTmp1);
		imTmp1 = imTmp2;
	}

	return imTmp1;
}

void SetImImage(_imImage *im, AxImage *img)
{
	if (!im || !img) return;

	if ( !img->Ok() || (im->width != img->GetWidth()) || (im->height != img->GetHeight()) )
	{
		img->Destroy();
		img->Create(im->width,im->height);
	}
	
	/*img->Create(im->width,im->height);
	unsigned char *dest = img->GetData();
	unsigned char *src = (unsigned char *)im->data[0];
	for (int i=0; i < img->GetWidth() * img->GetHeight(); dest+=3, src++, i++)
	{
		memcpy(dest, src, 1);
		memcpy(dest+1, src, 1);
		memcpy(dest+2, src, 1);
	}
	return; */

	/*imImage* imTmp1 = imImageClone(m_imPtr);
	imImageCopyData(m_imPtr,imTmp1);
	if ((imTmp1->color_space == IM_GRAY) || (imTmp1->color_space == IM_BINARY))
	{
		wxImage image(imTmp1->width,imTmp1->height);
		unsigned char *image1Ptr = image.GetData();
		unsigned char *image2Ptr = (unsigned char *)imTmp1->data[0];
		unsigned char pixel[3];
		for (int i=0; i < image.GetWidth() * image.GetHeight(); image1Ptr+=3, image2Ptr++, i++)
		{
			memcpy(image1Ptr, image2Ptr, 1);
			memcpy(image1Ptr+1, image2Ptr, 1);
			memcpy(image1Ptr+2, image2Ptr, 1);
		}
		wxBitmap bmp(image);
		imImageDestroy(imTmp1);
		return bmp;
	}
	else
	{
		if (imTmp1->color_space != IM_RGB)
		{
			imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, IM_RGB, imTmp1->data_type);
			imConvertColorSpace(imTmp1,imTmp2);
			imImageDestroy(imTmp1);
			imTmp1 = imTmp2;
		}

		if (imTmp1->data_type != IM_BYTE)
		{
			imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, IM_RGB, IM_BYTE);
			imConvertDataType(imTmp1,imTmp2,0,0,0,0);
			imImageDestroy(imTmp1);
			imTmp1 = imTmp2;
		}

		if (imTmp1->data_type != IM_PACKED) Ce doit pas etre juste ???????
		{
			imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, (IM_RGB | IM_PACKED ), IM_INT);
			imConvertPacking(imTmp1->data[0],imTmp2->data[0],imTmp1->width,imTmp1->height,imTmp1->depth,imTmp1->data_type,0);
			imImageDestroy(imTmp1);
			imTmp1 = imTmp2;
		}
		
		wxImage image(imTmp1->width,imTmp1->height,(unsigned char *)imTmp1->data[0],true);
		wxBitmap bmp(image);
		imImageDestroy(imTmp1);
		return bmp;
	}*/


	imImage* imTmp1 = imImageDuplicate(im);

	if (!imColorModeMatch(imTmp1->color_space,IM_RGB))
	{
		imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, IM_RGB, imTmp1->data_type);
		imConvertColorSpace(imTmp1,imTmp2);
		imImageDestroy(imTmp1);
		imTmp1 = imTmp2;
	}

	if (imTmp1->data_type != IM_BYTE)
	{
		imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, IM_RGB, IM_BYTE);
		imConvertDataType(imTmp1,imTmp2,0,0,0,0);
		imImageDestroy(imTmp1);
		imTmp1 = imTmp2;
	}
	
	if (!imColorModeIsTopDown(imTmp1->color_space))
	{
		imImage* imTmp2 = imImageClone( imTmp1 );
		imProcessFlip(imTmp1, imTmp2);
		imImageDestroy(imTmp1);
		imTmp1 = imTmp2;
	}

	if (!imColorModeIsPacked(imTmp1->color_space))
	{
		//imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, (IM_RGB | IM_PACKED ), IM_INT);
		imImage* imTmp2 = imImageCreate(imTmp1->width, imTmp1->height, ( IM_RGB ), IM_INT);
		imConvertPacking(imTmp1->data[0],imTmp2->data[0],imTmp1->width,imTmp1->height,imTmp1->depth,imTmp2->depth,imTmp1->data_type,0);
		imImageDestroy(imTmp1);
		imTmp1 = imTmp2;
	}

	memcpy(img->GetData(),(unsigned char *)imTmp1->data[0],imTmp1->width * imTmp1->height * 3);
	imImageDestroy(imTmp1);
	return;
}





