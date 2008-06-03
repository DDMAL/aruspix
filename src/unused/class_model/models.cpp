/////////////////////////////////////////////////////////////////////////////
// Name:        models.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "models.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"
#include "wx/txtstrm.h"

#include "models.h"
#include "recfile.h"

#include "app/axapp.h"
#include "app/axprocess.h"

#include "wg/iomlf.h"


// WDR: class implementations

/*
//----------------------------------------------------------------------------
// RecModel
//----------------------------------------------------------------------------

RecModel::RecModel( wxString filename )
{
	m_isLoaded = false;
	m_filename = filename;
	wxFileName::SplitPath( filename, NULL, &m_shortname, NULL );
	m_basename = wxGetApp().m_workingDir + "/" + m_shortname + "/";
	
	if ( wxDirExists( m_basename ) )
		AxDirCleaner clean( m_basename );
	else
		wxMkdir( m_basename );
}


RecModel::~RecModel()
{

}



void RecModel::Read()
{
	wxZipEntryPtr entry;
    wxFFileInputStream in( m_filename );
    wxZipInputStream zip(in);

    while (entry.reset( zip.GetNextEntry()), entry.get() != NULL)
    {
        // access meta-data
        wxString name = entry->GetName();
		//wxLogMessage(name);
		this->ReadContent( &zip, name );
        // read 'zip' to access the entry's data
    }
	m_isLoaded = true;
}
	
	
void RecModel::Write()
{


}

*/

// WDR: handler implementations for RecModel




//----------------------------------------------------------------------------
// RecTypModel
//----------------------------------------------------------------------------

RecTypModel::RecTypModel( wxString filename ) :
	AxFile( filename, AX_FILE_TYP_MODEL )
{
	m_mlfoutput = NULL;
}


RecTypModel::~RecTypModel()
{
	//wxASSERT( m_mlfoutput );
	if ( m_mlfoutput )
		delete m_mlfoutput;
}


void RecTypModel::NewContent( )
{
	wxASSERT( !m_mlfoutput );

	m_mlfoutput = new MLFOutput( NULL, m_basename + "mlf", "MLFSymbole" );
	m_mlfoutput->CreateSubFile();
	m_nbfiles = 0;
	//mlfoutput->LoadTypes( types );
	//m_mlfoutput->m_writePosition = RecEnv::s_train_positions;
	//mlfoutput->m_addPageNo = false;
}

void RecTypModel::UpdateInputFiles( )
{
	wxRemoveFile( m_basename + "mfcs" );
	wxRemoveFile( m_basename + "labs" );
	wxRemoveFile( m_basename + "alis" );
	
	wxFile mfcs, labs, alis;
	mfcs.Open( m_basename + "mfcs", wxFile::write );
	labs.Open( m_basename + "labs", wxFile::write );
	alis.Open( m_basename + "alis", wxFile::write );
	
	wxASSERT( mfcs.IsOpened() );
	wxASSERT( labs.IsOpened() );
	wxASSERT( alis.IsOpened() );

	wxArrayString names;
	wxDir::GetAllFiles( m_basename, &names , "*.lab", wxDIR_FILES );
	
	m_nbfiles = (int)names.GetCount();
	for( int i = 0; i < (int)names.GetCount(); i++ )
	{
		mfcs.Write(	names[i].BeforeLast('.') + ".mfc\n" );
		labs.Write(	names[i].BeforeLast('.') + ".lab\n" );
		alis.Write(	names[i].BeforeLast('.') + ".ali\n" );
	}

	
	mfcs.Close();
	labs.Close();
	alis.Close();
}

bool RecTypModel::AddFiles( wxArrayPtrVoid params, ProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "ProgressDlg cannot be NULL" );
	
    // params 0: wxString: output_dir
	int nbOfFiles = *(int*)params[0];
	wxArrayString paths = *(wxArrayString*)params[1];

	dlg->SetMaxJobBar( nbOfFiles );
	dlg->SetJob( _("Add the files") );
	
	bool failed = false;

    for ( int i = 0; i < (int)nbOfFiles; i++ )
    {
        if ( dlg->GetCanceled() )
            break;
		
		wxArrayPtrVoid params;
		params.Add( &paths[i] );
		params.Add( &i );
		params.Add( &nbOfFiles );
		
		if ( !failed )
			failed = !this->AddFile( params, dlg );
    }

	return !failed;
}

