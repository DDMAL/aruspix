/////////////////////////////////////////////////////////////////////////////
// Name:        cmpfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "cmpfile.h"
#include "cmp.h"

#include "recognition/recfile.h"

#include "mus/layer.h"
#include "mus/layerelement.h"
#include "mus/measure.h"
#include "mus/staff.h"
#include "mus/system.h"
#include "mus/page.h"
#include "mus/iomei.h"
#include "mus/app.h"


#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfCmpBookItems );

WX_DEFINE_OBJARRAY( ArrayOfCmpCollations );

WX_DEFINE_OBJARRAY( ArrayOfCmpPartPages );

WX_DEFINE_OBJARRAY( ArrayOfCmpBookParts );

WX_DEFINE_OBJARRAY( ArrayOfCmpCollationParts );

int SortCmpBookItems( CmpBookItem **first, CmpBookItem **second )
{
    if ( (*first)->m_filename < (*second)->m_filename )
        return -1;
    else if ( (*first)->m_filename > (*second)->m_filename )
        return 1;
    else
        return 0;
}

int CmpBookItem::s_index = 0;


//----------------------------------------------------------------------------
// CmpCollationPart
//----------------------------------------------------------------------------

CmpCollationPart::CmpCollationPart(  CmpBookPart *bookPart, int flags )
{
	m_bookPart = bookPart;
	m_flags = flags;
	m_del = 0;
	m_ins = 0;
	m_subst = 0;
	m_recall = 0.0;
	m_precision = 0.0;
	m_seq = 0; // length of the sequence

}

CmpCollationPart::~CmpCollationPart( )
{
}

//----------------------------------------------------------------------------
// CmpCollation
//----------------------------------------------------------------------------

CmpCollation::CmpCollation( wxString id, wxString name, wxString basename  )
{
	m_id = id;
	m_name = name;
	m_basename = basename;
	m_isColLoaded = false;


    // new Doc
    m_musDocPtr = new Doc();
    m_musDocSrc1Ptr = new Doc();    
    m_musDocSrc2Ptr = new Doc();
}

CmpCollation::~CmpCollation( )
{
	if ( m_musDocPtr )
		delete m_musDocPtr;
	if ( m_musDocSrc1Ptr )
		delete m_musDocSrc1Ptr;
	if ( m_musDocSrc2Ptr )
		delete m_musDocSrc2Ptr;
}

bool CmpCollation::IsCollationLoaded( CmpCollationPart *part )
{
	//if ( !m_isColLoaded )
	//	return false;
    
    // The collation file
    wxString filename = m_basename + m_id + "." + part->m_bookPart->m_id + ".mei" ;
	if ( !wxFileExists( filename ) )
		return false;
	MeiInput mei_input( m_musDocPtr, filename.c_str()  );
	if (!mei_input.ImportFile()) {
		return false;
    }
    m_musDocPtr->Layout();
    
    // The source 1
    filename = m_basename + GetRefPartFilename() + ".mei";
	if ( !wxFileExists( filename ) )
		return false;
	MeiInput mei_input1( m_musDocSrc1Ptr, filename.c_str()  );
	if (!mei_input1.ImportFile()) {
		return false;
    }
    
    // The source 2
    filename = m_basename + part->m_bookPart->m_id + ".mei" ;
	if ( !wxFileExists( filename ) )
		return false;
	MeiInput mei_input2( m_musDocSrc2Ptr, filename.mb_str()  );
	if (!mei_input2.ImportFile()) {
		return false;
    }
    
	m_isColLoaded = true;
	return true;
}

wxString CmpCollation::GetRefPartFilename()
{
    int i;
    for( i = 0; i < (int)m_collationParts.GetCount(); i++ )
	{
		if ( (&m_collationParts[i])->m_flags & PART_REFERENCE )
		{
            return (&m_collationParts[i])->m_bookPart->m_id;
		}
	}
    return "";
}

