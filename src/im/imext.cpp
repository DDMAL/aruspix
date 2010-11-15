/////////////////////////////////////////////////////////////////////////////
// Name:        im_ext.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// Extenstion de IM_LIB
/////////////////////////////////////////////////////////////////////////////

#include "imext.h"
#include "imkmeans.h"

#include <im.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_counter.h>

//#define 	min(a, b)   ((a)<(b)?(a):(b))
//#define 	max(a, b)   ((a)>(b)?(a):(b))


// Function taken from im_convolve_rank.cpp in imlib
template <class T, class DT> 
static int DoConvolveRankFunc(T *map, DT* new_map, int width, int height, int kw, int kh, DT (*func)(T* value, int count, int center), int counter)
{
  T* value = new T[kw*kh];
  int offset, new_offset, i, j, x, y, v, c;
  int kh1, kw1, kh2, kw2;

  kh2 = kh/2;
  kw2 = kw/2;
  kh1 = -kh2;
  kw1 = -kw2;
  if (kh%2==0) kh2--;  // if not odd decrease 1
  if (kw%2==0) kw2--;

  for(j = 0; j < height; j++)
  {
    new_offset = j * width;

    for(i = 0; i < width; i++)
    {
      v = 0; c = 0;
    
      for(y = kh1; y <= kh2; y++)
      {
        if ((j + y < 0) ||        // pass the bottom border
            (j + y >= height))    // pass the top border
          continue;

        offset = (j + y) * width;

        for(x = kw1; x <= kw2; x++)
        {
          if ((i + x < 0) ||      // pass the left border
              (i + x >= width))   // pass the right border
            continue;

          if (x == 0 && y == 0)
            c = v;

          value[v] = map[offset + (i + x)];
          v++;
        }
      }
      
      new_map[new_offset + i] = (DT)func(value, v, c);
    }    

    if (!imCounterInc(counter))
    {
      delete[] value;
      return 0;
    }
  }

  delete[] value;
  return 1;
}


void imSetData( _imImage *image, _imImage *selection, int pos_x, int pos_y )
{
    int w = selection->width;
    int h = selection->height;
    int sel_pos_x = 0;
    int sel_pos_y = 0;
    
	if ((pos_x > image->width) || (pos_y > image->height)) // we cannot copy outside the image
		return;
        
    // first adjust the origine
    if (pos_x < 0) { // move the origine and reduce the width
        w += pos_x;
        sel_pos_x = -pos_x;
        pos_x = 0;
    }    
    if (pos_y < 0) { // idem
        h += pos_y;
        sel_pos_y = -pos_y;
        pos_y = 0;
    }
    
    // then adjust the with/height     
	if (pos_x + w > image->width) {
        w = image->width - pos_x;
    } 
    if (pos_y + h > image->height) {
		h = image->height - pos_y;
    }
    
	if ((w <= 0) || (h <= 0)) // we cannot copy nothing or less...
		return;

	int type_size = imDataTypeSize(image->data_type);
	for (int i = 0; i < image->depth; i++)
	{
		imbyte *im_map = (imbyte*)image->data[i];
		imbyte *sel_map = (imbyte*)selection->data[i];

		for	(int y = 0; y < h ; y++)
		{
			int im_offset = (y + pos_y) * image->line_size + pos_x * type_size;
			int sel_offset = (y + sel_pos_y) * selection->line_size + sel_pos_x * type_size;

			memcpy(&im_map[im_offset], &sel_map[sel_offset], w * type_size);
		}
	}
}


bool imProcessSafeCrop( _imImage *image, int *width, int *height, int *pos_x, int *pos_y )
{
    int x = *pos_x;
    int y = *pos_y;
    int w = *width;
    int h = *height;

	if ((x > image->width) || (y > image->height)) // we cannot crop outside the image
		return false;
     
    // first adjust the origine
    if (x < 0) { // move the origine and reduce the width
        w += x;
        x = 0;
    }    
    if (y < 0) { // idem
        h += y;
        y = 0;
    }
    
    // then adjust the with/height     
	if (x + w > image->width) {
        w = image->width - x;
    } 
    if (y + h > image->height) {
		h = image->height - y;
    }
    
	if ((w <= 0) || (h <= 0)) // we cannot nothing or less...
		return false;
        
    // create the image
    *pos_x = x;
    *pos_y = y;
    *width = w;
    *height = h;
    return true;
}