bool RecTypModel::AddFile( wxArrayPtrVoid params, ProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "ProgressDlg cannot be NULL" );
	wxASSERT( m_mlfoutput );
	
	this->Modify();

    // params 0: wxString: output_dir
	wxString file = *(wxString*)params[0];
	int file_nb = *(int*)params[1];
	int nb_files = *(int*)params[2];
	
    wxString ext, shortname;
    wxFileName::SplitPath( file, NULL, &shortname, &ext );
	
	RecFile recFile( "rec_typ_model_add_file" );
	recFile.New();
	
	if ( !recFile.Open( file ) )
		return false;
	
	if ( !recFile.IsRecognized() ) 
	{
		wxLogWarning(_("File '%s' skipped"), file.c_str() );
		return true;
	}
	
	wxString operation =  wxString::Format( _("Adding data from file '%s' (%d/%d)"), shortname.c_str(), file_nb + 1, nb_files ) ;
	dlg->SetOperation( operation );
	
	wxArrayString names;
	wxDir::GetAllFiles( recFile.m_basename, &names , "*.mfc", wxDIR_FILES );
	
	// counter
    int counter = dlg->GetCounter();
    int count = names.GetCount() + 1;
    imCounterTotal( counter, count , operation.c_str() );
	
	m_mlfoutput->WritePage( &recFile.m_wgFilePtr->m_pages[0], shortname, recFile.m_imPagePtr );
	imCounterInc( dlg->GetCounter() );
	
	for( int i = 0; i < (int)names.GetCount(); i++ )
	{
		m_nbfiles++;
		wxString out = m_basename + shortname + "_" + names[i].AfterLast( '_' );
		wxCopyFile( names[i], out );
		imCounterInc( dlg->GetCounter() );
	}	
	
	return true;
}

bool RecTypModel::Commit( ProgressDlg *dlg )
{
	wxASSERT( m_mlfoutput );
	
	m_mlfoutput->Close();
	
	wxArrayString mfc_input, lab_input, ali_input;
	
	wxFileInputStream input( m_basename + "mlf" );
	if ( !input.Ok() )
		return false;
		
	wxTextInputStream text( input );
	wxFile output;
	int nbfiles = 0;
	
	dlg->SetMaxJobBar( 3 );
	dlg->SetJob( _("Commit the data set") );
    wxYield();
	// counter
    int counter = dlg->GetCounter();
    int count = m_nbfiles;
	dlg->SetOperation( _("Generate label files ...") );
    imCounterTotal( counter, count , "Generate label files ..." );

	while( !input.Eof() )
	{
		wxString str = text.ReadLine();
		if ( str.IsEmpty() || ( str[0]=='#' ) )
			continue; // skip line
		else if ( str[0]=='"' )
		{
			wxASSERT( !output.IsOpened() );
			wxString out = str.AfterFirst('/').BeforeLast('.');
			output.Open( m_basename + out + ".lab", wxFile::write );
			nbfiles++;
			imCounterInc( dlg->GetCounter() );
			
			wxASSERT_MSG( wxFileExists( m_basename + out + ".mfc" ), "MFC file is missing" );
		}
		else if ( str[0]=='.' )
		{
			wxASSERT( output.IsOpened() );
			output.Close();
		}
		else // write content
		{
			wxASSERT( output.IsOpened() );
			output.Write( str + "\n" );
		}
	}
	
	dlg->SetOperation( _("Generate aligned label files ...") );
    imCounterTotal( counter, count , "Generate aligned label files ..." );
	
	// align targets
		wxFileInputStream input_align( m_basename + "mlf_align" );
	if ( !input_align.Ok() )
		return false;
		
	wxTextInputStream text_align( input_align );
	nbfiles = 0;

	while( !input_align.Eof() )
	{
		wxString str = text_align.ReadLine();
		if ( str.IsEmpty() || ( str[0]=='#' ) )
			continue; // skip line
		else if ( str[0]=='"' )
		{
			wxASSERT( !output.IsOpened() );
			wxString out = str.AfterFirst('/').BeforeLast('.');
			output.Open( m_basename + out + ".ali", wxFile::write );
			nbfiles++;
			imCounterInc( dlg->GetCounter() );
			wxASSERT_MSG( wxFileExists( m_basename + out + ".mfc" ), "MFC file is missing" );
		}
		else if ( str[0]=='.' )
		{
			wxASSERT( output.IsOpened() );
			output.Close();
		}
		else // write content
		{
			wxASSERT( output.IsOpened() );
			output.Write( str + "\n" );
		}
	}
	
    count = 4;
	dlg->SetOperation( _("Write files ...") );
    imCounterTotal( counter, count , "Write files ..." );
	
	UpdateInputFiles();
	imCounterInc( dlg->GetCounter() );
	
	//wxASSERT_MSG( nbfiles == m_nbfiles, "Number of MFC and LAB files mismatch");

	//write dictionary
	m_mlfoutput->WriteDictionary( m_basename + "dic" );
	imCounterInc( dlg->GetCounter() );
	
	//write states per symbol
	//wxString states_filename = outdir + "/states";
	m_mlfoutput->WriteStatesPerSymbol( m_basename + "states" );
	imCounterInc( dlg->GetCounter() );
	
	//write symbols
	//wxString symb_filename = outdir + "/symb";
	m_mlfoutput->WriteHMMSymbols( m_basename + "symb" );
	imCounterInc( dlg->GetCounter() );
	
	return true;

}

