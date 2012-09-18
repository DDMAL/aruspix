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
#include "cmpmlf.h"

#include "recognition/recfile.h"

#include "mus/musiobin.h"
#include "mus/muslayer.h"
#include "mus/musiomei.h"
//#include "musnote.h"
//#include "mussymbol.h"


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
	m_length = 0; // x_abs of the last element + 50
	m_basename = basename;
	m_isColLoaded = false;
	
	m_insStaff = NULL;

	// new MusDoc
    m_musDocPtr = new MusDoc();
    m_musDocPtr->m_fname = m_basename + m_id + ".wwg";
}

CmpCollation::~CmpCollation( )
{
	if ( m_musDocPtr )
		delete m_musDocPtr;
}

bool CmpCollation::IsCollationLoaded( )
{
	if ( m_isColLoaded )
		return true;

	if ( !wxFileExists( m_musDocPtr->m_fname ) )
		return false;
		
	bool failed = false;
	MusBinInput_1_X *bin_input = new MusBinInput_1_X( m_musDocPtr, m_musDocPtr->m_fname, MUS_BIN_ARUSPIX_CMP  );
	failed = !bin_input->ImportFile();
	delete bin_input;
	if ( failed )
		return false;

	m_isColLoaded = true;
	return true;
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
		MusPage *page = new MusPage();
		page->lrg_lign = lrg_lign;
		for( int j = 0; j < nstaff; j++ )
		{
			MusLaidOutStaff *staff = new MusLaidOutStaff();
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
		MusLaidOutStaff *full_staff = new MusLaidOutStaff();
		CmpCollationPart *part = &m_collationParts[i];
		wxString staffname = m_basename + m_id + "." + part->m_bookPart->m_id + ".swwg";
		if ( part->m_flags & PART_REFERENCE )
		{
			staffname = m_basename + m_id + ".swwg";
		}
		MusBinInput_1_X bin_input( NULL, staffname, MUS_BIN_ARUSPIX_CMP );
		bin_input.ReadStaff( full_staff );
		MusSymbol1 *clef = NULL; // we keep last clef for next page
		for( int j = 0; j < npages; j++ )
		{	
			int clef_offset = 0;
			MusLaidOutStaff *staff = &m_musDocPtr->m_pages[j].m_staves[i];
			if ( clef )
			{
				staff->m_elements.Add( new MusSymbol1( *clef ) );
				clef_offset += 45;
			}
			//FillStaff( staff, full_staff, j, correct_lrg_lign );
			while( !full_staff->m_elements.IsEmpty() )
			{
				if ( (int)full_staff->m_elements[0]m_x_abs > (j + 1) * correct_lrg_lign)
					break;
				if ( full_staff->m_elements[0].IsNote() )
				{
					MusNote1 *nnote = (MusNote1*)full_staff->m_elements.Detach( 0 );
					nnote->m_x_abs -= (j * correct_lrg_lign) - clef_offset;
					staff->m_elements.Add( nnote );
				}
				else if ( full_staff->m_elements[0].IsSymbol() )
				{
					MusSymbol1 *nsymbol = (MusSymbol1*)full_staff->m_elements.Detach( 0 );
					if ( nsymbol->flag == CLE ) // we keep last clef for next pages
					{
						if ( clef )
							delete clef;
						clef = new MusSymbol1( *nsymbol );
						clef->m_im_filename = "";
						clef->m_x_abs = 0;
						clef->m_cmp_flag = 0;
					}
					nsymbol->m_x_abs -= (j * correct_lrg_lign) - clef_offset;
					staff->m_elements.Add( nsymbol );
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
    wxLogError( "CmpCollation::Realize method missing in ax2") ;
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
	
	MusDoc *docs = new MusDoc[ nparts ];
	MusDoc *reference = NULL;
	for( i = 0; i < (int)m_collationParts.GetCount(); i++ )
	{
		CmpCollationPart *part = &m_collationParts[i];
        MusMeiInput meiInput( &docs[i], m_basename + part->m_bookPart->m_id + ".mei" );
        if ( !meiInput.ImportFile() ) {
            wxLogError( "Cannot open MEI part file");
        }

		if ( part->m_flags & PART_REFERENCE )
		{
			reference = &docs[i];
			//m_length = (&reference->m_elements.Last())->m_x_abs + 50;
		}
	}
	
	if ( reference == NULL )
	{
		wxLogError( _("No reference part found") );
		delete[] docs;
		return false;
	}

	for( i = 0; i < (int)m_collationParts.GetCount(); i++ )
	{
		CmpCollationPart *part = &m_collationParts[i];
        MusLayer *layer_ref = &reference->m_divs[0].m_score->m_sections[0].m_staves[0].m_layers[0];
        MusLayer *layer_var = &(&docs[i])->m_divs[0].m_score->m_sections[0].m_staves[0].m_layers[0];
		if ( &docs[i] != reference )
			Align( layer_ref, layer_var, part );
			
		//MusBinOutput bin_output( NULL, m_basename + book->m_shortname + ".swwg", MUS_BIN_ARUSPIX_CMP  );
		//bin_output.WriteStaff( musStaff );
	}

	
	// save reference
	/*
    MusBinOutput bin_output( NULL, m_basename + m_id + ".swwg", MUS_BIN_ARUSPIX_CMP  );
	bin_output.WriteStaff( reference );
	*/ // ax2
     
	delete[] docs;
	return true;
	
}

void CmpCollation::SetCmpValues( MusLayerElement *dest, MusLayerElement *src, int flag )
{
    /*
	dest->m_cmp_flag = flag;
	if ( src )
	{
		dest->m_im_filename = src->m_im_filename;
		dest->m_im_staff = src->m_im_staff;
		dest->m_im_pos = src->m_im_pos;
	}
	else
	{
		dest->m_im_filename = "";
		dest->m_im_staff = 0;
		dest->m_im_pos = 0;
	}
    */ // ax2
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

	
bool CmpCollation::Align( MusLayer *layer_ref, MusLayer *layer_var, CmpCollationPart *part_var )
{
    /*
	CmpMLFOutput m_cmpoutput1( NULL, wxGetApp().m_workingDir + "/cmp_staff1", "CmpMLFSymb"  );
	m_cmpoutput1.WriteStaff( staff_ref );
		
	CmpMLFOutput m_cmpoutput2( NULL, wxGetApp().m_workingDir + "/cmp_staff2", "CmpMLFSymb"  );
	m_cmpoutput2.WriteStaff( staff_var );
	
	ArrayOfMLFSymbols *reference = m_cmpoutput1.GetSymbols();
	ArrayOfMLFSymbols *variant = m_cmpoutput2.GetSymbols();
    */
	
	
	// We may eventually avoid the use of CmpMLFOuputs working directy on MusLaidOutStaff
	// A comparison method for MusElement would have to be provided though

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
		return NULL;
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
			if ( layer_ref->m_elements[i-1] == layer_var->m_elements[j-1] ) 
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
	bool match;
	int n_insert = 0;
	int n_delete = 0;
	int n_subst = 0;
    
	// principle : the aligned staff is identical to the reference.
	// for every substitution, we replace the element using the one in the variant (and change the flag)
	// for every deletion, we change the flag of the element
	// for every insertion, we put the element in insertion staff, grouping consecutive insertions into one single staff
	// insertions make the code complicated ...
	// AddInsertion add the element to the current insertion staff
	// if needed, AddInsertion create the insertion staff and add an element into the aligned staff (before position i)
	// EndInsertion write the current insertion staff if it exists
	
    ///MusLaidOutStaff aligned = *staff_ref;
    wxASSERT( !m_insStaff );
	m_insStaff = NULL;
	int ii, jj;
	while ((j > 0) && (i > 0))	
	{
		match = false;
		if ( layer_ref->m_elements[i-1] == layer_var->m_elements[j-1] )
		{
			if (d[j*n+i] == d[(j-1)*n+i-1]) // align
			{
				printf("%d - ", d[j*n+i]);
				i--; j--;
				printf("   \t%10s\t%10s\n", (&layer_ref->m_elements[i])->MusClassName().c_str(), (&layer_var->m_elements[j])->MusClassName().c_str() );
				match = true;
				///ii = ((CmpMLFSymb*)&reference->Item(i))->m_index;
				///jj = ((CmpMLFSymb*)&variant->Item(j))->m_index;
				///SetCmpValues( &aligned.m_elements[ii], &staff_var->m_elements[jj], CMP_MATCH );
				EndInsertion( part_var );
			}
		}
		else
		{
			if (d[j*n+i] == d[(j-1)*n+i-1] + subst_cost) // substitution
			{
				printf("%d - ", d[j*n+i]);
				i--; j--;
				printf("S |\t%10s\t%10s\n", (&layer_ref->m_elements[i])->MusClassName().c_str(), (&layer_var->m_elements[j])->MusClassName().c_str() );
				//aligned.m_elements[i] = staff_var->m_elements[j];
				/**ii = ((CmpMLFSymb*)&reference->Item(i))->m_index;
				jj = ((CmpMLFSymb*)&variant->Item(j))->m_index;
				if ( staff_var->m_elements[jj].IsNote() )
					aligned.m_elements.Insert( new MusNote1( *(MusNote1*)&staff_var->m_elements[jj] ), ii );
				else if ( staff_var->m_elements[jj].IsSymbol() )
					aligned.m_elements.Insert( new MusSymbol1( *(MusSymbol1*)&staff_var->m_elements[jj] ), ii );
				aligned.m_elements[ii]m_x_abs = aligned.m_elements[ii+1]m_x_abs;
				aligned.m_elements.RemoveAt(ii+1);
				SetCmpValues( &aligned.m_elements[ii], &staff_var->m_elements[jj], CMP_SUBST );
                **/
				match = true;
				n_subst++;
				EndInsertion( part_var );
			}
		}
		if ( !match )
		{
			if ((j > 0) && (d[j*n+i] == d[(j-1)*n+i] + insert_cost))
			{
				printf("%d - ", d[j*n+i]);
				j--;
				///ii = ((CmpMLFSymb*)&reference->Item(i))->m_index;
				///jj = ((CmpMLFSymb*)&variant->Item(j))->m_index;
				printf("I |\t%10s\t%10s\n", "-", (&layer_var->m_elements[j])->MusClassName().c_str() );
				n_insert++;
				//AddInsertion( &staff_var->m_elements[jj], &aligned, ii );
			}
			else if ((i > 0) && (d[j*n+i] == d[(j)*n+i-1] + delete_cost))
			{
				printf("%d - ", d[j*n+i]);
				i--;
				///ii = ((CmpMLFSymb*)&reference->Item(i))->m_index;
				///jj = ((CmpMLFSymb*)&variant->Item(j))->m_index;
				printf("D |\t%10s\t%10s\n", (&layer_ref->m_elements[i])->MusClassName().c_str(), "-" );
				//SetCmpValues( &aligned.m_elements[ii], NULL, CMP_DEL );
				n_delete++;
				EndInsertion( part_var );
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
		//jj = ((CmpMLFSymb*)&variant->Item(j))->m_index;
		printf("I |\t%10s\t%10s\n", "-", (&layer_var->m_elements[j])->MusClassName().c_str() );
		n_insert++;
		//AddInsertion( &staff_var->m_elements[jj], &aligned, 0 );
	}
	while (i > 0)
	{
		i--;
		//ii = ((CmpMLFSymb*)&reference->Item(i))->m_index;
		printf("D |\t%10s\t%10s\n", (&layer_ref->m_elements[i])->MusClassName().c_str(), "-" );
		//SetCmpValues( &aligned.m_elements[ii], NULL, CMP_DEL );
		n_delete++;
	}
	printf("\nEnd (i = %d, j = %d)\n", i , j);
	//EndInsertion( part_var ); // flush the last insertion
	
    /*
    
	MusBinOutput bin_output( NULL, m_basename + m_id + "." + part_var->m_bookPart->m_id + ".swwg", MUS_BIN_ARUSPIX_CMP  );
	bin_output.WriteStaff( &aligned );
	
	part_var->m_ins = n_insert;
	part_var->m_del = n_delete;
	part_var->m_subst = n_subst;
	part_var->m_recall = (n - n_delete - n_subst) * 100. / (double)n;
	part_var->m_precision = (n - n_delete - n_subst) * 100. / (double)(n - n_delete + n_insert);
	
	
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
    */
    wxLogError( "CmpCollation::Align missing in ax2" );
	
	return new MusLayer();
}

void  CmpCollation::EndInsertion( CmpCollationPart *part_var  )
{
	if ( !m_insStaff )
		return;
	
	// save staff
	delete m_insStaff;
	m_insStaff = NULL;
}

void  CmpCollation::AddInsertion( MusLayerElement *elem, MusLayer *aligned, int i )
{
    /*
	if ( !m_insStaff )
	{
		m_insStaff = new MusLaidOutStaff();
	    MusSymbol1 *asterix = new MusSymbol1();
		asterix->flag = AX_VARIANT;
		//asterix->oct = 5;
		//SetCmpValues( asterix, NULL, CMP_INS );
		SetCmpValues( asterix, elem, CMP_INS );
		if ( i >= (int)aligned->m_elements.GetCount() )
		{
			asterix->m_x_abs = aligned->m_elements.Last()m_x_abs + 20;
			aligned->m_elements.Add( asterix );
		}
		else
		{
			asterix->m_x_abs = aligned->m_elements[i]m_x_abs - 10;
			aligned->m_elements.Insert( asterix, i );
		}
		aligned->CheckIntegrity( );
	}
    */ // ax2
}

//----------------------------------------------------------------------------
// CmpPartPage
//----------------------------------------------------------------------------


CmpPartPage::CmpPartPage() 
{ 
    uuid_clear( m_start );
    uuid_clear( m_end );
}

CmpPartPage::CmpPartPage(  wxString axfile ) 
{
    uuid_clear( m_start );
    uuid_clear( m_end );
    m_axfile = axfile; 
}

//----------------------------------------------------------------------------
// CmpBookPart
//----------------------------------------------------------------------------

CmpBookPart::CmpBookPart( wxString id, wxString name, wxString bookname, CmpBookItem *book )
{
	m_id = id;
	m_name = name;
	m_bookname = bookname;
	m_book = book;
}

CmpBookPart::~CmpBookPart( )
{
}

//----------------------------------------------------------------------------
// CmpBookItem
//----------------------------------------------------------------------------

CmpBookItem::CmpBookItem( wxString filename, int flags )
{
	m_filename = filename;
	m_shortname = wxFileName( filename ).GetFullName(); 
	m_flags = flags;
    
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
	wxLogMessage( m_shortname );
	
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
        if (!elem || !elem->Attribute("filename") || !elem->Attribute( "flags" ) ) 
            continue;
			
		wxFileName filename(elem->Attribute("filename"));
		//wxLogDebug( wxFileName( m_filename ).GetPath() );
		filename.MakeAbsolute( wxFileName( m_filename ).GetPath() );
		//wxLogDebug( filename.GetFullPath() );
        CmpBookItem *book = new CmpBookItem( filename.GetFullPath(), atoi(elem->Attribute( "flags" )) );
		
		TiXmlNode *node1 = NULL;
		for( node1 = node->FirstChild( "part" ); node1; node1 = node1->NextSibling( "part" ) )
		{	
			elem = node1->ToElement();
			if (!elem || !elem->Attribute("id") || !elem->Attribute( "name" ) || !elem->Attribute( "bookname" ) ) 
				continue;
			
			CmpBookPart *part = new CmpBookPart( elem->Attribute("id"), elem->Attribute("name"), elem->Attribute("bookname"), book );
			TiXmlNode *node2 = NULL;
			for( node2 = node1->FirstChild( "axfile" ); node2; node2 = node2->NextSibling( "axfile" ) )
			{
				elem = node2->ToElement();
				if (!elem || !elem->Attribute( "filename" ) ) 
					continue;
				CmpPartPage *page = new CmpPartPage( elem->Attribute( "filename" ) );
                
				TiXmlNode *node3 = NULL;
                if ( ( node3 = node2->FirstChild( "start" ) ) ) {
                    elem = node3->ToElement();
                    if (!elem || !elem->Attribute( "uuid" ) ) 
						continue;
                    uuid_parse( elem->Attribute( "uuid" ), page->m_start );
                }
                if ( ( node3 = node2->FirstChild( "end" ) ) ) {
                    elem = node3->ToElement();
                    if (!elem || !elem->Attribute( "uuid" ) ) 
						continue;
                    uuid_parse( elem->Attribute( "uuid" ), page->m_end );
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
	int i, j, k, l;
    char uuidStr[37];
    
    TiXmlElement books("books");
    for ( i = 0; i < (int)m_bookFiles.GetCount(); i++)
    {
        TiXmlElement book("book");
		wxFileName filename = m_bookFiles[i].m_filename;
		filename.MakeRelativeTo( wxFileName( m_filename ).GetPath() );
		//wxLogDebug( filename.GetFullPath() );
        book.SetAttribute("filename", filename.GetFullPath().c_str() );
        book.SetAttribute("flags", wxString::Format("%d", m_bookFiles[i].m_flags ).c_str() );
		
		// parts
		for ( j = 0; j < (int)m_bookFiles[i].m_bookParts.GetCount(); j++ )
		{
			CmpBookPart *bookpart = &m_bookFiles[i].m_bookParts[j];
			TiXmlElement part("part");
			part.SetAttribute("id", bookpart->m_id.c_str() );
			part.SetAttribute("name", bookpart->m_name.c_str() );
			part.SetAttribute("bookname", bookpart->m_bookname.c_str() );
			for ( k = 0; k < (int)bookpart->m_partpages.GetCount(); k++)
			{
				TiXmlElement axfile("axfile");
				axfile.SetAttribute("filename", bookpart->m_partpages[k].m_axfile.c_str() );
				axfile.SetAttribute("flags", wxString::Format("%d", 0 ) );
				// add staves if any
				CmpPartPage *partpage = &bookpart->m_partpages[k];
                if ( !uuid_is_null( partpage->m_start ) ) {
                    TiXmlElement start("start");
                    uuid_unparse( partpage->m_start, uuidStr ); 
                    start.SetAttribute("uuid", uuidStr );
					axfile.InsertEndChild( start );
                }
                if ( !uuid_is_null( partpage->m_end ) ) {
                    TiXmlElement end("end");
                    uuid_unparse( partpage->m_end, uuidStr ); 
                    end.SetAttribute("uuid", uuidStr );
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
			
            MusLayer *partLayer = new MusLayer();
            
		
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
					wxLogWarning(_("File '%s' skipped"), file.c_str() );
					continue;
				}

				if ( !failed && !dlg->GetCanceled() )
				{
                    // Obviously not safe at all!
                    MusLayer *recLayer = &recFile.m_musDocPtr->m_divs[0].m_score->m_sections[0].m_staves[0].m_layers[0];
                    recLayer->CopyContent( partLayer, partPage->m_start, partPage->m_end );
				
				}
				//imCounterInc( dlg->GetCounter() );	
			}
            
            MusDoc *partDoc = new MusDoc( );
            MusDiv *div = new MusDiv( );
            MusScore *score = new MusScore( );
            MusSection *section = new MusSection( );
            MusStaff *staff = new MusStaff();
            
            staff->AddLayer( partLayer );
            section->AddStaff( staff );
            score->AddSection( section );
            div->AddScore( score );
            partDoc->AddDiv( div );
            
            
			imCounterInc( dlg->GetCounter() );
            
            wxLogMessage( _("Saving MEI part file ...") );
            MusMeiOutput *mei_output = new MusMeiOutput( partDoc, m_basename + part->m_id + ".mei"  );
            mei_output->ExportFile();
            delete mei_output;
            
            
			imCounterInc( dlg->GetCounter() );

            

			/*
            wxLogMessage( _("Convert data ...") );
            //wxLogMessage( _("Convert data ... " + m_basename + part->m_id) );
			CmpMLFInput *m_cmpinput = new CmpMLFInput( NULL, m_basename + part->m_id + ".mlf"  );
			MusLaidOutStaff *musStaff = m_cmpinput->ImportFileInStaff( );
			musStaff->no = 0;
			musStaff->vertBarre = START_END;
			delete m_cmpinput;
			imCounterInc( dlg->GetCounter() );
			
		
			wxLogMessage( _("Write data ...") );
			MusBinOutput *bin_output = new MusBinOutput( NULL, m_basename + part->m_id + ".swwg", MUS_BIN_ARUSPIX_CMP  );
			bin_output->WriteStaff( musStaff );
			delete bin_output;
			delete musStaff;
			imCounterInc( dlg->GetCounter() );
            */
		}
    }
	
	//imCounterEnd( dlg->GetCounter() );
	
	if (!failed)	
		this->Modify();
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

/*
bool CmpFile::Collate( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );

    wxArrayString paths, filenames;
    size_t nbOfFiles, book_nbOfFiles;
    
	nbOfFiles = m_bookFiles.GetCount();
    if ( nbOfFiles == 0 )
        return false;  
    
    bool failed = false;

    wxString ext, shortname, outfile;
    for ( int i = 0; i < (int)nbOfFiles; i++ )
    {
        if ( dlg->GetCanceled() )
            break;
			
		RecBookFile *book = m_bookFiles[i].m_recBookFilePtr;
		
		MusMLFOutput *m_mlf = new MusMLFOutput( NULL, m_basename + book->m_shortname + ".mlf", "MusMLFSymbol" );
		m_mlf->m_writePosition = true;
		
		book_nbOfFiles = book->RecognizedFiles( &filenames, &paths );
		for ( int j = 0; j < (int)book_nbOfFiles; j++ )
		{
			RecFile recFile( "cmp_add_file" );
			recFile.New();
	
			if ( !recFile.Open( paths[j] ) )
				continue;
	
			if ( !recFile.IsRecognized() ) 
			{
				wxLogWarning(_("File '%s' skipped"), paths[j].c_str() );
				continue;
			}
	
			//wxString operation =  wxString::Format( _("Adding data from file '%s' (%d/%d)"), shortname.c_str(), file_nb + 1, nb_files ) ;
			//dlg->SetOperation( operation );
	
			// counter
			//int counter = dlg->GetCounter();
			//int count = 1;
			//imCounterTotal( counter, count , operation.c_str() );

			if ( !failed && !dlg->GetCanceled() )
				failed = !m_mlf->WritePage( &recFile.m_musDocPtr->m_pages[0], wxFileName( filenames[j] ).GetName(), recFile.m_imPagePtr );
			//imCounterInc( dlg->GetCounter() );	
		}
		m_mlf->Close();
		delete m_mlf;
    }

    imCounterEnd( dlg->GetCounter() );
	
	if (!failed)	
		this->Modify();
	
	return !failed;
}
*/

/*
bool CmpFile::Collate( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
	nbOfFiles = m_bookFiles.GetCount();
    if ( nbOfFiles == 0 )
        return false;  
    
    bool failed = false;

    wxString ext, shortname, outfile;
    for ( int i = 0; i < (int)nbOfFiles; i++ )
    {
        if ( dlg->GetCanceled() )
            break;
			
		RecBookFile *book = m_bookFiles[i].m_recBookFilePtr;
		/ *
		CmpMLFInput *m_cmpinput = new CmpMLFInput( NULL, m_basename + book->m_shortname + ".mlf"  );
		
		MusLaidOutStaff *musStaff = m_cmpinput->ImportFileInStaff( );
        musStaff->no = 0;
        //musStaff->indent = imStaff->CalcIndentation( x1 );
        //musStaff->ecart = (m_imPagePtr->ToViewY( imStaff->m_y ) -  previous ) / musPage->defin;
        musStaff->vertBarre = START_END;
        //previous += musStaff->ecart * musPage->defin;
        //musPage->m_staves.Add( musStaff );

		//m_cmpinput->Close();
		delete m_cmpinput;
		
		MusBinOutput *bin_output = new MusBinOutput( NULL, m_basename + book->m_shortname + ".swwg", MUS_BIN_ARUSPIX_CMP  );
		bin_output->WriteStaff( musStaff );
		delete bin_output;
		* /
		
		MusBinInput_1_X *bin_input = new MusBinInput_1_X( NULL, m_basename + book->m_shortname + ".swwg", MUS_BIN_ARUSPIX_CMP );
		MusLaidOutStaff musStaff_loaded;
		bin_input->ReadStaff( &musStaff_loaded );
		delete bin_input;
		
		
		CmpMLFOutput *m_cmpoutput = new CmpMLFOutput( NULL, m_basename + book->m_shortname + ".staff", "CmpMLFSymb"  );
		m_cmpoutput->WriteStaff( &musStaff_loaded );
		m_cmpoutput->Close();
		delete m_cmpoutput;
		
		
    }

    imCounterEnd( dlg->GetCounter() );
	
	if (!failed)	
		this->Modify();
	
	return !failed;
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

