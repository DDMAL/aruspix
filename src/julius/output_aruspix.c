/**
 * @file   output_file.c
 * 
 * <EN>
 * @brief  Output recognition result to each separate file
 * </EN>
 * 
 * <JA>
 * @brief  認識結果を個別ファイルへ出力する. 
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Wed Dec 12 11:07:46 2007
 * 
 * $Revision: 1.4 $
 * 
 */
/*
 * Copyright (c) 1991-2012 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2012 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include "app.h"

/**
 * Assumed tty width for graph view output
 * 
 */
#define TEXTWIDTH 70
#define MAXBUFLEN 4096 ///< Maximum line length of a message sent from a client

static char fname[MAXBUFLEN]; // the output file rec.mlf
static char labname[MAXBUFLEN]; // the label name
static FILE *fp = NULL;

void
outfile_set_fname(char *input_filename)
{
    char *p;
    
    strncpy(fname, input_filename, MAXBUFLEN);
    if ((p = strrchr(fname, '/')) != NULL) {
        *p = '\0';
    }
    strcat(fname, "/rec.mlf" );
    
    // label name
    char *labstart = input_filename;
    if ((p = strrchr(input_filename, '/')) != NULL) {
        labstart = p;
    }
    strncpy(labname, labstart, MAXBUFLEN);
    if ((p = strrchr(labname, '.')) != NULL) {
        *p = '\0';
    }
    strcat(labname, ".lab" );
}

static void
outfile_open(Recog *recog, void *dummy)
{
    if ((fp = fopen(fname, "a")) == NULL) {
        fprintf(stderr, "output_rec: failed to open \"%s\", result not saved\n", fname);
        return;
    }
    // check if new file, add header if necessary
    fpos_t pos;
    fgetpos( fp, &pos );
    if ( pos == 0 ) {
        fprintf( fp, "#!MLF!#\n" );
    }
    fprintf( fp, "\"*%s\"\n", labname );
}

static void
outfile_close(Recog *recog, void *dummy)
{
    if (fp != NULL) {
        fprintf( fp, ".\n" );
        fclose(fp);
        fprintf(stderr, "result written to \"%s\"\n", fname);
    }
    fp = NULL;
}

static void
outfile_sentence(Recog *recog, void *dummy)
{
    RecogProcess *r;
    Sentence *s;
    WORD_INFO *winfo;
    WORD_ID *seq;
    int seqnum;
    int n, num;
    int i, j;
    boolean multi;
    static char phbuf[MAX_HMMNAME_LEN];
    SentenceAlign *align;
    HMM_Logical *p;
    
    if (recog->process_list->next != NULL) multi = TRUE;
    else multi = FALSE;
    
    for(r=recog->process_list;r;r=r->next) {
        if (! r->live) continue;
        
        if (r->result.status < 0) {
            continue;
        }
        
        winfo = r->lm->winfo;
        num = r->result.sentnum;
        
        // we should have only none sentence....
        for(n=0;n<num;n++) {
            s = &(r->result.sent[n]);
            seq = s->word;
            seqnum = s->word_num;
            
            /* output alignment result if exist */
            for (align = s->align; align; align = align->next) {
                for(i=1;i<align->num-1;i++) {
                    // * 2 because of the MFCC sampling
                    fprintf(fp, "%d %d %s\n", align->begin_frame[i] * 2, align->end_frame[i] * 2, winfo->wname[align->w[i]]);
                }
            }
        }
    }
    
}

static void
outfile_gmm(Recog *recog, void *dummy)
{
    HTK_HMM_Data *d;
    GMMCalc *gc;
    int i;
    
    gc = recog->gc;
    
    fprintf(fp, "--- GMM result begin ---\n");
    i = 0;
    for(d=recog->gmm->start;d;d=d->next) {
        fprintf(fp, "  [%8s: total=%f avg=%f]\n", d->name, gc->gmm_score[i], gc->gmm_score[i] / (float)gc->framecount);
        i++;
    }
    fprintf(fp, "  max = \"%s\"", gc->max_d->name);
#ifdef CONFIDENCE_MEASURE
    fprintf(fp, " (CM: %f)", gc->gmm_max_cm);
#endif
    fprintf(fp, "\n");
    fprintf(fp, "--- GMM result end ---\n");
}

static void
outfile_graph(Recog *recog, void *dummy)
{
    WordGraph *wg;
    int tw1, tw2, i;
    WORD_INFO *winfo;
    RecogProcess *r;
    boolean multi;
    
    if (recog->process_list->next != NULL) multi = TRUE;
    else multi = FALSE;
    
    for(r=recog->process_list;r;r=r->next) {
        if (! r->live) continue;
        if (r->result.wg == NULL) continue;	/* no graphout specified */
        if (multi) fprintf(fp, "[#%d %s]\n", r->config->id, r->config->name);
        
        winfo = r->lm->winfo;
        
        /* debug: output all graph word info */
        wordgraph_dump(fp, r->result.wg, winfo);
        
        fprintf(fp, "-------------------------- begin wordgraph show -------------------------\n");
        for(wg=r->result.wg;wg;wg=wg->next) {
            tw1 = (TEXTWIDTH * wg->lefttime) / r->peseqlen;
            tw2 = (TEXTWIDTH * wg->righttime) / r->peseqlen;
            fprintf(fp, "%4d:", wg->id);
            for(i=0;i<tw1;i++) fprintf(fp, " ");
            fprintf(fp, " %s\n", winfo->woutput[wg->wid]);
            fprintf(fp, "%4d:", wg->lefttime);
            for(i=0;i<tw1;i++) fprintf(fp, " ");
            fprintf(fp, "|");
            for(i=tw1+1;i<tw2;i++) fprintf(fp, "-");
            fprintf(fp, "|\n");
        }
        fprintf(fp, "-------------------------- end wordgraph show ---------------------------\n");
    }
}

static void
outfile_confnet(Recog *recog, void *dummy)
{
    CN_CLUSTER *c;
    int i;
    RecogProcess *r;
    boolean multi;
    
    if (recog->process_list->next != NULL) multi = TRUE;
    else multi = FALSE;
    
    for(r=recog->process_list;r;r=r->next) {
        if (! r->live) continue;
        if (r->result.confnet == NULL) continue;	/* no confnet obtained */
        if (multi) fprintf(fp, "[#%d %s]\n", r->config->id, r->config->name);
        
        fprintf(fp, "---- begin confusion network ---\n");
        for(c=r->result.confnet;c;c=c->next) {
            for(i=0;i<c->wordsnum;i++) {
                fprintf(fp, "(%s:%.3f)", (c->words[i] == WORD_INVALID) ? "-" : r->lm->winfo->woutput[c->words[i]], c->pp[i]);
                if (i == 0) fprintf(fp, "  ");
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "---- end confusion network ---\n");
    }
}

/******************************************************************/
void
setup_output_file(Recog *recog, void *data)
{
    callback_add(recog, CALLBACK_EVENT_RECOGNITION_BEGIN, outfile_open, data);
    callback_add(recog, CALLBACK_EVENT_RECOGNITION_END, outfile_close, data);
    callback_add(recog, CALLBACK_RESULT, outfile_sentence, data);
    callback_add(recog, CALLBACK_RESULT_GMM, outfile_gmm, data);
    callback_add(recog, CALLBACK_RESULT_GRAPH, outfile_graph, data);
    callback_add(recog, CALLBACK_RESULT_CONFNET, outfile_confnet, data);
}  

