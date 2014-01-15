/////////////////////////////////////////////////////////////////////////////
// Name:        recfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/txtstrm.h"

#include "recfile.h"
#include "rec.h"
#include "recmodels.h"
#include "im/impage.h"
#include "im/imstaff.h"

#include "app/axapp.h"
#include "app/axprocess.h"

#include "vrv/doc.h"
#include "vrv/system.h"
#include "vrv/staff.h"
#include "vrv/measure.h"
#include "layerelement.h"
#include "vrv/iomei.h"

#include "musapp/musiomlf.h"
#include "musapp/musiobin.h"

using namespace vrv;

//#include "ml/mldecoder.h"



//----------------------------------------------------------------------------
// RecFile
//----------------------------------------------------------------------------

RecFile::RecFile( wxString name, RecEnv *env )
	: AxFile( name, AX_FILE_DEFAULT, AX_FILE_RECOGNITION )
{
	m_envPtr = env;

	m_musDocPtr = NULL;
	m_imPagePtr = NULL;
	
	m_isPreprocessed = false;
	m_isRecognized = false;
}



RecFile::~RecFile()
{
	if ( m_musDocPtr )
		delete m_musDocPtr;
	if ( m_imPagePtr )
		delete m_imPagePtr;
}

void RecFile::SetforMEI()
{
	m_isPreprocessed = true;
	m_isRecognized = true;
}

void RecFile::UpgradeTo_1_4_0()
{
	if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) >= AxFile::FormatVersion(1, 4, 0) )
		return; // when do not need to upgrade the file
		
	// From version 1.4.0, they are no staff segments anymore
	// Files with staff multiple segments cannot be upgraded
	// Margin were fixed in the music editor
	// To upgrade, we neww to remap the xml file

	
	if ( !wxFileExists( m_basename + "page.wwg") )
		return; // nothing has to be done for files that were only preprocessed
		
	bool failed = false;
		
	// map the xml file
	TiXmlElement *root = NULL;
    TiXmlNode *node1 = NULL;
	TiXmlNode *node2 = NULL;
    TiXmlElement *elem1 = NULL;
	TiXmlElement *elem2 = NULL;
	
	node1 = m_xml_root->FirstChild( "impage" );
	if ( !node1 ) 
		failed = true;
	else
		root = node1->ToElement();
    if ( !root )
		failed = true;

    // staves
    for( node1 = root->FirstChild( "staff" ); node1; node1 = node1->NextSibling( "staff" ) )
    {
		if ( failed )
			break;
			
		int x1 = 0;
		int x2 = 0;
        elem1 = node1->ToElement();
        if (!elem1)
			continue;
			
		int i = 0;
		for( node2 = elem1->FirstChild( "staffsegment" ); node2; node2 = node2->NextSibling( "staffsegment" ) )
		{
			if ( i > 0 ) // Upgrade cannot handle file with multiple segments
			{
				failed = true;
				break;
			}
			i++;
			elem2 = node2->ToElement();
			if (!elem2) 
				continue;
			elem2->Attribute("x1", &x1 );
			elem2->Attribute("x2", &x2 );
			elem2->Attribute("order", &m_rec_lm_order );
			elem2->Attribute("scaling", &m_rec_lm_scaling );
		}
		elem1->SetAttribute("x1", x1 );
		elem1->SetAttribute("x2", x2 );
    }
	
	if ( failed )
	{
		wxLogError(_("This file was generated with a previous version of Aruspix and the recognition results cannot be loaded"));
		this->CancelRecognition( false );
	}
	else
		wxLogMessage( "File successfully upgraded to 1.4.0");
}

