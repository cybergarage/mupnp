/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_filesys.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "cdms_filesys.h"
#include <mupnp/std/av/cmediaserver.h>
#include <mupnp/net/interface.h>

#include <stdio.h>
#include <ctype.h>

#if defined(WIN32)
#include <windows.h>
#include <tchar.h>
#if !defined(WINCE)
#include <sys/stat.h>
#include <conio.h>
#endif
#else
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif

/****************************************
* mupnp_upnpav_dms_filesys_getmimetype
****************************************/

char* mupnp_upnpav_dms_filesys_getmimetype(char* ext)
{
  if (mupnp_strcaseeq(ext, "jpeg") || mupnp_strcaseeq(ext, "jpg"))
    return CG_UPNPAV_MIMETYPE_JPEG;
  if (mupnp_strcaseeq(ext, "mpeg") || mupnp_strcaseeq(ext, "mpg"))
    return CG_UPNPAV_MIMETYPE_MPEG;
  if (mupnp_strcaseeq(ext, "mp3"))
    return CG_UPNPAV_MIMETYPE_MP3;
  return NULL;
}

/****************************************
* mupnp_upnpav_dms_filesys_getupnpclass
****************************************/

char* mupnp_upnpav_dms_filesys_getupnpclass(char* ext)
{
  if (mupnp_strcaseeq(ext, "jpeg") || mupnp_strcaseeq(ext, "jpg"))
    return CG_UPNPAV_UPNPCLASS_PHOTO;
  if (mupnp_strcaseeq(ext, "mpeg") || mupnp_strcaseeq(ext, "mpg"))
    return CG_UPNPAV_UPNPCLASS_MOVIE;
  if (mupnp_strcaseeq(ext, "mp3"))
    return CG_UPNPAV_UPNPCLASS_MUSIC;
  return NULL;
}

/****************************************
* mupnp_upnpav_dms_filesys_updatecontentlist
****************************************/

