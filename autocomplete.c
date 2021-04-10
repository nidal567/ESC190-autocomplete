#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include "autocomplete.h"


//SORTS TERMS INTO LEXICOGRAPHIC ORDER FOR READ_IN_TERMS FUNCTION
int lexicographic_terms(const void *argument_1, const void *argument_2){
    struct term *first = (struct term *) argument_1;
    struct term *second = (struct term *) argument_2;

    return strcmp(first->term, second->term);
}

//TAKES IN A POINTER TO A POINTER TO STRUCT TERM, A POINTER TO AN INT, AND THE NAME OF A FILE THAT IS FORMATTED LIKE CITIES.TXT
//THE FUNCTION ALLOCATES MEMORY FOR ALL THE TERMS IN THE FILE AND STORES A POINTER TO THE BLOCK IN *TERMS. THE FUNCTION STORES THE NUMBER OF TERMS IN *PNTERMS. THE FUNCTION READS IN ALL THE TERMS FROM FILENAME
//AND PLACES THEM IN THE BLOCK POINTED TO BY TERMS, WHICH SHOULD BE SORTED IN LEXICOGRAPHIC ORDER

void read_in_terms(struct term **terms, int *pnterms, char *filename){
    int i = 0;
    int number_terms = 0;
    struct term *p_terms = NULL;

    char line[200];
    FILE *fp = fopen(filename, "r");
    
    fgets(line, sizeof(line), fp); 
    number_terms = atoi(line);
    *pnterms = number_terms;

    p_terms = malloc(number_terms * sizeof(struct term));
    *terms = p_terms;

    for(i = 0; i < number_terms; i++){
        char *str = line;
        char *p_term2 = p_terms[i].term;

        fgets(line, sizeof(line), fp); 
        
        while(*str && ! isdigit(*str) ){
            str = str + 1;
        }
        p_terms[i].weight = atof(str);

        while(*str && isdigit( *str ) ){
            str = str + 1;
        }
        while(*str && isblank(*str) ){
            str = str + 1;
        }
        while(*str && *str != '\n' && *str != '\r'){
            *p_term2 = *str;
            p_term2 = p_term2 + 1;
            str = str + 1;
        }
        *p_term2 = '\0';
    }

    qsort((void *)p_terms, (size_t) number_terms, sizeof(struct term), lexicographic_terms);
}

//Checks if it's starting, returns 0 or 1 so it's binary

int find_binary(struct term *terms, int nterms, char *substr){
    int low = 0;
    int high = nterms - 1;
    int average = 0;
    int substring_length = strlen(substr);

    while(high - low >= 2){
        average = (low + high) / 2;

        if(strncmp (terms[average].term, substr, substring_length) > 0){
            high = average - 1;
        }   
        else if(strncmp (terms[average].term, substr, substring_length) < 0){
            low = average + 1;
        } else{
            return average;
        }
    }

    if(strncmp (terms[low].term, substr, substring_length) == 0){
        return low;
    }
    else if(strncmp (terms[high].term, substr, substring_length) == 0){
        return high;
    }
    return -1;
}

//RETURNS THE INDEX IN TERMS OF THE FIRST TERM IN LEXICOGRAPHIC ORDERING THAT MATCHES THE STRING SUBSTR
//THIS FUNCTION MUST RUN IN 0(LOG(NTERMS)) TIME, WHERE NTERMS IS THE NUMBER OF TERMS IN TERMS

int lowest_match(struct term *terms, int nterms, char *substr){
    int substring_length = strlen(substr);

    if(substring_length == 0){
        return -1;
    }

    int i = find_binary(terms, nterms, substr);

    if(i >= 0){
        while(i >=0){
            if(strncmp(terms[i].term, substr, substring_length) == 0){
                --i;
            } else{
                return i + 1;
            }
        }
        return 0;
    } else{
        return -1;
    }
}

//SAME AS BEFORE EXCEPT FOR HIGHEST_MATCH

int highest_match(struct term *terms, int nterms, char *substr){
    int substring_length = strlen(substr);

    if(substring_length == 0){
        return -1;
    }
   
    int i = find_binary(terms, nterms, substr);

    if (i >= 0){
        while (i <= nterms - 1){
            if (strncmp(terms[i].term, substr, substring_length) == 0){
                i++;
            } else{
                return i - 1;
            }
        }
        return nterms - 1;
    } else{
        return -1;
    }
}

int lexicographic_weights(const void *argument_1, const void *argument_2){
    struct term *first = (struct term *) argument_1;
    struct term *second = (struct term *) argument_2;

    return second->weight - first->weight;
}

//TAKES TERMS, THE NUMBER OF TERMS NTERMS, AND THE QUERY STRING SUBSTR, AND PLACES THE ANSWERS IN ANSWER, WITH *N_ANSWER BEING THE NUMBER OF ANSWERS. tHE ANSWER SHOULD BE SORTED BY WEIGHT

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr){
    int low = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);
    
    *n_answer = (high - low) + 1;
    struct term *p_answer = NULL;

    p_answer = malloc(*n_answer *sizeof(struct term));
    *answer = p_answer;

    if (high < 0 || low < 0){
        *n_answer = 0;
        *answer = NULL;
        return;
    }

    memcpy(p_answer, &terms[low], *n_answer * sizeof(struct term));
    qsort((void *)p_answer, (size_t) *n_answer, sizeof(struct term), lexicographic_weights);
}