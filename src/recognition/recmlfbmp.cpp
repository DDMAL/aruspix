/////////////////////////////////////////////////////////////////////////////
// Name:        recmlfbmp.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/filename.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"

#include "recmlfbmp.h"

#include "im/impage.h"
#include "im/imstaff.h"

#include "verovio/layer.h"
#include "verovio/layerelement.h"


#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMLFBitmapTypes );

IMPLEMENT_DYNAMIC_CLASS(RecMLFSymbolBmp, MusMLFSymbol)

using namespace vrv;

//----------------------------------------------------------------------------
// RecMLFBmpType
//----------------------------------------------------------------------------


RecMLFBmpType::RecMLFBmpType( ) :
	wxObject()
{
	//m_width = 0;
	m_bmpoffset = 0;
	m_flip = false;
}

//----------------------------------------------------------------------------
// RecMLFBmpTypes
//----------------------------------------------------------------------------

RecMLFBmpTypes::RecMLFBmpTypes()
{
}


RecMLFBmpTypes::~RecMLFBmpTypes()
{
	m_types.Clear();
}


bool RecMLFBmpTypes::Load( wxString file )
{
	m_types.Clear();

	//wxFileName fn( file );
	//fn.SetExt( "xml" );

    TiXmlDocument dom( file.c_str() );
    bool ok = dom.LoadFile();

    if ( !ok )
    {
        wxLogWarning(_("Unable to load file '%s'"), file.c_str() );
        return false;
    }

    TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
    TiXmlElement *elem = NULL;

    root = dom.RootElement();
    if ( !root ) return false;

	wxString impath = "";
	wxFileName::SplitPath( file, &impath, NULL, NULL );
	impath += "/";
	wxString imtype = "png";

	node = root->FirstChild("bitmaps");
    if ( node && (elem = node->ToElement()) && elem)
	{
        impath += elem->Attribute("path");
		imtype = elem->Attribute("type");
	}

    for( node = root->FirstChild( "type" ); node; node = node->NextSibling( "type" ) )
    {
        elem = node->ToElement();
        if (!elem) 
			return false;

		RecMLFBmpType tp;
		bool vflip = false;
     
		if ( elem->Attribute("label"))
			tp.m_label = elem->Attribute("label");      	
		//if ( elem->Attribute("width"))
		//	tp.m_width = atoi(elem->Attribute("width"));
		if ( elem->Attribute("offset"))
			tp.m_bmpoffset = atoi(elem->Attribute("offset"));
		if ( elem->Attribute("flip"))
			tp.m_flip = true; //= atoi(elem->Attribute("flip"));
		if ( elem->Attribute("vflip"))
			vflip = true;
					
		wxString imfile = impath + tp.m_label + "." + imtype;
		if ( wxFileExists( imfile ) )
		{
			wxImage image;
			image.LoadFile( imfile );
			tp.m_bitmap = wxBitmap( image );
	
			if ( tp.m_flip )
			{
				wxImage img = tp.m_bitmap.ConvertToImage();
				tp.m_flip_bitmap = wxBitmap( img.Mirror( false ).Mirror( true ) );
			}
			else if ( vflip )
			{
				tp.m_flip = true;
				wxImage img = tp.m_bitmap.ConvertToImage();
				tp.m_flip_bitmap = wxBitmap( img.Mirror( false ) );
			}
			
		}
		else
			wxLogWarning( _("Bitmap file '%s' is missing"), imfile.c_str() );

        m_types.Add( tp );
    }
    
    return true;
}

/*
int RecMLFBmpTypes::GetWidth( MusMLFSymbol *symbol )
{
	if ( !symbol )
		return 0;

	wxString label = symbol->GetLabelType();
	for(int i = 0; i < (int)m_bitmap_types.GetCount(); i++)
		if (m_bitmap_types[i].m_label == label)
			return m_bitmap_types[i].m_width;

	return 0;
}
*/

RecMLFBmpType *RecMLFBmpTypes::GetType( MusMLFSymbol *symbol )
{
	if ( !symbol )
		return NULL;

	wxString label = symbol->GetLabelType();
	for(int i = 0; i < (int)m_types.GetCount(); i++)
		if (m_types[i].m_label == label)
			return &m_types[i];

	return NULL;
}


//----------------------------------------------------------------------------
// RecMLFSymbolBmp
//----------------------------------------------------------------------------

RecMLFSymbolBmp::RecMLFSymbolBmp( ) :
	MusMLFSymbol()
{
	m_type = 0;
	m_value = 0;
	m_pitch = 0;
	m_oct = 0;
	m_flag = 0;
	m_position = 0;

	m_isCurrent = false;
}