void RecFile::UpgradeTo_1_5_0()
{
	if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) >= AxFile::FormatVersion(1, 5, 0) )
		return; // when do not need to upgrade the file
		
	// From version 1.5.0, we don't use wwg anymore, but aruspix binaries
    // We need to reload the page from the mlf
	
	if ( !wxFileExists( m_basename + "page.wwg") )
		return; // nothing has to be done for files that were only preprocessed
    // backup rec.mlf
	if ( !wxCopyFile( m_basename + "rec.mlf", m_basename + "rec.old.mlf") )  
        return;
    // we need to read page.mlf 
	if ( !wxCopyFile( m_basename + "page.mlf", m_basename + "rec.mlf", true) )  
        return;
        
    wxArrayPtrVoid params;
    
    if ( !this->RealizeFromMLF( params, NULL ) )
        return;
	if ( !wxCopyFile( m_basename + "rec.old.mlf", m_basename + "rec.mlf", true) )  
        return;
        
    // output the new binary file
    wxLogDebug("%s", m_musDocPtr->m_fname.c_str() );
    MeiOutput mei_output( m_musDocPtr, m_musDocPtr->m_fname );
    mei_output.ExportFile();
        
    wxRemoveFile( m_basename + "rec.old.mlf" );        
    wxRemoveFile( m_basename + "page.wwg" );
    
    wxLogMessage( "File successfully upgraded to 1.5.0");
}


void RecFile::UpgradeTo_2_0_0()
{
    if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) >= AxFile::FormatVersion(2, 0, 0) )
		return; // when do not need to upgrade the file
    if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) < AxFile::FormatVersion(1, 5, 0) )
        return; // the file has been upgraded by Upgrade_1_5_0
    
    if ( !wxFileExists( m_basename + "page.bin") )
        return; // file is missing
    // backup old binary file
    wxString backup = wxString::Format( "page.1.%d.%d.bin", m_vmin, m_vrev );
    if ( !wxCopyFile( m_basename + "page.bin", m_basename + backup ) )  
        return;
        
    /*
    MusBinInput_1_X bin_input( m_musDocPtr, m_basename + "page.bin" );
    bin_input.ImportFile();
    */
    
    // we need to read page.mlf 
    // backup rec.mlf
	if ( !wxCopyFile( m_basename + "rec.mlf", m_basename + "rec.old.mlf") )  
        return;
    // we need to read page.mlf 
	if ( !wxCopyFile( m_basename + "page.mlf", m_basename + "rec.mlf", true) )  
        return;
    
    wxArrayPtrVoid params;
    
    if ( !this->RealizeFromMLF( params, NULL ) )
        return;
	if ( !wxCopyFile( m_basename + "rec.old.mlf", m_basename + "rec.mlf", true) )  
        return;
    
    // output the new binary file
    MeiOutput mei_output( m_musDocPtr, m_musDocPtr->m_fname );
    mei_output.ExportFile(); 
    wxLogMessage( "File successfully upgraded to 2.0.0");
    this->Modify();
}


void RecFile::UpgradeTo_2_1_0()
{
    if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) >= AxFile::FormatVersion(2, 1, 0) )
		return; // when do not need to upgrade the file
    if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) < AxFile::FormatVersion(2, 0, 0) )
        return; // the file has been upgraded by Upgrade_2_0_0
    
    /*
    if ( !m_musDocPtr ) {
        wxLogError( "File cannot be upgraded to 2.1.0 (No document)");    
        return;
    }
    if ( m_musDocPtr->m_layouts.GetCount() != 1 ) {
        wxLogError( "File cannot be upgraded to 2.1.0 (Wrong number of layouts)");    
        return;
    }
    if ( m_musDocPtr->m_layouts[0].m_pages.GetCount() != 1  ) {
        wxLogError( "File cannot be upgraded to 2.1.0 (Wrong number of pages)");    
        return;
    }
    
    Page *page = &m_musDocPtr->m_layouts[0].m_pages[0];
    
    int i, j;
    int elementCount = 0;
    
    for ( i = 0; i < (int)page->m_systems.GetCount(); i++ ) {
        Layer *laidOutLayer = &page->m_systems[i].m_staves[0].m_layers[0];
        if ( !laidOutLayer ) {
            wxLogError( "File cannot be upgraded to 2.1.0 (Missing Layer)");    
            return;
        }
        Layer *currentLayer = laidOutLayer->GetSection()->GetStaff( laidOutLayer->m_n - 1 )->GetLayer( laidOutLayer->m_n - 1 );
        if ( !currentLayer ) {
            wxLogError( "File cannot be upgraded to 2.1.0 (Missing Layer)");    
            return;
        }
        for ( j = 0; j < laidOutLayer->GetElementCount(); j++ ) {
            if ( j >=  (int)currentLayer->m_elements.GetCount() ) {
                wxLogError( "File cannot be upgraded to 2.1.0 (Too many LayerElement)");    
                return;
            }
            Object *target = &currentLayer->m_elements[ elementCount ];
            elementCount++;
            if ( dynamic_cast<LayerElement*>(target) ) {
                LayerElement *element = &laidOutLayer->m_elements[j];
                element->m_layerElement = dynamic_cast<LayerElement*>(target);
            }
        }
        
    }
    */
    
    wxLogMessage( "File NOT successfully upgraded to 2.1.0");
    this->Modify();
}

