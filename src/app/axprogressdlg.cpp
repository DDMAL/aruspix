/////////////////////////////////////////////////////////////////////////////
// Name:        axprogressdlg.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axprogressdlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//#include "wx/config.h"
#include "wx/fontdlg.h"
#include "wx/fontutil.h"
#include "wx/config.h"

#include "axprogressdlg.h"
#include "axapp.h"
//#include "resource.h"

// static 
bool AxProgressDlg::s_close_at_end = false;
bool AxProgressDlg::s_instance_existing = false;
//timer values
int AxProgressDlg::s_timer_decoding = 20000; // time for staff decoding one unit = on staff 
int AxProgressDlg::s_timer_model_bigram = 5000;
int AxProgressDlg::s_timer_full_adapting = 20000; // time for staff adapting one unit = on staff
int AxProgressDlg::s_timer_fast_adapting = 20000; // time for staff adapting one unit = on staff

//----------------------------------------------------------------------------
// AxProgressDlg
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( AxProgressDlg, wxDialog )

BEGIN_EVENT_TABLE(AxProgressDlg,wxDialog)
    EVT_BUTTON( wxID_CANCEL, AxProgressDlg::OnCancel )
END_EVENT_TABLE()

AxProgressDlg::AxProgressDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( NULL, id, title, position, size, style & ~wxSTAY_ON_TOP )
	//wxDialog( NULL, id, title, position, size, style )
{
	wxASSERT_MSG( !AxProgressDlg::s_instance_existing , "Single instance checker should be false" );
	m_parent = parent;

    ProgressDlgFunc( this, TRUE );
	this->GetCbCloseAtEnd()->SetValue( AxProgressDlg::s_close_at_end );

    m_stopWatch.Start();
    m_lastWatch = 0;

    m_jobBar = 0;
    m_batchBar = 0;
    m_maxJobBar = 10;
    m_maxBatchBar = 1;
    m_old_ts = wxTimeSpan::Seconds(0);
    m_ts = wxTimeSpan::Seconds(0);

    GetTxRestProgress()->SetLabel( m_ts.Format() );

    m_canceled = false;

#ifdef __WXMOTIF__
    // For some reason, we get a memcpy crash in wxLogStream::DoLogStream
    // on gcc/wxMotif, if we use wxLogTextCtl. Maybe it's just gcc?
    delete wxLog::SetActiveTarget(new wxLogStderr);
#else
    // set our text control as the log target
    wxLogTextCtrl *logWindow = new wxLogTextCtrl( GetTcLogProgress() );
    delete wxLog::SetActiveTarget( logWindow );
#endif

        // counter callback
    imCounterSetCallback( this, imDlgCounter );

    m_counter = imCounterBegin( title.c_str() );
    imCounterTotal( m_counter, 1, title.c_str() );
	
	AxProgressDlg::s_instance_existing = true;
	
	m_parent->Disable();
}

AxProgressDlg::AxProgressDlg()  
{
}

AxProgressDlg::~AxProgressDlg()
{
    delete wxLog::SetActiveTarget(NULL);
    // counter callback
    imCounterSetCallback(NULL, NULL);

	wxASSERT_MSG( AxProgressDlg::s_instance_existing , "Single instance checker should be true" );	
	AxProgressDlg::s_instance_existing = false;
	m_parent->Enable( true );
	m_parent->SetFocus( );
}

void AxProgressDlg::AxShowModal( bool failed )
{
	SetCanceled( true );
	GetCancel()->SetLabel( _("Close") );
    GetGaugeOperation()->SetValue( 100 );
    GetGaugeJob()->SetValue( 100 );
    GetGaugeBatch()->SetValue( 100 );
#ifdef __WXMAC__
    this->Show(false);  // otherwise doesn't stop
    this->SetFocus();
    this->GetCancel()->SetFocus();
#endif
	AxProgressDlg::s_close_at_end = this->GetCbCloseAtEnd()->IsChecked();
	if ( failed || !AxProgressDlg::s_close_at_end )
		this->ShowModal();
}


