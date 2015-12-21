/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_http.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "cdms_filesys.h"

#if !defined(WINCE)
#include <sys/stat.h>
#endif

#define CG_FILE_READ_CHUNK_SIZE (32 * 1024)
#define CG_USE_CHUNKED_STREAM 1

/**********************************************************************
* mupnp_dms_filesys_http_listener
**********************************************************************/

void mupnp_upnpav_dms_filesys_http_listener(mUpnpHttpRequest* httpReq)
{
  mUpnpMediaServer* dms;
  mUpnpDevice* dev;
  char* httpURI;
  int contentMD5Idx;
  char* contentMd5;
  mUpnpMediaContent* content;
  mUpnpMediaResource* resource;
  char* pubDir;
  mUpnpHttpResponse* httpRes;
  mUpnpSocket* sock;
  char chunkedChar[32];
  BOOL isHeadRequest;
#if !defined(WINCE)
  struct stat fileStat;
  off_t fileSize;
#else
  fpos_t fileSize;
#endif
  FILE* fp;
  char readBuf[CG_FILE_READ_CHUNK_SIZE];
  int nRead;
#if defined(WINCE) && defined(UNICODE)
  TCHAR wCharBuf[MAX_PATH];
#endif

  dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);
  if (!dev) {
    mupnp_http_request_postbadrequest(httpReq);
    return;
  }

  dms = (mUpnpMediaServer*)mupnp_device_getuserdata(dev);
  if (!dms) {
    mupnp_http_request_postbadrequest(httpReq);
    return;
  }

  httpURI = mupnp_http_request_geturi(httpReq);
  if (mupnp_strlen(httpURI) <= 0) {
    mupnp_http_request_postbadrequest(httpReq);
    return;
  }

  if (mupnp_strstr(httpURI, CG_UPNPAV_FILESYS_RESURL_PATH) < 0) {
    mupnp_device_httprequestrecieved(httpReq);
    return;
  }

  contentMD5Idx = mupnp_strrchr(httpURI, "/", 1);

  if (contentMD5Idx < 0) {
    mupnp_http_request_postbadrequest(httpReq);
    return;
  }

  contentMd5 = httpURI + contentMD5Idx + 1;

  mupnp_upnpav_dms_lock(dms);

  content = mupnp_upnpav_dms_findcontentbyid(dms, contentMd5);
  if (content == NULL) {
    mupnp_upnpav_dms_unlock(dms);
    mupnp_http_request_postbadrequest(httpReq);
    return;
  }

  pubDir = mupnp_upnpav_dms_filesys_content_getpubicdirectory(content);
  resource = mupnp_upnpav_content_getresources(content);

  if (!pubDir || !resource) {
    mupnp_upnpav_dms_unlock(dms);
    mupnp_http_request_postbadrequest(httpReq);
    return;
  }

  isHeadRequest = mupnp_http_request_isheadrequest(httpReq);

  httpRes = mupnp_http_response_new();
#if defined(CG_USE_CHUNKED_STREAM)
  mupnp_http_response_setversion(httpRes, CG_HTTP_VER11);
#else
  mupnp_http_response_setversion(httpRes, CG_HTTP_VER10);
#endif
  mupnp_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
  mupnp_http_response_setcontenttype(httpRes, mupnp_upnpav_resource_getmimetype(resource));

  sock = mupnp_http_request_getsocket(httpReq);

#if defined(WINCE)
#if defined(UNICODE)
  MultiByteToWideChar(CP_UTF8, 0, pubDir, -1, wCharBuf, (MAX_PATH - 1));
#endif
#endif

  fileSize = 0;

#if !defined(WINCE)
  if (stat(pubDir, &fileStat) == 0)
    fileSize = fileStat.st_size;
#else
#if defined(UNICODE)
  fp = _tfopen(wCharBuf, _T("rb"));
#else
  fp = fopen(pubDir, "rb");
#endif
  if (fp) {
    fseek(fp, 0, SEEK_END);
    fgetpos(fp, &fileSize);
    fclose(fp);
  }
#endif

#if defined(CG_USE_CHUNKED_STREAM)
  mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes, "Transfer-Encoding", "chunked");
#else
  mupnp_http_response_setcontentlength(httpRes, fileSize);
#endif
  mupnp_http_request_postresponse(httpReq, httpRes);

  if (0 < fileSize) {
#if defined(WINCE) && defined(UNICODE)
    fp = _tfopen(wCharBuf, _T("rb"));
#else
    fp = fopen(pubDir, "rb");
#endif
    if (fp) {
      nRead = fread(readBuf, sizeof(char), CG_FILE_READ_CHUNK_SIZE, fp);
      while (0 < nRead) {
#if defined(CG_USE_CHUNKED_STREAM)
        sprintf(chunkedChar, "%x%s", nRead, CG_HTTP_CRLF);
        mupnp_socket_write(sock, chunkedChar, mupnp_strlen(chunkedChar));
#endif
        mupnp_socket_write(sock, readBuf, nRead);
#if defined(CG_USE_CHUNKED_STREAM)
        mupnp_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF) - 1);
#endif
        nRead = fread(readBuf, sizeof(char), CG_FILE_READ_CHUNK_SIZE, fp);
      }
      fclose(fp);
    }
  }

#if defined(CG_USE_CHUNKED_STREAM)
  sprintf(chunkedChar, "%x%s", 0, CG_HTTP_CRLF);
  mupnp_socket_write(sock, chunkedChar, mupnp_strlen(chunkedChar));
#endif

  mupnp_socket_close(sock);
  mupnp_http_response_delete(httpRes);

  mupnp_upnpav_dms_unlock(dms);
}
