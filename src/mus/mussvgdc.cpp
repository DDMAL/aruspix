/////////////////////////////////////////////////////////////////////////////
// Name:        mussvgdc.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

//#include <iostream>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/filefn.h"

#include "musleipzigbbox.h"
#include "mussvgdc.h"
#include "musdef.h"
#include "musdoc.h"
#include "musrc.h"

#include "app/axapp.h"

#define space " "
#define semicolon ";"
 
//#include "app/axapp.h"

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }
static inline double RadToDeg(double deg) { return (deg * 180.0) / M_PI; }

//----------------------------------------------------------------------------
// MusSvgDC
//----------------------------------------------------------------------------


MusSvgDC::MusSvgDC (wxString f, int width, int height):
    MusDC()
{	
	
    m_correctMusicAscent = false; // do not correct the ascent in the Leipzig font

    m_width = width;
    m_height = height;
    
    m_userScaleX = 1.0;
    m_userScaleY = 1.0;
    m_originX = 0;
    m_originY = 0;
    
    SetBrush( AxBLACK, wxSOLID );
    SetPen( AxBLACK, 1, wxSOLID );

    //m_font  = *wxNORMAL_FONT;

    m_graphics = 0;
    m_indents = 1;
    
    m_leipzig_glyphs.clear();
    
    m_committed = false;
    m_filename = f ;
    
    m_outfile = new wxMemoryOutputStream() ;   
}


MusSvgDC::~MusSvgDC ( )
{
    if (!m_committed) {
        Commit();
    }
    delete m_outfile ;
}


bool MusSvgDC::copy_wxTransferFileToStream(const wxString& filename, wxFileOutputStream& stream)
{
    wxFFile file(filename, _T("rb"));
    if ( !file.IsOpened() )
        return false;
    
    char buf[4096];
    
    size_t nRead;
    do
    {
        nRead = file.Read(buf, WXSIZEOF(buf));
        if ( file.Error() )
            return false;
        
        stream.Write(buf, nRead);
        if ( !stream )
            return false;
    }
    while ( !file.Eof() );
    
    return true;
}

void MusSvgDC::Commit() {

    if (m_committed) {
        return;
    }

    wxFileOutputStream outfile(m_filename);
    if (!outfile.Ok()) {
        return;
    }
    
    int i;
    // close unclosed graphics, just in case
    for (i = m_graphics; i < 0; m_graphics-- ) {
        WriteLine("/*- MusSvgDC::Flush - Unclosed graphic */");
        WriteLine("</g>");
        m_indents--;
    }
    m_indents = 0;
    WriteLine("</svg>\n") ;

    // header
    wxString s ;
    s.Printf ( "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n<svg width=\"%dpx\" height=\"%dpx\"",  
            (int)((double)m_width * m_userScaleX), (int)((double)m_height * m_userScaleY));
    s += " version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\"  xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
    
    const wxWX2MBbuf buf = s.mb_str(wxConvUTF8);
    outfile.Write(buf, strlen((const char *)buf));
    
    // insert the def for the leipzig glyphes if any
    if (m_leipzig_glyphs.Count() > 0)
    {
        s = "\t<defs>\n";
        outfile.Write(s, strlen(s));
        for (i = 0; i < (int)m_leipzig_glyphs.Count(); i++) {
            s = "\t\t";
            outfile.Write(s, strlen(s));
            copy_wxTransferFileToStream( AxApp::GetResourcesPath() + "/svg/" + m_leipzig_glyphs[i] + ".xml", outfile );
        }
        s = "\t</defs>\n";
        outfile.Write(s, strlen(s));
    
    }
    wxMemoryInputStream input(*m_outfile);
    input.SeekI(0);
    //input.Read( *m_outfile );
    outfile.Write( input );
    m_committed = true;
}


void MusSvgDC::WriteLine( wxString string )
{
    wxString output;
    output.Append( '\t', m_indents );
    output += string + "\n"; 
    const wxWX2MBbuf buf = output.mb_str(wxConvUTF8);
    m_outfile->Write(buf, strlen((const char *)buf));
}


void MusSvgDC::StartGraphic( MusDocObject *object, wxString gClass, wxString gId )
{
    WriteLine(wxString::Format("<g class=\"%s\" id=\"%s\" style=\"%s %s %s %s\">", gClass.c_str(), gId.c_str(), m_penColour.c_str(), m_penStyle.c_str(),
        m_brushColour.c_str(), m_brushStyle.c_str() ) );
    m_graphics++;
    m_indents++;
}
  
      
void MusSvgDC::EndGraphic(MusDocObject *object, MusRC *rc ) 
{
    m_graphics--;
    m_indents--;
    WriteLine("</g>");
}