void RecFile::UpgradeTo_2_3_0()
{
    if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) >= AxFile::FormatVersion(2, 3, 0) )
		return; // when do not need to upgrade the file
    if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) < AxFile::FormatVersion(2, 0, 0) )
        return; // the file has been upgraded by Upgrade_2_0_0
    
	if ( !wxCopyFile( m_basename + "rec.mlf", m_basename + "rec.old.mlf") )  
        return;
    // we need to read page.mlf 
	if ( !wxCopyFile( m_basename + "page.mlf", m_basename + "rec.mlf", true) )  
        return;
    
    wxArrayPtrVoid params;
    
    if ( !this->RealizeFromMLF( params, NULL ) )
        return;
	if ( !wxCopyFile( m_basename + "rec.old.mlf", m_basename + "rec.mlf", true) )  
        return;
    
    // output the new binary file
    MeiOutput mei_output( m_musDocPtr, m_musDocPtr->m_fname );
    mei_output.ExportFile(); 
    wxLogMessage( "File successfully upgraded to 2.3.0");
    this->Modify();
}

void RecFile::NewContent( )
{
	wxASSERT_MSG( !m_imPagePtr, "ImPage should be NULL" );
	wxASSERT_MSG( !m_musDocPtr, "Doc should be NULL" );

	// new Doc
    m_musDocPtr = new Doc();
    m_musDocPtr->m_fname = m_basename + "page.mei";  
        
	// new ImPage and Load
    m_imPagePtr = new ImPage( m_basename, &m_isModified );

	m_pre_image_binarization_method = ImOperator::s_pre_image_binarization_method;
	m_pre_page_binarization_method =  ImPage::s_pre_page_binarization_method;
	m_pre_page_binarization_method_size = ImPage::s_pre_page_binarization_method_size;
	
	m_imPagePtr->m_pre_image_binarization_methodPtr = &m_pre_image_binarization_method;
	m_imPagePtr->m_pre_page_binarization_methodPtr = &m_pre_page_binarization_method;
	m_imPagePtr->m_pre_page_binarization_method_sizePtr = &m_pre_page_binarization_method_size;
	
	m_isPreprocessed = false;
	m_isRecognized = false;
}



