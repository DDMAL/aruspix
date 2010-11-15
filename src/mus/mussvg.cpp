/////////////////////////////////////////////////////////////////////////////
// Name:        mussvg.cpp (original name: dcsvg.cpp)
// Purpose:     SVG sample
// Author:      Chris Elliott
// Modified by: Laurent Pugin
// RCS-ID:      $Id: dcsvg.cpp 30993 2004-12-13 20:10:59Z ABX $
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "mussvg.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "mussvg.h"
#include "musdef.h"

#include "app/axapp.h"

#include "wx/image.h"

#define wxSVG_DEBUG FALSE
// or TRUE to see the calls being executed
#define newline    wxString(wxT("\n"))
#define space      wxString(wxT(" "))
#define semicolon  wxString(wxT(";"))
#define     wx_round(a)    (int)((a)+.5)

#ifdef __BORLANDC__
#pragma warn -rch
#pragma warn -ccc
#endif

// BAD EQUATION TO ADJUST THE FONT SIZE/POSITION IN THE SVG
// Size factor seems to be OK for music and lyric font
// Position is not OK; kind of works with default size
#define SVG_TEXT_DY_X 0.565
#define SVG_TEXT_DY_Y 0.0
#define SVG_TEXT_DX 0
#define SVG_TEXT_SCALE 0.75

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; } ;

wxString wxColStr ( wxColour c )
{
    unsigned char r, g, b ;
    r = c.Red ();
    g = c.Green ();
    b = c. Blue ();

    // possible Unicode bug here
    wxString s = wxDecToHex(r) + wxDecToHex(g) + wxDecToHex(b) ;
    return s ;
}


wxString wxBrushString ( wxColour c, int style )
{
    wxString s = wxT("fill:#") + wxColStr (c)  + semicolon + space ;
    switch ( style )
    {
        case wxSOLID :
            s = s + wxT("fill-opacity:1.0; ");
            break ;
        case wxTRANSPARENT:
            s = s + wxT("fill-opacity:0.0; ");
            break ;

        default :
            s = s + wxT("fill-opacity:1.0; "); // solid brush as default
            //wxASSERT_MSG(FALSE, wxT("MusSVGFileDC::Requested Brush Style not available")) ;

    }
    s = s + newline ;
    return s ;
}


void MusSVGFileDC::Init (wxString f, int Width, int Height, float dpi)

{
    //set up things first  wxDCBase does all this?
    m_width = Width ;
    m_height = Height ;

    m_clipping = FALSE;
    m_OK = TRUE;

    m_mm_to_pix_x = dpi/25.4;
    m_mm_to_pix_y = dpi/25.4;

    m_signX = m_signY = 1;

    m_userScaleX = m_userScaleY = 
        m_deviceOriginX = m_deviceOriginY = 0;

    m_OriginX = m_OriginY = 0;
    m_logicalOriginX = m_logicalOriginY = 0;
    m_logicalScaleX = m_logicalScaleY = 0 ;
    m_scaleX = m_scaleY = 1.0 ;

    m_logicalFunction = wxCOPY;
    m_backgroundMode = wxTRANSPARENT;
    m_mappingMode = wxMM_TEXT;

    m_backgroundBrush = *wxTRANSPARENT_BRUSH;
    m_textForegroundColour = *wxBLACK;
    m_textBackgroundColour = *wxWHITE;
    m_colour = wxColourDisplay();

    m_pen   = *wxBLACK_PEN;
    m_font  = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_graphics_changed = TRUE ;
    // no graphic yet
    m_graphic_exists = FALSE;
    
    m_leipzig_glyphs.clear();
    
#if wxMAC_USE_CORE_GRAPHICS
    // the cg context we got when starting the page isn't valid anymore, so replace it
    SetGraphicsContext( wxGraphicsContext::Create() );
#endif

    ////////////////////code here
    
    m_flushed = false;
    m_filename = f ;
    
    m_outfile = new wxMemoryOutputStream() ;
    m_OK = true; //m_outfile->Ok ();
    if (m_OK)
    {
        m_sub_images = 0 ;
    }
}
    
    
    
