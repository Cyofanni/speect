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
/* A tokenizer class implementation.                                                */
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

#include "base/containers/buffer/buffer.h"
#include "base/strings/utf8.h"
#include "utils/tokenizer.h"


/************************************************************************************/
/*                                                                                  */
/* Macros                                                                           */
/*                                                                                  */
/************************************************************************************/

/**
 * @hideinitializer
 * Call the given function method of the given #STokenizer,
 * see full description #S_TOKENIZER_CALL for usage.
 *
 * @param SELF The given #STokenizer*.
 * @param FUNC The function method of the given object to call.
 *
 * @note This casting is not safety checked.
 * @note Example usage: @code S_TOKENIZER_CALL(self, func)(param1, param2, ..., paramN); @endcode
 * where @c param1, @c param2, ..., @c paramN are the parameters passed to the object function
 * @c func.
 */
#define S_TOKENIZER_CALL(SELF, FUNC)				\
	((STokenizerClass *)S_OBJECT_CLS(SELF))->FUNC


/**
 * @hideinitializer
 * Test if the given function method of the given #STokenizer
 * can be called.
 *
 * @param SELF The given #STokenizer*.
 * @param FUNC The function method of the given object to check.
 *
 * @return #TRUE if function can be called, otherwise #FALSE.
 *
 * @note This casting is not safety checked.
 */
#define S_TOKENIZER_METH_VALID(SELF, FUNC)		\
	S_TOKENIZER_CALL(SELF, FUNC) ? TRUE : FALSE


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

static STokenizerClass TokenizerClass; /* STokenizer class declaration. */

/* default character symbols */
static const char * const s_default_whitespacesymbols      = " \t\n\r";
static const char * const s_default_singlecharsymbols      = "(){}[]";
static const char * const s_default_prepunctuationsymbols  = "\"'`({[";
static const char * const s_default_postpunctuationsymbols = "\"'`.,:;!?(){}[]";


/************************************************************************************/
/*                                                                                  */
/* Static function prototypes                                                       */
/*                                                                                  */
/************************************************************************************/

static void s_add_to_buffer(s_buffer *buffer, uint32 c, s_erc *error);

static SToken *s_get_token(STokenizer *ts, s_erc *error);

static s_bool s_char_is_whitespace(STokenizer *ts, uint32 c, s_erc *error);

static s_bool s_char_is_singlechar(STokenizer *ts, uint32 c, s_erc *error);

static s_bool s_char_is_prepunc(STokenizer *ts, uint32 c, s_erc *error);

static s_bool s_char_is_postpunc(STokenizer *ts, uint32 c, s_erc *error);

static void s_get_whitespace(STokenizer *ts, s_erc *error);

static void s_get_pre_punc(STokenizer *ts, s_erc *error);

static void s_get_post_punc(STokenizer *ts, s_erc *error);

static void s_get_string(STokenizer *ts, s_erc *error);


/************************************************************************************/
/*                                                                                  */
/* Function implementations                                                         */
/*                                                                                  */
/************************************************************************************/

S_API uint32 STokenizerGetChar(STokenizer *self, s_erc *error)
{
	uint32 uc;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerGetChar",
				  "Argument \"self\" is NULL");
		return 0;
	}

	if (!S_TOKENIZER_METH_VALID(self, get_char))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerGetChar",
				  "Token method \"get_char\" not implemented");
		return 0;
	}

	uc = S_TOKENIZER_CALL(self, get_char)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "STokenizerGetChar",
				  "Call to class method \"get_char\" failed"))
		return 0;

	return uc;
}


S_API void STokenizerSeek(STokenizer *self, ulong pos, s_erc *error)
{
	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSeek",
				  "Argument \"self\" is NULL");
		return;
	}

	if (!S_TOKENIZER_METH_VALID(self, seek))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerSeek",
				  "Token method \"seek\" not implemented");
		return;
	}

	S_TOKENIZER_CALL(self, seek)(self, pos, error);
	S_CHK_ERR(error, S_CONTERR,
			  "STokenizerSeek",
			  "Call to class method \"seek\" failed");
}


S_API ulong STokenizerTell(const STokenizer *self, s_erc *error)
{
	ulong pos;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerTell",
				  "Argument \"self\" is NULL");
		return 0;
	}

	if (!S_TOKENIZER_METH_VALID(self, tell))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerTell",
				  "Token method \"tell\" not implemented");
		return 0;
	}

	pos = S_TOKENIZER_CALL(self, tell)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "STokenizerTell",
				  "Call to class method \"tell\" failed"))
		return 0;

	return pos;
}


