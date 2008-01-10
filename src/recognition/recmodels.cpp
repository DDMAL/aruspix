/////////////////////////////////////////////////////////////////////////////
// Name:        recmodels.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "recmodels.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"
#include "wx/txtstrm.h"
#include "wx/stdpaths.h"

#include "recmodels.h"
#include "recfile.h"

#include "app/axapp.h"
#include "app/axprocess.h"

#include "mus/musiomlf.h"

// WDR: class implementations


//----------------------------------------------------------------------------
// RecModel
//----------------------------------------------------------------------------

RecModel::RecModel( wxString filename, int type ) :
	AxFile( filename, type, AX_FILE_RECOGNITION )
{
	m_mlfoutput = NULL;
	m_nbfiles = 0;
}


RecModel::~RecModel()
{
	if ( m_mlfoutput )	
		delete m_mlfoutput; 
}

void RecModel::OpenContent( )
{
    wxASSERT( m_xml_root );
	wxASSERT( !m_mlfoutput );
	
	wxString fname = m_basename + "mlf";
	//wxFile *load = new wxFile( fname, wxFile::write_append );
	wxFile load( fname, wxFile::write_append );
	m_mlfoutput = new MusMLFOutput( NULL, load.fd(), fname, "MusMLFSymbol" );
	load.Detach();
	if ( wxFileExists( m_basename + "dic_cache" ) )
		m_mlfoutput->LoadSymbolDictionary( m_basename + "dic_cache" );

    TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
    //TiXmlElement *elem = NULL;
    
    // book information
    node = m_xml_root->FirstChild( "cache" );
    if ( !node ) return;        
    root = node->ToElement();
    if ( !root ) return;

    if ( root->Attribute("nbfiles"))
        m_nbfiles = atoi(root->Attribute("nbfiles"));
		
	OpenModelContent();
}

void RecModel::SaveContent( )
{
    wxASSERT( m_xml_root );
	wxASSERT( m_mlfoutput );

    TiXmlElement axfiles("axfiles");
    for ( int i = 0; i < (int)m_files.GetCount(); i++)
    {
        TiXmlElement axfile("axfile");
        axfile.SetAttribute("filename", m_files[i] );
        axfiles.InsertEndChild( axfile );
    }   
    m_xml_root->InsertEndChild( axfiles );

    TiXmlElement cache("cache"); 
    cache.SetAttribute("nbfiles", wxString::Format("%d", m_nbfiles ).c_str() );
    m_xml_root->InsertEndChild( cache );
	
	SaveModelContent();
}

bool RecModel::AddFiles( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
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
		
		m_files.Add( wxFileName( paths[i] ).GetFullName() ) ;
		
		wxArrayPtrVoid params;
		params.Add( &paths[i] );
		params.Add( &i );
		params.Add( &nbOfFiles );
		
		if ( !failed )
			failed = !this->AddFile( params, dlg );
    }

	return !failed;
}

// WDR: handler implementations for RecModel




//----------------------------------------------------------------------------
// RecTypModel
//----------------------------------------------------------------------------

RecTypModel::RecTypModel( wxString filename ) :
	RecModel( filename, AX_FILE_TYP_MODEL )
{
}


RecTypModel::~RecTypModel()
{
}


void RecTypModel::NewContent( )
{
	wxASSERT( !m_mlfoutput );

	m_mlfoutput = new MusMLFOutput( NULL, m_basename + "mlf", "MusMLFSymbol" );
	m_mlfoutput->CreateSubFile();
	m_nbfiles = 0;
	//mlfoutput->LoadTypes( types );
	//m_mlfoutput->m_writePosition = RecEnv::s_train_positions;
	//mlfoutput->m_addPageNo = false;
}

void RecTypModel::CloseContent( )
{
	if ( m_mlfoutput )	
		delete m_mlfoutput; 
	m_mlfoutput = NULL;
}


void RecTypModel::OpenModelContent( )
{
    wxASSERT( m_xml_root );
	wxASSERT( m_mlfoutput );
	
	m_mlfoutput->LoadSubFile();
}


