#include "unzip.h"
#include "zip.h"

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>

#define MKDIR(d) mkdir(d, 0775)

const int WRITE_BUFFER_SIZE = 16384;
const int MAX_FILENAME_LEN = 256;

// Errors id
const int ERROR_CREATE_ZIP = -100;
const int ERROR_GET_CRC32 = -101;
const int ERROR_WHILE_READ = -102;
const int ERROR_FILE_NOT_FOUND = -103;
const int ERROR_ZIP_FILE_NOT_FOUND = -104;
const int ERROR_ZIP_FILE = -105;


void getFileTime(const char *filename, tm_zip *tmzip, uLong *dostime) {
    struct stat s = { 0 };
    time_t tm_t = 0;

    if (strcmp(filename, "-") != 0) {
        char name[MAX_FILENAME_LEN + 1];

        int len = strlen(filename);
        if (len > MAX_FILENAME_LEN) {
            len = MAX_FILENAME_LEN;
        }

        strncpy(name, filename, MAX_FILENAME_LEN - 1);
        name[MAX_FILENAME_LEN] = 0;

        if (name[len - 1] == '/') {
            name[len - 1] = 0;
        }

        /* not all systems allow stat'ing a file with / appended */
        if (stat(name, &s) == 0) {
            tm_t = s.st_mtime;
        }
    }

    struct tm* filedate = localtime(&tm_t);
    tmzip->tm_sec  = filedate->tm_sec;
    tmzip->tm_min  = filedate->tm_min;
    tmzip->tm_hour = filedate->tm_hour;
    tmzip->tm_mday = filedate->tm_mday;
    tmzip->tm_mon  = filedate->tm_mon;
    tmzip->tm_year = filedate->tm_year;
}

void setFileTime(const char *filename, uLong dosdate, tm_unz tmu_date) {
    struct tm newdate;
    newdate.tm_sec  = tmu_date.tm_sec;
    newdate.tm_min  = tmu_date.tm_min;
    newdate.tm_hour = tmu_date.tm_hour;
    newdate.tm_mday = tmu_date.tm_mday;
    newdate.tm_mon  = tmu_date.tm_mon;

    if (tmu_date.tm_year > 1900) {
        newdate.tm_year = tmu_date.tm_year - 1900;
    } else {
        newdate.tm_year = tmu_date.tm_year;
    }
    newdate.tm_isdst = -1;

    struct utimbuf ut;
    ut.actime = ut.modtime = mktime(&newdate);
    utime(filename, &ut);
}

int isLargeFile(const char* filename) {
    FILE* pFile = fopen64(filename, "rb");
    if (pFile == NULL) return 0;

    fseeko64(pFile, 0, SEEK_END);
    ZPOS64_T pos = ftello64(pFile);
    fclose(pFile);

    return (pos >= 0xffffffff);
}

// Calculate the CRC32 of a file
int getCRC32(const char* filenameinzip, Bytef *buf, unsigned long size_buf, unsigned long* result_crc) {
    unsigned long calculate_crc = 0;

    int status = ZIP_OK;

    FILE *fin = fopen64(filenameinzip, "rb");
    if (fin == NULL) status = ERROR_GET_CRC32;
    else {
        unsigned long size_read = 0;
        do {
            size_read = (int) fread(buf, 1, size_buf, fin);

            if ((size_read < size_buf) && (feof(fin) == 0)) {
                status = ERROR_WHILE_READ;
            }

            if (size_read > 0) {
                calculate_crc = crc32(calculate_crc, buf, size_read);
            }
        } while ((status == ZIP_OK) && (size_read > 0));
    }

    if (fin) {
        fclose(fin);
    }

    *result_crc = calculate_crc;
    return status;
}

int extractCurrentFile(unzFile uf, const char* filename_inzip,const char *password) {
	
    uInt size_buf = WRITE_BUFFER_SIZE;
    void* buf = (void*) malloc(size_buf);
    if (buf == NULL) return UNZ_INTERNALERROR;

    int status = unzOpenCurrentFilePassword(uf, password);
    const char* write_filename = filename_inzip;

    // Create the file on disk so we can unzip to it
    FILE* fout = NULL;
    if (status == UNZ_OK) {
        fout = fopen64(write_filename, "wb");
    }

    // Read from the zip, unzip to buffer, and write to disk
    if (fout != NULL) {
        do {
            status = unzReadCurrentFile(uf, buf, size_buf);
            if (status <= 0) break;
            if (fwrite(buf, status, 1, fout) != 1) {
                status = UNZ_ERRNO;
                break;
            }
        } while (status > 0);

        if (fout) fclose(fout);

        // Set the time of the file that has been unzipped
        if (status == 0) {
        	//setFileTime(write_filename, file_info.dosDate, file_info.tmu_date);
        }
    }

    unzCloseCurrentFile(uf);

    free(buf);
    return status;
}

