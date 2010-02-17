/************************************************************************************/
/* Copyright (c) 2010 The Department of Arts and Culture,                           */
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
/* DATE    : February 2010                                                          */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* An utterance processor to do HTS Engine synthesis of a segment relation stream.  */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "synthesize_hts_engine.h"
#include "audio.h"


/************************************************************************************/
/*                                                                                  */
/* Defines                                                                          */
/*                                                                                  */
/************************************************************************************/

/* default values for HTS Engine params */
#define SPCT_DEF_SAMPLING_RATE 16000
#define SPCT_DEF_FPERIOD 80
#define SPCT_DEF_ALPHA 0.42
#define SPCT_DEF_STAGE 0.0
#define SPCT_DEF_BETA 0.0
#define SPCT_DEF_AUDIO_BUFF_SIZE 1600
#define SPCT_DEF_UV_THRESHOLD 0.5
#define SPCT_DEF_GV_WEIGHT_MCP 0.7
#define SPCT_DEF_GV_WEIGHT_LF0 1.0


/************************************************************************************/
/*                                                                                  */
/* Typedefs                                                                         */
/*                                                                                  */
/************************************************************************************/

typedef struct
{
	int sampling_rate;
	int fperiod;
	double alpha;
	int stage;
	double beta;
	int audio_buff_size;
	double uv_threshold;
	HTS_Boolean use_log_gain;
	double gv_weight_mcp;
	double gv_weight_lf0;
} hts_params;


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

/* SHTSEngineSynthUttProc class declaration. */
static SHTSEngineSynthUttProcClass HTSEngineSynthUttProcClass;


/************************************************************************************/
/*                                                                                  */
/* Static function prototypes                                                       */
/*                                                                                  */
/************************************************************************************/

static hts_params *get_hts_engine_params(const SMap *features, s_erc *error);

static void get_windows(const SList *windows, char ***cwindows, int *num, s_erc *error);

static void get_trees_pdfs(const SList *trees, const SList *pdfs,
						   char ***ctrees, char ***cpdfs, int *num,
						   s_erc *error);

static void load_hts_engine_data(const SMap *data, HTS_Engine *engine, s_erc *error);

/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/

/* local functions to register and free classes */
S_LOCAL void _s_hts_engine_synth_utt_proc_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(&HTSEngineSynthUttProcClass, error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_hts_engine_synth_utt_proc_class_reg",
			  "Failed to register SHTSEngineSynthUttProcClass");
}


S_LOCAL void _s_hts_engine_synth_utt_proc_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(&HTSEngineSynthUttProcClass, error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_hts_engine_synth_utt_proc_class_free",
			  "Failed to free SHTSEngineSynthUttProcClass");
}


/************************************************************************************/
/*                                                                                  */
/* Static function implementations                                                  */
/*                                                                                  */
/************************************************************************************/

static hts_params *get_hts_engine_params(const SMap *features, s_erc *error)
{
	hts_params *engine_params;
	const char *tmp;


	S_CLR_ERR(error);

	engine_params = S_CALLOC(hts_params, 1);
	if (engine_params == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "get_hts_engine_params",
				  "Failed to allocate memory for 'hts_params' object");
		return NULL;
	}

	engine_params->sampling_rate = (int)SMapGetIntDef(features, "sampling_rate",
													  SPCT_DEF_SAMPLING_RATE, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetIntDef\" failed"))
		goto quit_error;

	engine_params->fperiod = (int)SMapGetIntDef(features, "fperiod",
												SPCT_DEF_FPERIOD, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetIntDef\" failed"))
		goto quit_error;

	engine_params->alpha = (double)SMapGetFloatDef(features, "alpha",
												   SPCT_DEF_ALPHA, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->stage = (double)SMapGetFloatDef(features, "stage",
												   SPCT_DEF_STAGE, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->beta = (double)SMapGetFloatDef(features, "beta",
												   SPCT_DEF_BETA, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->audio_buff_size = (int)SMapGetIntDef(features, "audio_buff_size",
														SPCT_DEF_AUDIO_BUFF_SIZE, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetIntDef\" failed"))
		goto quit_error;

	engine_params->uv_threshold = (double)SMapGetFloatDef(features, "uv_threshold",
														  SPCT_DEF_UV_THRESHOLD, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->gv_weight_lf0 = (double)SMapGetFloatDef(features, "gv_weight_lf0",
														   SPCT_DEF_GV_WEIGHT_LF0, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	engine_params->gv_weight_mcp = (double)SMapGetFloatDef(features, "gv_weight_mcp",
														   SPCT_DEF_GV_WEIGHT_MCP, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetFloatDef\" failed"))
		goto quit_error;

	/* set "use_log_gain" to FALSE as default */
	engine_params->use_log_gain = FALSE;

	tmp = SMapGetStringDef(features, "use_log_gain", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_hts_engine_params",
				  "Call to \"SMapGetStringDef\" failed"))
		goto quit_error;

	if (tmp != NULL)
	{
		int scomp;


		scomp = s_strcmp("TRUE", tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "set_viterbi_params",
					  "Call to \"s_strcmp\" failed"))
			goto quit_error;

		if (scomp == 0)
			engine_params->use_log_gain = TRUE;
	}

	/* all OK, no errors */
	return engine_params;

	/* error clean up */
quit_error:
	S_FREE(engine_params);
	return NULL;
}