void AxProgressDlg::SuspendCounter()
{
    imCounterSetCallback(NULL, NULL);
}


void AxProgressDlg::ReactiveCounter()
{
    imCounterSetCallback( this, imDlgCounter );
}

/*
bool AxProgressDlg::IncOperation( )
{
    if (m_canceled)
        return false;

    this->GetGaugeOperation()->SetValue( m_operationBar++ * 100 / m_maxOperationBar );
    if ( HasToBeUpdated() )
        wxGetApp().Yield( );
    
    return true;
}
*/

void AxProgressDlg::UpdateBatchBar( )
{
	this->GetGaugeBatch()->SetValue( (m_batchBar - 1) * 100 / m_maxBatchBar + ( this->GetGaugeJob()->GetValue( ) / m_maxBatchBar ) );
}

void AxProgressDlg::UpdateJobBar( )
{
	this->GetGaugeJob()->SetValue( (m_jobBar - 1) * 100 / m_maxJobBar + ( this->GetGaugeOperation()->GetValue( ) / m_maxJobBar ) );
	UpdateBatchBar( );
}

bool AxProgressDlg::SetOperation( wxString msg )
{
    if (m_canceled)
        return false;

    this->GetTxMsgOperation()->SetLabel( msg );
    this->GetGaugeOperation()->SetValue( m_operationBar = 0 );
    this->GetGaugeJob()->SetValue( m_jobBar++ * 100 / m_maxJobBar );
	UpdateBatchBar( );
    //wxLogMessage( msg );
    wxGetApp().Yield( );
    
    return true;
}


bool AxProgressDlg::SetJob( wxString msg )
{
    if (m_canceled)
        return false;

    msg += wxString::Format(" (%d/%d)", m_batchBar + 1 , m_maxBatchBar );
    this->GetTxMsgJob()->SetLabel( msg );
    this->GetGaugeJob()->SetValue( m_jobBar = 0 );
    this->GetGaugeBatch()->SetValue( m_batchBar++ * 100 / m_maxBatchBar );
    wxLogMessage( msg );
    wxGetApp().Yield( );

    return true; 
}

bool AxProgressDlg::IncTimerOperation( )
{
    if (m_canceled)
        return false;

    m_operationBar = this->m_stopWatch.Time() - m_timer_start;
    if ( ( m_maxOperationBar != 0 ) && ( m_operationBar * 100 / m_maxOperationBar < 99 ) ) 
	{
        // don't complete gauge if process is not complete: stay at 99% !!
        this->GetGaugeOperation()->SetValue( m_operationBar * 100 / m_maxOperationBar);
		UpdateJobBar( );
	}	
    if ( HasToBeUpdated() )
        wxGetApp().Yield( );
    
    return true;
}


void AxProgressDlg::StartTimerOperation( int code, int nb_units )
{
    m_timer_start = this->m_stopWatch.Time();
    m_timer_units = nb_units;
    switch (code)
    {
    case (TIMER_DECODING): m_maxOperationBar = s_timer_decoding; break;
    case (TIMER_MODEL_BIGRAM): m_maxOperationBar = s_timer_model_bigram; break;
	case (TIMER_FAST_ADAPTING): m_maxOperationBar = s_timer_fast_adapting; break;
	case (TIMER_FULL_ADAPTING): m_maxOperationBar = s_timer_full_adapting; break;
    }
    m_maxOperationBar *= m_timer_units;
}


void AxProgressDlg::EndTimerOperation( int code )
{
    int milliseconds = m_timer_units ? (this->m_stopWatch.Time() - m_timer_start) / m_timer_units : 0;
    switch (code)
    {
    case (TIMER_DECODING): s_timer_decoding = milliseconds; break;
    case (TIMER_MODEL_BIGRAM): s_timer_model_bigram = milliseconds; break;
	case (TIMER_FAST_ADAPTING): s_timer_fast_adapting = milliseconds; break;
	case (TIMER_FULL_ADAPTING): s_timer_full_adapting = milliseconds; break;
    }
}