void MusSVGFileDC::Flush() {

    if (m_flushed) {
        return;
    }

    wxFileOutputStream outfile(m_filename);
    if (!outfile.Ok()) {
        return;
    }
    
    int i;
    wxString s ;
    s.Printf ( wxT("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n<svg width=\"%dpx\" height=\"%dpx\""),  
            m_width, m_height);
    s += wxT(" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\"  xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n");
    const wxWX2MBbuf buf = s.mb_str(wxConvUTF8);
    outfile.Write(buf, strlen((const char *)buf));
    
    if ( m_graphic_exists ) {
        write("</g>\n");
    }
    write("</svg>\n") ;
    
    // insert the def for the leipzig glyphes if any
    if (m_leipzig_glyphs.Count() > 0)
    {
        s = "<defs>\n";
        outfile.Write(s, strlen(s));
        for (i = 0; i < (int)m_leipzig_glyphs.Count(); i++) {
            wxTransferFileToStream( wxGetApp().m_resourcesPath + "/svg/" + m_leipzig_glyphs[i] + ".xml", outfile );
        }
        s = "</defs>\n";
        outfile.Write(s, strlen(s));
    
    }
    wxMemoryInputStream input(*m_outfile);
    input.SeekI(0);
    //input.Read( *m_outfile );
    outfile.Write( input );
    m_flushed = true;
}


// constructors
MusSVGFileDC::MusSVGFileDC (wxString f)
{
    // quarter 640x480 screen display at 72 dpi
    Init (f,320,240,72.0);
};

MusSVGFileDC::MusSVGFileDC (wxString f, int Width, int Height)
{
    Init (f,Width,Height,72.0);
};

MusSVGFileDC::MusSVGFileDC (wxString f, int Width, int Height, float dpi)
{
    Init (f,Width,Height,dpi);
};

MusSVGFileDC::~MusSVGFileDC()
{
    if (!m_flushed) {
        Flush();
    }
    delete m_outfile ;
}

bool MusSVGFileDC::ConcatFile ( wxString filename )
{
    return wxTransferFileToStream( filename, *m_outfile );
}


//////////////////////////////////////////////////////////////////////////////////////////

void MusSVGFileDC::DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    if (m_graphics_changed) NewGraphics ();
    wxString s ;
    s.Printf ( wxT("<path d=\"M%d %d L%d %d\" /> \n"), x1,y1,x2,y2 );
    if (m_OK)
    {
        write(s);
    }
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DrawLine Call executed")) ;
    CalcBoundingBox(x1, y1) ;
    CalcBoundingBox(x2, y2) ;
    return;
};

void MusSVGFileDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset , wxCoord yoffset )
{
    for ( int i = 1; i < n ; i++ )
    {
        DoDrawLine ( points [i-1].x + xoffset, points [i-1].y + yoffset,
            points [ i ].x + xoffset, points [ i ].y + yoffset ) ;
    }
}


void MusSVGFileDC::DoDrawPoint (wxCoord x1, wxCoord y1)
{
    wxString s;
    if (m_graphics_changed) NewGraphics ();
    s = wxT("<g style = \"stroke-linecap:round;\" > ") + newline ;
    write(s);
    DrawLine ( x1,y1,x1,y1 );
    s = wxT("</g>");
    write(s);
}


void MusSVGFileDC::DoDrawCheckMark(wxCoord x1, wxCoord y1, wxCoord width, wxCoord height)
{
    wxDCBase::DoDrawCheckMark (x1,y1,width,height) ;
}


void MusSVGFileDC::DoDrawText(const wxString& text, wxCoord x1, wxCoord y1)
{
    DoDrawRotatedText(text, x1,y1,0.0);
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DrawText Call executed")) ;
}