void MusSvgDC::StartPage( )
{
    WriteLine(wxString::Format("<g class=\"page\" transform=\"translate(%d, %d)  scale(%f, %f)\">", 
        (int)((double)m_originX * m_userScaleX), (int)((double)m_originY * m_userScaleY), m_userScaleX, m_userScaleY ) );
    m_graphics++;
    m_indents++;
}
 
       
void MusSvgDC::EndPage() 
{
    m_graphics--;
    m_indents--;
    WriteLine("</g>");
}

        
void MusSvgDC::SetBrush( int colour, int style )
{
    m_brushColour = "fill:#" + GetColour(colour) + semicolon;
    switch ( style )
    {
        case wxSOLID :
            m_brushStyle = "fill-opacity:1.0; ";
            break ;
        case wxTRANSPARENT:
            m_brushStyle = "fill-opacity:0.0; ";
            break ;
        default :
            m_brushStyle = "fill-opacity:1.0; "; // solid brush as default
    }
}
        
void MusSvgDC::SetBackground( int colour, int style )
{
    // nothing to do, we do not handle Background
}

void MusSvgDC::SetBackgroundImage( void *image, double opacity )
{

}
        
void MusSvgDC::SetBackgroundMode( int mode )
{
    // nothing to do, we do not handle Background Mode
}
        
void MusSvgDC::SetPen( int colour, int width, int style )
{
    m_penColour = "stroke:#" + GetColour(colour)  + semicolon;
    m_penWidth = "stroke-width:" + wxString::Format("%d", width) + semicolon;
    switch ( style )
    {
        case wxSOLID :
            m_penStyle = "stroke-opacity:1.0; ";
            break ;
        case wxTRANSPARENT:
            m_penStyle = "stroke-opacity:0.0; ";
            break ;
        default :
            m_penStyle = "stroke-opacity:1.0; "; // solid brush as default
    }
}
        
void MusSvgDC::SetFont( MusFontInfo *font_info )
{
    m_font = *font_info;
    //wxFont font( font_info->pointSize, (wxFontFamily)font_info->family, font_info->style,
    //    (wxFontWeight)font_info->weight, font_info->underlined, font_info->faceName,
    //    (wxFontEncoding)font_info->encoding );
    //m_dc->SetFont( font );
}
            

void MusSvgDC::SetTextForeground( int colour )
{
    m_brushColour = "fill:#" + GetColour(colour); // we use the brush colour for text
}
        
void MusSvgDC::SetTextBackground( int colour )
{
    // nothing to do, we do not handle Text Background Mode
}
       
void MusSvgDC::ResetBrush( )
{
    SetBrush( AxBLACK, wxSOLID );
}
        
void MusSvgDC::ResetPen( )
{
    SetPen( AxBLACK, 1, wxSOLID );
} 

void MusSvgDC::SetLogicalOrigin( int x, int y ) 
{
    m_originX = -x;
    m_originY = -y;
} 

void MusSvgDC::SetUserScale( double xScale, double yScale ) 
{
    m_userScaleX = xScale;
    m_userScaleY = yScale;
}       

// Copied from bBoxDc, TODO find another more generic solution
void MusSvgDC::GetTextExtent( wxString& string, int *w, int *h )
{
    int x, y, partial_w, partial_h;
    
    *w = 0;
    *h = 0;
    
    for (unsigned int i = 0; i < string.Len(); i++) {
        
        MusLeipzigBBox::GetCharBounds(string.c_str()[i], &x, &y, &partial_w, &partial_h);
        
        partial_w *= ((m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM));
        partial_h *= ((m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM));
        
        *w += partial_w;
        *h += partial_h;
    }
}
       

MusPoint MusSvgDC::GetLogicalOrigin( ) 
{
    return MusPoint( m_originX, m_originY );
}



