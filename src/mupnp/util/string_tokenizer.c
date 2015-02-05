/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cstring_tokenizer.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_string_tokenizer_new
****************************************/

CgStringTokenizer *mupnp_string_tokenizer_new(const char *value, const char *delim)
{
	CgStringTokenizer *strToken;

	mupnp_log_debug_l4("Entering...\n");

	strToken = (CgStringTokenizer *)malloc(sizeof(CgStringTokenizer));

	if ( NULL != strToken )
	{
		strToken->value = mupnp_strdup(value);
		strToken->delim = mupnp_strdup(delim);
		strToken->delimCnt = mupnp_strlen(strToken->delim);
		strToken->nextStartPos = 0;
		strToken->lastPos = mupnp_strlen(value) - 1;
		strToken->currToken = NULL;
		strToken->nextToken = NULL;
		mupnp_string_tokenizer_nexttoken(strToken);
	}
  
	mupnp_log_debug_l4("Leaving...\n");
  
	return strToken;
}

/****************************************
* mupnp_string_tokenizer_delete
****************************************/

void mupnp_string_tokenizer_delete(CgStringTokenizer *strToken)
{
	mupnp_log_debug_l4("Entering...\n");

	free(strToken->value);
	free(strToken->delim);
	free(strToken);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_string_tokenizer_hasmoretoken
****************************************/

BOOL mupnp_string_tokenizer_hasmoretoken(CgStringTokenizer *strToken)
{
	mupnp_log_debug_l4("Entering...\n");

	return strToken->hasNextTokens;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_string_tokenizer_nexttoken
****************************************/

char *mupnp_string_tokenizer_nexttoken(CgStringTokenizer *strToken)
{
	size_t tokenCnt;
	size_t i, j;
	
	mupnp_log_debug_l4("Entering...\n");

	strToken->currToken = strToken->nextToken;
	strToken->nextToken = NULL;
	strToken->hasNextTokens = FALSE;
	strToken->repToken = '\0';
	
	tokenCnt = 0;
	for (i=strToken->nextStartPos; i<=strToken->lastPos; i++) {
		BOOL isDelimChar = FALSE;
		for (j=0; j<strToken->delimCnt; j++) {
			if (strToken->value[i] == strToken->delim[j]) {
				isDelimChar = TRUE;
				if (tokenCnt == 0) {
					strToken->nextStartPos = i + 1;
					break;
				}
				strToken->hasNextTokens = TRUE;
				strToken->repToken = strToken->value[i];
				strToken->value[i] = '\0';
				strToken->nextToken = strToken->value + strToken->nextStartPos;
				strToken->nextStartPos = i + 1;
			}
		}
		if (strToken->hasNextTokens == TRUE)
			break;
		if (isDelimChar == FALSE)
			tokenCnt++;
	}
	
	if (strToken->hasNextTokens == FALSE && 0 < tokenCnt) {
		strToken->hasNextTokens = TRUE;
		strToken->nextToken = strToken->value + strToken->nextStartPos;
		strToken->nextStartPos = strToken->lastPos + 1;
	}

	return strToken->currToken;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_string_tokenizer_nextalltoken
****************************************/

char *mupnp_string_tokenizer_nextalltoken(CgStringTokenizer *strToken)
{
	size_t nextTokenLen;

	mupnp_log_debug_l4("Entering...\n");

	nextTokenLen = mupnp_strlen(strToken->nextToken);
	strToken->nextToken[nextTokenLen] = strToken->repToken;
	strToken->currToken = strToken->nextToken;
	strToken->nextToken = NULL;
	strToken->hasNextTokens = FALSE;
	return strToken->currToken;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_string_tokenizer_print
****************************************/

void mupnp_string_tokenizer_print(CgStringTokenizer *strToken)
{
	mupnp_log_debug_l4("Entering...\n");

mupnp_log_debug_s(
		"mupnp_string_tokenizer_print\n"
		"value = %s\n, delim = %s,\n delimCnt = %d,\n nextStartPos = %d,\n lastPos = %d,\n currToken = %s,\n nextToken = %s,\n repToken = %c,\n hasNextTokens = %d\n",
		strToken->value,
		strToken->delim,
		strToken->delimCnt,
		strToken->nextStartPos,
		strToken->lastPos,
		strToken->currToken,
		strToken->nextToken,
		strToken->repToken,
		strToken->hasNextTokens);

	mupnp_log_debug_l4("Leaving...\n");
}