bool CmpCollation::Realize( )
{
    /*
	int i;

	wxASSERT( m_musDocPtr );
	m_musDocPtr->m_pages.Clear();
	m_isColLoaded = false;
	int nstaff = (int)m_collationParts.GetCount();
	 
	if ( (m_length == 0) || (nstaff < 2) )
	{
		wxLogError( "Wrong length of staff number" );
		return false;
	}

	// prepare the file, with empty pages and empty staves
	int lrg_lign = 190;
	int correct_lrg_lign = lrg_lign * 10 - 90; // margin of 100 pt for the width of the last element + clef
	int npages = ceil((float)m_length / (float)correct_lrg_lign);
	for( i = 0; i < npages; i++ )
	{					
		Page *page = new Page();
		page->lrg_lign = lrg_lign;
		for( int j = 0; j < nstaff; j++ )
		{
			Staff *staff = new Staff();
			staff->no = j;
			if ( j == 0 )
				staff->vertBarre = START;
			else if ( j == nstaff - 1 ) 
				staff->vertBarre = END;
			//staff->vertBarre = START_END;
			page->m_staves.Add( staff );
		}
		m_musDocPtr->m_pages.Add( page );
	}
	
	// fill the pages
	for( i = 0; i < nstaff; i++ )
	{
		Staff *full_staff = new Staff();
		CmpCollationPart *part = &m_collationParts[i];
		wxString staffname = m_basename + m_id + "." + part->m_bookPart->m_id + ".swwg";
		if ( part->m_flags & PART_REFERENCE )
		{
			staffname = m_basename + m_id + ".swwg";
		}
		MusBinInput_1_X bin_input( NULL, staffname, MUS_BIN_ARUSPIX_CMP );
		bin_input.ReadStaff( full_staff );
		Symbol1 *clef = NULL; // we keep last clef for next page
		for( int j = 0; j < npages; j++ )
		{	
			int clef_offset = 0;
			Staff *staff = &m_musDocPtr->m_pages[j].m_staves[i];
			if ( clef )
			{
				staff->m_children.Add( new Symbol1( *clef ) );
				clef_offset += 45;
			}
			//FillStaff( staff, full_staff, j, correct_lrg_lign );
			while( !full_staff->m_children.IsEmpty() )
			{
				if ( (int)full_staff->m_children[0]m_x_abs > (j + 1) * correct_lrg_lign)
					break;
				if ( full_staff->m_children[0].IsNote() )
				{
					Note1 *nnote = (Note1*)full_staff->m_children.Detach( 0 );
					nnote->m_x_abs -= (j * correct_lrg_lign) - clef_offset;
					staff->m_children.Add( nnote );
				}
				else if ( full_staff->m_children[0].IsSymbol() )
				{
					Symbol1 *nsymbol = (Symbol1*)full_staff->m_children.Detach( 0 );
					if ( nsymbol->flag == CLE ) // we keep last clef for next pages
					{
						if ( clef )
							delete clef;
						clef = new Symbol1( *nsymbol );
						clef->m_im_filename = "";
						clef->m_x_abs = 0;
						clef->m_cmp_flag = 0;
					}
					nsymbol->m_x_abs -= (j * correct_lrg_lign) - clef_offset;
					staff->m_children.Add( nsymbol );
				}
			}
		}
		if ( clef )
			delete clef;
	}

    m_musDocPtr->CheckIntegrity();
	MusBinOutput *bin_output = new MusBinOutput( m_musDocPtr, m_musDocPtr->m_fname, MUS_BIN_ARUSPIX_CMP );
	bin_output->ExportFile();
	delete bin_output;
	
	m_isColLoaded = true;
	
	return true;
    */

    m_isColLoaded = true;
    return true;
}

bool CmpCollation::Collate( )
{
    
	int i;
	// !!! THERE IS NO CHECK THAT THE FILES HAVE BEEN LOADED
	// IF NOT, THE READING STAFF WILL CRASH...

	int nparts = (int)m_collationParts.GetCount();
	 
	if ( nparts == 0 )
		return false;
    
    // pointers for the refence doc
	CmpCollationPart *reference = NULL;
    wxString refFilename;
    
    for( i = 0; i < (int)m_collationParts.GetCount(); i++ )
	{
		if ( (&m_collationParts[i])->m_flags & PART_REFERENCE )
		{
			reference = &m_collationParts[i];
            // we will need to change this to a more sophisticated source structure
            m_refSource = reference->m_bookPart->m_book->m_bookname;
		}
	}
	
	if ( reference == NULL )
	{
		wxLogError( _("No reference part found") );
		return false;
	}

	for( i = 0; i < (int)m_collationParts.GetCount(); i++ )
	{
        if ( &m_collationParts[i] == reference ) {
            // we are not going to compare the reference with itself
            continue;
        }
        
        CmpCollationPart * variant = &m_collationParts[i];
        
        Layer *layer_ref = reference->m_bookPart->GetContentToAlign( m_basename );
        layer_ref->RemoveClefAndCustos();

        Layer *layer_var = variant->m_bookPart->GetContentToAlign( m_basename );    
        layer_var->RemoveClefAndCustos();

        
		m_varSource = variant->m_bookPart->m_book->m_bookname;
        bool success = Align( layer_ref, layer_var, reference->m_bookPart->m_id, variant->m_bookPart->m_id );
        
        if ( !success ) {
            wxLogError( _("Alignement failed, unknonw error ... " ) );
        }
        
        wxLogMessage( _("Saving MEI collation file ... ") );
        
        // create the full mei doc
        Doc collationDoc;
        collationDoc.Reset( Raw );
        Page *page = new Page( );
        System *system = new System( );
        Staff *staff = new Staff( 1 );
        Measure *measure = new Measure( false, 1 );
        staff->AddLayer( layer_ref );
        measure->AddStaff( staff );
        system->AddMeasure( measure );
        page->AddSystem( system );
        collationDoc.AddPage( page );
        
        MeiOutput mei_output( &collationDoc, (m_basename + m_id + "." + variant->m_bookPart->m_id + ".mei").mb_str()  );
        mei_output.ExportFile();
        
        delete layer_var;
	}

	return true;
	
}