/* function that calculate median of an array with bubble sort algorithm */
int median( int a[], int size, bool sort_array )
{
	if ( !a || ( size==0 ) )
		return 0;

   //bubbleSort
   int i, j, tmp;

   if ( !sort_array )
   {
	   int *b = new int[ size ];
	   memcpy( b, a, size * sizeof(int) );
	   a = b;
   }

   for ( i = 1; i < size; i++ ) {
      for ( j = 0; j < size - 1; j++ ) {
         if ( a[ j ] > a[ j + 1 ] ) {
            tmp = a[ j ];
            a[ j ] = a[ j + 1 ];
            a[ j + 1 ] = tmp;
         }
      }
   }
   int med_value;
   if ( ( size % 2 ) == 0)
	   med_value = ( a[ (size - 1)/2 ] + a[ (size + 1)/2 ] ) / 2;
   else
	   med_value =  a[ size / 2 ];

   if ( !sort_array )
		delete[] a;

   return med_value;
}


float medianf( float a[], int size, bool sort_array )
{
	if ( !a || ( size==0 ) )
		return 0;

   //bubbleSort
   int i, j;
   float tmp;

   if ( !sort_array )
   {
	   float *b = new float[ size ];
	   memcpy( b, a, size * sizeof(float) );
	   a = b;
   }

   for ( i = 1; i < size; i++ ) {
      for ( j = 0; j < size - 1; j++ ) {
         if ( a[ j ] > a[ j + 1 ] ) {
            tmp = a[ j ];
            a[ j ] = a[ j + 1 ];
            a[ j + 1 ] = tmp;
         }
      }
   }
   float med_value;
   if ( ( size % 2 ) == 0)
	   med_value = ( a[ (size - 1)/2 ] + a[ (size + 1)/2 ] ) / 2;
   else
	   med_value =  a[ size / 2 ];

   if ( !sort_array )
		delete[] a;

   return med_value;
}

int max_val( int a[], int size , int *pos )
{
	if (pos)
		*pos = 0;

	if ( !a || ( size==0 ) )
		return 0;

	int max = 0;
	int p = 0;

	for (int  i = 0; i < size; i++ )
	{
		if ( a[i] >= max )
		{
			max = a[i];
			p = i;
		}
	}

	if (pos)
		*pos = p;

	return max;
}


int sum( int a[], int size )
{
	if ( !a || ( size==0 ) )
		return 0;

	int sum = 0;
	for (int  i = 0; i < size; i++ )
		sum  += a[i];

	return sum;
} 


int count( int a[], int size )
{
	if ( !a || ( size==0 ) )
		return 0;

	int count = 0;
	for (int  i = 0; i < size; i++ )
		if ( a[i] )
			count++;

	return count;
}

void corr( int a[], int b[], int size, int win, int *dec, int *max)
{
	if ( !a || !b || !dec || !max ) 
		return;

	int pad = size + 2 * win;
	int *c = new int[ pad ];
	memset( c, 0, pad * sizeof(int));
	memcpy( c + win, a, size * sizeof(int) ); 
    
    int conv_width = 2 * win;
	int *mask = new int[ size ];

    int max_val = 0, max_pos = 0;
    for (int x = 0; x < conv_width; x++)
    {
		memcpy( mask, c + x, size * sizeof(int) );
        int sum = 0;
        for (int i = 0; i < size; i++)
        {
			sum += mask[i] * b[i];
        }
		if (sum > max_val)
        {
			max_val = sum;
            max_pos = x;
        }
    }

    *dec = max_pos - win;
	*max = max_val;
	if ( max_val == 0 )
		*dec = 0;
    delete[] c;
	delete[] mask;
}

/*
	Analyse les runs dans une image b/w
	peak_val est la longueur du run le plus represente dans l'image
	median_val est la longueur median de tous les runs
 */
void imAnalyzeRuns(const imImage* image, int *peak_val, int *median_val, int type, bool vertical)
{
    imbyte *bufIm = (imbyte*)image->data[0];
	int h, w;
	if ( vertical )
	{
		h = image->height;
		w = image->width;
	}
	else
	{
		w = image->height;
		h = image->width;		
	}
	
	// runs
	int* runs = (int*)malloc( h * w * sizeof(int) );
	memset(runs, 0, h * w * sizeof(int) );
	
	// tableau compter les runs de chaque longueur (pour touver peak)
	int* vals = (int*)malloc( h * sizeof(int) );
	memset(vals, 0, h * sizeof(int) );

    int x, y;
	int run_type, run_val;
	int i = 0;

    for (x = 0; x < w; x++)
    {
        run_type = 0;
        run_val = 0;
        for (y = 0; y < h; y++)
        {
            int offset; 
			
			if (vertical)
				offset = y * w + x;
			else
				offset = x * h + y;
				
            if ( bufIm[ offset ] == run_type )
                run_val++;
            else // changement
            {
                if ( ( run_type == type ) && ( run_val > 0 )  ) // run recherche
                {
                    runs[i] = run_val;
					vals[run_val] = vals[run_val] + 1;
                    i++;
                }
                run_type = ( run_type == 1 ) ? 0 : 1 ;
                run_val = 0;
            }
        }
    }

	if ( i > 0 )
	{
		max_val( vals, h, peak_val );
		(*median_val) = median( runs, i, false );
	}
	else
	{
		*peak_val = 0;
		*median_val = 0;
	}
	
	free( runs );
	free( vals );

}