int createZip(const char* zipfilename, const char* filename, const char* password) {

    int status = 0;
    int opt_compress_level = Z_DEFAULT_COMPRESSION;

    // Create archive zipfilename
    zipFile zf = zipOpen64(zipfilename, APPEND_STATUS_CREATE);
    if (zf == NULL) {
        status = ERROR_CREATE_ZIP;
    }

    int size_buf = WRITE_BUFFER_SIZE;
    Bytef* buf = (Bytef*) malloc(size_buf);

    // Get information about the file on disk so we can store it in zip
    zip_fileinfo zi = { {0} };
    getFileTime(filename, &zi.tmz_date, &zi.dosDate);

    unsigned long crcFile = 0;
    if (status == ZIP_OK) {
        status = getCRC32(filename, buf, size_buf, &crcFile);
    }

    int zip64 = isLargeFile(filename);

    // Construct the filename that our file will be stored in the zip as.
    const char *savefilenameinzip = filename;
    {
        const char *tmpptr = NULL;
        const char *lastslash = 0;

        for (tmpptr = savefilenameinzip; *tmpptr; tmpptr++) {
            if (*tmpptr == '\\' || *tmpptr == '/') {
                lastslash = tmpptr;
            }
        }
        if (lastslash != NULL) {
            savefilenameinzip = lastslash + 1;
        }
    }

    // Create zip file
    status = zipOpenNewFileInZip3_64(zf, savefilenameinzip, &zi, NULL, 0, NULL, 0, NULL /* comment*/,
    		(opt_compress_level != 0) ? Z_DEFLATED : 0, opt_compress_level, 0,
            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, password, crcFile, zip64);

    // Add file to zip
    FILE *fin = NULL;
    if (status == ZIP_OK) {
        fin = fopen64(filename, "rb");
        if (fin == NULL) {
        	status = ERROR_FILE_NOT_FOUND;
        }
    }

    int size_read = 0;
    if (status == ZIP_OK) {
        // Read contents of file and write it to zip
        do {
            size_read = (int) fread(buf, 1, size_buf, fin);
            if ((size_read < size_buf) && (feof(fin) == 0)) {
                status = ERROR_WHILE_READ;
            }

            if (size_read > 0) {
                status = zipWriteInFileInZip(zf, buf, size_read);
            }
        } while ((status == ZIP_OK) && (size_read > 0));
    }

    if (fin) {
    	fclose(fin);
    }

    if (status >= 0) {
        status = zipCloseFileInZip(zf);
    }

    zipClose(zf, NULL);

    // Release memory
    free(buf);

    return status;
}

/* int extractZip(const char* zipfilename, const char* dirname, const char* password) {

    int status = 0;

    unzFile uf = NULL;

    // Open zip file
    if (zipfilename != NULL) {
        uf = unzOpen64(zipfilename);
    }
    if (uf == NULL) {
    	return ERROR_ZIP_FILE_NOT_FOUND;
    }

    // Extract all
    status = unzGoToFirstFile(uf);
    if (status != UNZ_OK) {
    	return ERROR_ZIP_FILE;
    }

    chdir(dirname);
    status = extractCurrentFile(uf, password);

    return status;
} */


#include <string.h>
#include "debug.h"

#define dir_delimter '/'
#define MAX_FILENAME 512
#define READ_SIZE 8192
/*解压zip文件到指定目录*/
int extractZip(const char* file,const char* dir)
{
    // Open the zip file
    unzFile *zipfile = unzOpen64( file );
    if ( zipfile == NULL )
    {
        LOG_PERROR_INFO("%s: not found\n" ,file);
        return -1;
    }

    // Get info about the zip file
    unz_global_info64 global_info;
    if ( unzGetGlobalInfo64( zipfile, &global_info ) != UNZ_OK )
    {
        LOG_PERROR_INFO("could not read file global info\n" );
        unzClose( zipfile );
        return -1;
    }

    // Buffer to hold data read from the zip file.
    //char read_buffer[ READ_SIZE ];

    // Loop to extract all files
    uLong i;
    for ( i = 0; i < global_info.number_entry; ++i )
    {
        // Get info about current file.
        unz_file_info64 file_info;
        char filename[ MAX_FILENAME ];
		char savename[ MAX_FILENAME ];
		
		int status = unzGetCurrentFileInfo64(zipfile, &file_info, filename, MAX_FILENAME, NULL, 0, NULL, 0);
		if (status != UNZ_OK) {
			LOG_PERROR_INFO("could not read file info\n" );
            unzClose( zipfile );
			return status;
		}

        // Check if this entry is a directory or file.
        const size_t filename_length = strlen( filename );
        if ( filename[ filename_length-1 ] == dir_delimter )
        {
            // Entry is a directory, so create it.
            LOG_PERROR_INFO("dir:%s\n", filename );
			snprintf(savename, MAX_FILENAME , "%s/%s",dir,filename);
            mkdir( savename,755 );
        }
		else
        {
            // Entry is a file, so extract it.
			snprintf(savename, MAX_FILENAME , "%s/%s",dir,filename);
            LOG_PERROR_INFO("-----> extract to file:%s\n", savename );
            extractCurrentFile(zipfile,savename,NULL);
        }

        // Go the the next entry listed in the zip file.
        if ( ( i+1 ) < global_info.number_entry )
        {
            if ( unzGoToNextFile( zipfile ) != UNZ_OK )
            {
                LOG_PERROR_INFO("cound not read next file\n" );
                unzClose( zipfile );
                return -1;
            }
        }
    }

    unzClose( zipfile );

    return 0;
}