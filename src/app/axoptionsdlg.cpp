/////////////////////////////////////////////////////////////////////////////
// Name:        optionsdlg.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/valgen.h"
#include "wx/fontdlg.h"
#include "wx/fontutil.h"

//#include "wx/datetime.h"

#include "axoptionsdlg.h"
#include "aximage.h"
#include "axapp.h"

#include "mus/musdoc.h"
#include "mus/muslayer.h"
#include "mus/muslayerelement.h"

#include "im/impage.h"

#ifdef AX_SUPERIMPOSITION
	#include "superimposition/sup.h"
#endif

#ifdef AX_RECOGNITION
	#include "recognition/rec.h"
#endif

//----------------------------------------------------------------------------
// AxOptionsDlg
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AxOptionsDlg,wxDialog)
    EVT_BUTTON( wxID_OK, AxOptionsDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, AxOptionsDlg::OnCancel )
    EVT_COMBOBOX( ID_CB_LANGUAGE_OPS, AxOptionsDlg::OnChangeLanguage )
    EVT_BUTTON( ID_BROWSE_WORKING_DIR, AxOptionsDlg::OnBrowseWorkDir )
	EVT_BUTTON( ID_BROWSE_DOCUMENTS_DIR, AxOptionsDlg::OnBrowseDocDir )
    EVT_BUTTON( ID_CHOOSE_FONT, AxOptionsDlg::OnChooseFont )
	EVT_BUTTON( ID_CHOOSE_LYRIC_FONT, AxOptionsDlg::OnChooseLyricFont )
    EVT_SPIN( ID_SC_MUS_OFFSET, AxOptionsDlg::OnSpinCtrl )
	EVT_TEXT( ID_SC_MUS_OFFSET, AxOptionsDlg::OnSpinCtrlText )
    EVT_SPIN( ID_SC_MUS_SIZE, AxOptionsDlg::OnSpinCtrl )
	EVT_TEXT( ID_SC_MUS_SIZE, AxOptionsDlg::OnSpinCtrlText )
#ifdef AX_SUPERIMPOSITION
#endif	
#ifdef AX_RECOGNITION
	EVT_BUTTON( ID4_PRE_DEFAULT, AxOptionsDlg::OnPreDefault )
    EVT_BUTTON( ID4_REC_BT_MUS_MODELS, AxOptionsDlg::OnBrowse4 )
    EVT_BUTTON( ID4_REC_BT_TYP_MODELS, AxOptionsDlg::OnBrowse4 )
#endif
END_EVENT_TABLE()

int AxOptionsDlg::s_last_open_tab = 0;

AxOptionsDlg::AxOptionsDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    m_changeLanguage = false;
    m_changeFont = false;
	
	this->SetAutoLayout( true );

    this->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
		
    //OptionsDlgFunc( this, true ); do it manually to allow conditionnal adds
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    m_notebook = new wxNotebook( this, ID_NOTEBOOK_OP, wxDefaultPosition, wxSize(50,50) );//, 0 );
	m_min_size.Set( 0, 0 );

	// Standard tabs
	this->OptionsDlgStandard( m_notebook );	
	
	// Superimposition tabs
	this->OptionsDlgSuperimposition( m_notebook );	
	
	// Recognition tabs
	this->OptionsDlgRecognition( m_notebook );	

	// Notebook and Buttons
	// Layout 'a la main' .... pffff
	m_notebook->SetMinSize( m_notebook->CalcSizeFromPage( m_min_size ) );
    item0->Add( m_notebook, 0, wxALIGN_CENTER|wxALL, 5 );
    wxBoxSizer *item5 = new wxBoxSizer( wxHORIZONTAL );
    wxButton *item6 = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );
    wxButton *item7 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item5, 0, wxALIGN_CENTER|wxALL, 0 );


	item0->Layout();
	this->SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

    m_musWinPtr = new AxOptMusWindow( GetMusPanel(), -1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxNO_BORDER );
	m_musWinPtr->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_MENU )  );
    m_musWinPtr->Resize( );
	
	m_notebook->SetSelection(s_last_open_tab);
}