void CmpCollation::CreateApp( Layer *layer_aligned, int i, Layer *layer_var, int j, int appType, wxString refFileId, wxString varFileId )
{
    LayerApp *app = new LayerApp();
    
    // we create two <rdg> because we are going to have an empty rdg for insertions and deletions
    LayerRdg *ref = new LayerRdg( );
    LayerRdg *var = new LayerRdg( );
    // set the source ids available as member variables
    ref->m_source = m_refSource;
    var->m_source = m_varSource;
    
    if ( (appType == CMP_APP_SUBST) || (appType == CMP_APP_DEL) ) {
        ref->AddElement( (((LayerElement*)layer_aligned->m_children[i]))->GetChildCopy()  );
        (ref->m_children[0])->AddSameAs( (layer_aligned->m_children[i])->GetUuid(), refFileId.mb_str() );
    }
    if ( (appType == CMP_APP_SUBST) || (appType == CMP_APP_INS) ) {
        var->AddElement( (((LayerElement*)layer_var->m_children[j]))->GetChildCopy()  );
        (var->m_children[0])->AddSameAs( (layer_var->m_children[j])->GetUuid(), varFileId.mb_str() );
    }
    
    // then insert the rdg 
    app->AddLayerRdg( ref );
    app->AddLayerRdg( var );
    // insert the app in the layer
    layer_aligned->Insert( app, (LayerElement*)layer_aligned->m_children[i] );

    // remove the original element (if any)
    if ( appType != CMP_APP_INS ) {
        layer_aligned->Delete( (LayerElement*)layer_aligned->m_children[i+1] );
        //delete layer_aligned->m_children[i+1];
    }
}

int minimum(int a,int b,int c)
/*Gets the minimum of three values*/
{
	int min=a;
	if(b<min)
		min=b;
	if(c<min)
		min=c;
	return min;
}

	
bool CmpCollation::Align( Layer *layer_ref, Layer *layer_var, wxString refFileId, wxString varFileId )
{	
	//Step 1
	int k,i,j,n,m,cost,*d,distance;
	n = layer_ref->GetElementCount();
	m = layer_var->GetElementCount();
	
	int insert_cost = 2;
	int delete_cost = 2;
	int subst_cost = 1;
	
	if((n==0) || (m==0))
	{
		wxLogMessage( _("Nothing to align") );
		return false;
	}
	
	// print sequences
	//for(k=0;k<n;k++)
	//	printf("%s\n", reference->Item(k).GetLabel().c_str() );
	//printf("\n");
	//for(k=0;k<m;k++)
	//	printf("%s\n", variant->Item(k).GetLabel().c_str() );
	//printf("\n");
	
	d=(int*)malloc((sizeof(int))*(m+1)*(n+1));
	if (d == NULL)
	{
		wxLogError( _("Memory allocation of '%d' bytes failed"), (sizeof(int))*(m+1)*(n+1) );
		return false;
	}

	m++;
	n++;
	//Step 2	
	for(k=0;k<n;k++)
		d[k]=k;
	for(k=0;k<m;k++)
		d[k*n]=k;
	//Step 3 and 4	
	for(i=1;i<n;i++)
    {
		for(j=1;j<m;j++)
		{
			//Step 5
			if ( (*layer_ref->m_children[i-1]) == (*layer_var->m_children[j-1]) )
			//if(s[i-1]==t[j-1])
				cost=0;
			else
				cost=subst_cost;
			//Step 6			 
			d[j*n+i]=minimum(d[(j-1)*n+i]+delete_cost,d[j*n+i-1]+insert_cost,d[(j-1)*n+i-1]+cost);
		}
    }
	
	i=n-1;
	j=m-1;
	bool match = false;
	int n_insert = 0;
	int n_delete = 0;
	int n_subst = 0;
    
	// principle : the aligned staff is identical to the reference.
	// for every substitution, we replace the element using the one in the variant (and change the flag)
	// for every deletion, we change the flag of the element
	// for every insertion, we put the element in insertion staff, grouping consecutive insertions into one single staff
	// insertions make the code complicated ...

	while ((j > 0) && (i > 0))	
	{
		match = false;
		if ( (*layer_ref->m_children[i-1]) == (*layer_var->m_children[j-1]) )
		{
			if (d[j*n+i] == d[(j-1)*n+i-1]) // align
			{
				//printf("%d - ", d[j*n+i]);
				i--; j--;
				//printf("   \t%10s\t%10s\n", (layer_ref->m_children[i])->MusClassName().c_str(), (layer_var->m_children[j])->MusClassName().c_str() );
				match = true;
                (layer_ref->m_children[i])->AddSameAs( (layer_ref->m_children[i])->GetUuid(), refFileId.mb_str() );
                (layer_ref->m_children[i])->AddSameAs( (layer_var->m_children[j])->GetUuid(), varFileId.mb_str() );
			}
		}
		else
		{
			if (d[j*n+i] == d[(j-1)*n+i-1] + subst_cost) // substitution
			{
				//printf("%d - ", d[j*n+i]);
				i--; j--;
				//printf("S |\t%10s\t%10s\n", (layer_ref->m_children[i])->MusClassName().c_str(), (layer_var->m_children[j])->MusClassName().c_str() );
                CreateApp( layer_ref, i, layer_var, j, CMP_APP_SUBST, refFileId, varFileId );
				match = true;
				n_subst++;
			}
		}
        // !match means we did not have a match or a substitution. It will be an insertion or a deletion
		if ( !match )
		{
			if ((j > 0) && (d[j*n+i] == d[(j-1)*n+i] + insert_cost))
			{
				printf("%d - ", d[j*n+i]);
				j--;
				///ii = ((CmpMLFSymb*)&reference->Item(i))->m_index;
				///jj = ((CmpMLFSymb*)&variant->Item(j))->m_index;
				printf("I |\t%10s\t%10s\n", "-", (layer_var->m_children[j])->MusClassName().c_str() );
                CreateApp( layer_ref, i, layer_var, j, CMP_APP_INS, refFileId, varFileId );
				n_insert++;
			}
			else if ((i > 0) && (d[j*n+i] == d[(j)*n+i-1] + delete_cost))
			{
				printf("%d - ", d[j*n+i]);
				i--;
				//printf("D |\t%10s\t%10s\n", (layer_ref->m_children[i])->MusClassName().c_str(), "-" );
                CreateApp( layer_ref, i, layer_var, j, CMP_APP_DEL, refFileId, varFileId );
				n_delete++;
			}
			else
			{
				printf("Error (i = %d, j = %d)\n", i , j);
				//break;
			}
		}
	}
	while (j > 0)
	{
		j--;
		//printf("I |\t%10s\t%10s\n", "-", (layer_var->m_children[j])->MusClassName().c_str() );
        CreateApp( layer_ref, i, layer_var, j, CMP_APP_INS, refFileId, varFileId );
		n_insert++;
	}
	while (i > 0)
	{
		i--;
		//printf("D |\t%10s\t%10s\n", (layer_ref->m_children[i])->MusClassName().c_str(), "-" );
        CreateApp( layer_ref, i, layer_var, j, CMP_APP_DEL, refFileId, varFileId );
		n_delete++;
	}
	printf("\nEnd (i = %d, j = %d)\n", i , j);

    /*
	part_var->m_ins = n_insert;
	part_var->m_del = n_delete;
	part_var->m_subst = n_subst;
	part_var->m_recall = (n - n_delete - n_subst) * 100. / (double)n;
	part_var->m_precision = (n - n_delete - n_subst) * 100. / (double)(n - n_delete + n_insert);
    */
	
	
	//printf("Recall %5.2f | Precision %5.2f || N %d | I %d | D %d | S %d\n",
	//	(n - n_delete - n_subst) * 100. / (double)n,
	//	(n - n_delete - n_subst) * 100. / (double)(n - n_delete + n_insert),
	//	n, n_insert, n_delete, n_subst);
	
	
	// print matrice
	//for(i=0;i<n;i++)
	//{
	//	for(j=0;j<m;j++)
	//	{
	//		printf("%d\t", d[j*n+i] );
	//	}
	//	printf("\n");
	//}
	
	distance=d[n*m-1];
	free(d);
	
    return true;
}