void RecFile::OpenContent( )
{
	this->NewContent();
	
	TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
	
	UpgradeTo_1_4_0();
	
	// start
    bool failed = false;
	
	failed = !m_imPagePtr->Load( m_xml_root );
	if ( failed )
		return;
	else
		m_isPreprocessed = true;
        
    UpgradeTo_1_5_0();
    
    UpgradeTo_2_0_0();
	
    // binarization variables	
    node = m_xml_root->FirstChild( "binarization" );
    if ( node )
    {
        root = node->ToElement();
        if ( !root ) return;
            
        if ( root->Attribute( "pre_image_binarization_method" ) )
            RecFile::m_pre_image_binarization_method = atoi( root->Attribute( "pre_image_binarization_method" ) );
        if ( root->Attribute( "pre_page_binarization_method" ) )
            RecFile::m_pre_page_binarization_method = atoi( root->Attribute( "pre_page_binarization_method" ) );
        if ( root->Attribute( "pre_page_binarization_method_size" ) )
            RecFile::m_pre_page_binarization_method_size = atoi( root->Attribute( "pre_page_binarization_method_size" ) );
    }
		
	if ( wxFileExists( m_basename + "page.mei") )
	{
        UpgradeTo_2_3_0();

        MeiInput *mei_input = new MeiInput( m_musDocPtr, m_musDocPtr->m_fname );
        failed = !mei_input->ImportFile();
        delete mei_input;           
		if ( failed )
			return;
		
        m_isRecognized = true;
        
        UpgradeTo_2_1_0();
        
	} //else if there's an MEI in there?
	
	return;       
	// Load XML file. Don't know what's necessary yet...
	/*
    TiXmlDocument dom( m_xmlfile.c_str() );
    bool ok = dom.LoadFile();

    if ( !ok )
    {
        wxLogWarning(_("Unable to load file '%s'"), m_xmlfile.c_str() );
        return;
    }

    TiXmlNode *node = NULL;
    TiXmlElement *elem = NULL;

    TiXmlElement *root = dom.RootElement();
	if ( !root )
    {
        wxLogError( _("Unknown error" ) );
        return;
    }

	// models
	node = root->FirstChild("models");
    if ( node && (elem = node->ToElement()) && elem)
	{
		m_rec_typ_model = elem->Attribute("typographic_model");
		m_rec_mus_model = elem->Attribute("ic_model");
	}

	// decoder
	node = root->FirstChild("decoder");
    if ( node && (elem = node->ToElement()) && elem)
	{
		m_rec_wrdtrns = elem->Attribute("wrdtrns");
		int val;
		elem->Attribute("lm_delayed", &val );
		m_rec_delayed = (val > 0) ? true : false;
		elem->Attribute("order", &m_rec_lm_order );
		elem->Attribute("scaling", &m_rec_lm_scaling );
	}
	*/
}

void RecFile::SaveContent( )
{
	wxASSERT_MSG( m_imPagePtr, "ImPage should not be NULL" );
	wxASSERT_MSG( m_musDocPtr, "Doc should not be NULL" );
	wxASSERT( m_xml_root );
    
		
	if ( !m_isPreprocessed )
		return;
	else {
		m_imPagePtr->Save( m_xml_root ); // save in the RecFile directory		
		
		// binarization variables
		TiXmlElement binarization( "binarization" );
		binarization.SetAttribute( "pre_image_binarization_method", RecFile::m_pre_image_binarization_method );
		binarization.SetAttribute( "pre_page_binarization_method", RecFile::m_pre_page_binarization_method );
		binarization.SetAttribute( "pre_page_binarization_method_size", RecFile::m_pre_page_binarization_method_size );
		m_xml_root->InsertEndChild( binarization );
	}

	if ( !m_isRecognized )
		return;
	else
	{
		//possibility of Doc being a .mei not a .bin
		//MeiOutput *mei_output = new MeiOutput( m_musDocPtr, m_musDocPtr->m_fname );
		// save
		MeiOutput *mei_output = new MeiOutput( m_musDocPtr, m_musDocPtr->m_fname );
		mei_output->ExportFile();
		delete mei_output;
		
		MusMLFOutput *mlfoutput = new MusMLFOutput( m_musDocPtr, m_basename + "page.mlf", NULL );
		mlfoutput->m_pagePosition = true;
        // !!! No check if layout and page exist!
		mlfoutput->WritePage( (Page*)m_musDocPtr->m_children[0] , "staff", m_imPagePtr );
		delete mlfoutput;
		TiXmlElement root( "recpage" );
    
		// models
		TiXmlElement models( "models" );
		models.SetAttribute( "typographic_model",  m_rec_typ_model.c_str() );
		models.SetAttribute( "ic_model",  m_rec_mus_model.c_str() );	
		root.InsertEndChild( models );
		
		// decoder
		TiXmlElement decoder( "decoder" );
		decoder.SetAttribute( "wrdtrns",  m_rec_wrdtrns.c_str() );
		decoder.SetAttribute( "lm_delayed",  m_rec_delayed );
		decoder.SetAttribute( "order",  m_rec_lm_order );
		decoder.SetDoubleAttribute( "scaling",  m_rec_lm_scaling );
		root.InsertEndChild( decoder );
			
		m_xml_root->InsertEndChild( root );
	}
}

