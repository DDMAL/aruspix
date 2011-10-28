/////////////////////////////////////////////////////////////////////////////
// Name:        aximage.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __aximage_H__
#define __aximage_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "aximage.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"



//----------------------------------------------------------------------------
// wxAxImage
//----------------------------------------------------------------------------

class AxImage: public wxImage
{
public:
    // constructors and destructors
    AxImage();
	AxImage(const wxImage img);
    AxImage(int width, int height);
	bool LoadFile(const wxString& name, long type = wxBITMAP_TYPE_ANY, int index = -1);
	AxImage ScaleInterpolate(int width, int height, int interpolation);
	wxBitmap GetAxBitmap(int width = -1, int height = -1);
	void ShrinkDataInt(unsigned char *pInBuff, int wWidth, int wHeight,
                   unsigned char *pOutBuff, int wNewWidth, int wNewHeight);

        
private:
    
public:
	static bool s_zoomInterpolation;
	static bool s_reduceBigImages;
	static int s_imageSizeToReduce;
	bool static s_checkIfNegative;


private:
        DECLARE_CLASS(AxImage)
	
};


#if	defined(__cplusplus)
extern "C" {
#endif

struct _imImage;

_imImage* GetImImage(const AxImage *img, const int color_space = 0, const int data_type = 0);

void SetImImage(_imImage *im, AxImage *img);


#if defined(__cplusplus)
}
#endif



#endif // __AX_CORE_IMAGE_H__
