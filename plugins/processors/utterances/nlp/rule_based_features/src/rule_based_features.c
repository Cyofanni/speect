/************************************************************************************/
/* Copyright (c) 2010-2011 The Department of Arts and Culture,                      */
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
/* AUTHOR  : Giovanni Mazzocchin                                                    */
/* DATE    : July 2016                                                              */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Get features from rules.                                                         */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "rule_based_features.h"
//#include "hts_data_collector.h"


/************************************************************************************/
/*                                                                                  */
/* Defines                                                                          */
/*                                                                                  */
/************************************************************************************/



/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

static SRuleBasedFeaturesUttProcClass RuleBasedFeaturesUttProcClass;

/************************************************************************************/
/*                                                                                  */
/* Static function prototypes                                                       */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Function implementations                                                         */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/

/* FIRST TASK:
   - controllare il pos tag attuale, dire se è nella lista nouns0
   - controllare pos tag nella word precedente, e dire se è W
*/

/* local functions to register and free classes */
S_LOCAL void _s_rule_based_features_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(S_OBJECTCLASS(&RuleBasedFeaturesUttProcClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_rule_based_features_class_reg",
			  "Failed to register RuleBasedFeaturesUttProcClass");
}


S_LOCAL void _s_rule_based_features_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(S_OBJECTCLASS(&RuleBasedFeaturesUttProcClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_rule_based_features_class_free",
			  "Failed to free RuleBasedFeaturesUttProcClass");
}


/************************************************************************************/
/*                                                                                  */
/* Static function implementations                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/

static void Initialize(SUttProcessor *self, const SVoice *voice, s_erc *error)
{
	SRuleBasedFeaturesUttProc *ruleBasedFeaturesProc = (SRuleBasedFeaturesUttProc*)self;

	S_CLR_ERR(error);
}

static void Init(void *obj, s_erc *error)
{
	SRuleBasedFeaturesUttProc *self = obj;

	S_CLR_ERR(error);
}


static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);

	SObjectDecRef(obj);
}

static void Destroy(void *obj, s_erc *error)
{
	S_CLR_ERR(error);

}

static s_bool searchStringList(SList *list, char *str, s_erc *error)
{
	S_CLR_ERR(error);
	s_bool found = FALSE;
	SIterator *itrList = NULL;
	itrList = S_ITERATOR_GET(list, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "searchStringList",
				  "Call to \"S_ITERATOR_GET\" failed"))
		return FALSE;

	while (itrList != NULL && found == FALSE)
	{
		SObject *curObj = SIteratorObject(itrList, error);
		const char *curStr = SObjectGetString(curObj, error);

		if (s_strcmp(str, curStr, error) == 0)
		{
			found = TRUE;
		}
		itrList = SIteratorNext(itrList);
	}

	if (itrList != NULL)
	{
		S_DELETE(itrList, "searchStringList", error);
	}

	return found;
}

static char* filterPosTag(const char *posTagStr, s_erc *error)
{
	char *filteredPosTagStr = malloc(sizeof(char) * 10);
	if (s_strcmp(posTagStr, "default", error) == 0)
	{
		filteredPosTagStr[0] = posTagStr[0];
		filteredPosTagStr[1] = '\0';
	}

	else if (posTagStr[0] == 'N')
	{
		int i = 0;

		while (posTagStr[i] != '\0')
		{
			filteredPosTagStr[i] = posTagStr[i];
			i++;
		}

		filteredPosTagStr[i] = '\0';
	}

	else if (posTagStr[0] == 'E')
	{
		if (posTagStr[1] == 'A')
		{
			filteredPosTagStr[0] = 'E';
			filteredPosTagStr[1] = 'A';
			filteredPosTagStr[2] = '\0';
		}
		else
		{
			filteredPosTagStr[0] = 'E';
			filteredPosTagStr[1] = '\0';
		}
	}

	else if (posTagStr[0] == 'T')
	{
		filteredPosTagStr[0] = 'D';
		filteredPosTagStr[1] = '\0';
	}

	else if (posTagStr[0] == 'V')
	{
		int i = 0;
		while (!isdigit(posTagStr[i]))
		{
			filteredPosTagStr[i] = posTagStr[i];
			i++;
		}
		filteredPosTagStr[i] = '\0';

		/* index for the second to last cell */
		int j = i - 2;
		if (filteredPosTagStr[j] == 'm' || filteredPosTagStr[j] == 'f')
		{
			if (filteredPosTagStr[j + 1] == 's' || filteredPosTagStr[j + 1] == 'p')
			{
				filteredPosTagStr[j] = '\0';
			}

			if (filteredPosTagStr[1] == 'M' || filteredPosTagStr[1] == 'A')
			{
				int k = 2;
				while (filteredPosTagStr[k] != '\0')
				{
					filteredPosTagStr[k - 1] = filteredPosTagStr[k];
					k++;
				}
				filteredPosTagStr[k - 1] = '\0';
			}
		}
	}

	else if (posTagStr[0] == 'S')
	{
		/* check for at least 3 characters */
		if (posTagStr[1] != '\0' && posTagStr[2] != '\0')
		{
			if ((posTagStr[1] == 'm' || posTagStr[1] == 'f' || posTagStr[1] == 'n') &&
				(posTagStr[2] == 'p' || posTagStr[2] == 's' || posTagStr[2] == 'm'))
			{
				filteredPosTagStr[0] = posTagStr[0];
				filteredPosTagStr[1] = posTagStr[1];
				filteredPosTagStr[2] = posTagStr[2];
				filteredPosTagStr[3] = '\0';
			}
			else
			{
				if (posTagStr[1] == 'A' || posTagStr[1] == 'W' || posTagStr[1] == 'P')
				{
					filteredPosTagStr[0] = posTagStr[0];
					filteredPosTagStr[1] = posTagStr[1];
					filteredPosTagStr[2] = '\0';
				}
				else
				{
					filteredPosTagStr[0] = posTagStr[0];
					filteredPosTagStr[1] = '\0';
				}
			}
		}
	}

	//fprintf(stderr, "%s\n", filteredPosTagStr);
	return filteredPosTagStr;
}

        /**
         * determination of sentence type values: decl, excl, interrog, interrogYN or interrogW
         */