void RecFile::CloseContent( )
{
	// nouveau fichier ?
    if ( m_musDocPtr )
    {
        delete m_musDocPtr;
        m_musDocPtr = NULL;
    }

	// old ImPage
    if ( m_imPagePtr )
    {
        delete m_imPagePtr;
        m_imPagePtr = NULL;
    }
	
	m_isPreprocessed = false;
	m_isRecognized = false;
}

void RecFile::SetBinarization( int image_binarization_method, int page_binarization_method, int page_binarization_size ){
	this->m_pre_image_binarization_method = image_binarization_method;
	this->m_pre_page_binarization_method = page_binarization_method;
	this->m_pre_page_binarization_method_size = page_binarization_size;
}

// static
bool RecFile::IsRecognized( wxString filename )
{
	return AxFile::ContainsFile( filename, "page.mei"  ) 
        || AxFile::ContainsFile( filename, "page.bin"  )  // page.wwg for files before 2.0
        || AxFile::ContainsFile( filename, "page.wwg"  ); // page.wwg for files before 1.5
}

void RecFile::GetImage1( AxImage *image )
{
	wxASSERT_MSG( image, "AxImage cannot be NULL" );
	wxASSERT_MSG( m_imPagePtr, "imPagePtr cannot be NULL" );
	wxASSERT_MSG( m_imPagePtr->m_img0, "Img0 cannot be NULL" );
	
	// for backwards compatibility, return m_img0 if m_img1 doesn't exists
	if ( m_imPagePtr->m_img1 )
		SetImImage( m_imPagePtr->m_img1, image );
	else
		SetImImage( m_imPagePtr->m_img0, image );
}

void RecFile::GetImage0( AxImage *image )
{
	wxASSERT_MSG( image, "AxImage cannot be NULL" );
	wxASSERT_MSG( m_imPagePtr, "imPagePtr cannot be NULL" );
	wxASSERT_MSG( m_imPagePtr->m_img0, "Img0 cannot be NULL" );
	
	SetImImage( m_imPagePtr->m_img0, image );
}

void RecFile::SetImage( wxString filename )
{

}


void RecFile::SetImage( AxImage *image )
{

}

bool RecFile::CancelRecognition( bool ask_user )
{
	if ( ask_user )
	{	
		wxString msg = wxString::Format(_("This will erase previous recognition results. Do you want to continue ?") );
		int res = wxMessageBox( msg, wxGetApp().GetAppName() , wxYES_NO | wxICON_QUESTION );
		if ( res != wxYES )
			return false;
	}

	wxArrayString names;
	wxDir::GetAllFiles( m_basename, &names , "staff*", wxDIR_FILES );
	for( int i = names.GetCount() - 1; i >=0; i-- )
		wxRemoveFile( names[i] );
	wxRemoveFile( m_basename + "rec.mlf" );
	wxRemoveFile( m_basename + "rec.xml" );
	wxRemoveFile( m_basename + "mfc.input" );
	wxRemoveFile( m_basename + "page.mei" );
	wxRemoveFile( m_basename + "page.mlf" );
	wxRemoveFile( m_basename + "staves.tif" );
	
	m_isRecognized = false;
	m_isModified = true;
	return true;
	
}

// functors