void MusSVGFileDC::DoDrawRotatedText(const wxString& sText, wxCoord x, wxCoord y, double angle)
{
    //known bug; if the font is drawn in a scaled DC, it will not behave exactly as wxMSW
    if (m_graphics_changed) NewGraphics ();
    wxString s, sTmp;

    // calculate bounding box
    wxCoord w, h, desc;
    //DoGetTextExtent(sText, &w, &h, &desc);
    w = h = desc = 0;

    double rad = DegToRad(angle);

    // wxT("upper left") and wxT("upper right")
    CalcBoundingBox(x, y);
    CalcBoundingBox((wxCoord)(x + w*cos(rad)), (wxCoord)(y - h*sin(rad)));

    // wxT("bottom left") and wxT("bottom right")
    x += (wxCoord)(h*sin(rad));
    y += (wxCoord)(h*cos(rad));
    CalcBoundingBox(x, y);
    CalcBoundingBox((wxCoord)(x + h*sin(rad)), (wxCoord)(y + h*cos(rad)));

    if (m_backgroundMode == wxSOLID)
    {
        // draw background first
        // just like DoDrawRectangle except we pass the text color to it and set the border to a 1 pixel wide text background

        wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::Draw Rotated Text Call plotting text background")) ;
        sTmp.Printf ( wxT(" <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"  "), x,y+desc-h, w, h );
        s = sTmp + wxT("style=\"fill:#") + wxColStr (m_textBackgroundColour) + wxT("; ") ;
        s = s + wxT("stroke-width:1; stroke:#") + wxColStr (m_textBackgroundColour) + wxT("; ") ;
        sTmp.Printf ( wxT("\" transform=\"rotate( %.2g %d %d )  \">"), -angle, x,y ) ;
        s = s + sTmp + newline ;
        write(s);
    }
    
    sTmp = m_font.GetFaceName () ;
    if (sTmp == SVG_LEIPZIG_FONT) {
        write(s);
        DoDrawLeipzigText( sText, x + SVG_TEXT_DX, y + ((int)((double)m_font.GetPointSize() + SVG_TEXT_DY_Y) * SVG_TEXT_DY_X), angle );
    }
    else
    {    
        //now do the text itself
        s.Printf (wxT(" <text x=\"%d\" y=\"%d\" dx=\"%d\" dy=\"%d\" "),x,y, SVG_TEXT_DX, (int)((double)m_font.GetPointSize() * SVG_TEXT_SCALE + SVG_TEXT_DY_Y) );

        // For some reason, some browsers (e.g., Chrome) do not like spaces or dots in font names...
        sTmp.Replace(" ", "");
        sTmp.Replace(".", "");
        if (sTmp.Len () > 0)  s = s + wxT("style=\"font-family: '") + sTmp + wxT("'; ");
        else s = s + wxT("style=\" ") ;

        wxString fontweights [3] = { wxT("normal"), wxT("lighter"), wxT("bold") };
        s = s + wxT("font-weight:") + fontweights[m_font.GetWeight() - wxNORMAL] + semicolon + space;

        wxString fontstyles [5] = { wxT("normal"), wxT("style error"), wxT("style error"), wxT("italic"), wxT("oblique") };
        s = s + wxT("font-style:") + fontstyles[m_font.GetStyle() - wxNORMAL] + semicolon  + space;

        sTmp.Printf (wxT("font-size:%dpt; "), (int)((double)m_font.GetPointSize() * SVG_TEXT_SCALE) );
        s = s + sTmp ;
        /* remove the color information because normaly already in the graphic element */
        //s = s + wxT("fill:#") + wxColStr (m_textForegroundColour) + wxT("; stroke:#") + wxColStr (m_textForegroundColour) + wxT("; ") ;
        sTmp.Printf ( wxT("stroke-width:0;\"  transform=\"rotate( %.2g %d %d )  \" >"),  -angle, x,y ) ;
        s = s + sTmp + sText + wxT("</text> ") + newline ;
        if (m_OK)
        {
            write(s);
        }
    }
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DrawRotatedText Call executed")) ;

}

void MusSVGFileDC::DoDrawLeipzigText(const wxString& sText, wxCoord x, wxCoord y, double angle)
{
    wxString glyph;
    char c = (char)sText[0];
    switch (c) {
        /* figures */
        case 48: glyph = "figure_0"; break;
        case 49: glyph = "figure_1"; break;
        case 50: glyph = "figure_2"; break;
        case 51: glyph = "figure_3"; break;
        case 52: glyph = "figure_4"; break;
        case 53: glyph = "figure_5"; break;
        case 54: glyph = "figure_6"; break;
        case 55: glyph = "figure_7"; break;
        case 56: glyph = "figure_8"; break;
        case 57: glyph = "figure_9"; break;
        /* clef */
        case sSOL: glyph = "clef_G"; break;
        case sFA: glyph = "clef_F"; break;
        case sUT: glyph = "clef_C"; break;
        case sSOL_OCT: glyph = "clef_G8"; break;
        case sSOL + N_FONT: glyph = "clef_G_mensural"; break;
        case sFA + N_FONT: glyph = "clef_F_mensural"; break;
        case sUT + N_FONT: glyph = "clef_C_mensural"; break;
        case sSOL_OCT + N_FONT: glyph = "clef_G_chiavette"; break;
        /* alterations */
        case sDIESE: glyph = "alt_sharp"; break;
        case sBECARRE: glyph = "alt_natural"; break;
        case sBEMOL: glyph = "alt_flat"; break;
        case sDDIESE: glyph = "alt_double_sharp"; break;
        case sDIESE + N_FONT: glyph = "alt_sharp_mensural"; break;
        case sBECARRE + N_FONT: glyph = "alt_natural_mensural"; break;
        case sBEMOL + N_FONT: glyph = "alt_flat_mensural"; break;
        case sDDIESE + N_FONT: glyph = "alt_double_sharp_mensural"; break;
        /* rests */
        case sSilNOIRE: glyph = "rest_4"; break;
        case sSilNOIRE + 1: glyph = "rest_8"; break;
        case sSilNOIRE + 2: glyph = "rest_16"; break;
        case sSilNOIRE + 3: glyph = "rest_32"; break;
        case sSilNOIRE + 4: glyph = "rest_64"; break;
        case sSilNOIRE + 5: glyph = "rest_128"; break;
        case sSilNOIRE + N_FONT: glyph = "rest_4_mensural"; break;
        case sSilNOIRE + 1 + N_FONT: glyph = "rest_8_mensural"; break;
        case sSilNOIRE + 2 + N_FONT: glyph = "rest_16_mensural"; break;
        case sSilNOIRE + 3 + N_FONT: glyph = "rest_32_mensural"; break;
        case sSilNOIRE + 4 + N_FONT: glyph = "rest_64_mensural"; break;
        case sSilNOIRE + 5 + N_FONT: glyph = "rest_128_mensural"; break;
        /* note heads */
        case sRONDE_B: glyph = "head_whole"; break;
        case sRONDE_N: glyph = "head_whole_fill"; break;
        case sBLANCHE: glyph = "head_half"; break;
        case sNOIRE: glyph = "head_quarter"; break;
        case sRONDE_B + N_FONT: glyph = "head_whole_diamond"; break;
        case sRONDE_N + N_FONT: glyph = "head_whole_filldiamond"; break;
        case sBLANCHE + N_FONT: glyph = "head_half_diamond"; break;
        case sNOIRE + N_FONT: glyph = "head_quarter_filldiamond"; break;
        /* slashes */
        case sCROCHET_H: glyph = "slash_up"; break;
        case sCROCHET_B: glyph = "slash_down"; break;
        case sCROCHET_H + N_FONT: glyph = "slash_up_mensural"; break;
        case sCROCHET_B + N_FONT: glyph = "slash_down_mensural"; break;
        /* ornaments */
        case 35: glyph = "orn_mordent"; break;
        /* todo */
        default: glyph = "unknown";
    }
    
    if (m_leipzig_glyphs.Index(glyph) == wxNOT_FOUND) {
        m_leipzig_glyphs.Add(glyph);
    }
    
    wxString s;
    s.Printf ( wxT("<use xlink:href=\"#%s\" transform=\"translate(%d, %d) scale(%f, %f)\"/>"),
               glyph.c_str(), x, y, ((double)(m_font.GetPointSize() / 2)) * 0.001, ((double)(m_font.GetPointSize() / 2)) * -0.001 );
    write(s);    
}


void MusSVGFileDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    DoDrawRoundedRectangle(x, y, width, height, 0)  ;
}


void MusSVGFileDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius )

{
    if (m_graphics_changed) NewGraphics ();
    wxString s ;
    
    // negative heights or widths are not allowed in SVG
    if ( height < 0 ) {
        height = -height;
        y -= height;
    }
     if ( width < 0 ) {
        width = -width;
        x -= width;
    }   

    s.Printf ( wxT(" <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" rx=\"%.2g\" "),
               x, y, width, height, radius );

    s = s + wxT(" /> ") + newline ;
    write(s);

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoDrawRoundedRectangle Call executed")) ;
    CalcBoundingBox(x, y) ;
    CalcBoundingBox(x + width, y + height) ;

}


void MusSVGFileDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset,int fillStyle)
{
    if (m_graphics_changed) NewGraphics ();
    wxString s, sTmp ;
    s = wxT("<polygon style=\"") ;
    if ( fillStyle == wxODDEVEN_RULE )
        s = s + wxT("fill-rule:evenodd; ");
    else
        s = s + wxT("fill-rule:nonzero; ");

    s = s  + wxT("\" \npoints=\"") ;

    for (int i = 0; i < n;  i++)
    {
        sTmp.Printf ( wxT("%d,%d"), points [i].x+xoffset, points[i].y+yoffset );
        s = s + sTmp + newline ;
        CalcBoundingBox ( points [i].x+xoffset, points[i].y+yoffset);
    }
    s = s + wxT("\" /> ") ;
    s = s + newline ;
    write(s);

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoDrawPolygon Call executed")) ;
}