//----------------------------------------------------------------------------
// CmpPartPage
//----------------------------------------------------------------------------


CmpPartPage::CmpPartPage() 
{ 
    m_start.clear();
    m_end.clear();
    m_part = NULL;
}

CmpPartPage::CmpPartPage(  wxString axfile, CmpBookPart *part ) 
{
    m_start.clear();
    m_end.clear();
    m_axfile = axfile; 
    m_part = part;
}


void CmpPartPage::SetStartEnd( LayerElement *element, bool isStart ) 
{ 
    wxString uuidStr;
    if ( element ) {
        uuidStr = element->GetUuid().c_str();
    }
    
    if ( isStart ) {
        this->SetStart( uuidStr );
    }
    else
    {
        this->SetEnd( uuidStr );
    } 
}

void CmpPartPage::SetStart( wxString uuidStr ) 
{ 
    this->m_start.clear();
    if ( !uuidStr.IsEmpty() ) {
        //uuid_parse( uuidStr.c_str(), this->m_start );
        this->m_start = uuidStr;
    }
}

void CmpPartPage::SetEnd( wxString uuidStr ) 
{ 
    this->m_end.clear();
    if ( !uuidStr.IsEmpty() ) {
        //uuid_parse( uuidStr.c_str(), this->m_end );
        this->m_end = uuidStr;
    }
}

bool CmpPartPage::HasStart() 
{ 
    return ( !m_start.empty() );
}

bool CmpPartPage::HasEnd() 
{ 
    return ( !m_end.empty() );
}

//----------------------------------------------------------------------------
// CmpBookPart
//----------------------------------------------------------------------------

CmpBookPart::CmpBookPart( wxString id, wxString name, CmpBookItem *book )
{
	m_id = id;
	m_name = name;
	m_book = book;
}

CmpBookPart::~CmpBookPart( )
{
}