bool RecFile::Preprocess( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
    // params 0: wxString: output_dir
	wxString image_file = *(wxString*)params[0];
	
	m_imPagePtr->SetProgressDlg( dlg );

    wxString ext, shortname;
    wxFileName::SplitPath( image_file, NULL, &shortname, &ext );

    dlg->SetMaxJobBar( 17 );
    dlg->SetJob( shortname );
    wxYield();
		
    bool failed = false;
		
    if ( RecEnv::s_check && !failed ) 
        failed = !m_imPagePtr->Check( image_file, 5000, 1200 ); // 2 operations max
	
	//// TEST
	//m_error = m_imPagePtr->GetError();
	//this->m_isPreprocessed = true;
	//return true;
	
    if ( RecEnv::s_deskew && !failed ) 
        failed = !m_imPagePtr->Deskew( 10.0 ); // 2 operations max
    //op.m_inputfile = output + "/deskew." + shortname + ".tif";
        
    if ( RecEnv::s_staves_position && !failed ) 
        failed = !m_imPagePtr->FindStaves( 3, 50, true, RecEnv::s_crop );  // 4 operations max
    //op.m_inputfile = output + "/resize." + shortname + ".tif";

    if ( RecEnv::s_binarize_and_clean  && !failed ) 
        failed = !m_imPagePtr->BinarizeAndClean( );  // 3 operations max

    if ( RecEnv::s_find_borders && !failed ) 
        failed = !m_imPagePtr->FindBorders( );  // 1 operation max
    //op.m_inputfile = output + "/border." + shortname + ".tif";
        
    if ( RecEnv::s_find_ornate_letters && !failed ) 
        failed = !m_imPagePtr->FindOrnateLetters( ); // 1 operation max

    if ( RecEnv::s_find_text_in_staves && !failed ) 
        failed = !m_imPagePtr->FindTextInStaves( ); // 1 operation max

    if ( RecEnv::s_find_text && !failed ) 
        failed = !m_imPagePtr->FindText( ); // 1 operation max
		
	if (!failed)
		this->m_isPreprocessed = true;
			
	m_error = m_imPagePtr->GetError();
	
	return !failed;
}

bool RecFile::Recognize( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
    // params 0: RecTypModel: typModelPtr
	// params 1: RecMusModel: musModelPtr
	// params 2: bool: rec_delayed
	// params 3: int: rec_lm_order
	// params 4: double: rec_lm_scaling
	// params 5: wxString: rec_wrdtrns
	
	//if (this->m_isRecognized)
	//	return true;
	
	wxArrayPtrVoid mfc_params;
	wxString file = "";
	mfc_params.Add( &file );
	
	m_imPagePtr->SetProgressDlg( dlg );
	
    dlg->SetMaxJobBar( 5 );
    dlg->SetJob( m_shortname );
    wxYield();	
	
    bool failed = false;
	
    if ( !failed ) 
        failed = !m_imPagePtr->ExtractStaves( ); // 1 operation
	
    if ( RecEnv::s_save_images && !failed )
        failed = !m_imPagePtr->SaveStaffImages( );

    if ( !failed )
        failed = !this->GenerateMFC( mfc_params, dlg ); // 2 operations
	
	// just pre-classification
	/*
	wxLogMessage("Modified method RecFile::Recognize Pre-classification");
	if ( RecEnv::s_binarize_and_clean  && !failed ) 
        failed = !m_imPagePtr->BinarizeAndClean( );

    if ( RecEnv::s_find_borders && !failed ) 
        failed = !m_imPagePtr->FindBorders( );
    //op.m_inputfile = output + "/border." + shortname + ".tif";
        
    if ( RecEnv::s_find_ornate_letters && !failed ) 
        failed = !m_imPagePtr->FindOrnateLetters( );

    if ( RecEnv::s_find_text_in_staves && !failed ) 
        failed = !m_imPagePtr->FindTextInStaves( );

    if ( RecEnv::s_find_text && !failed ) 
        failed = !m_imPagePtr->FindText( );
	
	wxArrayString names;
	wxDir::GetAllFiles( m_basename, &names , "staff*", wxDIR_FILES );
	for( int i = names.GetCount() - 1; i >=0; i-- )
		wxRemoveFile( names[i] );
	wxRemoveFile( m_basename + "rec.mlf" );
	wxRemoveFile( m_basename + "rec.xml" );
	wxRemoveFile( m_basename + "mfc.input" );
	wxRemoveFile( m_basename + "staves.tif" );
		
    if ( !failed ) 
        failed = !m_imPagePtr->StaffSegments( ); // 1 operation
	
    if ( RecEnv::s_save_images && !failed )
        failed = !m_imPagePtr->SaveSegmentsImages( );

    if ( !failed )
        failed = !this->GenerateMFC( mfc_params, dlg ); // 2 operations
		
	return !failed;
	*/
    
	if ( !failed )
        failed = !this->Decode( params, dlg );
 
    if ( !failed )
		failed = !this->RealizeFromMLF( params, dlg );
	
	if (!failed)
		this->m_isRecognized = true;
	
	if (!failed)	
		this->Modify();

	
	return !failed;
}