bool RecTypModel::Adapt( wxArrayPtrVoid params, ProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "ProgressDlg cannot be NULL" );
	
    // params 0: RecTypModel: typModelPtr
	// params 1: RecMusModel: musModelPtr
	// params 2: bool: rec_delayed
	// params 3: int: rec_lm_order
	// params 4: double: rec_lm_scaling
	// params 5: wxString: rec_wrdtrns
	
	RecTypModel *typModelPtr = (RecTypModel*)params[0];
	RecTypModel *outModelPtr = (RecTypModel*)params[1];
	//bool rec_delayed =  *(bool*)params[2];
	//int rec_lm_order =  *(int*)params[3];
	//double rec_lm_scaling =  *(double*)params[4];
	//wxString rec_wrdtrns =  *(wxString*)params[5];
	
	double threshold = 0.01;
	int iter = 100;
	int n_gaussians = 5;
	double map_factor = 0.5;
	bool viterbi = true;
	bool symbol = false;

#ifdef __WXMSW__
	#if defined(_DEBUG)
		wxString cmd = "Adapt.exe";
	#else
		wxString cmd = "Adapt.exe";
	#endif   
#elif __WXGTK__
	#if defined(__DEBUG__)
		wxString cmd = "adaptd";
	#else
		wxString cmd = "adapt"; 
	#endif   
#elif __WXMAC__
	#ifdef __AXDEBUG__
		wxString cmd = "adaptd";
	#else
		wxString cmd = "adapt";
	#endif   