static void get_trees_pdfs(const SList *trees, const SList *pdfs,
						   char ***ctrees, char ***cpdfs, int *num,
						   s_erc *error)
{
	size_t tsize;
	size_t psize;
	SIterator *itr;
	int counter;


	S_CLR_ERR(error);
	tsize = SListSize(trees, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"SListSize\" failed"))
		return;

	psize = SListSize(pdfs, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"SListSize\" failed"))
		return;

	if (tsize != psize)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "get_trees_pdfs",
				  "Given trees and pdfs sizes mismatch");
		return;
	}

	*num = tsize;
	*ctrees = S_CALLOC(char*, tsize);
	if (*ctrees == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "get_trees_pdfs",
				  "Failed to allocate memory for 'char*' object");
		return;
	}

	itr = SListIterator(trees, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"SListIterator\" failed"))
	{
		S_FREE(*ctrees);
		return;
	}

	counter = 0;
	while (itr != NULL)
	{
		const char *tmp;


		tmp = SObjectGetString(SListIteratorValue(itr, error), error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"SListIteratorValue/SObjectGetString\" failed"))
		{
			S_FREE(*ctrees);
			return;
		}

		(*ctrees)[counter++] = s_strdup(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"s_strdup\" failed"))
		{
			S_FREE(*ctrees);
			return;
		}

		itr = SIteratorNext(itr);
	}

	*cpdfs = S_CALLOC(char*, tsize);
	if (*cpdfs == NULL)
	{
		S_FREE(*ctrees);
		S_FTL_ERR(error, S_MEMERROR,
				  "get_trees_pdfs",
				  "Failed to allocate memory for 'char*' object");
		return;
	}

	itr = SListIterator(pdfs, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_trees_pdfs",
				  "Call to \"SListIterator\" failed"))
	{
		S_FREE(*ctrees);
		S_FREE(*cpdfs);
		return;
	}

	counter = 0;
	while (itr != NULL)
	{
		const char *tmp;


		tmp = SObjectGetString(SListIteratorValue(itr, error), error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"SListIteratorValue/SObjectGetString\" failed"))
		{
			S_FREE(*ctrees);
			S_FREE(*cpdfs);
			return;
		}

		(*cpdfs)[counter++] = s_strdup(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_trees_pdfs",
					  "Call to \"s_strdup\" failed"))
		{
			S_FREE(*ctrees);
			S_FREE(*cpdfs);
			return;
		}

		itr = SIteratorNext(itr);
	}
}