Layer *CmpBookPart::GetContentToAlign( wxString basename )
{
    Layer *alignLayer = new Layer( 1 );
    Doc doc;
    
    MeiInput meiInput( &doc, (basename + this->m_id + ".mei").mb_str() );
    if ( !meiInput.ImportFile() ) {
        wxLogError( "Cannot open MEI part file");
    }
    
    int nfiles = (int)m_partpages.GetCount();
	    
    for ( int k = 0; k < (int)nfiles; k++ )
    {
        CmpPartPage *partPage = &this->m_partpages[k];
        Page *currentPage = (Page*)doc.m_children[k];
        
        bool has_started = false;
        bool has_ended = false;
        bool new_uuid = false;
        ArrayPtrVoid params;
        params.push_back( alignLayer );
        params.push_back( &partPage->m_start );
        params.push_back( &partPage->m_end );
        params.push_back( &has_started );
        params.push_back( &has_ended);
        params.push_back( &new_uuid );
        
        MusFunctor copyToLayer( &Object::CopyToLayer );
        currentPage->Process( &copyToLayer, params );
    }
    
    return alignLayer;
}

//----------------------------------------------------------------------------
// CmpBookItem
//----------------------------------------------------------------------------

CmpBookItem::CmpBookItem( wxString filename, wxString bookname, int flags )
{
	m_filename = filename;
	m_shortname = wxFileName( filename ).GetFullName(); 
	m_flags = flags;
	m_bookname = bookname;
    
	wxString name = wxString::Format("cmp_book_file_%d", CmpBookItem::s_index++ );
	m_recBookFilePtr = new RecBookFile( name );
	m_recBookFilePtr->Open( filename );
}

CmpBookItem::~CmpBookItem( )
{
	if ( m_recBookFilePtr )
		delete m_recBookFilePtr;
}


bool CmpBookItem::AssembleParts( )
{
	wxLogMessage( m_shortname.c_str() );
	
	return true;
}

//----------------------------------------------------------------------------
// CmpFile
//----------------------------------------------------------------------------

CmpFile::CmpFile( wxString name, CmpEnv *env )
: AxFile( name, AX_FILE_PROJECT, AX_FILE_COMPARISON )
{
    m_envPtr = env;
}

CmpFile::~CmpFile()
{
}


void CmpFile::NewContent( )
{
    // files
    m_bookFiles.Clear();
	m_collations.Clear();
	CmpBookItem::s_index = 0;
}


