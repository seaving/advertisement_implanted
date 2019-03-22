#ifndef __GZIP_H__
#define __GZIP_H__

#include "zlib.h"

int gzcompress(Bytef *data, uLong ndata,
               Bytef *zdata, uLong *nzdata);

int gzdecompress(Byte *zdata, uLong nzdata,
                 Byte *data, uLong *ndata);

#endif



