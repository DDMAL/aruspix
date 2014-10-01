/////////////////////////////////////////////////////////////////////////////
// Name:        axprogressdlg.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axprogressdlg_H__
#define __axprogressdlg_H__

#define TIMER_DECODING 0
#define TIMER_MODEL_BIGRAM 1
#define TIMER_FULL_ADAPTING 2
#define TIMER_FAST_ADAPTING 3

#define ERR_NONE 0
#define ERR_CANCELED 1
#define ERR_UNKNOWN 2
#define ERR_MEMORY 3
#define ERR_FILE 4
#define ERR_READING 5
#define ERR_WRITING 6

#ifdef AX_CMDLINE

#include "wx/string.h"

//----------------------------------------------------------------------------
// Commandline replacement class
//----------------------------------------------------------------------------

#include <im_counter.h>

class AxProgressDlg
{
public:
    // constructors and destructors
    AxProgressDlg() {};
    
    bool SetOperation( wxString msg ) { return true; };
    bool GetCanceled() { return false; };
    int GetCounter() { return 1; };
    bool SetJob( wxString msg  ) { return true; };
    void SetMaxJobBar( int value ) { };
    void StartTimerOperation( int code, int nb_units = 1 ) {};
    void EndTimerOperation( int code ) {};
    bool IncTimerOperation( ) { return true; };
    
};

#else

//----------------------------------------------------------------------------
// Gui
//----------------------------------------------------------------------------

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/timer.h"
#include "wx/datetime.h"

// IMLIB
#include <im_counter.h>

#include "axapp_wdr.h"


class wxConfigBase;

class AxProcess;

//----------------------------------------------------------------------------
// AxProgressDlg
//----------------------------------------------------------------------------

class AxProgressDlg: public wxDialog
{
public:
    // constructors and destructors
    AxProgressDlg();
    AxProgressDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL | wxSTAY_ON_TOP );
    virtual ~AxProgressDlg();

    // load and save timer values
    // must be done in application
    static void LoadValues( wxConfigBase *pConfig );
    static void SaveValues( wxConfigBase *pConfig );
    
    wxCheckBox* GetCbCloseAtEnd()  { return (wxCheckBox*) FindWindow( ID_CB_CLOSE_AT_END ); }
    wxButton* GetCancel()  { return (wxButton*) FindWindow( wxID_CANCEL ); }
    wxStaticText* GetTxRestProgress()  { return (wxStaticText*) FindWindow( ID_TX_REST_PROGRESS ); }
    wxGauge* GetGaugeOperation()  { return (wxGauge*) FindWindow( ID_GAUGE3_PROGRESS ); }
    wxGauge* GetGaugeJob()  { return (wxGauge*) FindWindow( ID_GAUGE2_PROGRESS ); }
    wxGauge* GetGaugeBatch()  { return (wxGauge*) FindWindow( ID_GAUGE1_PROGRESS ); }
    wxStaticText* GetTxMsgOperation()  { return (wxStaticText*) FindWindow( ID_TX_MSG3_PROGRESS ); }
    wxStaticText* GetTxMsgJob()  { return (wxStaticText*) FindWindow( ID_TX_MSG2_PROGRESS ); }
    wxTextCtrl* GetTcLogProgress()  { return (wxTextCtrl*) FindWindow( ID_TC_LOG_PROGRESS ); }
    // IM_LIB callback fonction as friend -> gere les barre de progression
    friend int imDlgCounter(int counter, void* user_data, const char* text, int progress);
    //
    void AxShowModal( bool failed = false ); // force show modal on os x
    void SuspendCounter();
    void ReactiveCounter();
    //bool IncOperation( ); // update gauge considering m_opBar et m_maxOpBar
    //                      // le plus souvent cette barre est geree par imDlgCounter
    bool SetOperation( wxString msg );
    bool SetJob( wxString msg  );
	void UpdateBatchBar( );
	void UpdateJobBar( );
    void SetMaxOperationBar( int value ) { m_maxOperationBar = value; };
    void SetMaxJobBar( int value ) { m_maxJobBar = value; };
    void SetMaxBatchBar( int value ) { m_maxBatchBar = value; };
    void SetCanceled( bool val ) { m_canceled = val; }
	void AddMaxBatchBar( int value ) { m_maxBatchBar += value; };
    // timer opertation
    void StartTimerOperation( int code, int nb_units = 1 );
        // le principe est de garder en memoire (variable statiques et dans le fichier de conf)
        // les temps d'execution pour certaines operations (code) et par unite (ex staff)
    void EndTimerOperation( int code );
    bool IncTimerOperation( );
    // etat
    bool HasToBeUpdated(); // check if the dialog must be updated ( only every 100 millisecond )
    int GetCounter( ) { return m_counter; }
    bool GetCanceled( ) { return m_canceled; }
    
private:
    wxStopWatch m_stopWatch;
    long m_lastWatch;
    wxTimeSpan m_ts, m_old_ts;
    int m_operationBar; // barre d'operation (3eme barre) // normalement geree par imDlgCounter
    int m_jobBar; // barre du traitement (2eme barre)
    int m_batchBar; // barre du batch global (1ere barre)
    int m_maxOperationBar;
    int m_maxJobBar;
    int m_maxBatchBar;
    wxString m_msg2, m_msg3;
    bool m_canceled;
    //time operation
    int m_timer_units;
    long m_timer_start;
	wxWindow *m_parent;


    // values are in milliseconds second per units
    static int s_timer_decoding; // time for staff decoding | one unit = on staff 
    static int s_timer_model_bigram;  // time for models generation | one unit = 1 file
	static int s_timer_full_adapting; // time for staff decoding | one unit = on staff
	static int s_timer_fast_adapting; // time for staff decoding | one unit = on staff

public:
    int m_counter;
	static bool s_close_at_end;
	static bool s_instance_existing;
    
private:
    void OnCancel( wxCommandEvent &event );

private:
    DECLARE_DYNAMIC_CLASS( AxProgressDlg )
    DECLARE_EVENT_TABLE()
};

// Callback function pour IMLIB
int imDlgCounter(int counter, void* user_data, const char* text, int progress);

#endif // AX_CMDLINE

#endif
