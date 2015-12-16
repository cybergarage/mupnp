/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/io/file.h>

#if defined(MUPNP_USE_CFILE)

#include <mupnp/util/log.h>
#include <stdio.h>

#if defined(WIN32) && !defined(WINCE)
#include <windows.h>
#include <sys/stat.h>
#elif defined(WINCE)
#include <windows.h>

/*
#if (_WIN32_WCE < 0x501)
#include <sys\types.h> //from PortSDK
#endif

struct stat {

        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        _off_t st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };
*/

#elif defined(BTRON) || defined(ITRON) || defined(TENGINE)
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include <bsys/unixemu.h>
#else
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif

/****************************************
* mupnp_file_new
****************************************/

mUpnpFile* mupnp_file_new()
{
  mUpnpFile* file;

  mupnp_log_debug_l4("Entering...\n");

  file = (mUpnpFile*)malloc(sizeof(mUpnpFile));

  if (NULL != file) {
    mupnp_list_node_init((mUpnpList*)file);
    file->name = mupnp_string_new();
    file->path = mupnp_string_new();
    file->content = NULL;
    file->fp = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return file;
}

/****************************************
* mupnp_file_delete
****************************************/

void mupnp_file_delete(mUpnpFile* file)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)file);

  mupnp_string_delete(file->name);
  mupnp_string_delete(file->path);
  if (file->content != NULL)
    free(file->content);
  free(file);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_setname
****************************************/