void CmpFile::OpenContent( )
{
    this->NewContent();
    
    TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
    TiXmlElement *elem = NULL;
    
    wxASSERT( m_xml_root );
     
    // we need to load the book first because of the references to parts in collations
    node = m_xml_root->FirstChild( "books" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
    
    for( node = root->FirstChild( "book" ); node; node = node->NextSibling( "book" ) )
    {
        elem = node->ToElement();
        if (!elem || !elem->Attribute("filename") || !elem->Attribute("bookname") || !elem->Attribute( "flags" ) ) 
            continue;
			
		wxFileName filename(elem->Attribute("filename"));
		//wxLogDebug( wxFileName( m_filename ).GetPath() );
		filename.MakeAbsolute( wxFileName( m_filename ).GetPath() );
		//wxLogDebug( filename.GetFullPath() );
        CmpBookItem *book = new CmpBookItem( filename.GetFullPath(), elem->Attribute("bookname"), atoi(elem->Attribute( "flags" )) );
		
		TiXmlNode *node1 = NULL;
		for( node1 = node->FirstChild( "part" ); node1; node1 = node1->NextSibling( "part" ) )
		{	
			elem = node1->ToElement();
			if (!elem || !elem->Attribute("id") || !elem->Attribute( "name" ) ) 
				continue;
			
			CmpBookPart *part = new CmpBookPart( elem->Attribute("id"), elem->Attribute("name"), book );
			TiXmlNode *node2 = NULL;
			for( node2 = node1->FirstChild( "axfile" ); node2; node2 = node2->NextSibling( "axfile" ) )
			{
				elem = node2->ToElement();
				if (!elem || !elem->Attribute( "filename" ) ) 
					continue;
				CmpPartPage *page = new CmpPartPage( elem->Attribute( "filename" ), part );
                
				TiXmlNode *node3 = NULL;
                if ( ( node3 = node2->FirstChild( "start" ) ) ) {
                    elem = node3->ToElement();
                    if (!elem || !elem->Attribute( "uuid" ) ) 
						continue;
                    page->SetStart( elem->Attribute( "uuid" ) );
                }
                if ( ( node3 = node2->FirstChild( "end" ) ) ) {
                    elem = node3->ToElement();
                    if (!elem || !elem->Attribute( "uuid" ) ) 
						continue;
                    page->SetEnd( elem->Attribute( "uuid" ) );
                }
				part->m_partpages.Add( page );
			}
			book->m_bookParts.Add( part );
			
		}
		m_bookFiles.Add( book );
    }
	
    // collations
    node = m_xml_root->FirstChild( "collations" );
    if ( !node ) return;        
    root = node->ToElement();
    if ( !root ) return;
	
	for( node = root->FirstChild( "collation" ); node; node = node->NextSibling( "collation" ) )
    {
		elem = node->ToElement();
        if (!elem || !elem->Attribute("id") || !elem->Attribute( "name" ) ) 
            continue;
			
        CmpCollation *collation = new CmpCollation(  elem->Attribute("id"), elem->Attribute("name"), this->m_basename );
		
		TiXmlNode *node1 = NULL;
		for( node1 = node->FirstChild( "part" ); node1; node1 = node1->NextSibling( "part" ) )
		{	
			elem = node1->ToElement();
			if (!elem || !elem->Attribute("idref") || !elem->Attribute( "flags" ) ) 
				continue;
			
			CmpBookPart *bookpart = FindBookPart( elem->Attribute("idref") );
			if ( !bookpart )
				continue;
				
			CmpCollationPart *part = new CmpCollationPart( bookpart,atoi(elem->Attribute( "flags" )) );
			collation->m_collationParts.Add( part );
			
		}
		m_collations.Add( collation );
	}
	
	/*
	if ( wxFileExists( m_basename + "collation.wwg") )
	{
		bool failed = false;
		MusBinInput_1_X *bin_input = new MusBinInput_1_X( m_musDocPtr, m_musDocPtr->m_fname, MUS_BIN_ARUSPIX_CMP  );
		failed = !bin_input->ImportFile();
		delete bin_input;
		if ( failed )
			return;
		else
			m_hasCollationLoaded = true;	
	}
	*/
}


void CmpFile::SaveContent( )
{
    wxASSERT( m_xml_root );
	int i, j, k;
    //char uuidStr[37];
    
    TiXmlElement books("books");
    for ( i = 0; i < (int)m_bookFiles.GetCount(); i++)
    {
        TiXmlElement book("book");
		wxFileName filename = m_bookFiles[i].m_filename;
		filename.MakeRelativeTo( wxFileName( m_filename ).GetPath() );
		//wxLogDebug( filename.GetFullPath() );
        book.SetAttribute("filename", filename.GetFullPath().c_str() );
        book.SetAttribute("flags", wxString::Format("%d", m_bookFiles[i].m_flags ).c_str() );
        book.SetAttribute("bookname", m_bookFiles[i].m_bookname.c_str() );
		
		// parts
		for ( j = 0; j < (int)m_bookFiles[i].m_bookParts.GetCount(); j++ )
		{
			CmpBookPart *bookpart = &m_bookFiles[i].m_bookParts[j];
			TiXmlElement part("part");
			part.SetAttribute("id", bookpart->m_id.c_str() );
			part.SetAttribute("name", bookpart->m_name.c_str() );
			for ( k = 0; k < (int)bookpart->m_partpages.GetCount(); k++)
			{
				TiXmlElement axfile("axfile");
				axfile.SetAttribute("filename", bookpart->m_partpages[k].m_axfile.c_str() );
				axfile.SetAttribute("flags", wxString::Format("%d", 0 ) );
				// add staves if any
				CmpPartPage *partpage = &bookpart->m_partpages[k];
                if ( !partpage->m_start.empty() ) {
                    TiXmlElement start("start");
                    start.SetAttribute("uuid", partpage->m_start.c_str() );
					axfile.InsertEndChild( start );
                }
                if ( !partpage->m_end.empty() ) {
                    TiXmlElement end("end");
                    end.SetAttribute("uuid", partpage->m_end.c_str() );
					axfile.InsertEndChild( end );
                }
				part.InsertEndChild( axfile );
			}
			book.InsertEndChild( part );	
		}
        books.InsertEndChild( book );
    }   
    m_xml_root->InsertEndChild( books );
	
    TiXmlElement collations("collations");
    for ( i = 0; i < (int)m_collations.GetCount(); i++)
    {
        TiXmlElement collation("collation");
        collation.SetAttribute("id", m_collations[i].m_id.c_str() );
        collation.SetAttribute("name", m_collations[i].m_name.c_str() );
		
		// parts
		for ( j = 0; j < (int)m_collations[i].m_collationParts.GetCount(); j++ )
		{
			CmpCollationPart *colpart = &m_collations[i].m_collationParts[j];
			TiXmlElement part("part");
			part.SetAttribute("idref", colpart->m_bookPart->m_id.c_str() );
			part.SetAttribute("flags", wxString::Format("%d", colpart->m_flags ).c_str() );
			collation.InsertEndChild( part );	
		}
        collations.InsertEndChild( collation );
    }
    m_xml_root->InsertEndChild( collations );
	
	/*if ( !m_hasCollationLoaded )
		return;
	else
	{
		// save
		MusBinOutput *bin_output = new MusBinOutput( m_musDocPtr, m_musDocPtr->m_fname, MUS_BIN_ARUSPIX_CMP );
		bin_output->ExportFile();
		delete bin_output;
	}*/
}

void CmpFile::CloseContent( )
{
	m_bookFiles.Clear();
	m_collations.Clear();
	CmpBookItem::s_index = 0;
}


CmpBookPart *CmpFile::FindBookPart( wxString id )
{
	for( int i = 0; i < (int)m_bookFiles.GetCount(); i++ )
		for( int j = 0; j < (int)m_bookFiles[i].m_bookParts.GetCount(); j++)
		{
			CmpBookPart *part = &m_bookFiles[i].m_bookParts[j];
			if ( part->m_id == id )
				return part;
		}
		
	return NULL;
}

bool CmpFile::LoadBooks( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
    
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );


    wxArrayString paths, filenames;
    int nbooks, nparts, nfiles;
    
	nbooks = (int)m_bookFiles.GetCount();
    if ( nbooks == 0 )
        return false;  
  
      
    bool failed = false;

    for ( int i = 0; i < nbooks; i++ )
    {
        if ( dlg->GetCanceled() ) // PROBLEM, canceled is true but don't know why ...
		   break;
			
		nparts = m_bookFiles[i].m_bookParts.GetCount( );
			
		wxString job =  wxString::Format( _("Loading the files for the book '%s'"), m_bookFiles[i].m_recBookFilePtr->m_shortname.c_str() );
		dlg->SetJob( job );
		dlg->SetMaxJobBar( nparts );
		wxLogMessage( "%s", m_bookFiles[i].m_recBookFilePtr->m_shortname.c_str() );
			
		for ( int j = 0; j < nparts; j++ )
		{
			CmpBookPart *part = &m_bookFiles[i].m_bookParts[j];
			
            // Create the new MEI file    
            Doc *partDoc = new Doc( );
            partDoc->Reset( Transcription );
            // we will need to change this to a more sophisticated source structure
            partDoc->m_source = part->m_book->m_bookname;
            
			nfiles = (int)part->m_partpages.GetCount();
			wxLogMessage( "%s", part->m_name.c_str() );
	
			wxString operation =  wxString::Format( _("Adding data to the part '%s' (%d/%d)"), part->m_name.c_str(), j + 1, nparts ) ;
			dlg->SetOperation( operation );	
			
			//dlg->SetMaxJobBar( nfiles );
			int counter = dlg->GetCounter();
			imCounterTotal( counter, nfiles + 2 , operation.c_str() );
		
			for ( int k = 0; k < (int)nfiles; k++ )
			{
                CmpPartPage *partPage = &part->m_partpages[k];
                
				RecFile recFile( "cmp_add_file" );
				recFile.New();
				
				imCounterInc( dlg->GetCounter() );
				wxLogMessage( "%s", partPage->m_axfile.c_str() );
	
				wxString file = m_bookFiles[i].m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() + partPage->m_axfile;
				if ( !recFile.Open( file ) )
					continue;
	
				if ( !recFile.IsRecognized() ) 
				{
					wxLogWarning(_("File '%s' not recognized, the part was skipped"), file.c_str() );
					break;
				}
                
                if ( recFile.IsOlderThan(2, 0, 0) ) 
				{
					wxLogWarning(_("File '%s' not upgraded to at least 2.0.0, the part was skipped"), file.c_str() );
					break;
				}
                
                wxImage image;
                if ( !image.LoadFile( recFile.m_basename + "img1.tif" ) ) {
                    wxLogWarning( "Image file could not be loaded '%s'", file.c_str() );
                }
                if ( !image.SaveFile( m_basename + recFile.m_shortname +".png", wxBITMAP_TYPE_PNG ) ) {
                    wxLogWarning( "Image file could not be saved '%s'", file.c_str() );
                }                    

				if ( !failed && !dlg->GetCanceled() )
				{            
                    // Detach the correspondant layout page from the file - we will not save the file, so who cares?
                    Page *partPage =  (Page*)recFile.m_musDocPtr->DetachChild( 0 );
                    partDoc->AddPage( partPage );
                    // temporary - for now we just put the filename in the surface - this is actually wrong
                    partPage->m_surface = recFile.m_shortname + ".png";
				}	
			}
            
			imCounterInc( dlg->GetCounter() );
            
            wxLogMessage( _("Saving MEI part file ...") );
            MeiOutput *mei_output = new MeiOutput( partDoc, (m_basename + part->m_id + ".mei").mb_str()  );
            mei_output->ExportFile();
            delete mei_output;
            
			imCounterInc( dlg->GetCounter() );
		}
    }
	
	//imCounterEnd( dlg->GetCounter() );
	
	if (!failed)	
		this->Modify();

    return false;
}