void AxOptionsDlg::OptionsDlgStandard( wxNotebook *notebook )
{
    wxPanel *item1 = new wxPanel( notebook, -1 );
    wxSize size = System1DlgFunc( item1, false )->GetMinSize();
	m_min_size.Set( max( m_min_size.GetWidth(), size.GetWidth() ), max( m_min_size.GetHeight(), size.GetHeight() ) );
    notebook->AddPage( item1, _("General") );

    // Various
    this->GetCbLanguageOps()->SetValidator(
        wxGenericValidator(&wxGetApp().m_language));
    this->GetDocDir()->SetValidator(
        wxTextValidator(wxFILTER_NONE,&wxGetApp().m_docDir));
    this->GetWorkingDir()->SetValidator(
        wxTextValidator(wxFILTER_NONE,&wxGetApp().m_workingDir));
		
	// Music Font
	wxNativeFontInfo info;
	info.FromString( wxGetApp().m_musicFontDesc );
	m_font.SetNativeFontInfo( info );
    //this->GetFontName()->SetLabel(wxGetApp().m_musicFontName);
	this->GetFontName()->SetLabel( info.GetFaceName() );
	m_ignoreSpinEvent = true;
    this->GetScMusOffset()->SetValidator(
        wxGenericValidator(&wxGetApp().m_fontPosCorrection));
    this->GetScMusSize()->SetValidator(
        wxGenericValidator(&wxGetApp().m_fontSizeCorrection));
	m_previousFontPosCorrection = wxGetApp().m_fontPosCorrection; // keep if canceled
	m_previousFontSizeCorrection = wxGetApp().m_fontSizeCorrection;
	m_ignoreSpinEvent = false;
	
	/** Neumes Font */ 
	info.FromString( wxGetApp().m_neumeFontDesc );
	m_font.SetNativeFontInfo( info );
	this->GetFontName()->SetLabel(wxGetApp().m_neumeFontDesc);
	m_ignoreSpinEvent = true;
	this->GetScMusOffset()->SetValidator(
		wxGenericValidator(&wxGetApp().m_fontPosCorrection));
    this->GetScMusSize()->SetValidator(
		wxGenericValidator(&wxGetApp().m_fontSizeCorrection));
	m_previousFontPosCorrection = wxGetApp().m_fontPosCorrection; // keep if canceled
	m_previousFontSizeCorrection = wxGetApp().m_fontSizeCorrection;
	m_ignoreSpinEvent = false;
	
	// Lyric Font
	info.FromString( wxGetApp().m_lyricFontDesc );
	m_lyric_font.SetNativeFontInfo( info );
	this->GetLyricFontName()->SetLabel( info.GetFaceName() );
		
	// Images
    this->GetCbGrayOpi()->SetValidator(
        wxGenericValidator(&AxImage::s_zoomInterpolation));
    this->GetCbReduceOpi()->SetValidator(
        wxGenericValidator(&AxImage::s_reduceBigImages));
    m_imageSizeToReduceStr = wxString::Format("%d",AxImage::s_imageSizeToReduce);
    this->GetTcMaxsizeOpi()->SetValidator(
        wxTextValidator(wxFILTER_NUMERIC,&m_imageSizeToReduceStr));
	this->GetCbNegativeOpi()->SetValidator(
        wxGenericValidator(&AxImage::s_checkIfNegative));
	this->GetRbImageBin()->SetValidator(
		wxGenericValidator(&ImOperator::s_pre_image_binarization_method));
}

void AxOptionsDlg::OptionsDlgSuperimposition( wxNotebook *notebook )
{
#ifdef AX_SUPERIMPOSITION	
    wxPanel *item1 = new wxPanel( notebook, -1 );
    wxSize size = Sup1DlgFunc( item1, false )->GetMinSize();
	m_min_size.Set( max( m_min_size.GetWidth(), size.GetWidth() ), max( m_min_size.GetHeight(), size.GetHeight() ) );
    notebook->AddPage( item1, _("Superimposition") );

    switch (SupEnv::s_interpolation)
    {
    case 0: m_interpolation_index = 0; break;
    case 2: m_interpolation_index = 1; break;
    case 3: m_interpolation_index = 2; break;
    default: m_interpolation_index = 0;
    }
    this->GetCcInterpOps1()->SetValidator(
        wxGenericValidator(&m_interpolation_index));

    this->GetScSubWindowOps1()->SetValidator(
        wxGenericValidator(&SupEnv::s_subWindowLevel));
    this->GetScSplitxOps1()->SetValidator(
        wxGenericValidator(&SupEnv::s_split_x));
    this->GetScSplityOps1()->SetValidator(
        wxGenericValidator(&SupEnv::s_split_y));
    this->GetScCorrxOps1()->SetValidator(
        wxGenericValidator(&SupEnv::s_corr_x));
    this->GetScCorryOps1()->SetValidator(
        wxGenericValidator(&SupEnv::s_corr_y));
    this->GetChFilter1Ops1()->SetValidator(
        wxGenericValidator(&SupEnv::s_filter1));
    this->GetChFilter2Ops1()->SetValidator(
        wxGenericValidator(&SupEnv::s_filter2));
#endif
}