/*
	Calcule la projection horizontale d'une image
	hist doit avoir la taille de la hauteur de l'image
 */
void imAnalyzeProjectionH(const imImage* image, int* hist)
{
	imbyte* img_data = (imbyte*)image->data[0];

	for (int y = 0; y < image->height; y++)
    {
		int hist_val = 0;
 		for (int x = 0; x < image->width; x++)
		{
			int offset = y * image->width + x;
			hist_val += img_data[ offset ];
		}
		hist[y] = hist_val;
    }
}

/*
	Calcule la projection verticale d'une image
	hist doit avoir la taille de la largeur de l'image
 */
void imAnalyzeProjectionV(const imImage* image, int* hist)
{
	imbyte* img_data = (imbyte*)image->data[0];

	for (int x = 0; x < image->width; x++)
	{
		int hist_val = 0;
 		for (int y = 0; y < image->height; y++)
		{
			int offset = y * image->width + x;
			hist_val += img_data[ offset ];

		}
		hist[x] = hist_val;
	}
}


/*
	Supprime les element dont la hauteur moyenne n'est pas entre min et max
	Si max = 0, le seuil superieur est ignore
	*image est une image labelisee (bg = 0, puis 1,2 ...)
	region_count est le nombre de regions
 */
void imAnalyzeClearHeight(const imImage* image, int region_count, int min_threshold, int max_threshold )
{
	imushort* img_data = (imushort*)image->data[0];
	int i;

	// tableau pour les sommes par colonne, 1 largeur par region
	int* heights = (int*)malloc( image->width * region_count * sizeof(int) );
	memset(heights, 0, image->width * region_count * sizeof(int) );

	for (i = 0; i < image->count; i++)
	{
		if (*img_data)
			heights[ ((*img_data) - 1) * image->width + i % image->width ]++;
		img_data++;
	}

	img_data = (imushort*)image->data[0];
	for (i = 0; i < image->count; i++)
	{
		if (*img_data)
		{
			if ( heights[ ((*img_data) - 1) * image->width + i % image->width ] < min_threshold)
				(*img_data) = 0;
			else if ( max_threshold && (heights[ ((*img_data) - 1) * image->width + i % image->width ] > max_threshold) )
				(*img_data) = 0;
		}
		img_data++;
	}
	free(heights);
}


/*
	Supprime les element dont la largeur ou la hauteur maximal est plus petite que threshold
	*image est une image labelisee (bg = 0, puis 1,2 ...)
	region_count est le nombre de regions
 */
void imAnalyzeClearMin(const imImage* image, int region_count, int threshold )
{
	imushort* img_data = (imushort*)image->data[0];
	int i, j;

	int* boxes = (int*)malloc(4 * region_count * sizeof(int));
    memset(boxes, 0, 4 *  region_count * sizeof(int));
    imAnalyzeBoundingBoxes(image, boxes, region_count);

	img_data = (imushort*)image->data[0];
	for (i = 0; i < image->count; i++)
	{
		if (*img_data)
		{
			j = ((*img_data) - 1) * 4;
			if ( (boxes[j+1] - boxes[j+0] < threshold) || (boxes[j+3] - boxes[j+2] < threshold) )
				(*img_data) = 0;
		}
		img_data++;
	}
	free( boxes );
}

/*
	Supprime les element dont la largeur moyenne n'est pas entre min et max
	Si max = 0, le seuil superieur est ignore
	*image est une image labelisee (bg = 0, puis 1,2 ...)
	region_count est le nombre de regions
 */
void imAnalyzeClearWidth(const imImage* image, int region_count, int min_threshold, int max_threshold )
{
	imushort* img_data = (imushort*)image->data[0];
	int i;

	// tableau pour les sommes par colonne, 1 largeur par region
	int* widths = (int*)malloc( image->height * region_count * sizeof(int) );
	memset(widths, 0, image->height * region_count * sizeof(int) );

	for (i = 0; i < image->count; i++)
	{
		if (*img_data)
			widths[ ((*img_data) - 1) * image->height + i / image->width ]++;
		img_data++;
	}

	img_data = (imushort*)image->data[0];
	for (i = 0; i < image->count; i++)
	{
		if (*img_data)
		{
			if ( widths[ ((*img_data) - 1) * image->height + i / image->width ] < min_threshold)
				(*img_data) = 0;
			else if ( max_threshold && (widths[ ((*img_data) - 1) * image->height + i / image->width ] > max_threshold) )
				(*img_data) = 0;
		}
		img_data++;
	}
	free(widths);
}