bool RecFile::Decode( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( m_imPagePtr, "ImPage cannot be NULL" );
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
    // params 0: RecTypModel: typModelPtr
	// params 1: RecMusModel: musModelPtr
	// params 2: bool: rec_delayed
	// params 3: int: rec_lm_order
	// params 4: double: rec_lm_scaling
	// params 5: wxString: rec_wrdtrns
	
	RecTypModel *typModelPtr = (RecTypModel*)params[0];
	RecMusModel *musModelPtr = (RecMusModel*)params[1];
	//bool rec_delayed =  *(bool*)params[2];
	int rec_lm_order =  *(int*)params[3];
	if ( rec_lm_order > MUS_NGRAM_ORDER )
		rec_lm_order = MUS_NGRAM_ORDER;
	//double rec_lm_scaling =  *(double*)params[4];
	wxString rec_wrdtrns =  *(wxString*)params[5];
	
	wxString input = m_basename + "mfc.input";
	input.Replace( "\\/", "/" );
	wxString rec_models = typModelPtr->m_basename + "hmm";
	wxString rec_dict = typModelPtr->m_basename + "dic";
	wxString rec_lm = musModelPtr->m_basename + "ngram.gram";
	wxString rec_output = m_basename + "rec.mlf";
    wxRemoveFile( rec_output );
    
	// cannot be changed from interface
	//double rec_phone_pen = RecEnv::s_rec_phone_pen;
	//double rec_int_prune = RecEnv::s_rec_int_prune;
	//double rec_word_pen = RecEnv::s_rec_word_pen;
	
#ifdef __WXMSW__
	#if defined(_DEBUG)
		wxString cmd = "Decoder.exe";
	#else
		wxString cmd = "Decoder.exe";
	#endif   
#elif __WXGTK__ || __WXMAC__
    wxString cmd = "julius";  
#endif

	wxString args = " ";
	
	wxString log = wxGetApp().m_logDir + "/julius.log";
    args << " -logfile " << "\"" << log.c_str() << "\"";
	
    args << " -input mfcfile ";
	args << " -h " << "\"" << rec_models.c_str() << "\"";
	
    args << " -spmodel \"{s}\" ";
	args << " -silhead \"SP_START\" ";
    args << " -siltail \"SP_END\" ";
    
    args << " -norealtime ";
    args << " -walign ";
    args << " -outfile ";
    
	args << " -v " << "\"" << rec_dict.c_str() << "\"";

	if ( rec_lm_order && !rec_lm.IsEmpty() )
	{
		args << " -nlr " << "\"" << rec_lm << "\"";
	}

	args << " -filelist " << "\"" << input.c_str() << "\"";

	wxLogDebug( args.c_str() );

	if (!dlg->SetOperation( _("Recognition...") ) )
		return this->Terminate( ERR_CANCELED );

	dlg->StartTimerOperation( TIMER_DECODING, m_imPagePtr->GetStaffCount() );
	int pid;
	AxProcess *process = new AxProcess( cmd, args, NULL );
	process->SetLog( log );
	if ( process->Start() )
	{
		pid = process->GetPid();
		while  ( !process->HasEnded() )
		{
			if( !dlg->IncTimerOperation( ) )
			{
				wxKill( pid, wxSIGKILL );
				wxYield();
				delete process; 
				return this->Terminate( ERR_CANCELED );
			}
			wxMilliSleep( 200 );
		}
		
	}
	wxYield(); // flush termination event before deleting the process.
	if ( process->m_status != 0 )
	{
		delete process;
		return this->Terminate( ERR_UNKNOWN );
	}
	dlg->EndTimerOperation( TIMER_DECODING );	
	delete process;

	return true;
}

