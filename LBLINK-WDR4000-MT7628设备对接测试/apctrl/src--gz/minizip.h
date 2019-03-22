/* Header for class com_dazhi_Minizip */

#ifndef __MINIZIP_H
#define __MINIZIP_H

/*
 * Class:     com_dazhi_Minizip
 * Method:    createZip
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
int createZip(const char* ,const char* ,const char*);

/*
 * Class:     com_dazhi_Minizip
 * Method:    extractZip
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
//int extractZip(const char* ,const char* ,const char*);
int extractZip(const char* file,const char* dir);

/*
 * Class:     com_dazhi_Minizip
 * Method:    getFilenameInZip
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
int getFilenameInZip(const char* ,char*);

#endif  //__MINIZIP_H