/*
int CmpFile::FilesToPreprocess( wxArrayString *filenames, wxArrayString *paths, bool add_axfiles )
{
	int index;
	
	filenames->Clear();
	paths->Clear();

	for( int i = 0; i < (int)m_imgFiles.GetCount(); i++)
	{
		if ( m_imgFiles[i].m_flags & FILE_DESACTIVATED )
			continue;
		wxFileName filename( m_imgFiles[i].m_filename );
		filename.SetExt("axz");
		//wxLogDebug( filename.GetFullName() );
		if ( CmpFile::FindFile( &m_axFiles, filename.GetFullName() , &index ) )
			continue;
		
		if ( add_axfiles )
			m_axFiles.Add( CmpBookItem( filename.GetFullName() ) );
		paths->Add( m_imgFileDir + wxFileName::GetPathSeparator() +  m_imgFiles[i].m_filename );
		filenames->Add( m_imgFiles[i].m_filename );
	}
	m_axFiles.Sort( SortCmpBookItems );
	return (int)filenames->GetCount();
}


int CmpFile::FilesToRecognize( wxArrayString *filenames, wxArrayString *paths )
{
	filenames->Clear();
	paths->Clear();

	for( int i = 0; i < (int)m_axFiles.GetCount(); i++)
	{
		if ( m_axFiles[i].m_flags & FILE_DESACTIVATED )
			continue;

		if ( RecFile::IsRecognized( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename ) )
			continue;
			
		paths->Add( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename );
		filenames->Add( m_axFiles[i].m_filename );
	}
	return (int)filenames->GetCount();
}

int CmpFile::FilesForAdaptation( wxArrayString *filenames, wxArrayString *paths, bool *isCacheOk )
{
	wxASSERT( isCacheOk );

	filenames->Clear();
	paths->Clear();
	*isCacheOk = false;

	for( int i = 0; i < (int)m_axFiles.GetCount(); i++)
	{
		if ( m_axFiles[i].m_flags & FILE_DESACTIVATED )
			continue;
			
		if ( !RecFile::IsRecognized( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename ) )
			continue;
			
		paths->Add( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename );
		filenames->Add( m_axFiles[i].m_filename );
	}
	if ( (int)filenames->GetCount() == 0 )
		return 0;
		
	// cache is empty
	if ( (int)m_optFiles.GetCount() == 0 )
		return (int)filenames->GetCount() ;
		
	// check the cache. The idea is that if a file in the cache is not in the list, the cache is not valid.
	// otherwise, just keep the files that are not in the cache
	// cache is not optimized for path changes
	
	// first go through and check is the cache is valid
	for ( int i = 0; i < (int)m_optFiles.GetCount(); i++ )
		if ( filenames->Index( m_optFiles[i] ) == wxNOT_FOUND )
			return (int)filenames->GetCount(); // cache is not valid, 
			
	// cache is valid, remove the files
	for ( int i = 0; i < (int)m_optFiles.GetCount(); i++ )
	{
		int idx = filenames->Index( m_optFiles[i] );
		wxASSERT( idx != wxNOT_FOUND );
		paths->RemoveAt( idx );
		filenames->RemoveAt( idx );
	}
	*isCacheOk = true;
	return (int)filenames->GetCount();
}

bool CmpFile::HasToBePreprocessed( wxString imagefile )
{
	wxArrayString paths, filenames;
	size_t nbOfFiles;
    
	nbOfFiles = FilesToPreprocess( &filenames, &paths, false );
	if ( paths.Index( imagefile ) == wxNOT_FOUND )
	{
		wxLogMessage( _("Nothing to do! Check if the file is desactivated or already preprocessed") );
		return false;
	}
	else
	{
		wxFileName filename( imagefile );
		filename.SetExt("axz");
		m_axFiles.Add( CmpBookItem( filename.GetFullName() ) );
		m_axFiles.Sort( SortCmpBookItems );
		return true;
	}
}
*/