bool AxProgressDlg::HasToBeUpdated()
{
    if ( this->m_lastWatch < this->m_stopWatch.Time() - 100 )
    {
        this->m_lastWatch = this->m_stopWatch.Time();
        this->m_ts = wxTimeSpan::Seconds( this->m_stopWatch.Time() / 1000  );
        if ( !this->m_ts.IsEqualTo( this->m_old_ts ) )
            this->GetTxRestProgress()->SetLabel( this->m_ts.Format() );
        this->m_old_ts = this->m_ts;
        return true;
    }
    else
        return false;
}

// static methods

void AxProgressDlg::LoadValues( wxConfigBase *pConfig )
{
	AxProgressDlg::s_close_at_end = (pConfig->Read("ProgressCloseAtEnd",0L)==1);

    pConfig->Read("Timer_decoding",&s_timer_decoding, 20000);
    pConfig->Read("Timer_model_bigram",&s_timer_model_bigram, 5000);
	pConfig->Read("Timer_fast_adapting",&s_timer_fast_adapting, 20000);
	pConfig->Read("Timer_full_adapting",&s_timer_full_adapting, 20000);
}

void AxProgressDlg::SaveValues( wxConfigBase *pConfig )
{
	pConfig->Write("ProgressCloseAtEnd", AxProgressDlg::s_close_at_end);

    pConfig->Write("Timer_decoding",s_timer_decoding);
    pConfig->Write("Timer_model_bigram",s_timer_model_bigram);
	pConfig->Write("Timer_fast_adapting",s_timer_fast_adapting);
	pConfig->Write("Timer_full_adapting",s_timer_full_adapting);
}

void AxProgressDlg::OnCancel(wxCommandEvent &event )
{
    if ( m_canceled )
    {
        event.Skip();
        return;
    }
    int res = wxYES;
	
	// doesn't work with wxSTAY_ON_TOP
	//int res = wxMessageBox("Cancel operation?", wxGetApp().GetAppName() ,
    //                        wxYES | wxNO | wxICON_QUESTION, this );
    if (res == wxYES)
    {
        wxLogMessage(_("Wait ...") );
        m_canceled = true;
        if ( m_counter != -1 )
        {
            imCounterEnd( m_counter );
            m_counter = -1;
        }
    }
}


int imDlgCounter(int counter, void* user_data, const char* text, int progress)
{
   AxProgressDlg *dlg = NULL;
   if ( user_data && ((wxObject*)user_data)->IsKindOf( CLASSINFO( AxProgressDlg ) ) )
       dlg = (AxProgressDlg*)user_data;
	   
	if ( dlg && dlg->GetCounter() != counter )
		return 1;


    // start of a sequence
    if (progress == -1)
    {
        if (dlg)
        {
            //dlg->GetTxMsgOperation()->SetLabel( text );
            dlg->GetGaugeOperation()->SetValue( 0 );
            wxGetApp().Yield( );
        }
        return 1;
    }

    //if (text && dlg)
    //    dlg->GetTxMsg3Progress()->SetLabel( text );

    /* end of sequence */
    if (progress == 1001)
    {
        //wxLogMessage("1001");
        dlg->SetCanceled( true );
        dlg->GetCancel()->SetLabel( _("Close") );
        dlg->GetGaugeOperation()->SetValue( 100 );
        dlg->GetGaugeJob()->SetValue( 100 );
        dlg->GetGaugeBatch()->SetValue( 100 );
        //wxGetApp().Yield( );
        return 1;
    }

    /* Now we must be between 0-1000 */
    if (dlg)
    {
        if ( dlg->HasToBeUpdated() )
        {
            dlg->GetGaugeOperation()->SetValue( progress / 10 );
			dlg->UpdateJobBar( );
            wxGetApp().Yield( );
        }
    }
    return 1;
}

