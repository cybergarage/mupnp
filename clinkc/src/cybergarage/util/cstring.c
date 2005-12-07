/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cstring.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/cstring.h>

#include <string.h>

/****************************************
* cg_string_new
****************************************/

CgString *cg_string_new()
{
	CgString *str = (CgString *)malloc(sizeof(CgString));
	str->value = NULL;
	str->freeFlag = TRUE;
	str->memSize = 0;
	str->valueSize = 0;
	return str;
}

/****************************************
* cg_string_delete
****************************************/

void cg_string_delete(CgString *str)
{
	cg_string_clear(str);
	free(str);
}

/****************************************
* cg_string_delete
****************************************/

void cg_string_clear(CgString *str)
{
	if (str->value != NULL) {
		if (str->freeFlag == TRUE)
			free(str->value);
		str->freeFlag = TRUE;
		str->value = NULL;
		str->memSize = 0;
		str->valueSize = 0;
	}
}

/****************************************
* cg_string_setvalue
****************************************/

void cg_string_setvalue(CgString *str, char *value)
{
	cg_string_clear(str);
	if (value != NULL)
		cg_string_setnvalue(str, value, cg_strlen(value));
}

/****************************************
* cg_string_setintvalue
****************************************/

void cg_string_setintvalue(CgString *str, int value)
{
	char buf[CG_STRING_INTEGER_BUFLEN];
	cg_string_setvalue(str, cg_int2str(value, buf, sizeof(buf)));
}

/****************************************
* cg_string_setlongvalue
****************************************/

void cg_string_setlongvalue(CgString *str, long value)
{
	char buf[CG_STRING_LONG_BUFLEN];
	cg_string_setvalue(str, cg_long2str(value, buf, sizeof(buf)));
}

/****************************************
* cg_string_setnvalue
****************************************/

void cg_string_setnvalue(CgString *str, char *value, int len)
{
	cg_string_clear(str);
	if (value != NULL) {
		str->valueSize = len;
		str->memSize = str->valueSize + 1;
		str->value = (char *)malloc(str->memSize * sizeof(char));
		strncpy(str->value, value, len);
		str->value[len] = '\0';
	}
}

/****************************************
* cg_string_setpointervalue
****************************************/

void cg_string_setpointervalue(CgString *str, char *value)
{
	cg_string_clear(str);
	str->value = value;
	str->valueSize = cg_strlen(value);
	str->memSize = str->valueSize + 1;
}

/****************************************
* cg_string_getvalue
****************************************/

char *cg_string_getvalue(CgString *str)
{
	return str->value;
}

/****************************************
* cg_string_getmemorysize
****************************************/

int cg_string_getmemorysize(CgString *str)
{
	return str->memSize;
}

/****************************************
* cg_string_length
****************************************/

int cg_string_length(CgString *str)
{
	if (str->value == NULL)
		return 0;

	return str->valueSize;
}

/****************************************
* cg_string_add
****************************************/

char *cg_string_addvalue(CgString *str, char *value)
{
	return cg_string_naddvalue(str, value, cg_strlen(value));
}

/****************************************
* cg_string_add
****************************************/

char *cg_string_naddvalue(CgString *str, char *value, int valueLen)
{
	if (str == NULL || value == NULL || valueLen <= 0)
	{
		/* Invalid parameters */
		return NULL;
	}

	if ((str->valueSize + valueLen + 1) < str->memSize)
	{
		/* Existing memory is adequate for the new data as well */
		
		/* memcpy works better with non-zero-terminated data
		   than strncpy */
		memcpy(str->value + str->valueSize, value, valueLen);
	}
	else
	{
		/* Need to allocate memory for the new data */
		str->memSize = str->valueSize + valueLen + 1;
		str->value = realloc(str->value, str->memSize * sizeof(char));
		
		/* memcpy works better with non-zero-terminated data
		   than strncpy */
		memcpy(str->value + str->valueSize, value, valueLen);
	}

	str->valueSize += valueLen;
	
	/* In case this is a string, append a termination character */
	str->value[str->valueSize] = '\0';

	return cg_string_getvalue(str);
}

/****************************************
* cg_string_addrep
****************************************/

char *cg_string_addrepvalue(CgString *str, char *value, int repeatCnt)
{
	int n;
	for (n = 0; n < repeatCnt; n++)
		cg_string_addvalue(str, value);
	return cg_string_getvalue(str);
}

/****************************************
* cg_string_naddrep
****************************************/

char *cg_string_naddrepvalue(CgString *str, char *value, int valueLen, int repeatCnt)
{
	int n;
	for (n = 0; n < repeatCnt; n++)
		cg_string_naddvalue(str, value, valueLen);
	return cg_string_getvalue(str);
}

/****************************************
* cg_string_replace
****************************************/

char *cg_string_replace(CgString *str, char *fromStr[], char *toStr[], int fromStrCnt)
{
	char *orgValue;
	int orgValueLen;
	int n;
	int copyPos;
	int *fromStrLen;
	CgString *repValue;
	BOOL isReplaced;

	repValue = cg_string_new();
	
	fromStrLen = (int *)malloc(sizeof(int) * fromStrCnt);
	for (n=0; n<fromStrCnt; n++)
		fromStrLen[n] = cg_strlen(fromStr[n]);
	
	orgValue = cg_string_getvalue(str);
	orgValueLen = cg_string_length(str);
	
	copyPos = 0;
	while (copyPos<orgValueLen) {
		isReplaced = FALSE;
		for (n=0; n<fromStrCnt; n++) {
			if (strncmp(fromStr[n], orgValue + copyPos,  fromStrLen[n]) == 0) {
				cg_string_addvalue(repValue, toStr[n]);
				copyPos += fromStrLen[n];
				isReplaced = TRUE;
				continue;
			}
		}
		if (isReplaced == TRUE)
			continue;
		cg_string_naddvalue(repValue, orgValue + copyPos, 1);
		copyPos++;
	}
	
	free(fromStrLen);

	cg_string_setpointervalue(str, cg_string_getvalue(repValue));	

	/**** free no use cg_string_delete() ****/
	cg_string_setfreeflag(repValue, FALSE);
	cg_string_delete(repValue);
		
	return cg_string_getvalue(str);
}