void RecMLFSymbolBmp::SetValue( char type, wxString subtype, int position, int value, char pitch, int oct, int flag )
{
	m_type = type;
	m_subtype = subtype;
	m_position = position;
	m_value = value;
	m_pitch = pitch;
	m_oct = oct;
	m_flag = flag;

	int noffset = 0;
	switch (pitch)
	{
	case ('C'): break;
	case ('D'):	noffset = 1; break;
	case ('E'):	noffset = 2; break;
	case ('F'):	noffset = 3; break;
	case ('G'):	noffset = 4; break;
	case ('A'):	noffset = 5; break;
	case ('B'):	noffset = 6; break;
	}

	m_voffset = (( oct - 4 ) * 7 ) + noffset;

	if (m_type == TYPE_KEY)
	{
		m_voffset = 0;
		wxString key = m_subtype;
		key << m_value;
		if (key == "S2") m_voffset = 2;
		else if (key == "S1") m_voffset = 0;
		else if (key == "S8") m_voffset = 2;
		else if (key == "PITCH_F") m_voffset = 8;
		else if (key == "PITCH_E") m_voffset = 6;
		else if (key == "PITCH_D") m_voffset = 4;
		else if (key == "U1") m_voffset = 0;
		else if (key == "U2") m_voffset = 2;
		else if (key == "U3") m_voffset = 4;
		else if (key == "U4") m_voffset = 6;
		else if (key == "U5") m_voffset = 8;
	}
	else if (m_type == TYPE_MESURE)
	{
		m_voffset = 4;
	}
	else if (m_type == TYPE_SYMBOLE)
	{
		m_voffset = 0;
	}
}


wxString RecMLFSymbolBmp::GetLabel( )
{
	wxString label = this->GetLabelType() << " " << m_voffset;
	return label;
}

//----------------------------------------------------------------------------
// RecMLFBmp
//----------------------------------------------------------------------------

RecMLFBmp::RecMLFBmp( Doc *file, wxString filename, wxString model_symbol_name ) :
    MusMLFOutput( file, filename, NULL, model_symbol_name )
{
	m_impage = NULL;
}

RecMLFBmp::~RecMLFBmp()
{
}

// specific

void RecMLFBmp::StartLabel( )
{
	m_symbols.Clear();
	m_currentX = -1;
	m_currentWidth = -1;
}

wxBitmap RecMLFBmp::GenerateBitmap( ImStaff *imstaff, Layer *musLayer, int currentElementNo )
{
	int mn, mx;
	int bx, by;
	imstaff->GetMinMax( &mn, &mx );
	if ( mx-mn < 1 )
	{
		wxBitmap empty;
		return empty;	
	}
	wxBitmap bmp( mx-mn, STAFF_HEIGHT );

	// fill symbol array with musStaff elements
	m_symbols.Clear();
    //MusMLFOutput::GetUt1( musLayer );
    m_layer = musLayer;
    WriteLayer( musLayer, currentElementNo );
    //MusMLFInput::GetNotUt1( musLayer );

	wxMemoryDC memDC;
	memDC.SelectObject(bmp);
	memDC.SetBackground( *wxWHITE_BRUSH );
	memDC.Clear();
    
	for (int i = 0; i < (int)m_symbols.GetCount(); i++ )
	{
		wxString symbol;
		int pos = (&m_symbols[i])->GetPosition();
		MusMLFSymbol *mlf = &m_symbols[i];
		RecMLFBmpType *tp = m_bitmap_types.GetType( mlf );
		if ( tp )
		{
			imstaff->GetXandPos( pos, &bx, &by );
			by += (STAFF_HEIGHT / 2) + 4 * STAFF_INTERLIGN; // UT1
			by -= ((RecMLFSymbolBmp*)mlf)->GetVOffset() * STAFF_INTERLIGN;
			by -= tp->m_bitmap.GetHeight(); // top
			by += tp->m_bmpoffset; // offset de l'image

			wxMemoryDC symDC;
			wxBitmap current;
			bool flip = ( tp->m_flip && (((RecMLFSymbolBmp*)mlf)->GetVOffset() > 4 ));
			if ( m_symbols[i].m_flag & NOTE_STEM )
				flip = !flip;
			if ( flip )
			{
				by += tp->m_bitmap.GetHeight(); // top
				by -= 2*tp->m_bmpoffset; // offset de l'image
				current = tp->m_flip_bitmap;
			}
			else
				current = tp->m_bitmap;
			if (((RecMLFSymbolBmp*)mlf)->IsCurrent())
			{
				wxImage img = current.ConvertToImage();
				img = img.ConvertToMono( 0, 255, 255 );
				img.Replace( 255, 255, 255, 255, 0, 0 );
				img.Replace( 0, 0, 0, 255, 255, 255 );
				current = wxBitmap( img );
				m_currentX = bx;
				m_currentWidth = current.GetWidth() + 5; // soyons genereux !!!
			}

			symDC.SelectObject(current);
			memDC.Blit(bx, by, tp->m_bitmap.GetWidth(), tp->m_bitmap.GetHeight(),
				&symDC, 0, 0 );
			symDC.SelectObject( wxNullBitmap );
		}
		//else
		//	wxLogWarning( _("Cant' find type ") );
	}
	//m_symbols.Clear();

	memDC.SelectObject( wxNullBitmap );
	return bmp;
	//wxString file = m_filename;
	//file << m_staff_i << ".bmp";
	//bmp.SaveFile( file, wxBITMAP_TYPE_BMP );
}