void MusSVGFileDC::DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height)

{
    if (m_graphics_changed) NewGraphics ();

    int rh = height /2 ;
    int rw = width  /2 ;

    wxString s;
    s.Printf ( wxT("<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" "), x+rw,y+rh, rw, rh );
    s = s + wxT(" /> ") + newline ;

    write(s);

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoDrawEllipse Call executed")) ;
    CalcBoundingBox(x, y) ;
    CalcBoundingBox(x + width, y + height) ;
}


void MusSVGFileDC::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
    /* Draws an arc of a circle, centred on (xc, yc), with starting point
    (x1, y1) and ending at (x2, y2). The current pen is used for the outline
    and the current brush for filling the shape.

    The arc is drawn in an anticlockwise direction from the start point to
    the end point.

    Might be better described as Pie drawing */

    if (m_graphics_changed) NewGraphics ();
    wxString s ;

    // we need the radius of the circle which has two estimates
    double r1 = sqrt ( double( (x1-xc)*(x1-xc) ) + double( (y1-yc)*(y1-yc) ) );
    double r2 = sqrt ( double( (x2-xc)*(x2-xc) ) + double( (y2-yc)*(y2-yc) ) );

    wxASSERT_MSG( (fabs ( r2-r1 ) <= 3), wxT("MusSVGFileDC::DoDrawArc Error in getting radii of circle")) ;
    if ( fabs ( r2-r1 ) > 3 )    //pixels
    {
        s = wxT("/*- MusSVGFileDC::DoDrawArc Error in getting radii of circle */ \n") ;
        write(s);
    }

    double theta1 = atan2((double)(yc-y1),(double)(x1-xc));
    if ( theta1 < 0 ) theta1 = theta1 + M_PI * 2;
    double theta2 = atan2((double)(yc-y2), (double)(x2-xc));
    if ( theta2 < 0 ) theta2 = theta2 + M_PI * 2;
    if ( theta2 < theta1 ) theta2 = theta2 + M_PI *2 ;

    int fArc  ;                  // flag for large or small arc 0 means less than 180 degrees
    if ( fabs(theta2 - theta1) > M_PI ) fArc = 1; else fArc = 0 ;

    int fSweep = 0 ;             // flag for sweep always 0

    s.Printf ( wxT("<path d=\"M%d %d A%.2g %.2g 0.0 %d %d %d %d L%d %d z "),
        x1,y1, r1, r2, fArc, fSweep, x2, y2, xc, yc );

    // the z means close the path and fill
    s = s + wxT(" \" /> ") + newline ;


    if (m_OK)
    {
        write(s);
    }

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoDrawArc Call executed")) ;
}


void MusSVGFileDC::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
    /*
    Draws an arc of an ellipse. The current pen is used for drawing the arc
    and the current brush is used for drawing the pie. This function is
    currently only available for X window and PostScript device contexts.

    x and y specify the x and y coordinates of the upper-left corner of the
    rectangle that contains the ellipse.

    width and height specify the width and height of the rectangle that
    contains the ellipse.

    start and end specify the start and end of the arc relative to the
    three-o'clock position from the center of the rectangle. Angles are
    specified in degrees (360 is a complete circle). Positive values mean
    counter-clockwise motion. If start is equal to end, a complete ellipse
    will be drawn. */

    //known bug: SVG draws with the current pen along the radii, but this does not happen in wxMSW

    if (m_graphics_changed) NewGraphics ();

    wxString s ;
    //radius
    double rx = w / 2 ;
    double ry = h / 2 ;
    // center
    double xc = x + rx ;
    double yc = y + ry ;

    double xs, ys, xe, ye ;
    xs = xc + rx * cos (DegToRad(sa)) ;
    xe = xc + rx * cos (DegToRad(ea)) ;
    ys = yc - ry * sin (DegToRad(sa)) ;
    ye = yc - ry * sin (DegToRad(ea)) ;

    ///now same as circle arc...

    double theta1 = atan2(ys-yc, xs-xc);
    double theta2 = atan2(ye-yc, xe-xc);

    int fArc  ;                  // flag for large or small arc 0 means less than 180 degrees
    if ( (theta2 - theta1) > 0 ) fArc = 1; else fArc = 0 ;

    int fSweep ;
    if ( fabs(theta2 - theta1) > M_PI) fSweep = 1; else fSweep = 0 ;

    // this version closes the arc
    //s.Printf ( wxT("<path d=\"M%d %d A%d %d 0.0 %d %d  %d %d L %d %d z "),
    //    int(xs), int(ys), int(rx), int(ry),
    //    fArc, fSweep, int(xe), int(ye), int(xc), int(yc)  );

    s.Printf ( wxT("<path d=\"M%d %d A%d %d 0.0 %d %d  %d %d "),
        int(xs), int(ys), int(rx), int(ry),
        fArc, fSweep, int(xe), int(ye)  );

    s = s + wxT(" \" /> ") + newline ;

    if (m_OK)
    {
        write(s);
    }

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoDrawEllipticArc Call executed")) ;
}