/* getSentenceType is made out of two parts:
 * 	    1) the first section searchs for the last punctuation element of the sentence
 * 				-> if it is a '.' --> set "decl" type (where should I set this feature value?)
 * 				-> if it is a '!' --> set "excl" type (where should I set this feature value?)
 * 				-> if it is a '?' --> set "interrog" type (where should I set this feature value?)
 * 		2) if the first part decides for "interrog" type, there should be other controls
 * 		   to establish the sentence's subtype
 * */

static void getSentenceType(const SItem *phrase, s_erc *error)
{
	S_CLR_ERR(error);

	/* types: "decl, "excl", "interrog" */
	/* stop at sentence's last token */
	/* keep track of the previous-to-current token each iteration */



}

/*protected String getSentenceType(NodeList tokens) {
	String sentenceType = "decl";

	for (int i = tokens.getLength() - 1; i >= 0; i--) { // search for sentence finishing punctuation mark
				Element t = (Element) tokens.item(i);
				String punct = MaryDomUtils.tokenText(t);
				if (punct.equals(".")) {
							sentenceType = "decl";
							break;
				} else if (punct.equals("!")) {
							sentenceType = "excl";
							break;
				} else if (punct.equals("?")) {
							sentenceType = "interrog";
							break;
				}
	}

	if (sentenceType.equals("interrog")) {
			for (int i = 0; i < tokens.getLength() - 1; i++) { // search for the first word in sentence
						Element t = (Element) tokens.item(i);
						if (!t.getAttribute("ph").equals("")) {
								Element firstToken = (Element) tokens.item(i);
								// To check for italian
								// setInterrogYN contains possible part of speechs of first word in yes-no question
								Set<String> setInterrogYN = (Set<String>) listMap.get("firstPosInQuestionYN");
								// setInterrogW contains possible part of speechs of first word in wh-question
								Set<String> setInterrogW = (Set<String>) listMap.get("firstPosInQuestionW");

								String posFirstWord = OpenNLPPosTagger.getReducedPOS(firstToken.getAttribute("pos"));
								if (setInterrogYN != null && setInterrogYN.contains(posFirstWord)) {
											// TOCHECK: Global interrogative
											sentenceType = "interrogYN";

								}
								if (setInterrogW != null && setInterrogW.contains(posFirstWord)) {
										// TOCHECK: Partial interrogative
										sentenceType = "interrogW";
								}
								break;
						}
					}
	}
                return sentenceType;
}
*/