S_API const SToken *STokenizerGetToken(STokenizer *self, s_erc *error)
{
	const SToken *token;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerGetToken",
				  "Argument \"self\" is NULL");
		return NULL;
	}

	if (!S_TOKENIZER_METH_VALID(self, get_token))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerGetToken",
				  "Token method \"get_token\" not implemented");
		return NULL;
	}

	token = S_TOKENIZER_CALL(self, get_token)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "STokenizerGetToken",
				  "Call to class method \"get_token\" failed"))
		return NULL;

	return token;
}


S_API const SToken *STokenizerPeekToken(STokenizer *self, s_erc *error)
{
	const SToken *token;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerPeekToken",
				  "Argument \"self\" is NULL");
		return NULL;
	}

	if (!S_TOKENIZER_METH_VALID(self, peek_token))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerPeekToken",
				  "Token method \"peek_token\" not implemented");
		return NULL;
	}

	token = S_TOKENIZER_CALL(self, peek_token)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "STokenizerPeekToken",
				  "Call to class method \"peek_token\" failed"))
		return NULL;

	return token;
}


S_API void STokenizerSetWhitespaceChars(STokenizer *self, const char *white_space_chars,
										s_erc *error)
{
	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetWhitespaceChars",
				  "Argument \"self\" is NULL");
		return;
	}

	if (white_space_chars == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetWhitespaceChars",
				  "Argument \"white_space_chars\" is NULL");
		return;
	}

	if (!S_TOKENIZER_METH_VALID(self, set_whitespace_chars))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerSetWhitespaceChars",
				  "Token method \"set_whitespace_chars\" not implemented");
		return;
	}

	S_TOKENIZER_CALL(self, set_whitespace_chars)(self, white_space_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "STokenizerSetWhitespaceChars",
			  "Call to class method \"set_whitespace_chars\" failed");
}


S_API void STokenizerSetSingleChars(STokenizer *self, const char *single_chars, s_erc *error)
{
	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetSingleChars",
				  "Argument \"self\" is NULL");
		return;
	}

	if (single_chars == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetSingleChars",
				  "Argument \"single_chars\" is NULL");
		return;
	}

	if (!S_TOKENIZER_METH_VALID(self, set_single_chars))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerSetSingleChars",
				  "Token method \"set_single_chars\" not implemented");
		return;
	}

	S_TOKENIZER_CALL(self, set_single_chars)(self, single_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "STokenizerSetSingleChars",
			  "Call to class method \"set_single_chars\" failed");
}


S_API void STokenizerSetPrePuncChars(STokenizer *self, const char *pre_punc_chars, s_erc *error)
{
	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetPrePuncChars",
				  "Argument \"self\" is NULL");
		return;
	}

	if (pre_punc_chars == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetPrePuncChars",
				  "Argument \"pre_punc_chars\" is NULL");
		return;
	}

	if (!S_TOKENIZER_METH_VALID(self, set_prepunc_chars))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerSetPrePuncChars",
				  "Token method \"set_prepunc_chars\" not implemented");
		return;
	}

	S_TOKENIZER_CALL(self, set_prepunc_chars)(self, pre_punc_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "STokenizerSetPrePuncChars",
			  "Call to class method \"set_prepunc_chars\" failed");
}


S_API void STokenizerSetPostPuncChars(STokenizer *self, const char *post_punc_chars, s_erc *error)
{
	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetPostPuncChars",
				  "Argument \"self\" is NULL");
		return;
	}

	if (post_punc_chars == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetPostPuncChars",
				  "Argument \"post_punc_chars\" is NULL");
		return;
	}

	if (!S_TOKENIZER_METH_VALID(self, set_postpunc_chars))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerSetPostPuncChars",
				  "Token method \"set_postpunc_chars\" not implemented");
		return;
	}

	S_TOKENIZER_CALL(self, set_postpunc_chars)(self, post_punc_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "STokenizerSetPostPuncChars",
			  "Call to class method \"set_postpunc_chars\" failed");
}


S_API void STokenizerSetQuotes(STokenizer *self, uint32 quote, uint32 escape, s_erc *error)
{
	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerSetQuotes",
				  "Argument \"self\" is NULL");
		return;
	}

	if (!S_TOKENIZER_METH_VALID(self, set_quotes))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerSetQuotes",
				  "Token method \"set_quotes\" not implemented");
		return;
	}

	S_TOKENIZER_CALL(self, set_quotes)(self, quote, escape, error);
	S_CHK_ERR(error, S_CONTERR,
			  "STokenizerSetQuotes",
			  "Call to class method \"set_quotes\" failed");
}


