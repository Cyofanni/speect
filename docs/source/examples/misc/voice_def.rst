
.. index:: 
   single: Examples (Miscellaneous); Voice Definiton

.. _examples/misc/voice_def:

========================
Voice Definition Example
========================

.. code-block:: javascript
  :linenos:

  {
    "voice-definition" :
    {
      "name" : "Afrikaans ZA",
      "description" : "South African Afrikaans HTS voice.",
      "gender" : "female",
      "language" : "afrikaans",
      "lang-code" : "afr",
      "version" :
      {
	"major" : 0,
	"minor" : 0
      }
    },
   
 
    /* plugins required by the voice */
    "plug-ins" : 
    [
      "utt_ebml.spi" /* for loading/saving utterances */
    ],
    

    /* data */
    "data" :
    {
      "phoneset" :
      {
	"format" : "spct_phoneset",
	"plug-in" : "phoneset_json.spi",
	"path" : "etc/phoneset.json"
      },
      "lexicon" :
      {
	"format" : "spct_lexicon",
	"plug-in" : "lexicon_json.spi",
	"path" : "etc/lexicon.json"
      },
      "addendum" : 
      {
	"format" : "spct_addendum",
	"plug-in" : "addendum_json.spi",
	"path" : "etc/addendum.json"
      },
      "g2p" :
      {
	"format" : "spct_g2p_rewrites",
	"plug-in" : "g2p_rewrites.spi",
	"path" : "etc/g2p.spct"
      },
      "syllabification" :
      {
	"format" : "spct_syllabification_rewrites_json",
	"plug-in" : "syllab_rewrites_json.spi",
	"path" : "etc/syllabification.json"
      }
    },
    

    /* features, any features which are global and should be easily accessible by end users */
    "features" : 
    {
      "hts engine data" :
      {
	"duration" :
	{
	  "trees" : [ "hts/tree-dur.inf" ],
	  "pdfs" : [ "hts/dur.pdf" ]
	},
	"log F0" :
	{
	  "trees" : [ "hts/tree-lf0.inf" ],
	  "pdfs" : [ "hts/lf0.pdf" ],
	  "windows" : 
	  [
	    "hts/lf0.win1",
	    "hts/lf0.win2",
	    "hts/lf0.win3"
	  ],
	  "gv-lf0" : "hts/gv-lf0.pdf"
	},
	"spectrum" :
	{
	  "trees" : [ "hts/tree-mgc.inf" ],
	  "pdfs" : [ "hts/mgc.pdf" ],
	  "windows" : 
	  [
	    "hts/mgc.win1",
	    "hts/mgc.win2",
	    "hts/mgc.win3"
	  ],
	  "gv-mgc" : "hts/gv-mgc.pdf"
	}
      }
    },
   
 
    /* feature processors */
    "feature-processors" : 
    {
      "segment_duration" :
      {
 	"class" : "SSegDurFeatProc",
	"plug-in" : "seg_dur_featproc.spi"
      },
      "segment_end" :
      {
 	"class" : "SSegEndFeatProc",
	"plug-in" : "seg_end_featproc.spi"
      },
      "segment_mid" :
      {
 	"class" : "SSegMidFeatProc",
	"plug-in" : "seg_mid_featproc.spi"
      },
      "segment_name_multilingual" :
      {
 	"class" : "SSegNameMultilingualFeatProc",
	"plug-in" : "seg_name_multilingual_featproc.spi"
      },
      "segment_pos_syl" :
      {
 	"class" : "SSegPosSylFeatProc",
	"plug-in" : "seg_pos_syl_featproc.spi"
      },
      "segment_pos_syl_rev" :
      {
 	"class" : "SSegPosSylRevFeatProc",
	"plug-in" : "seg_pos_syl_rev_featproc.spi"
      },
      "segment_start" :
      {
 	"class" : "SSegStartFeatProc",
	"plug-in" : "seg_start_featproc.spi"
      },
      "syllable_accent_all_in" :
      {
 	"class" : "SSylAccentAllInFeatProc",
	"plug-in" : "syl_accent_all_in_featproc.spi"
      },
      "syllable_accent_all_out" :
      {
 	"class" : "SSylAccentAllOutFeatProc",
	"plug-in" : "syl_accent_all_out_featproc.spi"
      },
      "syllable_accent_in" :
      {
 	"class" : "SSylAccentInFeatProc",
	"plug-in" : "syl_accent_in_featproc.spi"
      },
      "syllable_accent_out" :
      {
 	"class" : "SSylAccentOutFeatProc",
	"plug-in" : "syl_accent_out_featproc.spi"
      },
      "syllable_break" :
      {
 	"class" : "SSylBreakFeatProc",
	"plug-in" : "syl_break_featproc.spi"
      },
      "syllable_duration" :
      {
 	"class" : "SSylDurFeatProc",
	"plug-in" : "syl_dur_featproc.spi"
      },
      "syllable_end" :
      {
 	"class" : "SSylEndFeatProc",
	"plug-in" : "syl_end_featproc.spi"
      },
      "syllable_num_phones" :
      {
 	"class" : "SSylNumPhonesFeatProc",
	"plug-in" : "syl_num_phones_featproc.spi"
      },
      "syllable_pos_type" :
      {
 	"class" : "SSylPosTypeFeatProc",
	"plug-in" : "syl_pos_type_featproc.spi"
      },
      "syllable_pos_word" :
      {
 	"class" : "SSylPosWordFeatProc",
	"plug-in" : "syl_pos_word_featproc.spi"
      },
      "syllable_pos_word_rev" :
      {
 	"class" : "SSylPosWordRevFeatProc",
	"plug-in" : "syl_pos_word_rev_featproc.spi"
      },
      "syllable_pos_phrase" :
      {
 	"class" : "SSylPosPhraseFeatProc",
	"plug-in" : "syl_pos_phrase_featproc.spi"
      },
      "syllable_pos_phrase_rev" :
      {
 	"class" : "SSylPosPhraseRevFeatProc",
	"plug-in" : "syl_pos_phrase_rev_featproc.spi"
      },
      "segment_phoneset_feature" :
      {
 	"class" : "SSegPhonesetFeatureFeatProc",
	"plug-in" : "seg_ph_feat_featproc.spi"
      },
      "syllable_start" :
      {
 	"class" : "SSylStartFeatProc",
	"plug-in" : "syl_start_featproc.spi"
      },
      "syllable_vowel" :
      {
 	"class" : "SSylVowelFeatProc",
	"plug-in" : "syl_vowel_featproc.spi"
      },
      "syllable_stress_all_in" :
      {
 	"class" : "SSylStressAllInFeatProc",
	"plug-in" : "syl_stress_all_in_featproc.spi"
      },
      "syllable_stress_all_out" :
      {
 	"class" : "SSylStressAllOutFeatProc",
	"plug-in" : "syl_stress_all_out_featproc.spi"
      },
      "syllable_stress_in" :
      {
 	"class" : "SSylStressInFeatProc",
	"plug-in" : "syl_stress_in_featproc.spi"
      },
      "syllable_stress_out" :
      {
 	"class" : "SSylStressOutFeatProc",
	"plug-in" : "syl_stress_out_featproc.spi"
      },
      "word_break" :
      {
 	"class" : "SWordBreakFeatProc",
	"plug-in" : "word_break_featproc.spi"
      },
      "word_content_all_in" :
      {
 	"class" : "SWordContentAllInFeatProc",
	"plug-in" : "word_content_all_in_featproc.spi"
      },
      "word_content_all_out" :
      {
 	"class" : "SWordContentAllOutFeatProc",
	"plug-in" : "word_content_all_out_featproc.spi"
      },
      "word_content_in" :
      {
 	"class" : "SWordContentInFeatProc",
	"plug-in" : "word_content_in_featproc.spi"
      },
      "word_content_out" :
      {
 	"class" : "SWordContentOutFeatProc",
	"plug-in" : "word_content_out_featproc.spi"
      },
      "word_duration" :
      {
 	"class" : "SWordDurFeatProc",
	"plug-in" : "word_dur_featproc.spi"
      },
      "word_end" :
      {
 	"class" : "SWordEndFeatProc",
	"plug-in" : "word_end_featproc.spi"
      },
      "word_num_syls" :
      {
 	"class" : "SWordNumSylsFeatProc",
	"plug-in" : "word_num_syls_featproc.spi"
      },
      "word_pos_phrase" :
      {
 	"class" : "SWordPosPhraseFeatProc",
	"plug-in" : "word_pos_phrase_featproc.spi"
      },
      "word_pos_phrase_rev" :
      {
 	"class" : "SWordPosPhraseRevFeatProc",
	"plug-in" : "word_pos_phrase_rev_featproc.spi"
      },
      "word_start" :
      {
 	"class" : "SWordStartFeatProc",
	"plug-in" : "word_start_featproc.spi"
      },
      "phrase_num_syls" :
      {
 	"class" : "SPhraseNumSylsFeatProc",
	"plug-in" : "phrase_num_syls_featproc.spi"
      },
      "phrase_num_words" :
      {
 	"class" : "SPhraseNumWordsFeatProc",
	"plug-in" : "phrase_num_words_featproc.spi"
      },
      "phrase_pos_utt" :
      {
 	"class" : "SPhrasePosUttFeatProc",
	"plug-in" : "phrase_pos_utt_featproc.spi"
      },
      "phrase_pos_utt_rev" :
      {
 	"class" : "SPhrasePosUttRevFeatProc",
	"plug-in" : "phrase_pos_utt_rev_featproc.spi"
      },
      "utt_num_syls" :
      {
 	"class" : "SUttNumSylsFeatProc",
	"plug-in" : "utt_num_syls_featproc.spi"
      },
      "utt_num_words" :
      {
 	"class" : "SUttNumWordsFeatProc",
	"plug-in" : "utt_num_words_featproc.spi"
      },
      "utt_num_phrases" :
      {
 	"class" : "SUttNumPhrasesFeatProc",
	"plug-in" : "utt_num_phrases_featproc.spi"
      },
      "hts_labels" :
      {
 	"class" : "SHTSLabelsPrasa2011FeatProc",
	"plug-in" : "hts_labels_prasa2011_featproc.spi"
      }
    },
  
  
    /* utterance processors */
    "utterance-processors" :
    {
      "Tokenize" :
      {
	"class" : "STokenizationUttProc",
	"plug-in" : "tokenization_uttproc.spi",
	"features" : /* features of the utterance processor */
	{
	  "tokenizer symbols" :
	  {
	    "whitespace" : " \t\n\r",
	    "single-char" : "(){}[]",
	    "pre-punctuation" : "\"'`({[",
	    "post-punctuation" : "\"'`.,:;!?(){}[]"
	  }
	}
      },
      "Normalize" :
      {
	"class" : "SNormalizationUttProc",
	"plug-in" : "normalization_uttproc.spi"
      },
      "Phrasify" :
      {
	"class" : "SPhrasingUttProc",
	"plug-in" : "phrasing_uttproc.spi",
	"features" : 
	{
	  "phrasing symbols" :
	  {
	    "end-punctuation" : "?.,:;"
	  }
	}
      },
      "LexLookup" :
      {
	"class" : "SLexLookupUttProc",
	"plug-in" : "lexlookup_uttproc.spi"
      },
      "Pauses" :
      {
	"class" : "SPausesUttProc",
	"plug-in" : "pauses_uttproc.spi"
      },
      "HTS Engine Synthesizer" :
      {
	"class" : "SHTSEngineSynthUttProc103",
	"plug-in" : "hts_engine_synth_uttproc_103.spi",
	"features" :
	{
	  "sampling_rate" : 16000,
	  "fperiod" : 80,            
	  "alpha" : 0.42,            
	  "stage" : 0.0,             
	  "beta" : 0.0,              
	  "audio_buff_size" : 1600,  
	  "uv_threshold" : 0.5,      
	  "gv_weight_lf0" : 0.7,     
	  "gv_weight_mcp" : 1.0,     
	  "use_log_gain" : "FALSE"   
	}
      }
    },
    

    /* definiton of voice utterance types, the string names in the list refers to utterance processors */
    "utterance-types" : 
    {
      "text" :
      [
	"Tokenize",
	"Normalize",
	"Phrasify",
	"LexLookup",
	"Pauses",
	"HTS Engine Synthesizer"
      ],

      "text-to-segments" :
      [
	"Tokenize",
	"Normalize",
	"Phrasify",
	"LexLookup",
	"Pauses"
      ],    
      
      "text-to-words" :
      [
	"Tokenize",
	"Normalize",
	"Phrasify"
      ],    
      
      "syllabification" :
      [
	"Tokenize",
	"Normalize",
	"LexLookup"
      ]
    }
  }