bool RecFile::RealizeFromMLF( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
    wxASSERT_MSG( m_imPagePtr , "Page cannot be NULL");
    wxASSERT_MSG( m_musDocPtr , "Doc cannot be NULL");
	//wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
	if (dlg && !dlg->SetOperation( _("Load results...") ) )
		return this->Terminate( ERR_CANCELED );

    
    m_musDocPtr->Reset( Transcription );
    // here we need to create the logical tree 
    Page *musPage = new Page();
    
    // dimensions
	musPage->m_pageWidth = m_imPagePtr->m_size.GetWidth();
	musPage->m_pageHeight = m_imPagePtr->m_size.GetHeight();

    int nb = (int)m_imPagePtr->m_staves.GetCount();
    ImStaff *imStaff;   
    // currently creates one system/staff/layer per staff in imPage
    for (int i = 0; i < nb; i++)
    {
        imStaff = &m_imPagePtr->m_staves[i];
        System *musSystem = new System();
        Staff *musStaff = new Staff( 1 );
        musStaff->m_mensuralNotation = true;
        Measure *musMeasure = new Measure( false, 1 );
        Layer *musLayer = new Layer( 1 ); // only one layer per staff
        //musLayer->no = nb; ?? // ax2
        musSystem->m_systemLeftMar = imStaff->m_x1;  
        musSystem->m_systemRightMar = musPage->m_pageWidth - imStaff->m_x2;
        musSystem->m_yAbs =  imStaff->m_y + STAFF / 2;
        musStaff->m_yAbs =  imStaff->m_y + STAFF / 2;
        musStaff->notAnc = true;
        musStaff->vertBarre = START_END;
        musStaff->AddLayer( musLayer );
        musMeasure->AddStaff( musStaff );
        musSystem->AddMeasure( musMeasure );
        musPage->AddSystem( musSystem );
    } 
	
	wxString m_rec_output = m_basename + "rec.mlf";
	
    MusMLFInput *mlfinput = new MusMLFInput( m_musDocPtr, m_rec_output );
    mlfinput->ReadPage( musPage, true, m_imPagePtr );
    delete mlfinput;
    
    m_musDocPtr->AddPage( musPage );
    
	//wxString m_rec2_output = m_basename + "rec2.mlf";
    //MusMLFOutput *mlfoutput = new MusMLFOutput( m_musDocPtr, m_rec2_output, NULL, "MusMLFSymbol" );
    //mlfoutput->WritePage( musPage, true );
    //delete mlfoutput;
     
	// save ????
    // MusBinOutput *bin_output = new MusBinOutput( m_musDocPtr, m_musDocPtr->m_fname );
    // bin_output->ExportFile();
    // delete bin_output;

    return true;
}

bool RecFile::GenerateMFC( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	// params 0: wxString: output_dir
	
	wxString output_dir = *(wxString*)params[0];
	
    wxASSERT_MSG( m_imPagePtr , "Page cannot be NULL");
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
	m_imPagePtr->SetProgressDlg( dlg );

	// start
    bool failed = false;

	// if (m_imPagePtr->m_staff_height == 0) non, le faire de toute facon
	// la position verticale des portee n'a pas encore
	// ete calculee - un peu merdique de le faire ici ...
	//{
	if ( !failed )
		failed = !m_imPagePtr->StaffCurvatures();

	//if ( !failed ) 
	// attention, supprime les valeurs de reconnaissance dans le fichier xml
	// normalement, elles n'existe pas puisque m_staff_height n'a pas ete calcule
	//	failed = !m_imPagePtr->Save( m_basename + "img0.xml" );
	//}

    if ( !failed )
        failed = !m_imPagePtr->GenerateMFC( output_dir );

	//if ( !failed )
    //    failed = !m_imPagePtr->GenerateLyricMFC( output_dir );
	
	m_error = m_imPagePtr->GetError();

	return !failed;
}


#endif //AX_RECOGNITION