void mupnp_file_setname(mUpnpFile* file, char* name)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(file->name, name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_getname
****************************************/

char* mupnp_file_getname(mUpnpFile* file)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(file->name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_getname
****************************************/

long mupnp_file_getlastmodified(mUpnpFile* file)
{
  char* fileName;
#if !defined(WINCE)
  struct stat buf;
#endif

  mupnp_log_debug_l4("Entering...\n");

#if !defined(WINCE)
  buf.st_mtime = 0;
#endif

  fileName = mupnp_file_getname(file);
#if defined(BTRON) || defined(ITRON) || defined(TENGINE)
  if (u_stat(fileName, &buf) == -1)
    return 0;
  return buf.st_mtime;
#elif defined(WINCE)
#pragma message("Not Implemented yet")
#pragma message("WINCE - support for file I/O not provided by CSTL - fixmelater")
  return 0;
#else
  if (stat(fileName, &buf) == -1)
    return 0;
  return buf.st_mtime;
#endif

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_getname
****************************************/

long mupnp_file_getlength(mUpnpFile* file)
{
  char* fileName;
#if !defined(WINCE)
  struct stat buf;
#endif

  mupnp_log_debug_l4("Entering...\n");

  fileName = mupnp_file_getname(file);
#if defined(BTRON) || defined(ITRON) || defined(TENGINE)
  if (u_stat(fileName, &buf) == -1)
    return 0;
#elif defined(WINCE)
#pragma message("Not Implemented yet")
#pragma message("WIN32_WCE - support for file I/O not provided by CSTL - fixmelater")
  return 0;
#else
  if (stat(fileName, &buf) == -1)
    return 0;
#endif

  mupnp_log_debug_l4("Leaving...\n");

#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) && !defined(WINCE)
  return buf.st_size;
#endif
}

/****************************************
* mupnp_file_exists
****************************************/

bool mupnp_file_exists(mUpnpFile* file)
{
#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
  FILE* fp;
#else
  int fd;
#endif
  char* fileName;

  mupnp_log_debug_l4("Entering...\n");

  fileName = mupnp_file_getname(file);

#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
  fp = fopen(fileName, "r");
  if (fp == NULL)
    return false;
  fclose(fp);
  return true;
#else
  fd = open(fileName, O_RDONLY);
  if (fd == -1)
    return false;
  close(fd);
  return true;
#endif

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_remove
****************************************/

bool mupnp_file_remove(mUpnpFile* file)
{
  char* fileName;
  int removeSuccess = 0;

  mupnp_log_debug_l4("Entering...\n");

  fileName = mupnp_file_getname(file);
#if defined(WINCE)
#pragma message("############################## FIXMELATER cfile.c - Verify File removal (untested on CE)")
  removeSuccess = DeleteFile((void*)fileName);
#else
  removeSuccess = remove(fileName);
#endif

  return (removeSuccess == 0) ? true : false;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_setcontent
****************************************/

void mupnp_file_setcontent(mUpnpFile* file, char* content)
{
  mupnp_log_debug_l4("Entering...\n");

  if (file->content != NULL) {
    free(file->content);
    file->content = NULL;
  }
  file->content = mupnp_strdup(content);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_getcontent
****************************************/

char* mupnp_file_getcontent(mUpnpFile* file)
{
  mupnp_log_debug_l4("Entering...\n");

  return file->content;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_getfilename
****************************************/

char* mupnp_file_getfilename(mUpnpFile* file)
{
  char* fileName;
  int sepIdx;

  mupnp_log_debug_l4("Entering...\n");

  fileName = mupnp_file_getname(file);
  sepIdx = mupnp_strrchr(fileName, MUPNP_FILE_SEPARATOR, 1);
  if (0 < sepIdx)
    return (fileName + sepIdx + 1);
  return NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_getpath
****************************************/

char* mupnp_file_getpath(mUpnpFile* file)
{
  char* path;
  char* fileName;
  int sepIdx;

  mupnp_log_debug_l4("Entering...\n");

  path = mupnp_string_getvalue(file->path);
  if (path != NULL)
    return path;

  fileName = mupnp_file_getname(file);
  sepIdx = mupnp_strrchr(fileName, MUPNP_FILE_SEPARATOR, 1);

  if (0 < sepIdx) {
    path = (char*)malloc(sizeof(char) * (sepIdx + 1));

    if (NULL == path) {
      mupnp_log_debug_s("Memory allocation failure!\n");
      return NULL;
    }

    mupnp_strncpy(path, fileName, sepIdx);
    path[sepIdx] = '\0';
    mupnp_string_setpointervalue(file->path, path, mupnp_strlen(path));
  }

  return mupnp_string_getvalue(file->path);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_load
****************************************/

bool mupnp_file_load(mUpnpFile* file)
{
#if defined(WIN32) || defined(HAVE_FOPEN)
  FILE* fp;
  size_t nRead;
  size_t readCnt;
#else
  int fd;
  ssize_t nRead;
  ssize_t readCnt;
#endif
  char* fileName;
  long fileLen;

  mupnp_log_debug_l4("Entering...\n");

  if (NULL == file)
    return false;

  if (file->content != NULL) {
    free(file->content);
    file->content = NULL;
  }

  fileLen = mupnp_file_getlength(file);
  if (fileLen <= 0)
    return false;

  fileName = mupnp_file_getname(file);
  file->content = (char*)malloc(fileLen + 1);

  if (NULL == file->content) {
    mupnp_log_debug("Memory allocation failure!\n");
    return false;
  }

#if defined(WIN32) || defined(HAVE_FOPEN)
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    file->content[0] = '\0';
    return false;
  }
  readCnt = 0;
  do {
    int remaining = fileLen - readCnt;
    int chunkSize = remaining < MUPNP_FILE_READ_CHUNK_SIZE ? remaining : MUPNP_FILE_READ_CHUNK_SIZE;
    nRead = fread((file->content) + readCnt, sizeof(char), chunkSize, fp);
    readCnt += nRead;
  } while (0 < nRead);
  fclose(fp);
#else
  fd = open(fileName, O_RDONLY);
  if (fd == -1) {
    file->content[0] = '\0';
    return false;
  }
  readCnt = 0;
  nRead = read(fd, (file->content) + readCnt, MUPNP_FILE_READ_CHUNK_SIZE);
  while (0 < nRead) {
    readCnt += nRead;
    nRead = read(fd, (file->content) + readCnt, MUPNP_FILE_READ_CHUNK_SIZE);
  }
  close(fd);
#endif

  file->content[readCnt] = '\0';

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_save
****************************************/

bool mupnp_file_save(mUpnpFile* file)
{
#if defined(WIN32) || defined(HAVE_FOPEN)
  FILE* fp;
  size_t nWrite;
  size_t writeCnt;
  size_t contentLen;
#else
  int fd;
  ssize_t nWrite;
  ssize_t writeCnt;
  ssize_t contentLen;
#endif
  char* fileName;

  mupnp_log_debug_l4("Entering...\n");

  fileName = mupnp_file_getname(file);
  if (fileName == NULL)
    return false;

  if (file->content == NULL)
    return false;

  contentLen = mupnp_strlen(file->content);
  if (contentLen <= 0)
    return false;

#if defined(WIN32) || defined(HAVE_FOPEN)
  fp = fopen(fileName, "w");
  if (fp == NULL)
    return false;
  writeCnt = 0;
  nWrite = fwrite((file->content) + writeCnt, sizeof(char), contentLen - writeCnt, fp);
  while (0 < nWrite) {
    writeCnt += nWrite;
    if (contentLen <= writeCnt)
      break;
    nWrite = fwrite((file->content) + writeCnt, sizeof(char), contentLen - writeCnt, fp);
  }
  fclose(fp);
#else
  fd = open(fileName, O_WRONLY);
  if (fd == -1)
    return false;
  writeCnt = 0;
  nWrite = write(fd, (file->content) + writeCnt, contentLen - writeCnt);
  while (0 < nWrite) {
    writeCnt += nWrite;
    if (contentLen <= writeCnt)
      break;
    nWrite = write(fd, (file->content) + writeCnt, contentLen - writeCnt);
  }
  close(fd);
#endif

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_file_listfiles
****************************************/

int mupnp_file_listfiles(mUpnpFile* file, mUpnpFileList* fileList)
{
  char* dir;
  char* fileName;
  mUpnpString* fullPathStr;
  mUpnpFile* childFile;
#if defined(WIN32)
  mUpnpString* findDirStr;
#if defined(UNICODE)
  TCHAR wCharBuf[MAX_PATH];
  char mCharBuf[MAX_PATH];
#endif
  WIN32_FIND_DATA fd;
  HANDLE hFind;
#else
  struct dirent** dirFileList;
  int n;
#endif

  dir = mupnp_file_getname(file);
  if (mupnp_strlen(dir) <= 0)
    return 0;

#if defined(WIN32)
  findDirStr = mupnp_string_new();
  mupnp_string_addvalue(findDirStr, dir);
  mupnp_string_addvalue(findDirStr, "\\*.*");
#if defined(UNICODE)
  MultiByteToWideChar(CP_UTF8, 0, mupnp_string_getvalue(findDirStr), -1, wCharBuf, (MAX_PATH - 1));
  hFind = FindFirstFile(wCharBuf, &fd);
#else
  hFind = FindFirstFile(mupnp_string_getvalue(findDirStr), &fd);
#endif
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
#if defined(UNICODE)
      WideCharToMultiByte(CP_ACP, 0, fd.cFileName, -1, mCharBuf, (MAX_PATH - 1), NULL, NULL);
      fileName = mupnp_strdup(mCharBuf);
#else
      fileName = mupnp_strdup(fd.cFileName);
#endif
      if (!mupnp_streq(".", fileName) && !mupnp_streq("..", fileName)) {
        fullPathStr = mupnp_string_new();
        mupnp_string_addvalue(fullPathStr, dir);
        mupnp_string_addvalue(fullPathStr, "\\");
        mupnp_string_addvalue(fullPathStr, fileName);
//				conType = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? MUPNPAV_CONTENT_CONTAINER : MUPNPAV_CONTENT_ITEM;
#else
  n = scandir(dir, &dirFileList, 0, alphasort);
  if (0 <= n) {
    while (n--) {
      if (!mupnp_streq(".", dirFileList[n]->d_name) && !mupnp_streq("..", dirFileList[n]->d_name)) {
        fileName = mupnp_strdup(dirFileList[n]->d_name);
        fullPathStr = mupnp_string_new();
        mupnp_string_addvalue(fullPathStr, dir);
        mupnp_string_addvalue(fullPathStr, "/");
        mupnp_string_addvalue(fullPathStr, dirFileList[n]->d_name);
//				if(stat(mupnp_string_getvalue(fullPathStr), &fileStat) != -1)
//				conType = ((fileStat.st_mode & S_IFMT)==S_IFDIR) ? MUPNPAV_CONTENT_CONTAINER : MUPNPAV_CONTENT_ITEM;
#endif
        /* file */
        childFile = mupnp_file_new();
        if (!childFile)
          continue;

        /* title */
        mupnp_file_setname(childFile, fileName);

        mupnp_filelist_add(fileList, childFile);

        free(fileName);
        mupnp_string_delete(fullPathStr);

#if defined(WIN32)
      }
    } while (FindNextFile(hFind, &fd) != false);
    FindClose(hFind);
  }
  mupnp_string_delete(findDirStr);
#else
      }
      free(dirFileList[n]);
    }
    free(dirFileList);
  }
#endif

  return mupnp_filelist_size(fileList);
}

/****************************************
* mupnp_file_listfiles
****************************************/

bool mupnp_file_open(mUpnpFile* file, int mode)
{
  char* filename;
  char* stdioMode;

  filename = mupnp_file_getname(file);
  if (mupnp_strlen(filename) <= 0)
    return false;

  stdioMode = "";
  if (mode & MUPNP_FILE_OPEN_WRITE) {
    if (mode & MUPNP_FILE_OPEN_CREATE)
      stdioMode = "w+b";
    else
      stdioMode = "r+b";
  }
  else if (mode & MUPNP_FILE_OPEN_READ)
    stdioMode = "rb";

  file->fp = fopen(filename, stdioMode);

  return (file->fp) ? true : false;
}

/****************************************
* mupnp_file_listfiles
****************************************/

bool mupnp_file_close(mUpnpFile* file)
{
  if (!file->fp)
    return false;

  if (fclose(file->fp) == 0) {
    file->fp = NULL;
    return true;
  }

  return false;
}

/****************************************
* mupnp_file_listfiles
****************************************/

bool mupnp_file_write(mUpnpFile* file, mUpnpByte* buf, int bufLen)
{
  if (!file->fp)
    return false;

  return (fwrite(buf, 1, bufLen, file->fp) == bufLen) ? true : false;
}

/****************************************
* mupnp_file_listfiles
****************************************/

bool mupnp_file_read(mUpnpFile* file, mUpnpByte* buf, int bufLen)
{
  if (!file->fp)
    return false;

  return (fread(buf, 1, bufLen, file->fp) == bufLen) ? true : false;
}

/****************************************
* mupnp_file_listfiles
****************************************/

bool mupnp_file_seek(mUpnpFile* file, mUpnpInt64 offset, int whence)
{
  int stdioWhence;

  if (!file->fp)
    return false;

  switch (whence) {
  case MUPNP_FILE_SEEK_SET:
    stdioWhence = SEEK_SET;
    break;
  case MUPNP_FILE_SEEK_CUR:
    stdioWhence = SEEK_CUR;
    break;
  case MUPNP_FILE_SEEK_END:
    stdioWhence = SEEK_END;
    break;
  default:
    stdioWhence = 0;
  }

  return (fseek(file->fp, (long)offset, stdioWhence) == 0) ? true : false;
}

/****************************************
* mupnp_file_setfilename
****************************************/

void mupnp_file_setfilename(mUpnpFile* file, char* name)
{
  int nameLen;
  char* pathName;
  int pathNameLen;

  nameLen = mupnp_strlen(name);
  if (nameLen <= 0)
    return;

  pathName = mupnp_file_getname(file);
  pathNameLen = mupnp_strlen(pathName);
  if (pathNameLen <= 0) {
    mupnp_file_setname(file, name);
    return;
  }

  if (pathName[pathNameLen - 1] != MUPNP_FILE_SEPARATOR_CHAR) {
    if (name[nameLen - 1] != MUPNP_FILE_SEPARATOR_CHAR)
      mupnp_string_addvalue(file->name, MUPNP_FILE_SEPARATOR);
  }

  mupnp_string_addvalue(file->name, name);
}

#endif