#endif

	wxString args = " ";
	
	wxString states = m_basename + "states3";
	// input
	wxString in_dic = typModelPtr->m_basename + "dic";
	wxString in_symb = typModelPtr->m_basename + "symb";
	// data (current)
	wxString data_dic = m_basename + "dic";
	wxString data_symb = m_basename + "symb";
	// output
	wxString out_dic = outModelPtr->m_basename + "dic";
	wxString out_symb = outModelPtr->m_basename + "symb";
	// data (mfcs and labs)
	wxString mfcs = m_basename + "mfcs";
	wxString alis = m_basename + "alis";
	// hmm
	wxString in_models = typModelPtr->m_basename + "hmm";
	wxString out_models = outModelPtr->m_basename + "hmm";

	args << " -dir " << wxGetApp().m_workingDir.c_str();
	args << " -spacing_model \"{s}\"";
	args << " -threshold " << threshold;
	args << " -n_gaussians " << n_gaussians;
	args << " -iter " << iter;
	args << " -map " << map_factor;
	if ( viterbi )
		args << " -viterbi";
	if ( symbol )
	{
		args << " -symbol";
		alis = m_basename + "labs";
	}

	args << " -file_states " << states.c_str();
	args << " " << in_symb.c_str() << " " << in_dic.c_str();
	args << " " << data_symb.c_str() << " " << data_dic.c_str();
	args << " " << out_symb.c_str() << " " << out_dic.c_str();
	args << " " << mfcs.c_str() << " " << alis.c_str();
	args << " " << in_models.c_str() << " " << out_models.c_str();

	wxLogDebug(args);
	//printf(args.c_str());
	
	dlg->SetMaxJobBar( 1 ); 
	dlg->SetJob( _("Generate the new model") );
    wxYield();

	if (!dlg->SetOperation( _("Adapting...") ) )
		return this->Terminate( ERR_CANCELED );

	dlg->StartTimerOperation( TIMER_ADAPTING, m_nbfiles );
	AxProcess *process = new AxProcess( cmd, args, NULL );
	if ( process->Start() )
	{
		process->Detach();
		process->m_deleteOnTerminate = false;
		int pid = process->GetPid();
		while ( process->GetPid() == pid )
		{
			wxMilliSleep( 500 );
			if( !dlg->IncTimerOperation( ) )
			{
				process->m_deleteOnTerminate = true;
				wxKill( pid, wxSIGKILL ); 
				return this->Terminate( ERR_CANCELED );
			}
			//if ( process->IsInputAvailable() )
			//{
			//	wxTextInputStream tis( *process->GetInputStream() );
			//	wxLogMessage( tis.ReadLine() );
			//}
			//wxLogMessage("Still alive ...");
		}
		
	}
	if ( process->m_status != 0 )
	{
		delete process;
		return this->Terminate( ERR_UNKNOWN );
	}
	dlg->EndTimerOperation( TIMER_ADAPTING );
	delete process;
	
	outModelPtr->Modify();


	/*Torch::DiskXFile::setBigEndianMode() ;

	wxString input = wxGetApp().m_workingDir + "/" + imPage->GetShortName() + ".input";
	input.Replace( "\\/", "/" );

	Torch::LexiconInfo lex_info( m_rec_models.c_str() , "{s}" , "" , m_rec_dict.c_str() , 
                          "" , "" , "" ) ;

    Torch::PhoneModels phone_models ( lex_info.phone_info , (char*) m_rec_models.c_str(),
							   true , m_rec_phone_pen , 
                               false , "" , "" , 
                               9 , "" , false , 
                               (real)0.005 , (real)0.005 ) ;

    Torch::LinearLexicon lexicon( &lex_info , &phone_models ) ;

    Torch::LanguageModel *lang_model;
    if ( m_rec_lm_order <= 0 )
        lang_model = NULL ;
    else
    {
        lang_model = new Torch::LanguageModel( m_rec_lm_order , lex_info.vocabulary , 
			(char*)m_rec_lm.c_str() , m_rec_ls_scaling ) ;
    }

	real end_prune = m_rec_end_prune;
	if ( end_prune == 0 )
		end_prune = LOG_ZERO;
    Torch::BeamSearchDecoder bs_decoder( &lexicon , lang_model , m_rec_word_pen ,
			LOG_ZERO, end_prune ,
			m_rec_delayed , false ) ;


    Torch::DecoderBatchTest batch_tester( (char*)input.c_str() , Torch::DST_FEATS_HTK , (char*)m_rec_wrdtrns.c_str()  , &bs_decoder , 
                                   true , true , (char*)m_rec_output.c_str() , false , 10.0 ) ;
    	
	batch_tester.run() ;

    if ( lang_model != NULL )
        delete lang_model ; 
    return(0) ;*/

	return true;
}

// WDR: handler implementations for RecTypModel



//----------------------------------------------------------------------------
// RecMusModel
//----------------------------------------------------------------------------

RecMusModel::RecMusModel( wxString filename ) :
	AxFile( filename, AX_FILE_TYP_MODEL )
{
	m_mlf_no_mapping = NULL;
	m_mlf_symb = NULL;
	m_mlf_duration = NULL; 
	m_mlf_pitch = NULL; 
	m_mlf_intervals = NULL;
}


RecMusModel::~RecMusModel()
{
	if ( m_mlf_no_mapping )	
		delete m_mlf_no_mapping;
	if ( m_mlf_symb )
		delete m_mlf_symb;
	if ( m_mlf_duration )
		delete m_mlf_duration; 
	if ( m_mlf_pitch )
		delete m_mlf_pitch; 
	if ( m_mlf_intervals )
		delete m_mlf_intervals; 
}