void AxOptionsDlg::OptionsDlgRecognition( wxNotebook *notebook )
{
#ifdef AX_RECOGNITION	
    wxPanel *item1 = new wxPanel( notebook, -1 );
    wxSize size = Rec1DlgFunc( item1, false )->GetMinSize();
	m_min_size.Set( max( m_min_size.GetWidth(), size.GetWidth() ), max( m_min_size.GetHeight(), size.GetHeight() ) );
    notebook->AddPage( item1, _("Recognition") );
	
	GetCBinOps3()->SetString(0, BRINK_2CLASSES_DESCRIPTION);
	GetCBinOps3()->SetString(1, SAUVOLA_DESCRIPTION);
	GetCBinOps3()->SetString(2, BRINK_3CLASSES_DESCRIPTION);
	
	// pre-processing
    this->GetChTextOps3()->SetValidator(
        wxGenericValidator(&RecEnv::s_find_text));
    this->GetChBordersPosOps3()->SetValidator(
        wxGenericValidator(&RecEnv::s_find_borders));
    this->GetChTextInitOps3()->SetValidator(
        wxGenericValidator(&RecEnv::s_find_ornate_letters));
    this->GetChStavesSegOps3()->SetValidator(
        wxGenericValidator(&RecEnv::s_find_text_in_staves));
	// margins
	this->GetScMarginTopOps3()->SetValidator(
        wxGenericValidator(&ImPage::s_pre_margin_top));
	this->GetScMarginBottomOps3()->SetValidator(
        wxGenericValidator(&ImPage::s_pre_margin_bottom));
	this->GetScMarginLeftOps3()->SetValidator(
        wxGenericValidator(&ImPage::s_pre_margin_left));
	this->GetScMarginRightOps3()->SetValidator(
        wxGenericValidator(&ImPage::s_pre_margin_right));
	// binarization
	this->GetCBinOps3()->SetValidator(
        wxGenericValidator(&ImPage::s_pre_page_binarization_method));
	this->GetScBinSizeOps3()->SetValidator(
        wxGenericValidator(&ImPage::s_pre_page_binarization_method_size));
	this->GetCbBinSelect()->SetValidator(
		wxGenericValidator(&ImPage::s_pre_page_binarization_select));

    // typographic model
    this->GetTypModel()->SetValidator(
        wxTextValidator(wxFILTER_NONE,&RecEnv::s_rec_typ_model));

	// music model
    this->GetMusModel()->SetValidator(
        wxTextValidator(wxFILTER_NONE,&RecEnv::s_rec_mus_model));

    m_lmScalingStr = wxString::Format("%.02f",RecEnv::s_rec_lm_scaling);
    this->GetLMScaling()->SetValidator(
        wxTextValidator(wxFILTER_NUMERIC,&m_lmScalingStr));
    this->GetDelayed()->SetValidator(
        wxGenericValidator(&RecEnv::s_rec_delayed));
    this->GetLMOrder()->SetValidator(
        wxGenericValidator(&RecEnv::s_rec_lm_order));
#endif
}


void AxOptionsDlg::UpdateFontCorrections( int eventID )
{
	if ( !m_musWinPtr )
		return ;

	if ( !this->IsShown() || m_ignoreSpinEvent )
		return;

	wxGetApp().m_fontPosCorrection = GetScMusOffset()->GetValue();
	wxGetApp().m_fontSizeCorrection = GetScMusSize()->GetValue();
	m_musWinPtr->m_doc->UpdatePageValues();
	if ( eventID == ID_SC_MUS_SIZE)
		m_musWinPtr->m_doc->UpdateFontValues();
    m_musWinPtr->Resize( );
}