// Drawing mething
void MusSvgDC::DrawComplexBezierPath(int x, int y, int bezier1_coord[6], int bezier2_coord[6])
{
    WriteLine( wxString::Format("<path d=\"M%d,%d C%d,%d %d,%d %d,%d C%d,%d %d,%d %d,%d\" style=\"fill:#000; fill-opacity:1.0; stroke:#000000; stroke-linecap:round; stroke-linejoin:round; stroke-opacity:1.0; stroke-opacity:1.0; stroke-width:1\" />", 
                                x, y, // M command
                                bezier1_coord[0], bezier1_coord[1], bezier1_coord[2], bezier1_coord[3], bezier1_coord[4], bezier1_coord[5], // First bezier
                                bezier2_coord[0], bezier2_coord[1], bezier2_coord[2], bezier2_coord[3], bezier2_coord[4], bezier2_coord[5] // Second Bezier
                                ) );
}

void MusSvgDC::DrawCircle(int x, int y, int radius)
{
    DrawEllipse(x - radius, y - radius, 2*radius, 2*radius);
}


void MusSvgDC::DrawEllipse(int x, int y, int width, int height)
{
    int rh = height / 2;
    int rw = width  / 2;

    WriteLine(wxString::Format("<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" />", x+rw,y+rh, rw, rh ));
}

        
void MusSvgDC::DrawEllipticArc(int x, int y, int width, int height, double start, double end)
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

    wxString s ;
    //radius
    double rx = width / 2 ;
    double ry = height / 2 ;
    // center
    double xc = x + rx ;
    double yc = y + ry ;

    double xs, ys, xe, ye ;
    xs = xc + rx * cos (DegToRad(start)) ;
    xe = xc + rx * cos (DegToRad(end)) ;
    ys = yc - ry * sin (DegToRad(start)) ;
    ye = yc - ry * sin (DegToRad(end)) ;

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

    WriteLine( wxString::Format("<path d=\"M%d %d A%d %d 0.0 %d %d  %d %d \" />",
        int(xs), int(ys), int(rx), int(ry),
        fArc, fSweep, int(xe), int(ye) ) );
}
  
              
void MusSvgDC::DrawLine(int x1, int y1, int x2, int y2)
{
    WriteLine( wxString::Format("<path d=\"M%d %d L%d %d\" style=\"%s\" />", x1,y1,x2,y2, m_penWidth.c_str()) );
}
 
               
void MusSvgDC::DrawPolygon(int n, MusPoint points[], int xoffset, int yoffset, int fill_style)
{

    wxString s ;
    s = "<polygon style=\"";
    //if ( fillStyle == wxODDEVEN_RULE )
    //    s = s + wxT("fill-rule:evenodd; ");
    //else
    s += "fill-rule:nonzero; ";

    s += "\" points=\"" ;

    for (int i = 0; i < n;  i++)
    {
        s += wxString::Format("%d,%d ", points [i].x+xoffset, points[i].y+yoffset );
        //CalcBoundingBox ( points [i].x+xoffset, points[i].y+yoffset);
    }
    s += wxT("\" /> ") ;
    WriteLine(s);
}
    
            
void MusSvgDC::DrawRectangle(int x, int y, int width, int height)
{
    DrawRoundedRectangle( x, y, width, height, 0 );
}


void MusSvgDC::DrawRoundedRectangle(int x, int y, int width, int height, double radius)
{

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

    WriteLine ( wxString::Format(" <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" rx=\"%.2g\" />", x, y, width, height, radius ) );

}

        
void MusSvgDC::DrawText(const wxString& text, int x, int y)
{
    //DrawRotatedText( text, x, y, 0.0 );
    DrawMusicText(text, x, y);
}



void MusSvgDC::DrawRotatedText(const wxString& text, int x, int y, double angle)
{
    //known bug; if the font is drawn in a scaled DC, it will not behave exactly as wxMSW

    wxString s;

    // calculate bounding box
    wxCoord w, h, desc;
    //DoGetTextExtent(sText, &w, &h, &desc);
    w = h = desc = 0;

    //double rad = DegToRad(angle);

    
    //if (m_backgroundMode == wxSOLID)
    {
        WriteLine("/*- MusSVGFileDC::DrawRotatedText - Backgound not implemented */") ;
    }
    /*
    s = wxString::Format(" <text x=\"%d\" y=\"%d\" dx=\"%d\" dy=\"%d\" ", x, y, 1, 1) ;

    // For some reason, some browsers (e.g., Chrome) do not like spaces or dots in font names...
    sTmp.Replace(" ", "");
    sTmp.Replace(".", "");
    if (sTmp.Len () > 0)  s = s + wxT("style=\"font-family: '") + sTmp + wxT("'; ");
    else s = s + wxT("style=\" ") ;

    wxString fontweights [3] = { wxT("normal"), wxT("lighter"), wxT("bold") };
    s = s + wxT("font-weight:") + fontweights[m_font.GetWeight() - wxNORMAL] + semicolon + space;

    wxString fontstyles [5] = { wxT("normal"), wxT("style error"), wxT("style error"), wxT("italic"), wxT("oblique") };
    s = s + wxT("font-style:") + fontstyles[m_font.GetStyle() - wxNORMAL] + semicolon  + space;

    sTmp.Printf (wxT("font-size:%dpt; "), (int)((double)m_font.GetPointSize() * 1) );
    s = s + sTmp ;
    // remove the color information because normaly already in the graphic element
    //s = s + wxT("fill:#") + wxColStr (m_textForegroundColour) + wxT("; stroke:#") + wxColStr (m_textForegroundColour) + wxT("; ") ;
    sTmp.Printf ( wxT("stroke-width:0;\"  transform=\"rotate( %.2g %d %d )  \" >"),  -angle, x,y ) ;
    s = s + sTmp + sText + wxT("</text> ") + newline ;

    write(s);
    */
}

wxString FilenameLookup(unsigned char c) {
    wxString glyph;
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
            /* oblique figures */
        case 0x82: glyph = "oblique_figure_0"; break;
        case 0x83: glyph = "oblique_figure_1"; break;
        case 0x84: glyph = "oblique_figure_2"; break;
        case 0x85: glyph = "oblique_figure_3"; break;
        case 0x86: glyph = "oblique_figure_4"; break;
        case 0x87: glyph = "oblique_figure_5"; break;
        case 0x88: glyph = "oblique_figure_6"; break;
        case 0x89: glyph = "oblique_figure_7"; break;
        case 0x8A: glyph = "oblique_figure_8"; break;
        case 0x8B: glyph = "oblique_figure_9"; break;
            /* fermatas */
        case LEIPZIG_FERMATA_UP: glyph = "fermata_up"; break;
        case LEIPZIG_FERMATA_DOWN: glyph = "fermata_down"; break;          
            /* clef */
        case LEIPZIG_CLEF_G: glyph = "clef_G"; break;
        case LEIPZIG_CLEF_F: glyph = "clef_F"; break;
        case LEIPZIG_CLEF_C: glyph = "clef_C"; break;
        case LEIPZIG_CLEF_8va: glyph = "clef_G8"; break;
        case LEIPZIG_CLEF_G + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_G_mensural"; break;
        case LEIPZIG_CLEF_F + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_F_mensural"; break;
        case LEIPZIG_CLEF_C + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_C_mensural"; break;
        case LEIPZIG_CLEF_8va + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_G_chiavette"; break;
            /* meter */
        case LEIPZIG_METER_SYMB_COMMON: glyph = "meter_symb_common"; break;
        case LEIPZIG_METER_SYMB_CUT: glyph = "meter_symb_cut"; break;
        case LEIPZIG_METER_SYMB_2_CUT: glyph = "meter_symb_2_cut"; break;
        case LEIPZIG_METER_SYMB_3_CUT: glyph = "meter_symb_3_cut"; break;
            /* alterations */
        case LEIPZIG_ACCID_SHARP: glyph = "alt_sharp"; break;
        case LEIPZIG_ACCID_NATURAL: glyph = "alt_natural"; break;
        case LEIPZIG_ACCID_FLAT: glyph = "alt_flat"; break;
        case LEIPZIG_ACCID_DOUBLE_SHARP: glyph = "alt_double_sharp"; break;
        case LEIPZIG_ACCID_SHARP + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_sharp_mensural"; break;
        case LEIPZIG_ACCID_NATURAL + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_natural_mensural"; break;
        case LEIPZIG_ACCID_FLAT + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_flat_mensural"; break;
        case LEIPZIG_ACCID_DOUBLE_SHARP + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_double_sharp_mensural"; break;
            /* rests */
        case LEIPZIG_REST_QUARTER: glyph = "rest_4"; break;
        case LEIPZIG_REST_QUARTER + 1: glyph = "rest_8"; break;
        case LEIPZIG_REST_QUARTER + 2: glyph = "rest_16"; break;
        case LEIPZIG_REST_QUARTER + 3: glyph = "rest_32"; break;
        case LEIPZIG_REST_QUARTER + 4: glyph = "rest_64"; break;
        case LEIPZIG_REST_QUARTER + 5: glyph = "rest_128"; break;
        case LEIPZIG_REST_QUARTER + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_4_mensural"; break;
        case LEIPZIG_REST_QUARTER + 1 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_8_mensural"; break;
        case LEIPZIG_REST_QUARTER + 2 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_16_mensural"; break;
        case LEIPZIG_REST_QUARTER + 3 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_32_mensural"; break;
        case LEIPZIG_REST_QUARTER + 4 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_64_mensural"; break;
        case LEIPZIG_REST_QUARTER + 5 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_128_mensural"; break;
            /* note heads */
        case LEIPZIG_HEAD_WHOLE: glyph = "head_whole"; break;
        case LEIPZIG_HEAD_WHOLE_FILLED: glyph = "head_whole_fill"; break;
        case LEIPZIG_HEAD_HALF: glyph = "head_half"; break;
        case LEIPZIG_HEAD_QUARTER: glyph = "head_quarter"; break;
        case LEIPZIG_HEAD_WHOLE + LEIPZIG_OFFSET_MENSURAL: glyph = "head_whole_diamond"; break;
        case LEIPZIG_HEAD_WHOLE_FILLED + LEIPZIG_OFFSET_MENSURAL: glyph = "head_whole_filldiamond"; break;
        case LEIPZIG_HEAD_HALF + LEIPZIG_OFFSET_MENSURAL: glyph = "head_half_diamond"; break;
        case LEIPZIG_HEAD_QUARTER + LEIPZIG_OFFSET_MENSURAL: glyph = "head_quarter_filldiamond"; break;
            /* slashes */
        case LEIPZIG_STEM_FLAG_UP: glyph = "slash_up"; break;
        case LEIPZIG_STEM_FLAG_DOWN: glyph = "slash_down"; break;
        case LEIPZIG_STEM_FLAG_UP + LEIPZIG_OFFSET_MENSURAL: glyph = "slash_up_mensural"; break;
        case LEIPZIG_STEM_FLAG_DOWN + LEIPZIG_OFFSET_MENSURAL: glyph = "slash_down_mensural"; break;
            /* ornaments */
        case 35: glyph = "orn_mordent"; break;
        case LEIPZIG_EMB_TRILL: glyph = "orn_trill"; break;
            /* todo */
        default: glyph = "clef_G_chiavette";
    }

    return glyph;
}