/*
	calcule les bounding boxes pour chaque label
	*image est une image labelisee (bg = 0, puis 1,2 ...)
	region_count est le nombre de regions
	boxes est tableau des bounding boxes 4 * region_count : pour chaque region xmin xmax ymin ymax
 */
void imAnalyzeBoundingBoxes(const imImage* image, int* boxes, int region_count )
{
	// boxes = tableau des bounding boxes
	// 4 * region_count : pour chaque region xmin xmax ymin ymax
	int i;

	for (i = 0; i < region_count; i++)
	{
		boxes[4 * i + 0] = image->width;
		boxes[4 * i + 2] = image->height;
	}

	imushort* img_data = (imushort*)image->data[0];

	int x, y, idx;
	for (i = 0; i < image->count; i++)
	{
		if (*img_data)
		{
			idx = ((*img_data) - 1) * 4;
			x = i % image->width;
			y = i / image->width;
			if ( boxes[ idx + 0 ] > x ) 
				boxes[ idx + 0 ] = x;
			else if ( boxes[ idx + 1 ] < x ) 
				boxes[ idx + 1 ] = x;
			if ( boxes[ idx + 2 ] > y ) 
				boxes[ idx + 2 ] = y;
			else if ( boxes[ idx + 3 ] < y ) 
				boxes[ idx + 3 ] = y;
		}
		img_data++;
	}
}

static unsigned char Kittler(const imImage* src_image, double *mu_1, double *mu_2, double *mu)
{
  unsigned long h[256];
  int threshold;
  double criterion;
  int g;
  int n;
  int T_low, T_high;
  int P_1_T, P_2_T, P_tot;
  double mu_1_T, mu_2_T;
  double sum_gh_1, sum_gh_2, sum_gh_tot;
  double sum_ggh_1, sum_ggh_2, sum_ggh_tot;
  double sigma_1_T, sigma_2_T;
  double J_T;

  imCalcHistogram((imbyte*)src_image->data[0], src_image->count, h, 0);

  criterion = 1e10;
  threshold = 127;
  J_T = criterion;
  
  T_low = 0;
  while((h[T_low] == 0) && (T_low < 255))
    T_low++;

  T_high = 255;
  while((h[T_high] == 0) && (T_high > 0))
    T_high--;

  n = 0;
  for (g=T_low; g<=T_high; g++)
    n += h[g];

  P_1_T = h[T_low];
  P_tot = 0;
  for (g=T_low; g<= T_high; g++)
    P_tot += h[g];

  sum_gh_1 = T_low * h[T_low];
  sum_gh_tot = 0.0;
  for (g=T_low; g<=T_high; g++)
    sum_gh_tot += g*h[g];

  *mu = sum_gh_tot * 1.0 / n;

  sum_ggh_1 = T_low*T_low*h[T_low];
  sum_ggh_tot = 0.0;
  for (g=T_low; g<=T_high; g++)
    sum_ggh_tot += g*g*h[g];
  
  for (g=T_low+1; g<T_high-1; g++)
    {
      P_1_T += h[g];
      P_2_T = P_tot - P_1_T;

      sum_gh_1 += g*h[g];
      sum_gh_2 = sum_gh_tot - sum_gh_1;

      mu_1_T = sum_gh_1 / P_1_T;
      mu_2_T = sum_gh_2 / P_2_T;

      sum_ggh_1 += g*g*h[g];
      sum_ggh_2 = sum_ggh_tot - sum_ggh_1;

      sigma_1_T = sum_ggh_1/P_1_T - mu_1_T * mu_1_T;
      sigma_2_T = sum_ggh_2/P_2_T - mu_2_T * mu_2_T;

      /* Equation (15) in the article */
      if ((sigma_1_T != 0.0) && (P_1_T != 0) &&
	  (sigma_2_T != 0.0) && (P_2_T != 0))
	J_T = 1 + 2*(P_1_T*log(sigma_1_T) + P_2_T*log(sigma_2_T))
	  - 2*(P_1_T*log((double)P_1_T) + P_2_T*log((double)P_2_T) );

      if (criterion > J_T)
	{
	  criterion = J_T;
	  threshold = g;
	  *mu_1 = mu_1_T;
	  *mu_2 = mu_2_T;
	}
    }
  return threshold;
}

enum {
	IM_RANGE_MEAN,
	IM_RANGE_STDDEV
};


static float mean_op_byte(imbyte* value, int count, int center)
{
	float mean;
	for (int i = 0; i < count; i++)
		mean += (float)value[i];

	mean /= float(count);
	return mean;
}