S_API s_bool STokenizerQueryQuoteMode(const STokenizer *self, s_erc *error)
{
	s_bool query;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerQueryQuoteMode",
				  "Argument \"self\" is NULL");
		return FALSE;
	}

	if (!S_TOKENIZER_METH_VALID(self, query_quote_mode))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerQueryQuoteMode",
				  "Token method \"query_quote_mode\" not implemented");
		return FALSE;
	}

	query = S_TOKENIZER_CALL(self, query_quote_mode)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "STokenizerQueryQuoteMode",
				  "Call to class method \"query_quote_mode\" failed"))
		return FALSE;

	return query;
}


S_API s_bool STokenizerQueryEOF(const STokenizer *self, s_erc *error)
{
	s_bool query;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "STokenizerQueryEOF",
				  "Argument \"self\" is NULL");
		return FALSE;
	}

	if (!S_TOKENIZER_METH_VALID(self, query_eof))
	{
		S_WARNING(S_METHINVLD,
				  "STokenizerQueryEOF",
				  "Token method \"query_eof\" not implemented");
		return FALSE;
	}

	query = S_TOKENIZER_CALL(self, query_eof)(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "STokenizerQueryEOF",
				  "Call to class method \"query_eof\" failed"))
		return FALSE;

	return query;
}


/************************************************************************************/
/*                                                                                  */
/* Static function implementations                                                  */
/*                                                                                  */
/************************************************************************************/

static void s_add_to_buffer(s_buffer *buffer, uint32 c, s_erc *error)
{
	char tmp[4] = "\0";
	uchar *byte;


	S_CLR_ERR(error);
	s_setc(tmp, c, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_add_to_buffer",
				  "Call to \"s_setc\" failed"))
		return;

	byte = (uchar*)tmp;

	while (*byte != 0)
	{
		s_buffer_append(buffer, (const void*)byte, 1, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_add_to_buffer",
					  "Call to \"s_buffer_append\" failed"))
			return;
		byte++;
	}
}


static s_bool s_char_is_whitespace(STokenizer *ts, uint32 c, s_erc *error)
{
	const char *tmp;


	S_CLR_ERR(error);
	tmp = s_strchr(ts->white_space_chars, c, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_char_is_whitespace",
				  "Call to \"s_strchr\" failed"))
		return FALSE;

	if (tmp != NULL)
		return TRUE;

	return FALSE;
}


static s_bool s_char_is_singlechar(STokenizer *ts, uint32 c, s_erc *error)
{
	const char *tmp;


	S_CLR_ERR(error);
	tmp = s_strchr(ts->single_char_symbols, c, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_char_is_singlechar",
				  "Call to \"s_strchr\" failed"))
		return FALSE;

	if (tmp != NULL)
		return TRUE;

	return FALSE;
}


static s_bool s_char_is_prepunc(STokenizer *ts, uint32 c, s_erc *error)
{
	const char *tmp;


	S_CLR_ERR(error);
	tmp = s_strchr(ts->pre_punc_symbols, c, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_char_is_prepunc",
				  "Call to \"s_strchr\" failed"))
		return FALSE;

	if (tmp != NULL)
		return TRUE;

	return FALSE;
}


static s_bool s_char_is_postpunc(STokenizer *ts, uint32 c, s_erc *error)
{
	const char *tmp;


	S_CLR_ERR(error);
	tmp = s_strchr(ts->post_punc_symbols, c, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_char_is_postpunc",
				  "Call to \"s_strchr\" failed"))
		return FALSE;

	if (tmp != NULL)
		return TRUE;

	return FALSE;
}