void MusSVGFileDC::DoGetTextExtent(const wxString& string, wxCoord *w, wxCoord *h, wxCoord *descent , wxCoord *externalLeading , wxFont *font) const

{
    wxScreenDC sDC ;

    sDC.SetFont (m_font);
    if ( font != NULL ) sDC.SetFont ( *font );
    sDC.GetTextExtent(string, w,  h, descent, externalLeading );
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::GetTextExtent Call executed")) ;
}


wxCoord MusSVGFileDC::GetCharHeight() const

{
    wxScreenDC sDC ;
    sDC.SetFont (m_font);

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::GetCharHeight Call executing")) ;
    return ( sDC.GetCharHeight() );

}


wxCoord MusSVGFileDC::GetCharWidth() const
{
    wxScreenDC sDC ;
    sDC.SetFont (m_font);

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::GetCharWidth Call executing")) ;
    return ( sDC.GetCharWidth() ) ;

}


/// Set Functions /////////////////////////////////////////////////////////////////
void MusSVGFileDC::SetBackground( const wxBrush &brush )
{

    m_backgroundBrush = brush;
    return;
}


void MusSVGFileDC::SetBackgroundMode( int mode )
{
    m_backgroundMode = mode;
    return;
}


void MusSVGFileDC::SetBrush(const wxBrush& brush)

{
    m_brush = brush ;

    m_graphics_changed = TRUE ;
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::SetBrush Call executed")) ;
}


void MusSVGFileDC::SetPen(const wxPen& pen)
{
    // width, color, ends, joins : currently implemented
    // dashes, stipple :  not implemented
    m_pen = pen ;

    m_graphics_changed = TRUE ;
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::SetPen Call executed")) ;
}

void MusSVGFileDC::NewGraphics ()
{

    int w = m_pen.GetWidth ();
    wxColour c = m_pen.GetColour () ;

    wxString s, sEndGraphic, sBrush, sPenCap, sPenJoin, sPenStyle, sLast, sWarn;
    
    if ( m_graphic_exists ) {
        sEndGraphic = wxT("</g>\n");
    } else {
        m_graphic_exists = TRUE;
    }
    

    sBrush = wxT("<g style=\"") + wxBrushString ( m_brush.GetColour (), m_brush.GetStyle () )
             + wxT("  stroke:#") + wxColStr (c) + wxT("; ") ;

    switch ( m_pen.GetCap () )
    {
        case  wxCAP_PROJECTING :
            sPenCap = wxT("stroke-linecap:square; ") ;
            break ;
        case  wxCAP_BUTT :
            sPenCap = wxT("stroke-linecap:butt; ") ;
            break ;
        case    wxCAP_ROUND :
        default :
            sPenCap = wxT("stroke-linecap:round; ") ;
    };
    switch ( m_pen.GetJoin () )
    {
        case  wxJOIN_BEVEL :
            sPenJoin = wxT("stroke-linejoin:bevel; ") ;
            break ;
        case  wxJOIN_MITER :
            sPenJoin = wxT("stroke-linejoin:miter; ") ;
            break ;
        case    wxJOIN_ROUND :
        default :
            sPenJoin = wxT("stroke-linejoin:round; ") ;
    };

    switch ( m_pen.GetStyle () )
    {
        case  wxSOLID :
            sPenStyle = wxT("stroke-opacity:1.0; stroke-opacity:1.0; ") ;
            break ;
        case  wxTRANSPARENT :
            sPenStyle = wxT("stroke-opacity:0.0; stroke-opacity:0.0; ") ;
            break ;
        default : {}
            //wxASSERT_MSG(FALSE, wxT("MusSVGFileDC::SetPen Call called to set a Style which is not available")) ;
            //sWarn = sWarn + wxT("/*- MusSVGFileDC::SetPen Call called to set a Style which is not available */ \n") ;
    }

    sLast.Printf (   wxT("stroke-width:%d\" \n   transform=\"translate(%.2g %.2g) scale(%.2g %.2g)\">"),
                  w, m_OriginX, m_OriginY, m_scaleX, m_scaleY  );

    s = sEndGraphic + sBrush + sPenCap + sPenJoin + sPenStyle + sLast + newline + sWarn;
    write(s);
    m_graphics_changed = FALSE ;
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::NewGraphics Call executed")) ;
}