#ifdef AX_RECOGNITION
void AxOptionsDlg::OnBrowse4( wxCommandEvent &event )
{
    if ( event.GetId() == ID4_REC_BT_TYP_MODELS )
	{
		wxString filename = wxFileSelector( _("Select a typographic model"), wxGetApp().m_lastDirTypModel_in, this->GetTypModel()->GetValue(), _T(""), "Typographic models|*.axtyp", wxFD_OPEN);
		if ( filename.IsEmpty() )
			return;

		wxGetApp().m_lastDirTypModel_in = wxPathOnly( filename );
        this->GetTypModel()->SetValue( filename );
	}
    if ( event.GetId() == ID4_REC_BT_MUS_MODELS )
	{
		wxString filename = wxFileSelector( _("Select a music model"), wxGetApp().m_lastDirMusModel_in, this->GetMusModel()->GetValue(), _T(""), "Music models|*.axmus", wxFD_OPEN);
		if ( filename.IsEmpty() )
			return;

		wxGetApp().m_lastDirMusModel_in = wxPathOnly( filename );
        this->GetMusModel()->SetValue( filename );
	}
}
#endif


void AxOptionsDlg::OnSpinCtrlText( wxCommandEvent &event )
{
	UpdateFontCorrections( event.GetId() );
}

void AxOptionsDlg::OnSpinCtrl( wxSpinEvent &event )
{
	UpdateFontCorrections( event.GetId() );
}


#ifdef __WXMAC__
// -------------------------------------------------------------
#include "wx/fontenum.h"

class AxFontEnumerator : public wxFontEnumerator
{
public:
    bool GotAny() const
        { return !m_facenames.IsEmpty(); }

    const wxArrayString& GetFacenames() const
        { return m_facenames; }

protected:
    virtual bool OnFacename(const wxString& facename)
    {
        m_facenames.Add(facename);
        return true;
    }

    private:
        wxArrayString m_facenames;
} fontEnumerator;

#endif


void AxOptionsDlg::OnChooseFont( wxCommandEvent &event )
{
#if defined(__WXMAC__) && !wxCHECK_VERSION(2,8,3)

    AxFontEnumerator fontEnumerator;
	wxString facename;
    fontEnumerator.EnumerateFacenames( wxFONTENCODING_CP1252 );
	//fontEnumerator.EnumerateFacenames( wxFONTENCODING_SYSTEM );
    if ( fontEnumerator.GotAny() )
    {
        int nFacenames = fontEnumerator.GetFacenames().GetCount();
        wxString *facenames = new wxString[nFacenames];
		int n;
		for ( n = 0; n < nFacenames; n++ )
			facenames[n] = fontEnumerator.GetFacenames().Item(n);
            
		n = wxGetSingleChoiceIndex( _("Choose a facename"), wxGetApp().GetAppName(),
                                       nFacenames, facenames, this);

		if ( n != -1 )
            facename = facenames[n];
        delete [] facenames;
        
        if ( facename.empty() )
			return;
    }
	else
    {
        wxLogWarning(_("No font found"));
		return;
    }
	//wxFont::SetDefaultEncoding( wxFONTENCODING_CP1252 );
	wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
		wxFONTWEIGHT_NORMAL, false, facename, wxFONTENCODING_CP1252);
	//wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
	//	wxFONTWEIGHT_NORMAL, false, "Leipzig 4.4", wxFONTENCODING_CP1252);
#else
    wxFont font = wxGetFontFromUser( this );
    if ( !font.Ok() )
        return;
#endif
    m_font = font;
    m_changeFont = true;
    this->GetFontName()->SetLabel( m_font.GetFaceName() );
}


void AxOptionsDlg::OnChooseLyricFont( wxCommandEvent &event )
{
    wxFont font = wxGetFontFromUser( this, m_lyric_font );
    if ( !font.Ok() )
        return;
		
    m_lyric_font = font;
    m_changeFont = true;
    this->GetLyricFontName()->SetLabel( m_lyric_font.GetFaceName() );
}

void AxOptionsDlg::OnPreDefault( wxCommandEvent &event )
{
    this->GetChTextOps3()->SetValue( true );
    this->GetChBordersPosOps3()->SetValue( true );
    this->GetChTextInitOps3()->SetValue( true );
    this->GetChStavesSegOps3()->SetValue( true );
	// margins
	this->GetScMarginTopOps3()->SetValue( 150 );
	this->GetScMarginBottomOps3()->SetValue( 120 );
	this->GetScMarginLeftOps3()->SetValue( 30 );
	this->GetScMarginRightOps3()->SetValue( 20 );
	// binarization
	this->GetRbImageBin()->SetSelection( IM_BINARIZATION_BRINK );
	this->GetCBinOps3()->SetSelection( PRE_BINARIZATION_BRINK );
	this->GetScBinSizeOps3()->SetValue( 15 );
	this->GetCbBinSelect()->SetValue( true );
}