void mupnp_upnpav_dms_filesys_updatecontentlist(mUpnpMediaServer* dms, mUpnpMediaContentList* parentCon, char* pubdir)
{
  mUpnpMediaContent* con;
  mUpnpMediaResource* res;
  mUpnpNetworkInterfaceList* netIfList;
  mUpnpNetworkInterfaceList* netIf;
  int conType;
  char* fileName;
  mUpnpString* fullPathStr;
  int fileExtIdx;
  char* fileExt;
  char idmd5[CG_MD5_STRING_BUF_SIZE];
  //char resURL[CG_UPNPAV_FILESYS_RESURL_MAXLEN];
  char resURL[1024];
  char* mimeType;
  char* upnpClass;
  mUpnpMediaFileSystemContentData* conData;
  char dlnaAttr[CG_UPNPAV_DLNAATTR_MAXLEN];
#if !defined(WINCE)
  struct stat fileStat;
  off_t fileSize;
#else
  fpos_t fileSize;
  FILE* fp;
#endif
#if defined(WIN32)
  mUpnpString* findDirStr;
#if defined(UNICODE)
  TCHAR wCharBuf[MAX_PATH];
  char mCharBuf[MAX_PATH];
#endif
  WIN32_FIND_DATA fd;
  HANDLE hFind;
#else
  struct dirent** fileList;
  int n;
#endif

  for (con = mupnp_upnpav_content_getchildcontents(parentCon); con; con = mupnp_upnpav_content_next(con)) {
    conData = mupnp_upnpav_dms_filesys_content_getdata(con);
    if (!conData)
      continue;
    mupnp_upnpav_dms_filesys_content_data_delete(conData);
  }
  mupnp_upnpav_content_clearchildcontents(parentCon);

  if (mupnp_strlen(pubdir) <= 0)
    return;

  netIfList = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(netIfList);
  netIf = mupnp_net_interfacelist_gets(netIfList);
  if (!netIf) {
    mupnp_net_interfacelist_delete(netIfList);
    return;
  }

  conType = CG_UPNPAV_CONTENT_NONE;

#if defined(WIN32)
  findDirStr = mupnp_string_new();
  mupnp_string_addvalue(findDirStr, pubdir);
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
        mupnp_string_addvalue(fullPathStr, pubdir);
        mupnp_string_addvalue(fullPathStr, "\\");
        mupnp_string_addvalue(fullPathStr, fileName);
        conType = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? CG_UPNPAV_CONTENT_CONTAINER : CG_UPNPAV_CONTENT_ITEM;
#else
  n = scandir(pubdir, &fileList, 0, alphasort);
  if (0 <= n) {
    while (n--) {
      if (!mupnp_streq(".", fileList[n]->d_name) && !mupnp_streq("..", fileList[n]->d_name)) {
        fileName = mupnp_strdup(fileList[n]->d_name);
        fullPathStr = mupnp_string_new();
        mupnp_string_addvalue(fullPathStr, pubdir);
        mupnp_string_addvalue(fullPathStr, "/");
        mupnp_string_addvalue(fullPathStr, fileList[n]->d_name);
        if (stat(mupnp_string_getvalue(fullPathStr), &fileStat) != -1)
          conType = ((fileStat.st_mode & S_IFMT) == S_IFDIR) ? CG_UPNPAV_CONTENT_CONTAINER : CG_UPNPAV_CONTENT_ITEM;
#endif
        if (conType == CG_UPNPAV_CONTENT_NONE)
          continue;

        mimeType = NULL;
        if ((conType == CG_UPNPAV_CONTENT_ITEM)) {
          fileExtIdx = mupnp_strrchr(fileName, ".", 1);
          if (0 < fileExtIdx) {
            fileExt = (fileName + fileExtIdx + 1);
            fileName[fileExtIdx] = '\0';
            mimeType = mupnp_upnpav_dms_filesys_getmimetype(fileExt);
            upnpClass = mupnp_upnpav_dms_filesys_getupnpclass(fileExt);
          }
          if (!mimeType || !upnpClass) {
            free(fileName);
            mupnp_string_delete(fullPathStr);
            continue;
          }
        }

        con = mupnp_upnpav_content_new();

        /* id */
        mupnp_str2md5((char*)mupnp_string_getvalue(fullPathStr), idmd5);
        mupnp_upnpav_content_setid(con, idmd5);

        /* title */
        mupnp_upnpav_content_settitle(con, fileName);

        /* parent id */
        mupnp_upnpav_content_setparentid(con, mupnp_upnpav_content_getid(parentCon));

        /* type */
        mupnp_upnpav_content_settype(con, conType);

        /* upnp:class */
        mupnp_upnpav_content_setupnpclass(con, upnpClass);

        /* url */
        if ((conType == CG_UPNPAV_CONTENT_ITEM)) {

          res = mupnp_upnpav_resource_new();
          snprintf(resURL, sizeof(resURL), "http://%s:%d/%s/%s", mupnp_net_interface_getaddress(netIf), mupnp_device_gethttpport(mupnp_upnpav_dms_getdevice(dms)), CG_UPNPAV_FILESYS_RESURL_PATH, idmd5);
          mupnp_upnpav_resource_seturl(res, resURL);
          mupnp_upnpav_resource_setmimetype(res, mimeType);
          mupnp_upnpav_resource_setdlnaattribute(res, mupnp_upnpav_resource_getdlnaattributesbymimetype(res, dlnaAttr, sizeof(dlnaAttr)));
          //					mupnp_upnpav_resource_setdlnaattribute(res, mupnp_upnpav_resource_getdlnapnfrommimetype(res, dlnaAttr, sizeof(dlnaAttr)));
          mupnp_upnpav_content_addresource(con, res);

/* size */
#if !defined(WINCE)
          fileSize = 0;
          if (stat(mupnp_string_getvalue(fullPathStr), &fileStat) == 0)
            fileSize = fileStat.st_size;
#else
          fp = fopen(mupnp_string_getvalue(fullPathStr), "rb");
          if (fp) {
            fseek(fp, 0, SEEK_END);
            fgetpos(fp, &fileSize);
            fclose(fp);
          }
#endif
          mupnp_upnpav_resource_setsize(res, (long)fileSize);
        }

        mupnp_upnpav_dms_filesys_content_setpubicdirectory(con, mupnp_string_getvalue(fullPathStr));

        mupnp_upnpav_content_addchildcontent(parentCon, con);
        mupnp_upnpav_dms_condir_updatesystemupdateid(dms);

        free(fileName);
        mupnp_string_delete(fullPathStr);

        conType = CG_UPNPAV_CONTENT_NONE;
#if defined(WIN32)
      }
    } while (FindNextFile(hFind, &fd) != FALSE);
    FindClose(hFind);
  }
  mupnp_string_delete(findDirStr);
#else
      }
      free(fileList[n]);
    }
    free(fileList);
  }