static void s_get_whitespace(STokenizer *ts, s_erc *error)
{
	s_buffer *buffer;
	s_bool is_type;
	size_t buff_size;


	S_CLR_ERR(error);
	buffer = s_buffer_new(error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_whitespace",
				  "Failed to create new buffer"))
		return;

	is_type = s_char_is_whitespace(ts, ts->current_char, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_whitespace",
				  "Call to \"s_char_is_whitespace\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_whitespace",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	while ((ts->eof == FALSE)
	       && (is_type == TRUE))
	{
		s_add_to_buffer(buffer, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_add_to_buffer\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		STokenizerGetChar(ts, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"STokenizerGetChar\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		is_type = s_char_is_whitespace(ts, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_char_is_whitespace\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	buff_size = s_buffer_size(buffer, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_whitespace",
				  "Call to \"s_buffer_size\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_whitespace",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	if (buff_size != 0)
	{
		const char *whitespace;


		whitespace = (const char*)s_buffer_data(buffer, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_buffer_data\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		STokenSetWhitespace(ts->currentToken, whitespace, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"STokenSetWhitespace\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_whitespace",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	s_buffer_delete(buffer, error);
	S_CHK_ERR(error, S_CONTERR,
			  "s_get_whitespace",
			  "Call to \"s_buffer_delete\" failed");
}


static void s_get_pre_punc(STokenizer *ts, s_erc *error)
{
	s_buffer *buffer;
	s_bool is_type;
	size_t buff_size;


	S_CLR_ERR(error);
	buffer = s_buffer_new(error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_pre_punc",
				  "Failed to create new buffer"))
		return;

	is_type = s_char_is_prepunc(ts, ts->current_char, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_pre_punc",
				  "Call to \"s_char_is_pre_punc\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_pre_punc",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	while ((ts->eof == FALSE)
	       && (is_type == TRUE))
	{
		s_add_to_buffer(buffer, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_add_to_buffer\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		STokenizerGetChar(ts, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"STokenizerGetChar\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		is_type = s_char_is_prepunc(ts, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_char_is_whitespace\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	buff_size = s_buffer_size(buffer, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_pre_punc",
				  "Call to \"s_buffer_size\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_pre_punc",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	if (buff_size != 0)
	{
		const char *prepunc;


		prepunc = (const char*)s_buffer_data(buffer, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_buffer_data\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		STokenSetPrePunc(ts->currentToken, prepunc, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"STokenSetPrePunc\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_pre_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	s_buffer_delete(buffer, error);
	S_CHK_ERR(error, S_CONTERR,
			  "s_get_pre_punc",
			  "Call to \"s_buffer_delete\" failed");
}


static void s_get_post_punc(STokenizer *ts, s_erc *error)
{
	s_buffer *buffer;
	s_bool is_type;
	size_t buff_size;


	S_CLR_ERR(error);
	buffer = s_buffer_new(error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_post_punc",
				  "Failed to create new buffer"))
		return;

	is_type = s_char_is_postpunc(ts, ts->current_char, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_post_punc",
				  "Call to \"s_char_is_post_punc\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_post_punc",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	while ((ts->eof == FALSE)
	       && (is_type == TRUE))
	{
		s_add_to_buffer(buffer, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_add_to_buffer\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		STokenizerGetChar(ts, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"STokenizerGetChar\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		is_type = s_char_is_postpunc(ts, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_char_is_whitespace\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	buff_size = s_buffer_size(buffer, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_post_punc",
				  "Call to \"s_buffer_size\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_post_punc",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	if (buff_size != 0)
	{
		const char *postpunc;


		postpunc = (const char*)s_buffer_data(buffer, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_buffer_data\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		STokenSetPostPunc(ts->currentToken, postpunc, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"STokenSetPostPunc\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_post_punc",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	s_buffer_delete(buffer, error);
	S_CHK_ERR(error, S_CONTERR,
			  "s_get_post_punc",
			  "Call to \"s_buffer_delete\" failed");
}


static void s_get_string(STokenizer *ts, s_erc *error)
{
	s_buffer *buffer;
	s_bool is_whitespace;
	s_bool is_prepunc;
	s_bool is_postpunc;
	s_bool is_singlechar;
	size_t buff_size = 0;


	S_CLR_ERR(error);
	buffer = s_buffer_new(error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Failed to create new buffer"))
		return;

	is_whitespace = s_char_is_whitespace(ts, ts->current_char, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_char_is_whitespace\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	is_prepunc = s_char_is_prepunc(ts, ts->current_char, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_char_is_prepunc\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	is_postpunc = s_char_is_postpunc(ts, ts->current_char, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_char_is_postpunc\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	while ((ts->eof == FALSE)
	       && (is_whitespace == FALSE)
		   && (is_prepunc == FALSE)
		   && (is_postpunc == FALSE))
	{
		is_singlechar = s_char_is_singlechar(ts, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_char_is_singlechar\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		buff_size = s_buffer_size(buffer, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_size\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		if ((is_singlechar == TRUE) && (buff_size == 0))
		{
			s_add_to_buffer(buffer, ts->current_char, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "s_get_string",
						  "Call to \"s_add_to_buffer\" failed"))
			{
				s_buffer_delete(buffer, error);
				S_CHK_ERR(error, S_CONTERR,
						  "s_get_string",
						  "Call to \"s_buffer_delete\" failed");
				return;
			}

			STokenizerGetChar(ts, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "s_get_string",
						  "Call to \"STokenizerGetChar\" failed"))
			{
				s_buffer_delete(buffer, error);
				S_CHK_ERR(error, S_CONTERR,
						  "s_get_string",
						  "Call to \"s_buffer_delete\" failed");
				return;
			}

			break;
		}

		s_add_to_buffer(buffer, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_add_to_buffer\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		S_TOKENIZER_CALL(ts, get_char)(ts, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to method \"get_char\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		is_whitespace = s_char_is_whitespace(ts, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_char_is_whitespace\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		is_prepunc = s_char_is_prepunc(ts, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_char_is_prepunc\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		is_postpunc = s_char_is_postpunc(ts, ts->current_char, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_char_is_postpunc\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	buff_size = s_buffer_size(buffer, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_buffer_size\" failed"))
	{
		s_buffer_delete(buffer, error);
		S_CHK_ERR(error, S_CONTERR,
				  "s_get_string",
				  "Call to \"s_buffer_delete\" failed");
		return;
	}

	if (buff_size != 0)
	{
		const char *string;

		string = (const char*)s_buffer_data(buffer, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_data\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}

		STokenSetString(ts->currentToken, string, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"STokenSetString\" failed"))
		{
			s_buffer_delete(buffer, error);
			S_CHK_ERR(error, S_CONTERR,
					  "s_get_string",
					  "Call to \"s_buffer_delete\" failed");
			return;
		}
	}

	s_buffer_delete(buffer, error);
	S_CHK_ERR(error, S_CONTERR,
			  "s_get_string",
			  "Call to \"s_buffer_delete\" failed");
}


static SToken *s_get_token(STokenizer *ts, s_erc *error)
{
	/* make a new token */
	if (ts->currentToken != NULL)
		S_DELETE(ts->currentToken, "s_get_token", error);

	ts->currentToken = S_NEW(SToken, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_token",
				  "Failed to create new token"))
		return NULL;

	/* get whitespace */
	s_get_whitespace(ts, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_token",
				  "Call to \"s_get_whitespace\" failed"))
	{
		S_DELETE(ts->currentToken, "s_get_token", error);
		return NULL;
	}

	/* get pre punctuation */
	s_get_pre_punc(ts, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_token",
				  "Call to \"s_get_pre_punc\" failed"))
	{
		S_DELETE(ts->currentToken, "s_get_token", error);
		return NULL;
	}

	/* get token string */
	s_get_string(ts, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_token",
				  "Call to \"s_get_string\" failed"))
	{
		S_DELETE(ts->currentToken, "s_get_token", error);
		return NULL;
	}

	/* get post punctuation */
	s_get_post_punc(ts, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "s_get_token",
				  "Call to \"s_get_post_punc\" failed"))
	{
		S_DELETE(ts->currentToken, "s_get_token", error);
		return NULL;
	}

	return ts->currentToken;
}


/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/

static void Init(void *obj, s_erc *error)
{
	STokenizer *self = obj;


	S_CLR_ERR(error);

	self->white_space_chars = s_strdup(s_default_whitespacesymbols, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Init",
				  "Call to \"s_strdup\" failed"))
		return;

	self->single_char_symbols = s_strdup(s_default_singlecharsymbols, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Init",
				  "Call to \"s_strdup\" failed"))
	{
		S_FREE(self->white_space_chars);
		return;
	}

	self->pre_punc_symbols = s_strdup(s_default_prepunctuationsymbols, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Init",
				  "Call to \"s_strdup\" failed"))
	{
		S_FREE(self->white_space_chars);
		S_FREE(self->single_char_symbols);
		return;
	}

	self->post_punc_symbols = s_strdup(s_default_postpunctuationsymbols, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Init",
				  "Call to \"s_strdup\" failed"))
	{
		S_FREE(self->white_space_chars);
		S_FREE(self->single_char_symbols);
		S_FREE(self->pre_punc_symbols);
		return;
	}

  	self->peeked = FALSE;
	self->currentToken = NULL;
	self->eof = FALSE;
	self->current_char = 0;
	self->quote = 0;
	self->escape = 0;
	self->quote_mode = FALSE;
}


static void Destroy(void *obj, s_erc *error)
{
	STokenizer *self = obj;


	S_CLR_ERR(error);
	if (self->white_space_chars != NULL)
		S_FREE(self->white_space_chars);

	if (self->single_char_symbols != NULL)
		S_FREE(self->single_char_symbols);

	if (self->pre_punc_symbols != NULL)
		S_FREE(self->pre_punc_symbols);

	if (self->post_punc_symbols != NULL)
		S_FREE(self->post_punc_symbols);

	if (self->currentToken != NULL)
		S_DELETE(self->currentToken, "Destroy", error);
}


static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static const SToken *GetToken(STokenizer *self, s_erc *error)
{
	const SToken *token;


	S_CLR_ERR(error);
	if (self->peeked == TRUE)
	{
		self->peeked = FALSE;
		return self->currentToken;
	}

	token = s_get_token(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "GetToken",
				  "Call to \"s_get_token\" failed"))
		return NULL;

	return token;
}


static const SToken *PeekToken(STokenizer *self, s_erc *error)
{
	const SToken *token;


	S_CLR_ERR(error);

	self->peeked = TRUE;
	token = s_get_token(self, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "PeekToken",
				  "Call to \"s_get_token\" failed"))
		return NULL;

	return token;
}


static void SetWhitespaceChars(STokenizer *self, const char *white_space_chars,
							   s_erc *error)
{
	S_CLR_ERR(error);
	if (self->white_space_chars != NULL)
		S_FREE(self->white_space_chars);

	self->white_space_chars = s_strdup(white_space_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetWhitespaceChars",
			  "Call to \"s_strdup\" failed");
}


static void SetSingleChars(STokenizer *self, const char *single_chars,
						   s_erc *error)
{
	S_CLR_ERR(error);
	if (self->single_char_symbols != NULL)
		S_FREE(self->single_char_symbols);

	self->single_char_symbols = s_strdup(single_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetSingleChars",
			  "Call to \"s_strdup\" failed");
}


static void SetPrePuncChars(STokenizer *self, const char *pre_punc_chars,
							s_erc *error)
{
	S_CLR_ERR(error);
	if (self->pre_punc_symbols != NULL)
		S_FREE(self->pre_punc_symbols);

	self->pre_punc_symbols = s_strdup(pre_punc_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetPrePuncChars",
			  "Call to \"s_strdup\" failed");
}


static void SetPostPuncChars(STokenizer *self, const char *post_punc_chars,
							 s_erc *error)
{
	S_CLR_ERR(error);
	if (self->post_punc_symbols != NULL)
		S_FREE(self->post_punc_symbols);

	self->post_punc_symbols = s_strdup(post_punc_chars, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SetPostPuncChars",
			  "Call to \"s_strdup\" failed");
}


static void SetQuotes(STokenizer *self, uint32 quote, uint32 escape, s_erc *error)
{
	S_CLR_ERR(error);

	self->quote = quote;
	self->escape = escape;
	self->quote_mode = TRUE;
}


static s_bool QueryQuoteMode(const STokenizer *self, s_erc *error)
{
	S_CLR_ERR(error);

	return self->quote_mode;
}


static s_bool QueryEOF(const STokenizer *self, s_erc *error)
{
	S_CLR_ERR(error);

	return self->eof;
}


/************************************************************************************/
/*                                                                                  */
/* Class registration                                                               */
/*                                                                                  */
/************************************************************************************/

S_LOCAL void _s_tokenizer_class_add(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_add(S_OBJECTCLASS(&TokenizerClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_tokenizer_class_add",
			  "Failed to add STokenizerClass");
}


/************************************************************************************/
/*                                                                                  */
/* STokenizer class initialization                                                  */
/*                                                                                  */
/************************************************************************************/

static STokenizerClass TokenizerClass =
{
	/* SObjectClass */
	{
		"STokenizer",
		sizeof(STokenizer),
		{ 0, 1},
		Init,            /* init    */
		Destroy,         /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* STokenizerClass */
	NULL,                /* get_char             */
	NULL,                /* seek                 */
	NULL,                /* tell                 */
	GetToken,            /* get_token            */
	PeekToken,           /* peek_token           */
	SetWhitespaceChars,  /* set_whitespace_chars */
	SetSingleChars,      /* set_single_chars     */
	SetPrePuncChars,     /* set_prepunc_chars    */
	SetPostPuncChars,    /* set_postpunc_chars   */
	SetQuotes,           /* set_quotes           */
	QueryQuoteMode,      /* query_quote_mode     */
	QueryEOF             /* query_eof            */
};