void RecMusModel::NewContent( )
{
	wxASSERT( !m_mlf_no_mapping );
	wxASSERT( !m_mlf_symb );
	wxASSERT( !m_mlf_duration );
	wxASSERT( !m_mlf_pitch );
	wxASSERT( !m_mlf_intervals );
	
	m_nbfiles = 0;
	
	// order
	//m_order = wxGetNumberFromUser( "NGram order", "", "",  3, 1, 20);
	m_order = 3;
	if ( m_order == -1 )
		m_order = 2;

	// copy dictionnary (just to keep everything in directory)
	//???????wxString model_dic = output + "no_mapping.dic";
	//wxCopyFile( dic, model_dic, true );

    m_mlf_no_mapping = new MLFOutput( NULL, m_basename + "no_mapping.mlf", "MLFSymbole" );
	m_mlf_no_mapping->m_addPageNo = false;
	m_mlf_symb = new MLFOutput( NULL, m_basename + "symb.mlf", "MLFClassSymb" );
	m_mlf_symb->m_addPageNo = false;
    m_mlf_duration = new MLFOutput( NULL, m_basename + "duration.mlf", "MLFClassDuration" );
	m_mlf_duration->m_addPageNo = false;
    m_mlf_pitch = new MLFOutput( NULL, m_basename + "pitch.mlf", "MLFClassPitch" );
	m_mlf_pitch->m_addPageNo = false;;
    m_mlf_intervals = new MLFOutput( NULL, m_basename + "interval.notes.mlf", "MLFClassInterval" );
	m_mlf_intervals->m_addPageNo = false;

}


bool RecMusModel::AddFiles( wxArrayPtrVoid params, ProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "ProgressDlg cannot be NULL" );
	
    // params 0: wxString: output_dir
	int nbOfFiles = *(int*)params[0];
	wxArrayString paths = *(wxArrayString*)params[1];

	dlg->SetMaxJobBar( nbOfFiles );
	dlg->SetJob( _("Add the files") );
	
	bool failed = false;

    for ( int i = 0; i < (int)nbOfFiles; i++ )
    {
        if ( dlg->GetCanceled() )
            break;
		
		wxArrayPtrVoid params;
		params.Add( &paths[i] );
		params.Add( &i );
		params.Add( &nbOfFiles );
		
		if ( !failed )
			failed = !this->AddFile( params, dlg );
    }

	return !failed;
}



bool RecMusModel::AddFile( wxArrayPtrVoid params, ProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "ProgressDlg cannot be NULL" );

	wxASSERT( !m_mlf_no_mapping );
	wxASSERT( !m_mlf_symb );
	wxASSERT( !m_mlf_duration );
	wxASSERT( !m_mlf_pitch );
	wxASSERT( !m_mlf_intervals );
	
	this->Modify();

    // params 0: wxString: output_dir
	wxString file = *(wxString*)params[0];
	int file_nb = *(int*)params[1];
	int nb_files = *(int*)params[2];
	
    wxString ext, shortname;
    wxFileName::SplitPath( file, NULL, &shortname, &ext );
	
	RecFile recFile( "rec_mus_model_add_file" );
	recFile.New();
	
	if ( !recFile.Open( file ) )
		return false;
	
	if ( !recFile.IsRecognized() ) 
	{
		wxLogWarning(_("File '%s' skipped"), file.c_str() );
		return true;
	}
	
	wxString operation =  wxString::Format( _("Adding data from file '%s' (%d/%d)"), shortname.c_str(), file_nb + 1, nb_files ) ;
	dlg->SetOperation( operation );
	// counter
    int counter = dlg->GetCounter();
    int count = 5;
    imCounterTotal( counter, count , operation.c_str() );
	
	//??	WgFile wg_file;
	//??	WwgInput *wwginput = new WwgInput( &wg_file, paths[i] );
	//??	if ( !failed )
	//??		failed = !wwginput->ImportFile();
	//??	delete wwginput;
	
	bool failed = false;

	// no_mapping
	if ( !failed && !dlg->GetCancel() )
		failed = !m_mlf_no_mapping->WritePage( &recFile.m_wgFilePtr->m_pages[0], true );
	imCounterInc( dlg->GetCounter() );	

	// symbole
	if ( !failed && !dlg->GetCancel() )
		failed = !m_mlf_symb->WritePage( &recFile.m_wgFilePtr->m_pages[0], true );
	imCounterInc( dlg->GetCounter() );

	// duration
	if ( !failed && !dlg->GetCancel() )
		failed = !m_mlf_duration->WritePage( &recFile.m_wgFilePtr->m_pages[0], true );
	imCounterInc( dlg->GetCounter() );

	// pitch
	if ( !failed && !dlg->GetCancel() )
		failed = !m_mlf_pitch->WritePage( &recFile.m_wgFilePtr->m_pages[0], true );
	imCounterInc( dlg->GetCounter() );

	// interval - always order = 3
	if ( !failed && !dlg->GetCancel() )
		failed = !m_mlf_intervals->WritePage( &recFile.m_wgFilePtr->m_pages[0], true );
	imCounterInc( dlg->GetCounter() );
		
	return !failed;
}

