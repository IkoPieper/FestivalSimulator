#ifndef CONFIGh
    #define CONFIGh

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

struct configstruct {
	char *key;       	// Schluesselwort
	char *value;     	// Wert
	struct configstruct *next;	// Zeiger
};

typedef struct configstruct configentry;

//int KEYSIZE = 128;
//int VALUESIZE = 128;
//int LINESIZE = 512;

#ifndef KEYSIZE 
#define KEYSIZE	128 
#endif
#ifndef VALUESIZE
#define VALUESIZE 128 
#endif
#ifndef LINESIZE 
#define LINESIZE 512 
#endif

configentry *conf_load_data(configentry *data, const char *filename);
void conf_write_data(configentry *data, const char *filename);
void conf_print_data(configentry *data);
void conf_free_data(configentry *data);
char *conf_get_string(configentry *data, char *key);
int conf_get_int(configentry *data, char *key);
double conf_get_double(configentry *data, char *key);
int conf_set_string(configentry *data, char *key, char *value);
int conf_set_int(configentry *data, char *key, int valueInt);
int conf_set_double(configentry *data, char *key, double valueDouble);

#endif