void MusSvgDC::DrawMusicText(const wxString& text, int x, int y)
{

    int w, h, gx, gy;
        
    // print chars one by one
    for (unsigned int i = 0; i < text.Len(); i++) {
        unsigned char c = (unsigned char)text[i];
        
        wxString glyph = FilenameLookup(c);
        
        // Add the glyph to the array for the <defs>
        if (m_leipzig_glyphs.Index(glyph) == wxNOT_FOUND) {
            m_leipzig_glyphs.Add(glyph);
        }
        
        // Write the char in the SVG
        WriteLine ( wxString::Format("<use xlink:href=\"#%s\" transform=\"translate(%d, %d) scale(%f, %f)\"/>",
                                     glyph.c_str(), x, y, ((double)(m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM)),
                                     ((double)(m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM)) ) );
        
        // Get the bounds of the char
        MusLeipzigBBox::GetCharBounds(c, &gx, &gy, &w, &h);
        // Sum it to x so we move it to the start of the next char
        x += (w * ((double)(m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM)));
    }
    

    

}


void MusSvgDC::DrawSpline(int n, MusPoint points[])
{
    //m_dc->DrawSpline( n, (wxPoint*)points );
}

void MusSvgDC::DrawBackgroundImage( int x, int y )
{
   
}


wxString MusSvgDC::GetColour( int colour )
{ 
    switch ( colour )
    {
    case (AxBLACK): return "000000";
    case (AxWHITE): return "FFFFFF";
    case (AxRED): return "FF0000";
    case (AxGREEN): return "00FF00";
    case (AxBLUE): return "0000FF";
    case (AxCYAN): return "00FFFF";
    case (AxLIGHT_GREY): return "777777";
    default:
        int blue =  (colour & 255);
        int green = (colour >> 8) & 255;
        int red = (colour >> 16) & 255;
        wxString s = wxDecToHex(char(red)) + wxDecToHex(char(green)) + wxDecToHex(char(blue)) ;
        return s;
    }
}


