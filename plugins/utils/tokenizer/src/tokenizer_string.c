/************************************************************************************/
/* Copyright (c) 2009-2011 The Department of Arts and Culture,                      */
/* The Government of the Republic of South Africa.                                  */
/*                                                                                  */
/* Contributors:  Meraka Institute, CSIR, South Africa.                             */
/*                                                                                  */
/* Permission is hereby granted, free of charge, to any person obtaining a copy     */
/* of this software and associated documentation files (the "Software"), to deal    */
/* in the Software without restriction, including without limitation the rights     */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/* copies of the Software, and to permit persons to whom the Software is            */
/* furnished to do so, subject to the following conditions:                         */
/* The above copyright notice and this permission notice shall be included in       */
/* all copies or substantial portions of the Software.                              */
/*                                                                                  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN        */
/* THE SOFTWARE.                                                                    */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* AUTHOR  : Aby Louw                                                               */
/* DATE    : December 2009                                                          */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* A string tokenizer class implementation.                                         */
/* Loosely based on EST_Token of Edinburgh Speech Tools,                            */
/* http://www.cstr.ed.ac.uk/projects/speech_tools (1.2.96)                          */
/* Note that this is a derived work with no verbatim source code from above         */
/* mentioned project.                                                               */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/* Edinburgh Speech Tools license,  EST_Token                                       */
/*                                                                                  */
/*                     Centre for Speech Technology Research                        */
/*                          University of Edinburgh, UK                             */
/*                           Copyright (c) 1996                                     */
/*                             All Rights Reserved.                                 */
/*       Permission is hereby granted, free of charge, to use and distribute        */
/*       this software and its documentation without restriction, including         */
/*       without limitation the rights to use, copy, modify, merge, publish,        */
/*       distribute, sublicense, and/or sell copies of this work, and to            */
/*       permit persons to whom this work is furnished to do so, subject to         */
/*       the following conditions:                                                  */
/*        1. The code must retain the above copyright notice, this list of          */
/*           conditions and the following disclaimer.                               */
/*        2. Any modifications must be clearly marked as such.                      */
/*        3. Original authors' names are not deleted.                               */
/*        4. The authors' names are not used to endorse or promote products         */
/*           derived from this software without specific prior written              */
/*           permission.                                                            */
/*       THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK              */
/*       DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING            */
/*       ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT         */
/*       SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE           */
/*       FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES          */
/*       WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN         */
/*       AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,                */
/*       ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF             */
/*       THIS SOFTWARE.                                                             */
/*                                                                                  */
/*       Author :  Alan W Black                                                     */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "tokenizer_string.h"


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

static STokenizerStringClass TokenizerStringClass; /* STokenizerString class declaration. */

/* A tokenizer to give us access to the STokenizerClass functions */
static STokenizer *tokenizer = NULL;

static uint num_string_tokenizers = 0;


/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/

/* local functions to register and free classes */
S_LOCAL void _s_tokenizer_string_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(&TokenizerStringClass, error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_tokenizer_string_class_reg",
			  "Failed to register STokenizerStringClass");
}


S_LOCAL void _s_tokenizer_string_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(&TokenizerStringClass, error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_tokenizer_string_class_free",
			  "Failed to free STokenizerStringClass");
}

/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/

static void Init(void *obj, s_erc *error)
{
	STokenizerString *self = obj;


	S_CLR_ERR(error);

	if (num_string_tokenizers++ == 0)
	{
		/* create a tokenizer to give us access to the STokenizerClass functions */
		tokenizer = (STokenizer*)S_NEW("STokenizer", error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Init",
					  "Failed to create tokenizer to give STokenizerClass function access"))
			return;
	}

	self->string = NULL;
	self->pos = 0;
}


static void Destroy(void *obj, s_erc *error)
{
	STokenizerString *self = obj;


	S_CLR_ERR(error);

	if (self->string != NULL)
		S_FREE(self->string);

	if (--num_string_tokenizers == 0)
		S_DELETE(tokenizer, "Destroy", error);
}


static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static void GetChar(STokenizer *self, s_erc *error)
{
	uint32 utf8char = 0;
	STokenizerString *ts = S_TOKENIZER_STRING(self);
	size_t strlen;


	S_CLR_ERR(error);
	strlen = s_strlen(ts->string, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "GetChar",
				  "Call to \"s_strlen\" failed"))
		return;

	if (ts->pos < strlen)
	{
		utf8char = s_getat(ts->string, ts->pos++, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "GetChar",
					  "Call to \"s_getat\" failed"))
			return;
	}
	else
	{
		self->eof = TRUE;
	}

	self->current_char = utf8char;
}


static void Seek(STokenizer *self, ulong pos, s_erc *error)
{
	STokenizerString *ts = S_TOKENIZER_STRING(self);
	size_t strlen;


	S_CLR_ERR(error);
	strlen = s_strlen(ts->string, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Seek",
				  "Call to \"s_strlen\" failed"))
		return;

	if (pos < strlen)
	{
		ts->pos = (uint32)pos;
	}
	else
	{
		self->eof = TRUE;
	}
}


