
#ifndef __IM_KMEANS_H__
#define __IM_KMEANS_H__

#if	defined(__cplusplus)
extern "C" {
#endif

//typedef unsigned char imbyte;
#include <im_util.h>

double *kmeans(imbyte *im, int imsize, imbyte *mask, int k);

#if defined(__cplusplus)
}
#endif

#endif // __IM_KMEANS_H__