static float stddev_op_byte(imbyte* value, int count, int center)
{
	float stddev;
	float mean;
	for (int i = 0; i < count; i++)
	{
		stddev += ((float)value[i])*((float)value[i]);
		mean += (float)value[i];
	}

	mean /= float(count);
	stddev = (float)sqrt((stddev - count * mean*mean)/(count-1.0));
	return stddev;
}

/*
	Uses DoConvolveRankFunc from im_convovle_rank
*/
int imProcessRange(const imImage* src_image, imImage* dst_image, int ks, int op_type )
{
	int ret = 0;
	int counter = imCounterBegin("Range Mean");
	imCounterTotal(counter, src_image->depth*src_image->height, "Filtering...");

	switch( op_type )
	{
	case IM_RANGE_MEAN:
		ret = DoConvolveRankFunc((imbyte*)src_image->data[0], (float*)dst_image->data[0], 
                             src_image->width, src_image->height, ks, ks, mean_op_byte, counter);
		break;
	case IM_RANGE_STDDEV:
		ret = DoConvolveRankFunc((imbyte*)src_image->data[0], (float*)dst_image->data[0], 
                             src_image->width, src_image->height, ks, ks, stddev_op_byte, counter);
		break;
	}
							 
	imCounterEnd(counter);
	return ret;
}

/*
	implementation using imProcessRange with two float images
*/

/*
int imProcessSauvolaThreshold( const imImage* image, imImage* dest, int region_size,
	float sensitivity, int dynamic_range, int lower_bound, int upper_bound, bool white_is_255 )
{
    if ((region_size < 1) || (region_size > min(image->width, image->height)))
		return 0;
	
	imImage *src = imImageDuplicate( image );
     
	if ( !white_is_255 )
		imProcessNegative( src, src );

	imImage *im_means = imImageCreate( image->width, image->height, IM_GRAY, IM_FLOAT );
	imImage *im_std_dev = imImageCreate( image->width, image->height, IM_GRAY, IM_FLOAT );

	int ret = 1;
    // Compute regional statistics.
	if ( ret )
		ret = imProcessRange( src, im_means, region_size, IM_RANGE_MEAN );
	if ( ret )
		ret = imProcessRange( src, im_std_dev, region_size, IM_RANGE_STDDEV );
	
	int counter = imCounterBegin("Sauvola threshold");
	imCounterTotal(counter, src->height, "Sauvola threshold");

	imbyte* src_data = (imbyte*)src->data[0];
	imbyte* dest_data = (imbyte*)dest->data[0];
	float* means = (float*)im_means->data[0];
	float* std_dev = (float*)im_std_dev->data[0];
	
	
	int offset, pixel_value;
	float mean, deviation, adjusted_deviation, threshold;

    for (int y = 0; y < src->height; y++) {
		if ( !ret ) // aborted or error
			break; 
        for (int x = 0; x < src->width; x++) {
			offset = y * src->width + x;
			pixel_value = src_data[ offset ];
            // Check global thresholds and then threshold adaptively.
            if (pixel_value < lower_bound) {
                dest_data[ offset ] = 1; // black, 1 in the destination image
            } else if (pixel_value >= upper_bound) {
                dest_data[ offset ] = 0; // white
            } else {
                mean = means[ offset ];
                deviation = std_dev[ offset ];
                adjusted_deviation
                    = deviation / (float)dynamic_range - 1.0;
                threshold
                    = mean + (1.0 + sensitivity * adjusted_deviation);
                dest_data[ offset ] = (pixel_value > threshold) ? 0 : 1;
            }
        }
		ret = imCounterInc(counter);
    }
	imImageDestroy( src );
	imImageDestroy( im_means );
	imImageDestroy( im_std_dev );
	imCounterEnd( counter );
	return ret;
}
*/


int imMeanAndStdDevFilter(const imImage *image, int region_size, float *means, float *std_dev, int counter )
{
     if ((region_size < 1) || (region_size > min(image->width, image->height)))
		return 0;

    int half_region_size = region_size / 2;
	
	int ulx, uly, lrx, lry, offset;
	imImage *region;
	imStats stats;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // Define the region.
			offset = y * image->width + x;
			ulx = max( 0, x - half_region_size);
			uly = max( 0, y - half_region_size);
			lrx = min( x + half_region_size, image->width - 1 );
			lry = min( y + half_region_size, image->height - 1 );
			region = imImageCreate( lrx - ulx, lry - uly, image->color_space, image->data_type ); 
			imProcessCrop( image, region, ulx, uly );  
            imCalcImageStatistics( region, &stats );
            means[ offset ] = stats.mean;
			std_dev[ offset ] = stats.stddev;
			imImageDestroy( region );
			if (!imCounterInc(counter))
				return 0;
        }
    }
	return 1;
}