/*
void RecMLFBmp::EndLabel( )
{
	wxASSERT_MSG( m_impage , "ImPage cannot be NULL ");

	if ( m_staff_i > (int)m_impage->m_staves.GetCount() - 1 )
		return;

	int mn, mx;
	int bx, by;
	m_impage->m_staves[m_staff_i].GetMinMax( &mn, &mx );
	wxBitmap bmp( mx-mn, STAFF_HEIGHT );

	wxMemoryDC memDC;
	memDC.SelectObject(bmp);
	memDC.SetBackground( *wxWHITE_BRUSH );
	memDC.Clear();
    
	for (int i = 0; i < (int)m_symbols.GetCount(); i++ )
	{
		wxString symbol;
		int pos = (&m_symbols[i])->GetPosition();
		MusMLFSymbol *mlf = &m_symbols[i];
		RecMLFBmpType *tp = m_bitmap_types.GetType( mlf );
		if ( tp )
		{
			m_impage->m_staves[m_staff_i].GetXandPos( pos, &bx, &by );
			by += (STAFF_HEIGHT / 2) + 4 * STAFF_INTERLIGN; // UT1
			by -= ((RecMLFSymbolBmp*)mlf)->GetVOffset() * STAFF_INTERLIGN;
			by -= tp->m_bitmap.GetHeight(); // top
			by += tp->m_bmpoffset; // offset de l'image

			wxMemoryDC symDC;
			if ( tp->m_flip && (((RecMLFSymbolBmp*)mlf)->GetVOffset() > 4 ))
			{
				by += tp->m_bitmap.GetHeight(); // top
				by -= 2*tp->m_bmpoffset; // offset de l'image
				symDC.SelectObject(tp->m_flip_bitmap);
			}
			else
				symDC.SelectObject(tp->m_bitmap);
			memDC.Blit(bx, by, tp->m_bitmap.GetWidth(), tp->m_bitmap.GetHeight(),
				&symDC, 0, 0 );
			symDC.SelectObject( wxNullBitmap );
		}
		else
			wxLogWarning( _("Cant' find type ") );
	}
	m_symbols.Clear();

	memDC.SelectObject( wxNullBitmap );
	wxString file = m_filename;
	file << m_staff_i << ".bmp";
	bmp.SaveFile( file, wxBITMAP_TYPE_BMP );
}
*/

/*
bool RecMLFBmp::GenerateBitmaps( ImPage *impage )
{
	m_impage = impage;

    Page *page = NULL;

    for (m_page_i = 0; m_page_i < m_file->m_parameters.nbpage; m_page_i++ )
    {
        page = &m_file->m_pages.Item(m_page_i);
        WritePage( page );
    }

    return true;
}
*/

/*
bool RecMLFBmp::WritePage( const Page *page )
{
    m_staff = NULL;
    for (m_staff_i = 0; m_staff_i < page->GetStaffCount(); m_staff_i++) 
    {
        Staff *staff = &page->m_staves[m_staff_i];
		Staff *ut1_staff = MusMLFOutput::GetUt1( staff );
        WriteStaff( ut1_staff );
		delete ut1_staff;
    }
    return true;

}
*/

bool RecMLFBmp::WriteLayer( const Layer *layer, int currentElementNo )
{

	if (layer->GetElementCount() == 0)
		return true;
    
    int k;

	StartLabel();
	bool ok;

    for (k = 0;k < layer->GetElementCount(); k++ )
    {
        LayerElement *element = (LayerElement*)layer->m_children[k];
        // we could write all of the in one method, left over from version < 2.0.0
        if ( element->IsNote() || element->IsRest() || element->IsSymbol( SYMBOL_CUSTOS) )
        {
            ok = WriteNote( element );
        }
        else
        {
            ok = WriteSymbol( element );
        }
        if ( ok && ( currentElementNo == (signed int)k ) )
			((RecMLFSymbolBmp*)&m_symbols.Last())->SetCurrent();

    }
	//EndLabel();
    return true;
}


#endif //AX_RECOGNITION