void AxOptionsDlg::OnBrowseDocDir( wxCommandEvent &event )
{
    wxString input = wxDirSelector( _("Documents folder"), wxGetApp().m_docDir );
    if ( input.empty() )
        return;

    this->GetDocDir()->SetValue(input);
}

void AxOptionsDlg::OnBrowseWorkDir( wxCommandEvent &event )
{
    wxString input = wxDirSelector( _("Working folder"), wxGetApp().m_workingDir );
    if ( input.empty() )
        return;

    this->GetWorkingDir()->SetValue(input);
}

void AxOptionsDlg::OnChangeLanguage( wxCommandEvent &event )
{
    m_changeLanguage = true;
}

void AxOptionsDlg::OnOk(wxCommandEvent &event)
{
    if ( Validate() && TransferDataFromWindow() )
    {   
        AxImage::s_imageSizeToReduce = atoi(m_imageSizeToReduceStr);
        if ( m_changeLanguage )
            wxMessageBox(_("You must restart the program to change the language"), wxGetApp().GetAppName() ,
                             wxICON_INFORMATION );

        if ( m_changeFont )
        {
            const wxNativeFontInfo *info;
			info = m_font.GetNativeFontInfo();
            wxGetApp().m_musicFontDesc = info->ToString();
			
            info = m_lyric_font.GetNativeFontInfo();
            wxGetApp().m_lyricFontDesc = info->ToString();

        }
		
	#ifdef AX_SUPERIMPOSITION
		switch (m_interpolation_index)
        {
        case 0: SupEnv::s_interpolation = 0; break;
        case 1: SupEnv::s_interpolation = 2; break;
        case 2: SupEnv::s_interpolation = 3; break;
        default: SupEnv::s_interpolation = 0;
        }
	#endif
		
	#ifdef AX_RECOGNITION
		//RecEnv::s_rec_phone_pen = atof(m_phonePenStr);
        //RecEnv::s_rec_end_prune = atof(m_endPruneStr);
        //RecEnv::s_rec_word_pen = atof(m_wordPenStr);
        RecEnv::s_rec_lm_scaling = atof(m_lmScalingStr);
	#endif
		
		s_last_open_tab = m_notebook->GetSelection( );
        event.Skip();
    }
}

void AxOptionsDlg::OnCancel(wxCommandEvent &event)
{
	// reset previous values;
	wxGetApp().m_fontPosCorrection = m_previousFontPosCorrection;
	wxGetApp().m_fontSizeCorrection = m_previousFontSizeCorrection;
	s_last_open_tab = m_notebook->GetSelection( );
	event.Skip();
}



//----------------------------------------------------------------------------
// AxOptMusWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(AxOptMusWindow, MusWindow)

BEGIN_EVENT_TABLE(AxOptMusWindow,MusWindow)
    EVT_MOUSE_EVENTS( AxOptMusWindow::OnMouse )
END_EVENT_TABLE()

AxOptMusWindow::AxOptMusWindow( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style, bool center ) :
    MusWindow( parent, id, position, size, style, center )
{
    
    m_docPtr = NULL;
    m_docPtr = new MusDoc();
    m_docPtr->Reset( Transcription );
    
    m_docPtr->m_pageWidth = 150;
    m_docPtr->m_pageHeight = 150;
    m_docPtr->m_pageLeftMar = 0;
    m_docPtr->m_pageRightMar = 0;
    
    // create a new layout and the page
    MusPage *page = new MusPage();
    MusSystem *system = new MusSystem();
    system->m_y_abs = 120;
    MusStaff *staff = new MusStaff( 1 );
    staff->m_y_abs = 120;
    MusLayer *layer = new MusLayer( 1 );
    
    MusClef *clef = new MusClef();
    clef->m_clefId = UT3;
    clef->m_x_abs = 80;
    
    layer->AddElement( clef );
    staff->AddLayer( layer );
    system->AddStaff( staff );
    page->AddSystem( system );
    m_docPtr->AddPage( page );
    
	this->SetDoc( m_docPtr );
    this->SetZoom( 50 );
	this->Resize();
}

AxOptMusWindow::AxOptMusWindow()
{
}

AxOptMusWindow::~AxOptMusWindow()
{
    if (m_docPtr)
        delete m_docPtr;
}

void AxOptMusWindow::OnMouse(wxMouseEvent &event)
{
}