static void get_windows(const SList *windows, char ***cwindows,
						int *num, s_erc *error)
{
	size_t tsize;
	SIterator *itr;
	int counter;


	S_CLR_ERR(error);
	tsize = SListSize(windows, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_windows",
				  "Call to \"SListSize\" failed"))
		return;

	*num = tsize;
	*cwindows = S_CALLOC(char*, tsize);
	if (*cwindows == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "get_windows",
				  "Failed to allocate memory for 'char*' object");
		return;
	}

	itr = SListIterator(windows, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "get_windows",
				  "Call to \"SListIterator\" failed"))
	{
		S_FREE(*cwindows);
		return;
	}

	counter = 0;
	while (itr != NULL)
	{
		const char *tmp;


		tmp = SObjectGetString(SListIteratorValue(itr, error), error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_windows",
					  "Call to \"SListIteratorValue/SObjectGetString\" failed"))
		{
			S_FREE(*cwindows);
			return;
		}

		(*cwindows)[counter++] = s_strdup(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "get_windows",
					  "Call to \"s_strdup\" failed"))
		{
			S_FREE(*cwindows);
			return;
		}

		itr = SIteratorNext(itr);
	}
}


static void load_hts_engine_data(const SMap *data, HTS_Engine *engine, s_erc *error)
{
	const SMap *tmp;
	const SList *trees;
	const SList *pdfs;
	const SList *windows;
	int num;
	int i;
	char **ctrees;
	char **cpdfs;
	char **cwindows;
	int num_win;
	const char *gv;


	S_CLR_ERR(error);

	/* duration */
	tmp = S_MAP(SMapGetObjectDef(data, "duration", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (tmp == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'duration' HTS Engine data");
		return;
	}

	trees = S_LIST(SMapGetObjectDef(tmp, "trees", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (trees == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'duration:trees' HTS Engine data");
		return;
	}

	pdfs = S_LIST(SMapGetObjectDef(tmp, "pdfs", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (pdfs == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'duration:pdfs' HTS Engine data");
		return;
	}

	get_trees_pdfs(trees, pdfs, &ctrees, &cpdfs, &num, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_trees_pdfs\" failed"))
		return;

	HTS_Engine_load_duration_from_fn(engine, cpdfs, ctrees, num);
	for (i = 0; i < num; i++)
	{
		S_FREE(ctrees[i]);
		S_FREE(cpdfs[i]);
	}

	S_FREE(ctrees);
	S_FREE(cpdfs);


	/* log F0 */
	tmp = S_MAP(SMapGetObjectDef(data, "log F0", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (tmp == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0' HTS Engine data");
		return;
	}

	trees = S_LIST(SMapGetObjectDef(tmp, "trees", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (trees == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0:trees' HTS Engine data");
		return;
	}

	pdfs = S_LIST(SMapGetObjectDef(tmp, "pdfs", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (pdfs == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0:pdfs' HTS Engine data");
		return;
	}

	get_trees_pdfs(trees, pdfs, &ctrees, &cpdfs, &num, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_trees_pdfs\" failed"))
		return;

	windows = S_LIST(SMapGetObjectDef(tmp, "windows", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (windows == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'log F0:windows' HTS Engine data");
		return;
	}

	get_windows(windows, &cwindows, &num_win, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_windows\" failed"))
		return;

	/* log f0 is stream 1, and msd_flag is TRUE */
	HTS_Engine_load_parameter_from_fn(engine, cpdfs, ctrees, cwindows,
									  1, TRUE, num_win, num);

	for (i = 0; i < num; i++)
	{
		S_FREE(ctrees[i]);
		S_FREE(cpdfs[i]);
	}

	S_FREE(ctrees);
	S_FREE(cpdfs);

	for (i = 0; i < num_win; i++)
		S_FREE(cwindows[i]);

	S_FREE(cwindows);

	gv = SMapGetStringDef(tmp, "gv-lf0", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetStringDef\" failed"))
		return;

	if (gv != NULL)
		HTS_Engine_load_gv_from_fn(engine, (char**)&gv, NULL, 1, 1);


	/* spectrum */
	tmp = S_MAP(SMapGetObjectDef(data, "spectrum", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (tmp == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum' HTS Engine data");
		return;
	}

	trees = S_LIST(SMapGetObjectDef(tmp, "trees", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (trees == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum:trees' HTS Engine data");
		return;
	}

	pdfs = S_LIST(SMapGetObjectDef(tmp, "pdfs", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (pdfs == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum:pdfs' HTS Engine data");
		return;
	}

	get_trees_pdfs(trees, pdfs, &ctrees, &cpdfs, &num, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_trees_pdfs\" failed"))
		return;

	windows = S_LIST(SMapGetObjectDef(tmp, "windows", NULL, error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetObjectDef\" failed"))
		return;

	if (windows == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "load_hts_engine_data",
				  "Failed to find 'spectrum:windows' HTS Engine data");
		return;
	}

	get_windows(windows, &cwindows, &num_win, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"get_windows\" failed"))
		return;

	/* spectrum is stream 0, and msd_flag is FALSE */
	HTS_Engine_load_parameter_from_fn(engine, cpdfs, ctrees, cwindows,
									  0, FALSE, num_win, num);

	for (i = 0; i < num; i++)
	{
		S_FREE(ctrees[i]);
		S_FREE(cpdfs[i]);
	}

	S_FREE(ctrees);
	S_FREE(cpdfs);

	for (i = 0; i < num_win; i++)
		S_FREE(cwindows[i]);

	S_FREE(cwindows);

	gv = SMapGetStringDef(tmp, "gv-mgc", NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "load_hts_engine_data",
				  "Call to \"SMapGetStringDef\" failed"))
		return;

	if (gv != NULL)
		HTS_Engine_load_gv_from_fn(engine, (char**)&gv, NULL, 0, 1);
}


/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/

/* we need to delete the window plug-in if any */
static void Destroy(void *obj, s_erc *error)
{
	SHTSEngineSynthUttProc *self = obj;


	S_CLR_ERR(error);
	HTS_Engine_clear(&(self->engine));
}


static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static void Initialize(SUttProcessor *self, const SVoice *voice, s_erc *error)
{
	hts_params *engine_params;
	SHTSEngineSynthUttProc *HTSsynth = (SHTSEngineSynthUttProc*)self;
	const SMap *hts_data;


	S_CLR_ERR(error);

	/* get the HTS engine settings */
	engine_params = get_hts_engine_params(self->features, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"get_hts_engine_params\" failed"))
		return;

	/* initialize the engine */
	HTS_Engine_initialize(&(HTSsynth->engine), 2);

	/* set the engine parameters */
	HTS_Engine_set_sampling_rate(&(HTSsynth->engine), engine_params->sampling_rate);
	HTS_Engine_set_fperiod(&(HTSsynth->engine), engine_params->fperiod);
	HTS_Engine_set_alpha(&(HTSsynth->engine), engine_params->alpha);
	HTS_Engine_set_gamma(&(HTSsynth->engine), engine_params->stage);
	HTS_Engine_set_log_gain(&(HTSsynth->engine), engine_params->use_log_gain);
	HTS_Engine_set_beta(&(HTSsynth->engine), engine_params->beta);
	HTS_Engine_set_audio_buff_size(&(HTSsynth->engine), engine_params->audio_buff_size);
	HTS_Engine_set_msd_threshold(&(HTSsynth->engine), 1, engine_params->uv_threshold);
	HTS_Engine_set_gv_weight(&(HTSsynth->engine), 0, engine_params->gv_weight_mcp);
	HTS_Engine_set_gv_weight(&(HTSsynth->engine), 1, engine_params->gv_weight_lf0);

	S_FREE(engine_params);

	hts_data = S_MAP(SVoiceGetFeature(voice, "hts engine data", error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"SVoiceGetFeature\" failed"))
		goto quit_error;

	if (hts_data == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Initialize",
				  "Failed to get \"hts engine data\" map from voice features");
		goto quit_error;
	}

	load_hts_engine_data(hts_data, &(HTSsynth->engine), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Initialize",
				  "Call to \"load_hts_engine_data\" failed"))
		goto quit_error;

	/* all OK */
	return;

	/* error clean up */
quit_error:
	HTS_Engine_clear(&(HTSsynth->engine));
}


static void Run(const SUttProcessor *self, SUtterance *utt,
				s_erc *error)
{
	SHTSEngineSynthUttProc *HTSsynth = (SHTSEngineSynthUttProc*)self;
	const SRelation *segmentRel;
	SAudio *audio = NULL;
	s_bool is_present;
	char **label_data = NULL;
	int label_size;
	const SItem *item;
	const SItem *itemItr;
	int counter;
	uint i;


	S_CLR_ERR(error);

	/* we require the segment relation */
	is_present = SUtteranceRelationIsPresent(utt, "Segment", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceRelationIsPresent\" failed"))
		goto quit_error;

	if (!is_present)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "Failed to find 'Segment' relation in utterance");
		goto quit_error;
	}

	segmentRel = SUtteranceGetRelation(utt, "Segment", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceGetRelation\" failed"))
		goto quit_error;

	item = SRelationHead(segmentRel, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SRelationHead\" failed"))
		goto quit_error;

	itemItr = item;
	label_size = 0;
	while (itemItr != NULL)
	{
		label_size++;
		itemItr = SItemNext(itemItr, error);
	}

	label_data = S_CALLOC(char*, label_size);

	itemItr = item;
	counter = 0;
	while (itemItr != NULL)
	{
		SObject *dFeat;
		const char *tmp;


		dFeat = s_path_to_featproc(itemItr, "hts_labels", error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"s_path_to_featproc\" failed"))
			goto quit_error;

		if (dFeat == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
					  "Run",
					  "Failed to generate hts labels for segment item");
			goto quit_error;
		}

		tmp = SObjectGetString(dFeat, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SObjectGetString\" failed"))
			goto quit_error;

		label_data[counter++] = s_strdup(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"s_strdup\" failed"))
			goto quit_error;

		S_DELETE(dFeat, "Run", error);
		itemItr = SItemNext(itemItr, error);
	}

	/* speech synthesis part */
	HTS_Engine_load_label_from_string_list(&(HTSsynth->engine), label_data, label_size);
	HTS_Engine_create_sstream(&(HTSsynth->engine));
	HTS_Engine_create_pstream(&(HTSsynth->engine));
	HTS_Engine_create_gstream(&(HTSsynth->engine));

	/* create an audio object */
	audio = (SAudio*)S_NEW("SAudio", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Failed to create new 'SAudio' object"))
		goto quit_error;

	/* set audio feature in utterance */
	SUtteranceSetFeature(utt, "audio", S_OBJECT(audio), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceSetFeature\" failed"))
		goto quit_error;

	audio->sample_rate = HTSsynth->engine.global.sampling_rate;
	audio->num_samples = (uint32)HTS_GStreamSet_get_total_nsample(&(HTSsynth->engine).gss);
	audio->samples = S_MALLOC(float, audio->num_samples);
	if (audio->samples == NULL)
	{
		S_FTL_ERR(error, S_MEMERROR,
				  "Run",
				  "Failed to allocate memory for 'float' object");
		goto quit_error;
	}

	/* write data */
	for (i = 0; i < audio->num_samples; i++)
		audio->samples[i] = (float)(HTS_GStreamSet_get_speech(&(HTSsynth->engine).gss, i) * 1.0);

	for (counter = 0; counter < label_size; counter++)
		S_FREE(label_data[counter]);
	S_FREE(label_data);

	HTS_Engine_refresh(&(HTSsynth->engine));

	/* all OK here */
	return;

	/* error clean-up code */
quit_error:
	if (label_data != NULL)
	{
		for (counter = 0; counter < label_size; counter++)
		{
			if (label_data[counter] != NULL)
				S_FREE(label_data[counter]);
		}

		S_FREE(label_data);
	}

	return;
}


/************************************************************************************/
/*                                                                                  */
/* SCreateUnitsUttProc class initialization                                         */
/*                                                                                  */
/************************************************************************************/

static SHTSEngineSynthUttProcClass HTSEngineSynthUttProcClass =
{
	/* SObjectClass */
	{
		"SUttProcessor:SHTSEngineSynthUttProc",
		sizeof(SHTSEngineSynthUttProc),
		{ 0, 1},
		NULL,            /* init    */
		Destroy,         /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* SUttProcessorClass */
	Initialize,          /* initialize */
	Run                  /* run        */
};