static ulong Tell(STokenizer *self, s_erc *error)
{
	STokenizerString *ts = S_TOKENIZER_STRING(self);


	S_CLR_ERR(error);
	return (ulong)ts->pos;
}



static SToken *GetToken(STokenizer *self, s_erc *error)
{
	SToken *token;


	S_CLR_ERR(error);
	token = S_TOKENIZER_CALL(tokenizer, get_token)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "GetToken",
				  "Call to method \"get_token\" failed"))
		return NULL;

	return token;
}


static SToken *PeekToken(STokenizer *self, s_erc *error)
{
	SToken *token;


	S_CLR_ERR(error);
	token = S_TOKENIZER_CALL(tokenizer, peek_token)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "PeekToken",
				  "Call to method \"peek_token\" failed"))
		return NULL;

	return token;
}


static void SetWhitespaceChars(STokenizer *self, const char *white_space_chars,
							   s_erc *error)
{
	S_CLR_ERR(error);
	S_TOKENIZER_CALL(tokenizer, set_whitespace_chars)(self, white_space_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetWhitespaceChars",
			  "Call to method \"set_whitespace_chars\" failed");
}


static void SetSingleChars(STokenizer *self, const char *single_chars,
						   s_erc *error)
{
	S_CLR_ERR(error);
	S_TOKENIZER_CALL(tokenizer, set_single_chars)(self, single_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetSingleChars",
			  "Call to method \"set_single_chars\" failed");
}


static void SetPrePuncChars(STokenizer *self, const char *pre_punc_chars,
							s_erc *error)
{
	S_CLR_ERR(error);
	S_TOKENIZER_CALL(tokenizer, set_prepunc_chars)(self, pre_punc_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetPrePuncChars",
			  "Call to method \"set_prepunc_chars\" failed");
}


static void SetPostPuncChars(STokenizer *self, const char *post_punc_chars,
							 s_erc *error)
{
	S_CLR_ERR(error);
	S_TOKENIZER_CALL(tokenizer, set_postpunc_chars)(self, post_punc_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetPostPuncChars",
			  "Call to method \"set_postpunc_chars\" failed");
}


static void SetQuotes(STokenizer *self, uint32 quote, uint32 escape, s_erc *error)
{
	S_CLR_ERR(error);
	S_TOKENIZER_CALL(tokenizer, set_quotes)(self, quote, escape, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetQuotes",
			  "Call to method \"set_quotes\" failed");
}


static s_bool QueryQuoteMode(STokenizer *self, s_erc *error)
{
	S_CLR_ERR(error);

	return self->quote_mode;
}


static s_bool QueryEOF(STokenizer *self, s_erc *error)
{
	S_CLR_ERR(error);

	return self->eof;
}


void InitStringTokenizer(STokenizerString **self, const char *string, s_erc *error)
{
	S_CLR_ERR(error);

	if (string == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "InitStringTokenizer",
				  "Argument \"string\" is NULL");
		return;
	}

	(*self)->string = s_strdup(string, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "InitStringTokenizer",
				  "Call to \"s_strdup\" failed"))
	{
		S_DELETE(*self, "InitStringTokenizer", error);
		*self = NULL;
		return;
	}

	/* get the first character */
	GetChar(S_TOKENIZER(*self), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "InitStringTokenizer",
				  "Call to method \"get_char\" failed"))
	{
		S_DELETE(*self, "InitStringTokenizer", error);
		*self = NULL;
		return;
	}
}


/************************************************************************************/
/*                                                                                  */
/* STokenizerString class initialization                                            */
/*                                                                                  */
/************************************************************************************/

static STokenizerStringClass TokenizerStringClass =
{
	{
		/* SObjectClass */
		{
			"STokenizer:STokenizerString",
			sizeof(STokenizerString),
			{ 0, 1},
			Init,            /* init    */
			Destroy,         /* destroy */
			Dispose,         /* dispose */
			NULL,            /* compare */
			NULL,            /* print   */
			NULL,            /* copy    */
		},
		/* STokenizerClass */
		GetChar,             /* get_char             */
		Seek,                /* seek                 */
		Tell,                /* tell                 */
		GetToken,            /* get_token            */
		PeekToken,           /* peek_token           */
		SetWhitespaceChars,  /* set_whitespace_chars */
		SetSingleChars,      /* set_single_chars     */
		SetPrePuncChars,     /* set_prepunc_chars    */
		SetPostPuncChars,    /* set_postpunc_chars   */
		SetQuotes,           /* set_quotes           */
		QueryQuoteMode,      /* query_quote_mode     */
		QueryEOF,            /* query_eof            */
	},
	/* STokenizerStringClass */
	InitStringTokenizer        /* init  */
};