int imProcessSauvolaThreshold( const imImage* image, imImage* dest, int region_size,
	float sensitivity, int dynamic_range, int lower_bound, int upper_bound, bool white_is_255 )
{
    if ((region_size < 1) || (region_size > min(image->width, image->height)))
		return 0;
	
	imImage *src = imImageDuplicate( image );
     
	if ( !white_is_255 )
		imProcessNegative( src, src );

	float* means = (float*)malloc( src->height * src->width * sizeof( float ) );
	memset( means, 0, src->height * src->width * sizeof( float ) );
	float* std_dev = (float*)malloc( src->height * src->width * sizeof( float ) );
	memset( std_dev, 0, src->height * src->width * sizeof( float ) );
	
	int counter = imCounterBegin("Sauvola threshold");
	imCounterTotal(counter, src->size + src->height, "Sauvola threshold");

	int ret = 0;
    // Compute regional statistics.
    ret = imMeanAndStdDevFilter(src, region_size, means, std_dev, counter );

	imbyte* src_data = (imbyte*)src->data[0];
	imbyte* dest_data = (imbyte*)dest->data[0];
	
	int offset, pixel_value;
	float mean, deviation, adjusted_deviation, threshold;

    for (int y = 0; y < src->height; y++) {
		if ( !ret ) // aborted or error
			break; 
        for (int x = 0; x < src->width; x++) {
			offset = y * src->width + x;
			pixel_value = src_data[ offset ];
            // Check global thresholds and then threshold adaptively.
            if (pixel_value < lower_bound) {
                dest_data[ offset ] = 1; // black, 1 in the destination image
            } else if (pixel_value >= upper_bound) {
                dest_data[ offset ] = 0; // white
            } else {
                mean = means[ offset ];
                deviation = std_dev[ offset ];
                adjusted_deviation
                    = deviation / (float)dynamic_range - 1.0;
                threshold
                    = mean + (1.0 + sensitivity * adjusted_deviation);
                dest_data[ offset ] = (pixel_value > threshold) ? 0 : 1;
            }
        }
		ret = imCounterInc(counter);
    }
	imImageDestroy( src );
    free(means);
    free(std_dev);
	imCounterEnd( counter );
	return ret;
}

int imProcessPuginThreshold(const imImage* image, imImage* dest, bool white_is_255 )
{
	int i;
	imImage *src = imImageDuplicate( image );
     
	if ( !white_is_255 )
		imProcessNegative( src, src );

	imImage *otsu_dest = imImageDuplicate( image );
	int otsu = imProcessOtsuThreshold( src, otsu_dest );
	int background = 255 - ((255 - otsu) / 2);
	
	int counter = imCounterBegin("Pugin threshold");
	imCounterTotal(counter, 2*src->size, "Pugin threshold");	

	imbyte* src_data = (imbyte*)src->data[0];
	int ret = 1;
	for( i = 0; i < src->size; i++ )
	{
		if ( !ret ) // aborted or error
			break;
	
		if ((*src_data) > background)
			(*src_data) = background;
		src_data++;
		ret = imCounterInc(counter);
	}
	imProcessExpandHistogram( src, src, 0.0 );
	
	//imbyte* mask = (imbyte*)malloc( src->size * sizeof( imbyte ) );
	//memset( mask, 0, src->size * sizeof( imbyte ) );
	src_data = (imbyte*)src->data[0];
	imbyte* dest_data = (imbyte*)dest->data[0];
	//double *means = kmeans( src_data, src->size, dest_data, 3);
	for( i = 0; i < dest->size; i++ )
	{
		if ( !ret ) // aborted or error
			break;
			
		if ((*dest_data) > 1)
			(*dest_data) = 1; // white, 1 in the destination image
		else
			(*dest_data) = 0; // black, 0 in the destination image
		dest_data++;
		ret = imCounterInc(counter);
	}
	imCounterEnd( counter );
	
	return ret;
}


int imProcessKittlerThreshold(const imImage* image, imImage* NewImage )
{
  double dummy_1, dummy_2, dummy_3;
  int level = Kittler(image , &dummy_1, &dummy_2, &dummy_3);
  imProcessThreshold(image, NewImage, level, 1);
  return level;
}