void RecTypModel::SaveModelContent( )
{
    wxASSERT( m_xml_root );
	wxASSERT( m_mlfoutput );
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



bool RecTypModel::AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
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

bool RecTypModel::Commit( AxProgressDlg *dlg )
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
	
    count = 5;
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
	
	m_mlfoutput->WriteSymbolDictionary( m_basename + "dic_cache" );
	imCounterInc( dlg->GetCounter() );
	
	return true;

}

bool RecTypModel::Train( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	return true;
}

bool RecTypModel::Adapt( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
	// params 0: nbfiles (unused)
	// params 1: paths (unused)
	// params 2: filenames (unused)
	// params 3: is the cache ok ? (unused)
	// params 4: fast adaptation ?
    // params 5: RecTypModel: input
	// params 6: RecTypModel: output
	
	bool fast =  *(bool*)params[4];
	RecTypModel *typModelPtr = (RecTypModel*)params[5];
	RecTypModel *outModelPtr = (RecTypModel*)params[6];
	
	double threshold = TYP_THRESHOLD;
	int iter = TYP_ITER;
	int n_gaussians = TYP_N_GAUSSIANS;
	
	// map factor calculated according to the number of files
	double map_factor = 0.5;
	if ( fast )
		map_factor -= (m_nbfiles - 1) * 0.02;
	else
		map_factor -= (m_nbfiles - 1) * 0.01;
	if ( map_factor <= 0.0 )
		map_factor = 0.01;
	//wxLogDebug("MAP factor = %f", map_factor );
	
	bool viterbi = true;
	bool symbol = false;

#ifdef __WXMSW__
	#if defined(_DEBUG)
		wxString cmd = "AdaptD.exe";
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
	
	wxString log = "\"" + wxGetApp().m_logDir + "/adapt.log\"";
	
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
	
	wxLocale set( wxLANGUAGE_ENGLISH );
	
	args << " -dir " << wxGetApp().m_workingDir.c_str();
	args << " -log_fname " << log.c_str();
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
	if ( fast )
		args << " -adapt_separate";

	args << " -file_states " << states.c_str();
	args << " " << in_symb.c_str() << " " << in_dic.c_str();
	args << " " << data_symb.c_str() << " " << data_dic.c_str();
	args << " " << out_symb.c_str() << " " << out_dic.c_str();
	args << " " << mfcs.c_str() << " " << alis.c_str();
	args << " " << in_models.c_str() << " " << out_models.c_str();
	
	wxLocale reset( wxGetApp().m_locale.GetLanguage() );

	wxLogDebug(args);
	//printf(args.c_str());
	
	dlg->SetMaxJobBar( 1 ); 
	dlg->SetJob( _("Generate the optimized typographic model") );
    wxYield();
	if (!dlg->SetOperation( _("Optimizing...") ) )
		return this->Terminate( ERR_CANCELED );
		
	int dlg_timer = fast ? TIMER_FAST_ADAPTING : TIMER_FULL_ADAPTING;

	dlg->StartTimerOperation( dlg_timer, m_nbfiles );
	AxProcess *process = new AxProcess( cmd, args, NULL );
	if ( process->Start() )
	{
		process->Detach();
		process->m_deleteOnTerminate = false;
		int pid = process->GetPid();
		while ( process->GetPid() == pid )
		{
			wxMilliSleep( 200 );
			if( !dlg->IncTimerOperation( ) )
			{
				process->m_deleteOnTerminate = true;
				process->m_canceled = true;
				wxKill( pid, wxSIGKILL ); 
				return this->Terminate( ERR_CANCELED );
			}
		}
		
	}
	if ( process->m_status != 0 )
	{
		delete process;
		return this->Terminate( ERR_UNKNOWN );
	}
	dlg->EndTimerOperation( dlg_timer );
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
	RecModel( filename, AX_FILE_MUS_MODEL )
{
	m_nbfiles = 0;
	m_order = MUS_NGRAM_ORDER;
}


RecMusModel::~RecMusModel()
{
}


void RecMusModel::NewContent( )
{
	wxASSERT( !m_mlfoutput );

	// copy dictionnary (just to keep everything in directory)
	//???????wxString model_dic = output + "no_mapping.dic";
	//wxCopyFile( dic, model_dic, true );

    m_mlfoutput = new MusMLFOutput( NULL, m_basename + "ngram.mlf", "MusMLFSymbol" );
	m_mlfoutput->m_addPageNo = false;
}

void RecMusModel::CloseContent( )
{
	if ( m_mlfoutput )	
		delete m_mlfoutput; 
	m_mlfoutput = NULL;
}

void RecMusModel::OpenModelContent( )
{
    wxASSERT( m_xml_root );
	wxASSERT( m_mlfoutput );
	

	
	m_mlfoutput->m_addPageNo = false;

}


void RecMusModel::SaveModelContent( )
{
    wxASSERT( m_xml_root );
	wxASSERT( m_mlfoutput );
}



bool RecMusModel::AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	wxASSERT( m_mlfoutput );
	
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
    int count = 1;
    imCounterTotal( counter, count , operation.c_str() );
	
	bool failed = false;

	if ( !failed && !dlg->GetCanceled() )
		failed = !m_mlfoutput->WritePage( &recFile.m_wgFilePtr->m_pages[0], true );
	imCounterInc( dlg->GetCounter() );	
		
	return true;
}