#endif

  mupnp_net_interfacelist_delete(netIfList);
}

/****************************************
* mupnp_upnpav_dms_filesys_updaterootcontentlist
****************************************/

void mupnp_upnpav_dms_filesys_updaterootcontentlist(mUpnpMediaServer* dms)
{
  char* pubdir;
  mUpnpMediaContent* rootContent;

  pubdir = mupnp_upnpav_dms_filesys_getpublicationdirectory(dms);
  if (!pubdir)
    return;

  rootContent = mupnp_upnpav_dms_getrootcontent(dms);

  if (!rootContent)
    return;

  mupnp_upnpav_dms_lock(dms);

  mupnp_upnpav_dms_filesys_updatecontentlist(dms, rootContent, pubdir);

  mupnp_upnpav_dms_unlock(dms);
}

/****************************************
* mupnp_upnpav_dms_filesys_setpublicationdirectory
****************************************/

void mupnp_upnpav_dms_filesys_setpublicationdirectory(mUpnpMediaServer* dms, char* pubdir)
{
  mUpnpMediaContent* rootContent;

  rootContent = mupnp_upnpav_dms_getrootcontent(dms);
  if (!rootContent)
    return;

  mupnp_upnpav_dms_filesys_content_setpubicdirectory(rootContent, pubdir);
}

/****************************************
* mupnp_upnpav_dms_filesys_getpublicationdirectory
****************************************/

char* mupnp_upnpav_dms_filesys_getpublicationdirectory(mUpnpMediaServer* dms)
{
  mUpnpMediaContent* rootContent;

  rootContent = mupnp_upnpav_dms_getrootcontent(dms);
  if (!rootContent)
    return NULL;

  return mupnp_upnpav_dms_filesys_content_getpubicdirectory(rootContent);
}

/****************************************
* mupnp_upnpav_dms_filesys_actionlistner
****************************************/

BOOL mupnp_upnpav_dms_filesys_actionlistner(mUpnpAction* action)
{
  mUpnpMediaServer* dms;
  mUpnpDevice* dev;
  char* contentID;
  mUpnpMediaContent* content;
  char* contentDir;
  char* actionName;

  actionName = mupnp_action_getname(action);
  if (mupnp_strlen(actionName) <= 0)
    return FALSE;

  /* Browse */
  if (mupnp_streq(actionName, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE)) {
    contentID = mupnp_action_getargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID);
    if (mupnp_strlen(contentID) <= 0)
      return FALSE;

    dev = mupnp_service_getdevice(mupnp_action_getservice(action));
    if (!dev)
      return FALSE;

    dms = (mUpnpMediaServer*)mupnp_device_getuserdata(dev);
    if (!dms)
      return FALSE;

    mupnp_upnpav_dms_lock(dms);

    content = mupnp_upnpav_dms_findcontentbyid(dms, contentID);
    if (!content) {
      mupnp_upnpav_dms_unlock(dms);
      return FALSE;
    }

    contentDir = (char*)mupnp_upnpav_dms_filesys_content_getpubicdirectory(content);
    if (mupnp_strlen(contentDir) <= 0) {
      mupnp_upnpav_dms_unlock(dms);
      return FALSE;
    }

    mupnp_upnpav_dms_filesys_updatecontentlist(dms, content, contentDir);

    mupnp_upnpav_dms_unlock(dms);
  }

  return FALSE;
}

/****************************************
* mupnp_upnpav_dms_filesys_new
****************************************/

mUpnpMediaServer* mupnp_upnpav_dms_filesys_new()
{
  mUpnpMediaServer* dms;

  dms = mupnp_upnpav_dms_new();

  mupnp_upnpav_dms_setactionlistener(dms, mupnp_upnpav_dms_filesys_actionlistner);
  mupnp_upnpav_dms_sethttplistener(dms, mupnp_upnpav_dms_filesys_http_listener);

  return dms;
}

/****************************************
* mupnp_upnpav_dms_filesys_delete
****************************************/

void mupnp_upnpav_dms_filesys_delete(mUpnpMediaServer* dms)
{
  mupnp_upnpav_dms_delete(dms);
}