static void Run(const SUttProcessor *self, SUtterance *utt,
					s_erc *error)
{
	SRelation *wordRel = NULL;
	SRelation *phraseRel = NULL;
	const SItem *wordItem = NULL;
	/* loop on phraseItem(s) */
	const SItem *phraseItem = NULL;
	s_bool have_symbols;
	s_bool isFeatPresent;
	/* 'prosSymbols' holds the complete map read from voice.json's 'list definitions' */
	const SMap *prosSymbols = NULL;
	/* 'posValueStr' string value of current POS tag value */
	const char *posValueStr = NULL;

	/* 'valueList' is used to represent JSON value lists for each
	   entry in 'list definitions' map:
	   e.g.) ["S","SP"] or ["è","ha","ho"] or ["A","B","I","N","NO","S","SA","SP","SW","V","X"] */
	const SList *valueList = NULL;
	char *itemFromValueList;

	have_symbols = SUttProcessorFeatureIsPresent(self, "list definitions", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUttProcessorFeatureIsPresent\" failed"))
		goto quit_error;

	/* no Prosody symbols defined */
	if (have_symbols == FALSE)
	{
		return;
	}

	/* get the complete 'list definitions' map and put it into 'prosSymbols' SMap */
	prosSymbols = S_CAST(SUttProcessorGetFeature(self, "list definitions", error),
						SMap, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Failed to get 'list definitions' SMap feature"))
		goto quit_error;



	wordRel = SUtteranceGetRelation(utt, "Token", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceGetRelation\" failed"))
		goto quit_error;

	phraseRel = SUtteranceGetRelation(utt, "Phrase", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceGetRelation\" failed"))
		goto quit_error;

	wordItem = SRelationHead(wordRel, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SRelationHead\" failed"))
		goto quit_error;

	phraseItem = SRelationHead(phraseRel, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SRelationHead\" failed"))
		goto quit_error;

	/* try to loop on phraseItem, and
	 * get the tokens for each phrase inside an inner loop,
	 * looking at puncutation mark */
	while (phraseItem != NULL)
	{
		/* should I call 'getSentenceType' on 'phraseItem'
		 * at each iteration? */
		SItem *wordFromCurrentPhrase = SItemPathToItem(phraseItem, "daughter", error);
		SItem *wordAsToken = SItemAs(wordFromCurrentPhrase, "Token", error);
		SItem *tokenItem = SItemParent(wordAsToken, error);

		/* I need a loop that stops at phraseItem-pointed phrase's final token */
		s_bool isStopPunct = FALSE;
		while (isStopPunct == FALSE)
		{
			/* BEGIN OF RULES SECTION */
			isFeatPresent = SItemFeatureIsPresent(tokenItem, "POS", error);
			if (S_CHK_ERR(error, S_CONTERR,
					"Run",
					"Call to \"SItemFeatureIsPresent\" failed"))
				goto quit_error;

			/* we can continue only if the POS tag has been obtained */
			if (isFeatPresent == TRUE)
			{
				char *posValueStr_filtered = NULL;
				s_bool currPosInCurrList;

				/* ******************************* */
				/* FIRST RULE:
				*  if the current word item's pos value is contained in the list
				*  "pos_tonal_accent" from "list definitions" map -> then -->
				* 		set current's item "accent" attribute to the value "tone"
				*/

				/* check if we have 'pos_tonal_accent' symbol */
				have_symbols = SMapObjectPresent(prosSymbols, "pos_tonal_accent", error);
				if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"SMapObjectPresent\" failed"))
					goto quit_error;

				if (have_symbols)
				{
					valueList = S_CAST(SMapGetObject(prosSymbols, "pos_tonal_accent", error), SList, error);
					if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"SMapGetObject\" failed"))
					goto quit_error;
				}

				/* 'posValueStr' holds POS tag's value */
				posValueStr = SItemGetString(tokenItem, "POS", error);
				if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"SItemGetString\" failed"))
					goto quit_error;

				/* filter the current POS tag, remember to free the memory
				* pointed to by 'posValueStr_filtered' pointer */
				posValueStr_filtered = filterPosTag(posValueStr, error);
				if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"filterPosTag\" failed"))
					goto quit_error;

				/* check if the current POS tag exists in 'pos_tonal_accent' list */
				currPosInCurrList = searchStringList(valueList, posValueStr_filtered, error);
				if (currPosInCurrList == TRUE)
				{
					SItemSetString(tokenItem, "accent", "tone", error);
					if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"SItemSetString\" failed"))
					goto quit_error;
				}

				if (posValueStr_filtered)
				{
					S_FREE(posValueStr_filtered);
				}


				/* ******************************* */
				/* SECOND RULE:
				*  if the current word item's pos value is contained in the list
				*  "pos_no_accent" from "list definitions" map -> then -->
				* 		set current's item "accent" attribute to the value ""
				*/

				have_symbols = SMapObjectPresent(prosSymbols, "pos_no_accent", error);
				if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"SMapObjectPresent\" failed"))
					goto quit_error;

				if (have_symbols)
				{
					valueList = S_CAST(SMapGetObject(prosSymbols, "pos_no_accent", error), SList, error);
					if (S_CHK_ERR(error, S_CONTERR,
							"Run",
							"Call to \"SMapGetObject\" failed"))
						goto quit_error;
				}

				/* 'posValueStr' holds POS tag's value */
				posValueStr = SItemGetString(wordItem, "POS", error);
				if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"SItemGetString\" failed"))
					goto quit_error;

				/* filter the current POS tag, remember to free the memory
				* pointed to by 'posValueStr_filtered' pointer */
				posValueStr_filtered = filterPosTag(posValueStr, error);
				if (S_CHK_ERR(error, S_CONTERR,
						"Run",
						"Call to \"filterPosTag\" failed"))
					goto quit_error;

				/* check if the current POS tag exists in 'pos_tonal_accent' list */
				currPosInCurrList = searchStringList(valueList, posValueStr_filtered, error);
				if (currPosInCurrList == TRUE)
				{
					SItemSetString(wordItem, "accent", "", error);
					if (S_CHK_ERR(error, S_CONTERR,
							"Run",
							"Call to \"SItemSetString\" failed"))
						goto quit_error;
				}
				else
				{
					/* ******************************* */
					/* THIRD RULE:
					*   set "accent" feature to ""
					*/
					SItemSetString(wordItem, "accent", "", error);
					if (S_CHK_ERR(error, S_CONTERR,
							"Run",
							"Call to \"SItemSetString\" failed"))
						goto quit_error;
				}

				if (posValueStr_filtered)
				{
					S_FREE(posValueStr_filtered);
				}
			}

			/* END OF RULES SECTION */

			/* ****************************** */
			/* checks for inner loop */
			/* THESE CONTROLS MAKE WHAT getSenteceType should do */
			s_bool isPunct = SItemFeatureIsPresent(tokenItem, "IsPunctuation", error);

			if (isPunct)
			{
				/* get the puncutation mark as a string and
				 * set isStopPunct flag if it is ".", "?" or "!" */
				/*  *if it is a '.' --> set "decl" type
				    *if it is a '!' --> set "excl" type
           		    *if it is a '?' --> set "interrog" type
           		*/
				const char *punctStr = SItemGetName(tokenItem, error);
				if (s_strcmp(punctStr, ".", error) == 0 || s_strcmp(punctStr, "?", error) == 0 ||
							s_strcmp(punctStr, "!", error) == 0)
				{
						isStopPunct = TRUE;
						/* should I set the sentence type here? */
						if (s_strcmp(punctStr, ".", error) == 0)
						{
							SItemSetString(phraseItem, "type", "decl", error);
						}
						else if (s_strcmp(punctStr, "?", error) == 0)
						{
							SItemSetString(phraseItem, "type", "interrog", error);
						}
						else if (s_strcmp(punctStr, "!", error) == 0)
						{
							SItemSetString(phraseItem, "type", "excl", error);
						}
				}
			}

			tokenItem = SItemNext(tokenItem, error);
		}


		phraseItem = SItemNext(phraseItem, error);
		fprintf(stderr, "\n\n");
	}

	/* error cleanup */

quit_error:
	return;


	S_UNUSED(self);
}


/************************************************************************************/
/*                                                                                  */
/* SRuleBasedFeaturesUttProc class initialization                                   */
/*                                                                                  */
/************************************************************************************/

static SRuleBasedFeaturesUttProcClass RuleBasedFeaturesUttProcClass =
{
	/* SObjectClass */
	{
		"SUttProcessor:SRuleBasedFeaturesUttProc",
		sizeof(SRuleBasedFeaturesUttProc),
		{ 0, 1},
		Init,            /* init    */
		Destroy,         /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* SUttProcessorClass */
	Initialize,          /* initialize */
	Run                  /* run     */
};