bool RecMusModel::Commit( AxProgressDlg *dlg )
{
	wxASSERT( m_mlfoutput );
	
	dlg->SetMaxJobBar( 3 );
	dlg->SetJob( _("Commit the data set") );
	// counter
    int counter = dlg->GetCounter();
	
	m_mlfoutput->Close();
	
    int count = 2;
	dlg->SetOperation( _("Write files ...") );
    imCounterTotal( counter, count , "Write files ..." );
	
	wxString model_dic = m_basename + "ngram.dic";
	m_mlfoutput->WriteDictionary( model_dic );
	imCounterInc( dlg->GetCounter() );
	
	m_mlfoutput->WriteSymbolDictionary( m_basename + "dic_cache" );
	imCounterInc( dlg->GetCounter() );
	
	return true;
}
	
bool RecMusModel::Train( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
	wxString mlffile =  m_basename + "ngram.mlf";
	wxString vocfile =  m_basename + "ngram.dic";
	wxString outputfile =  m_basename + "ngram.gram";
	wxString outputdatafile =  m_basename + "ngram.data";

#ifdef __WXMSW__
	#if defined(_DEBUG)
		wxString cmd = "NgramD.exe";
	#else
		wxString cmd = "Ngram.exe";
	#endif   
#elif __WXGTK__
	#if defined(_DEBUG)
		wxString cmd = "ngramd";
	#else
		wxString cmd = "ngram";
	#endif   
#elif __WXMAC__
	#if defined(__AXDEBUG__)
		wxString cmd = "ngramd";
	#else
		wxString cmd = "ngram";
	#endif   
#endif

	wxString log = "\"" + wxGetApp().m_logDir + "/ngram.log\"";

	wxString args = " ";

	args << " -log_fname " << log.c_str();
	args << " -data_fname " << outputdatafile.c_str();
	args << " " << mlffile.c_str();
	args << " " << vocfile.c_str();
	args << " " << m_order;
	args << " " << outputfile.c_str();

	wxLogDebug(args);
	
	dlg->SetMaxJobBar( 1 );
	dlg->SetJob( "Generate the new model" );
	wxYield();
	if (!dlg->SetOperation(_("Generate the new model") ) )
		return this->Terminate( ERR_CANCELED );

	dlg->StartTimerOperation( TIMER_MODEL_BIGRAM, m_nbfiles );
	AxProcess *process = new AxProcess( cmd, args, NULL );
	if ( process->Start() )
	{
		process->Detach();
		process->m_deleteOnTerminate = false;
		int pid = process->GetPid();
		while ( process->GetPid() == pid )
		{
			wxMilliSleep( 200 );
			//wxSafeYield();
			if ( !dlg->IncTimerOperation() )
			{
				process->m_deleteOnTerminate = true;
				process->m_canceled = true;
				wxKill( pid, wxSIGKILL ); 
				return this->Terminate( ERR_CANCELED );
			}	
		}
	}
	if ( process->m_status != 0 )
	{
		delete process;
		return this->Terminate( ERR_UNKNOWN );
	}
	dlg->EndTimerOperation( TIMER_MODEL_BIGRAM );
	delete process;
	
	return true;

}