void imPhotogrammetric( const imImage* image, imImage* dest ){
	
	int height = image->height;
	int width = image->width;
	
	if ( height % 2 != 0 ) height += 1;
	if ( width % 2 != 0 ) width += 1;

	imImage *imagetmp = imImageCreate(width, height, image->color_space, image->data_type);
	imProcessAddMargins( image, imagetmp, 0, 0 );
	
	int i, j;
	double *vk = (double*) malloc( height * sizeof(double) );
	double *vl = (double*) malloc( width * sizeof(double) );
	double **matk = (double**) malloc( height * sizeof(double*) );
	double **matl = (double**) malloc( height * sizeof(double*) );
	
	imbyte* X = (imbyte*)imagetmp->data[0];
	double sum_vk, sum_vl;
	double a = 0;
	double b = 0;
	double c = 0;
	double **X2 = (double**) malloc ( height * sizeof(double*) );
	double **X3 = (double**) malloc ( height * sizeof(double*) );
	
	for ( i = 0; i < height; i++ ){
		matk[i] = (double*) malloc ( width * sizeof(double) );
		matl[i] = (double*) malloc ( width * sizeof(double) );
		X2[i] = (double*) malloc ( width * sizeof(double) );
		X3[i] = (double*) malloc ( width * sizeof(double) );
	}
	
	for ( i = 0; i < (height / 2); i++ ){
		vk[i] = 0;
		double val = -(height / 2) + i;
		if ( val <= -1 ) vk[i] = val;					// vk(1:(height/2)) = [-(height/2):1:-1]	
	}
	for ( i = (height / 2) ; i < height; i++ ){
		vk[i] = 0;
		double val = 1 + i - (height / 2);
		if ( val <= (height / 2) ) vk[i] = val;			// vk((height/2)+1:height) = [1:1:(height/2)]	
	}
	
	for ( i = 0; i < (width / 2); i++ ){
		vl[i] = 0;
		double val = -(width / 2) + i;
		if ( val <= -1 ) vl[i] = val;					// vl(1:(width/2)) = [-(width/2):1:-1]	
	}
	for ( i = (width / 2) ; i < width; i++ ){
		vl[i] = 0.0;
		double val = 1 + i - (width / 2);
		if ( val <= (width / 2) ) vl[i] = val;			// vl((width/2)+1:width) = [1:1:(width/2)]	
	}
	
	for ( i = 0; i < width; i++ )
		for ( j = 0; j < height; j++ )
			matk[j][i] = vk[j];							// matk = repmat(vk, 1, width)
		 
	for ( i = 0; i < height; i++ )
		for ( j = 0; j < width; j++ )
			matl[i][j] = vl[j];							// matl = repmat(vl, height, 1)

	/*
	 Coordinates of the plane:
	 a=sum(sum(X.*matk))/(width*sum(vk.^2));
	 b=sum(sum(X.*matl))/(height*sum(vl.^2));
	 c=sum(sum(X))/(height*width);
	*/	
	
	for ( i = 0; i < height; i++ ) sum_vk += (vk[i] * vk[i]);		// sum(vk.^2)
	for ( i = 0; i < width; i++ ) sum_vl += (vl[i] * vl[i]);		// sum(vl.^2)
	
	for ( i = 0; i < height; i++ ){
		for ( j = 0; j < width; j++ ){
			int offset = i*width + j;
			a += ( (double) X[offset] * matk[i][j] );				// sum(sum(X.*matk))
			b += ( (double) X[offset] * matl[i][j] );				// sum(sum(X.*matl))
			c += X[offset];											// sum(sum(X))
		}
	}
	
	a /= ( width * sum_vk );										// a=sum(sum(X.*matk))/(width*sum(vk.^2))
	b /= ( height * sum_vl );										// b=sum(sum(X.*matl))/(height*sum(vl.^2))
	c /= ( height * width );										// c=sum(sum(X))/(height*width)
	for ( i = 0; i < height; i++ ){
		for ( j = 0; j < width; j++ ){
			X2[i][j] = (matk[i][j] * a) + (matl[i][j] * b) + c;		// X2=matk.*a+matl.*b+c
			X3[i][j] = (double) X[i*width + j] - X2[i][j];			// X3 = X - X2
		}
	}
		
	/* Normalize */
	double min = X3[0][0];
	for ( i = 0; i < height; i++ )
		for ( j = 0; j < width; j++ )
			if ( min > X3[i][j] ) min = X3[i][j];					// Find minimum
			
	for ( i = 0; i < height; i++ )
		for ( j = 0; j < width; j++ )
			X3[i][j] = X3[i][j] + abs(min);							// X3=X3+abs(min(min(X3)))
			
	double max = X3[0][0];
	for ( i = 0; i < height; i++ )
		for ( j = 0; j < width; j++ )
			if ( max < X3[i][j] ) max = X3[i][j];					// Find maximum
	
	for ( i = 0; i < height; i++ ){
		for ( j = 0; j < width; j++ ){
			X3[i][j] = X3[i][j] / max;								// X3=X3+abs(min(min(X3)))
			
			//copy new image (X3) back into X
			X[i*width + j] = (imbyte) (X3[i][j] * 255);
		}
	}
	
	imProcessCrop(imagetmp, dest, 0, 0);
	
	for ( i = 0; i < height; i++ ){
		free( matk[i] );
		free( matl[i] );
		free( X2[i] );
		free( X3[i] );
	}
	free( matk );
	free( matl );
	free( X2 );
	free( X3 );
	free( vk );
	free( vl );
}

