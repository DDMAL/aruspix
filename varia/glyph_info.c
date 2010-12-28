/////////////////////////////////////////////////////////////////////////////
// Author:      Laurent Pugin
//
// Retrieve the bounding box size of a glyph (glyph code) of a font
//
// Requires the FreeType2 library 
// Compilation: gcc `freetype-config --cflags --libs` -o glyph_info glyph_info.c 
//
/////////////////////////////////////////////////////////////////////////////                                                   

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H 


int
main( int     argc,
      char**  argv )
{
  FT_Library    library;
  FT_Face       face;

  char*         filename;
  int           glyph_code, error;



  if ( argc != 3 )
  {
    fprintf ( stderr, "usage: %s font-file glyph-code\nexample: %s /path/to/test.ttf 0x002C\n", argv[0], argv[0] );
    exit( 1 );
  }

  filename      = argv[1];                           /* first argument     */
  glyph_code    = atoi(argv[2]);                           /* second argument    */

  error = FT_Init_FreeType( &library );              /* initialize library */
  if (error) {
    printf("Cannot load the library\n");
    return 1;
  }
  
  error = FT_New_Face( library, argv[1], 0, &face ); /* create face object */
  if (error) {
    printf("Cannot load the font %s\n", filename);
    return 1;
  }
  
  printf("Number of glyphs in the font: %d\n", (int)face->num_glyphs);
  printf("Units per EM: %d\n", (int)face->units_per_EM);
  
  int glyph_index = FT_Get_Char_Index( face, glyph_code );
  if (glyph_index == 0) {
    printf("Cannot find the glyph %d\n", glyph_code);
    return 1;
  }
  
  printf("Glyph index: %d\n", glyph_index );

  FT_Glyph glyph;
  FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_SCALE );   
  FT_Get_Glyph( face->glyph, &glyph ); 
  
  FT_BBox bbox; 
  FT_Glyph_Get_CBox( glyph, FT_GLYPH_BBOX_UNSCALED, &bbox ); 
  
  int width = bbox.xMax - bbox.xMin; 
  int height = bbox.yMax - bbox.yMin; 
  
  printf("Glyph width: %d\nGlyph height: %d\n", width, height);

  FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  return 0;
}

/* EOF */