void MusSVGFileDC::SetFont(const wxFont& font)

{
    m_font = font ;

    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::SetFont Call executed")) ;
}


void MusSVGFileDC::ComputeScaleAndOrigin()
{
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
    m_OriginX = m_logicalOriginX * m_logicalScaleX + m_deviceOriginX ;
    m_OriginY = m_logicalOriginY * m_logicalScaleY + m_deviceOriginY ;
    m_graphics_changed = TRUE;
}


int MusSVGFileDC::GetMapMode()
{
    return m_mappingMode ;
}


void MusSVGFileDC::SetMapMode( int mode )
{
    switch (mode)
    {
        case wxMM_TWIPS:
            SetLogicalScale( twips2mm*m_mm_to_pix_x, twips2mm*m_mm_to_pix_y );
            break;
        case wxMM_POINTS:
            SetLogicalScale( pt2mm*m_mm_to_pix_x, pt2mm*m_mm_to_pix_y );
            break;
        case wxMM_METRIC:
            SetLogicalScale( m_mm_to_pix_x, m_mm_to_pix_y );
            break;
        case wxMM_LOMETRIC:
            SetLogicalScale( m_mm_to_pix_x/10.0, m_mm_to_pix_y/10.0 );
            break;
        default:
        case wxMM_TEXT:
            SetLogicalScale( 1.0, 1.0 );
            break;
    }
    m_mappingMode = mode;

    /*  we don't do this mega optimisation
        if (mode != wxMM_TEXT)
        {
            m_needComputeScaleX = TRUE;
            m_needComputeScaleY = TRUE;
        }
    */
}


void MusSVGFileDC::GetUserScale(double *x, double *y) const
{
    *x = m_userScaleX ;
    *y = m_userScaleY ;
}


void MusSVGFileDC::SetUserScale( double x, double y )
{
    // allow negative ? -> no
    m_userScaleX = x;
    m_userScaleY = y;
    ComputeScaleAndOrigin();
}


void MusSVGFileDC::SetLogicalScale( double x, double y )
{
    // allow negative ?
    m_logicalScaleX = x;
    m_logicalScaleY = y;
    ComputeScaleAndOrigin();
}


void MusSVGFileDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    // is this still correct ?
    m_logicalOriginX = x * m_signX;
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}


void MusSVGFileDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    // only wxPostScripDC has m_signX = -1,
    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}


void MusSVGFileDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    // only wxPostScripDC has m_signX = -1,
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp  ? -1 :  1);
    ComputeScaleAndOrigin();
}


// export a bitmap as a raster image in png
bool MusSVGFileDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                         wxDC* source, wxCoord xsrc, wxCoord ysrc,
                         int logicalFunc /*= wxCOPY*/, bool useMask /*= FALSE*/,
                         wxCoord /*xsrcMask = -1*/, wxCoord /*ysrcMask = -1*/)
{
    if (logicalFunc != wxCOPY)
    {
        wxASSERT_MSG(FALSE, wxT("MusSVGFileDC::DoBlit Call requested nonCopy mode; this is not possible")) ;
        return FALSE ;
    }
    if (useMask != FALSE)
    {
        wxASSERT_MSG(FALSE, wxT("MusSVGFileDC::DoBlit Call requested False mask ; this is not possible")) ;
        return FALSE ;
    }
    wxBitmap myBitmap (width, height) ;
    wxMemoryDC memDC;
    memDC.SelectObject( myBitmap );
    memDC.Blit(0, 0, width, height, source, xsrc, ysrc);
    memDC.SelectObject( wxNullBitmap );
    DoDrawBitmap(myBitmap, xdest, ydest);
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoBlit Call executed")) ;
    return FALSE ;
}