/*
	ecrit les valeurs d'un tableaux d'int (fonction de debbuging)
*/
void imSaveValues( int *values, int count, const char *filename )
{
	FILE *fid = fopen(filename, "w" );
	if ( !fid )
		return;

	for(int i = 0; i < count; i++)
		fprintf(fid,"%d\t%d\n", i, values[i]);

	//fprintf(fid, "\n");
	fclose( fid );

}

/*
void SupOldFile::DistByCorrelationFFT(const _imImage *im1, const _imImage *im2,
                                wxSize window, int *decalageX, int *decalageY)
{
    wxASSERT_MSG(decalageX, wxT("decalageX cannot be NULL") );
    wxASSERT_MSG(decalageY, wxT("decalagY cannot be NULL") );
    wxASSERT_MSG(im1, wxT("Image 1 cannot be NULL") );
    wxASSERT_MSG(im2, wxT("Image 2 cannot be NULL") );

    imImage *corr = imImageCreate( im1->width, im1->height, im1->color_space, IM_CFLOAT);
    imProcessCrossCorrelation( im1, im2, corr );
    imImage *corrCrop = imImageCreate( window.GetWidth() * 2 + 1, window.GetHeight() * 2 + 1,
        corr->color_space, IM_CFLOAT );
    int xmin = im1->width / 2 - window.GetWidth();
    int ymin = im1->height / 2 - window.GetHeight();
    imProcessCrop( corr, corrCrop, xmin, ymin );

    imImage *corrReal = imImageCreate( corrCrop->width , corrCrop->height , corrCrop->color_space, IM_BYTE );
    imConvertDataType( corrCrop, corrReal, IM_CPX_MAG, IM_GAMMA_LINEAR, 0, IM_CAST_MINMAX);

    int width = corrReal->width;
    int height = corrReal->height;
    int max = 0, maxX = 0, maxY = 0;
    imbyte *buf = (imbyte*)corrReal->data[0];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if ( buf[y * width + x] > max )
            {
                max = buf[y * width + x];
                maxX = x;
                maxY = y;

            }
        }
    }

    *decalageX = maxX - window.GetWidth();
    *decalageY = maxY - window.GetHeight();

    //int error;
    //imFile* ifile = NULL;
    //ifile = imFileNew("D:/Mes Images/corr1.tif", "TIFF", &error);
    //imFileSaveImage(ifile,corrReal);
    //imFileClose(ifile);

    imImageDestroy( corrReal );
    imImageDestroy( corrCrop );
    imImageDestroy( corr );
}
*/

/*
void DistByCorrelation( imImage *im1, imImage *im2, int width, int height, int *decalageX, int *decalageY)
{
    wxASSERT_MSG(decalageX, wxT("decalageX cannot be NULL") );
    wxASSERT_MSG(decalageY, wxT("decalagY cannot be NULL") );
    wxASSERT_MSG(im1, wxT("Image 1 cannot be NULL") );
    wxASSERT_MSG(im2, wxT("Image 2 cannot be NULL") );

    
	imProcessNegative( im1, im1 );
	imProcessNegative( im2, im2 );

	imImage *imTmp1 = imImageCreate(
            im1->width +  width * 2,
            im1->height +  height * 2,
            im1->color_space, im1->data_type);
	imProcessAddMargins( im1 ,imTmp1, width, height );


    int conv_width = 2 * width;
    int conv_height = 2 * height;
    imImage *mask = imImageCreate(im2->width, im2->height,im2->color_space, im2->data_type);
    imbyte *bufIm2 = (imbyte*)im2->data[0];
	
    int maxSum = 0, maxX = 0, maxY = 0;
    for (int y = 0; y < conv_height; y++)
    {
        for (int x = 0; x < conv_width; x++)
        {
            imProcessCrop(imTmp1,mask, x, y);
            imbyte *bufMask = (imbyte*)mask->data[0]; 
            int sum = 0;
            for (int i = 0; i < mask->plane_size; i++)
            {
                sum += (bufIm2[i] / 255) * (bufMask[i] / 255);
            }
            if (sum > maxSum)
            {
                maxSum = sum;
                maxX = x;
                maxY = y;
            }
        }
    }

    *decalageX = maxX - width;
    *decalageY = maxY - height;
    imImageDestroy(imTmp1);
    imImageDestroy(mask);
}
*/