bool RecMusModel::Commit( ProgressDlg *dlg )
{
	wxASSERT( !m_mlf_no_mapping );
	wxASSERT( !m_mlf_symb );
	wxASSERT( !m_mlf_duration );
	wxASSERT( !m_mlf_pitch );
	wxASSERT( !m_mlf_intervals );
	
	m_mlf_no_mapping->Close();
	wxString model_dic = m_basename + "no_mapping.dic";
	m_mlf_no_mapping->WriteDictionary( model_dic );
	wxCopyFile( model_dic, m_basename + "dic", true );
	
	if ( dlg->SetJob( "no_mapping.gram" ) )
	{
		dlg->SetOperation( _("Bigram model...")  );
		dlg->StartTimerOperation( TIMER_MODEL_BIGRAM, m_nbfiles );
		if ( m_mlf_no_mapping->GenerateNGram( m_basename + "no_mapping", 2, dlg  ) )
			dlg->EndTimerOperation( TIMER_MODEL_BIGRAM );
	}
	
	if ( dlg->SetJob( "symb.gram" ) )
	{
		dlg->SetOperation( _("Symboles model...")  );
		m_mlf_symb->LoadDictionary( model_dic );
		m_mlf_symb->GenerateMapping( m_basename + "symb", m_basename + "no_mapping" );
		dlg->StartTimerOperation( TIMER_MODEL_SYMB, m_nbfiles );
		m_mlf_symb->GenerateNGram( m_basename + "symb", m_order, dlg );
			dlg->EndTimerOperation( TIMER_MODEL_SYMB );
	}

	if ( dlg->SetJob( "duration.gram" ) )
	{
		dlg->SetOperation( _("Durations model...")  );
		m_mlf_duration->LoadDictionary( model_dic );
		m_mlf_duration->GenerateMapping( m_basename + "duration", m_basename + "no_mapping" );
		dlg->StartTimerOperation( TIMER_MODEL_DURATION, m_nbfiles );
		if ( m_mlf_duration->GenerateNGram( m_basename + "duration", m_order, dlg  ) )
			dlg->EndTimerOperation( TIMER_MODEL_DURATION );
	}

	if ( dlg->SetJob( "pitch.gram" ) )
	{
		dlg->SetOperation( _("Pitches model...")  );
		m_mlf_pitch->LoadDictionary( model_dic );
		m_mlf_pitch->GenerateMapping( m_basename + "pitch", m_basename + "no_mapping" );
		dlg->StartTimerOperation( TIMER_MODEL_PITCH, m_nbfiles );
		if ( m_mlf_pitch->GenerateNGram( m_basename + "pitch", m_order, dlg  ) )
			dlg->EndTimerOperation( TIMER_MODEL_PITCH );
	}

	if ( dlg->SetJob( "interval.gram" ) )
	{
		dlg->SetOperation( _("Intervals model...")  );
		m_mlf_intervals->LoadDictionary( model_dic );
		m_mlf_intervals->GenerateMapping( m_basename + "interval", m_basename + "no_mapping", true );
		dlg->StartTimerOperation( TIMER_MODEL_INTERVAL, m_nbfiles );
		m_mlf_intervals->PitchMLFtoIntervalMLF( m_basename + "interval" );
		if ( m_mlf_intervals->GenerateNGram( m_basename + "interval", 3, dlg  ) )
			dlg->EndTimerOperation( TIMER_MODEL_INTERVAL );
	}
	
	return true;

}

// WDR: handler implementations for RecMusModel