bool RecMusModel::Adapt( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
	// params 0: nbfiles (unused)
	// params 1: paths (unused)
	// params 2: is the cache ok ? (unused)
    // params 3: RecMusModel: input
	// params 4: RecTypModel: input (for dictionnary)
	
	RecMusModel *musModelPtr = (RecMusModel*)params[3];
	RecTypModel *typModelPtr = (RecTypModel*)params[4];
	
	wxString mlffile =  m_basename + "ngram.mlf";
	wxString vocfile =  m_basename + "ngram.dic";
	wxString merged_vocfile = typModelPtr->m_basename + "dic";
	wxString outputfile =  m_basename + "ngram.gram";
	wxString outputdatafile =  m_basename + "ngram.data";
	wxString inputdatafile = musModelPtr->m_basename + "ngram.data";
	
	// copy dictionnary
	wxCopyFile( merged_vocfile, vocfile );

#ifdef __WXMSW__
	#if defined(_DEBUG)
		wxString cmd = "NgramD.exe";
	#else
		wxString cmd = "Ngram.exe";
	#endif   
#elif __WXGTK__
	#if defined(_DEBUG)
		wxString cmd = "ngramd";
	#else
		wxString cmd = "ngram";
	#endif   
#elif __WXMAC__
	#if defined(__AXDEBUG__)
		wxString cmd = "ngramd";
	#else
		wxString cmd = "ngram";
	#endif   
#endif

	wxString log = "\"" + wxGetApp().m_logDir + "/ngram.log\"";

	wxString args = " ";

	args << " -log_fname " << log.c_str();
	args << " -reload_data_fname " << inputdatafile.c_str();
	args << " -data_fname " << outputdatafile.c_str();
	args << " " << mlffile.c_str();
	args << " " << vocfile.c_str();
	args << " " << m_order;
	args << " " << outputfile.c_str();

	wxLogDebug(args);
	
	dlg->SetMaxJobBar( 1 );
	dlg->SetJob( "Generate the optimized music model" );
	wxYield();
	if (!dlg->SetOperation(_("Optimizing...") ) )
		return this->Terminate( ERR_CANCELED );

	dlg->StartTimerOperation( TIMER_MODEL_BIGRAM, m_nbfiles );
	AxProcess *process = new AxProcess( cmd, args, NULL );
	if ( process->Start() )
	{
		process->Detach();
		process->m_deleteOnTerminate = false;
		int pid = process->GetPid();
		while ( process->GetPid() == pid )
		{
			wxMilliSleep( 200 );
			//wxSafeYield();
			if ( !dlg->IncTimerOperation() )
			{
				process->m_deleteOnTerminate = true;
				process->m_canceled = true;
				wxKill( pid, wxSIGKILL ); 
				return this->Terminate( ERR_CANCELED );
			}
		}
	}
	if ( process->m_status != 0 )
	{
		delete process;
		return this->Terminate( ERR_UNKNOWN );
	}
	dlg->EndTimerOperation( TIMER_MODEL_BIGRAM );
	delete process;
	
	return true;

	/*
	wxASSERT( m_mlfoutput );
	
	dlg->SetMaxJobBar( 1 );
		
	dlg->SetMaxJobBar( 1 );
	
	dlg->SetJob( "Generate the new model" );
	dlg->SetOperation( _("N-gram model...")  );
	dlg->StartTimerOperation( TIMER_MODEL_BIGRAM, m_nbfiles );
	if ( m_mlfoutput->GenerateNGram( m_basename + "ngram", m_order, dlg  ) )
		dlg->EndTimerOperation( TIMER_MODEL_BIGRAM );
	else
		return this->Terminate( ERR_UNKNOWN );
	
	return true;
	*/
}

// WDR: handler implementations for RecMusModel