bool CmpFile::Collate( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );

	int ncollation = (int)m_collations.GetCount();
    if ( ncollation == 0 )
        return false;  
    
    bool failed = false;

    for ( int i = 0; i < ncollation; i++ )
    {
        if ( dlg->GetCanceled() ) // PROBLEM, canceled is true but don't know why ... (really ?)
		   break;
			
		CmpCollation *collation = &m_collations[i];
			
		wxString job =  wxString::Format( _("Build the collation '%s'"), collation->m_name.c_str() );
		dlg->SetJob( job );
		dlg->SetMaxJobBar( 1 );
		wxLogMessage( "%s", collation->m_name.c_str() );
	
		if (!failed)
			failed = !collation->Collate( );
	
		if (!failed)
			failed = !collation->Realize( );
	
		//if (!failed)
		//	this->m_hasCollationLoaded = true;
	}

	imCounterEnd( dlg->GetCounter() );
	
	if (!failed)	
		this->Modify();
	
	return !failed;
}
	
// static
CmpBookItem *CmpFile::FindFile( ArrayOfCmpBookItems *array, wxString filename, int* index )
{
    wxASSERT( array );
    wxASSERT( index );

    *index = -1;

    for( int i = 0; i < (int)array->GetCount(); i++ )
        if ( array->Item(i).m_filename == filename )
        {
            *index = i;
            return &array->Item(i);
        }
        
    return NULL;
}

/*
bool CmpFile::LoadAxfiles( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_axFileDir ) )
    {
        nbfiles = wxDir::GetAllFiles( m_axFileDir, &paths, "*.axz" );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !CmpFile::FindFile( &m_axFiles, name.GetFullName(), &index ) )
                m_axFiles.Add( CmpBookItem( name.GetFullName() ) );
        }
        m_axFiles.Sort( SortCmpBookItems );
    }
    
    return true;
}


bool CmpFile::LoadImages( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_imgFileDir ) )
    {
        nbfiles = wxDir::GetAllFiles( m_imgFileDir, &paths, "*.tif" );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !CmpFile::FindFile( &m_imgFiles, name.GetFullName(), &index ) )
                m_imgFiles.Add( CmpBookItem( name.GetFullName() ) );
        }
        m_imgFiles.Sort( SortCmpBookItems );
    }
    
    return true;
}
*/

bool CmpFile::DesactivateAxfile( wxString filename )
{
    int index;
    CmpBookItem *book = CmpFile::FindFile( &m_bookFiles, filename, &index );
    if ( book )
    {
        if (book->m_flags & FILE_DESACTIVATED)
            book->m_flags &= ~FILE_DESACTIVATED;
        else
            book->m_flags |= FILE_DESACTIVATED;
    }
    return true;
}


	#endif //AX_COMPARISON
#endif //AX_RECOGNITION