void MusSVGFileDC::DoDrawIcon(const class wxIcon & myIcon, wxCoord x, wxCoord y)
{
    wxBitmap myBitmap (myIcon.GetWidth(), myIcon.GetHeight() ) ;
    wxMemoryDC memDC;
    memDC.SelectObject( myBitmap );
    memDC.DrawIcon(myIcon,0,0);
    memDC.SelectObject( wxNullBitmap );
    DoDrawBitmap(myBitmap, x, y);
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoDrawIcon Call executed")) ;
    return ;
}



void MusSVGFileDC::DoDrawBitmap(const class wxBitmap & bmp, wxCoord x, wxCoord y , bool  WXUNUSED(bTransparent) /*=0*/ )
{
    if (m_graphics_changed) NewGraphics ();

    wxString sTmp, s, sPNG ;
    wxImage::AddHandler(new wxPNGHandler);

// create suitable file name
    sTmp.Printf ( wxT("_image%d.png"), m_sub_images);
    sPNG = m_filename.BeforeLast(wxT('.')) + sTmp;
    while (wxFile::Exists(sPNG) )
    {
        m_sub_images ++ ;
        sTmp.Printf ( wxT("_image%d.png"), m_sub_images);
        sPNG = m_filename.BeforeLast(wxT('.')) + sTmp;
    }

//create copy of bitmap (wxGTK doesn't like saving a constant bitmap)
    wxBitmap myBitmap = bmp ;
//save it
    bool bPNG_OK = myBitmap.SaveFile(sPNG,wxBITMAP_TYPE_PNG);

// refrence the bitmap from the SVG doc
    int w = myBitmap.GetWidth();
    int h = myBitmap.GetHeight();
    sTmp.Printf ( wxT(" <image x=\"%d\" y=\"%d\" width=\"%dpx\" height=\"%dpx\" "), x,y,w,h );
    s = s + sTmp ;
    sTmp.Printf ( wxT(" xlink:href=\"%s\"> \n"), sPNG.c_str() );
    s = s + sTmp + wxT("<title>Image from wxSVG</title>  </image>") + newline;

    if (m_OK && bPNG_OK)
    {
        write(s);
    }
    // m_OK = m_outfile->Ok () && bPNG_OK;
    wxASSERT_MSG(!wxSVG_DEBUG, wxT("MusSVGFileDC::DoDrawBitmap Call executed")) ;

    return  ;
}


// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

wxCoord MusSVGFileDC::DeviceToLogicalX(wxCoord x) const
{
    return XDEV2LOG(x);
}


wxCoord MusSVGFileDC::DeviceToLogicalY(wxCoord y) const
{
    return YDEV2LOG(y);
}


wxCoord MusSVGFileDC::DeviceToLogicalXRel(wxCoord x) const
{
    return XDEV2LOGREL(x);
}


wxCoord MusSVGFileDC::DeviceToLogicalYRel(wxCoord y) const
{
    return YDEV2LOGREL(y);
}


wxCoord MusSVGFileDC::LogicalToDeviceX(wxCoord x) const
{
    return XLOG2DEV(x);
}


wxCoord MusSVGFileDC::LogicalToDeviceY(wxCoord y) const
{
    return YLOG2DEV(y);
}


wxCoord MusSVGFileDC::LogicalToDeviceXRel(wxCoord x) const
{
    return XLOG2DEVREL(x);
}


wxCoord MusSVGFileDC::LogicalToDeviceYRel(wxCoord y) const
{
    return YLOG2DEVREL(y);
}

void MusSVGFileDC::write(const wxString &s)
{
    const wxWX2MBbuf buf = s.mb_str(wxConvUTF8);
    m_outfile->Write(buf, strlen((const char *)buf));
    //m_OK = m_outfile->Ok();
}

#ifdef __BORLANDC__
#pragma warn .rch
#pragma warn .ccc
#endif
