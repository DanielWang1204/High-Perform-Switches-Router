/*
 * EL7373 (Fall 2015) High Performance Switches and Routers
 *
 * Lab 3 - Single String Matching
 *
 * TA: Kuan-yin Chen (kyc257@nyu.edu)
 *     Cing-yu Chu (cyc391@nyu.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAX_TEXT_LEN            10000000                    /* Largest possible text length (chars) */
#define MAX_PATTERN_LEN         1000                        /* Largest possible pattern length (chars)*/
#define ALPHABET_LEN             256
#define max(a, b)          ((a < b) ? b : a)

typedef struct
{
    size_t      n_match;        /* # of pattern matches found */
    size_t      n_compare;      /* # of character comparisons performed */
}MatchResult;


void make_badCharacter(int *badCharacter, const char* p) {
    size_t  n = strlen(p);
    int i;
    for (i=0; i < ALPHABET_LEN; i++) {
        badCharacter[i] = n;
    }
    for (i=0; i < n-1; i++) {
        badCharacter[p[i]] = n-1 - i;
    }
}
int is_prefix(const char* p, int pos) {
    size_t  n = strlen(p);
    int i;
    int suffixlen =n - pos;
    for (i = 0; i < suffixlen; i++) {
        if (p[i] != p[pos+i]) {
            return 0;
        }
    }
    return 1;
}

int suffix_length(const char* p, int pos) {
    size_t  n = strlen(p);
    int i;
    // increment suffix length i to the first mismatch or beginning
    // of the word
    for (i = 0; (p[pos-i] ==p[n-1-i]) && (i < pos); i++);
    return i;
}

void make_goodSuffix(int *goodSuffix, const char* p) {
    size_t  n = strlen(p);
    int q;
    int last_prefix_index = n-1;

    // first loop
    for (q=n-1; q>=0; q--) {
        if (is_prefix(p, q+1)) {
            last_prefix_index = q+1;
        }
        goodSuffix[q] = last_prefix_index + (n-1 - q);
    }

    // second loop
    for (q=0; q < n-1; q++) {
        int slen = suffix_length(p,q);
        if (p[q - slen] != p[n-1 - slen]) {
            goodSuffix[n-1 - slen] = n-1 - q + slen;
        }
    }
}

MatchResult Boyer_Moore(const char* t, const char* p){
    size_t  m = strlen(t);
    size_t  n = strlen(p);
    int i;
    int badCharacter[ALPHABET_LEN];
    int *goodSuffix = (int *)malloc(n * sizeof(int));
    make_badCharacter(badCharacter, p);
    make_goodSuffix(goodSuffix, p);

    MatchResult ret;
    ret.n_match = 0;
    ret.n_compare = 0;
    i = n - 1; // text location
    while (i < m) {
    int j = n - 1; // pattern location
    while (j >= 0 && (t[i] == p[j])) {
        ret.n_compare++;
        i--;
        j--;
        };  
        if (j < 0) {
        ret.n_match++;
        printf("A pattern match is found at location %d.\n", i+1);
        i += max(badCharacter[t[i + 1]], goodSuffix[j + 1]) + 1;
        }
        else{
        ret.n_compare++;
        i += max(badCharacter[t[i]], goodSuffix[j]);
        }
    }
    free(goodSuffix);
    return ret;
}



/*
 * main function
 */
int main(int argc, char **argv)
{
    int     ret;                            /* return code */
    double  t0;                             /* clock timer */

    /* argc < 3 means no filename is input from the command line. */
    if( argc < 3 ){
        printf("You forgot to enter trace file and keyword file name!\n");
        exit(1);
    }

    /* Read text from text file. */
    FILE *text_file     = fopen(argv[1], "r");      /* open the text file */
    char* text_str = (char *) malloc (MAX_TEXT_LEN);                    /* string for reading text file */
    size_t text_len = fread(text_str, sizeof(char), MAX_TEXT_LEN, text_file);   /* read file into string */
    if (text_len == 0)                              /* sanity check */
    {
        printf("Text is empty or cannot read text file!\n");
        exit(1);
    }

    printf("Text length = %lu characters\n", text_len);

    /* read pattern from pattern file. */
    FILE *pattern_file  = fopen(argv[2], "r");      /* open the pattern file */
    char pattern_str[MAX_PATTERN_LEN];              /* string for reading text file */
    size_t pattern_len = fread(pattern_str, sizeof(char), MAX_PATTERN_LEN, pattern_file);
                                                    /* read file into string */
    if (pattern_len == 0)                           /* sanity check */
    {
        printf("Pattern is empty or cannot read pattern file!\n");
        exit(1);
    }

    if (pattern_str[pattern_len-1] == '\n')         /* remove trailing newline character */
    {
        pattern_str[pattern_len-1] = '\0';
        pattern_len = strlen(pattern_str);
    }

    printf("Pattern is %s. Pattern length = %lu characters\n", pattern_str, pattern_len);

    t0 = clock();                       /* Record start time */

    /* Run naive string matching */
    MatchResult res = Boyer_Moore(text_str, pattern_str);

    /* print results */
    printf("Number of matches found = %lu\n", res.n_match);
    printf("Number of character comparison performed = %lu\n", res.n_compare);
    printf("duration: %f sec\n", (clock()-t0)/(double)CLOCKS_PER_SEC );

    return 0;
}
