#include "own_config.h"

configentry *conf_load_data(configentry *data, const char *filename) {
	configentry *entry;
	configentry *newEntry;
	char line [LINESIZE];
	int i, j, lineSize;	
	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL)
		return NULL;
	while (fgets(line, LINESIZE, file) != NULL) { // line-weise auslesen bis dateiende
		lineSize = strlen(line);
		if (lineSize > 1) { // wenn in line was steht
			char *key = (char *) malloc(KEYSIZE * sizeof(char));
			char *value = (char *) malloc(VALUESIZE * sizeof(char));
			i = 0;
			j = 0;
			while (line[i] != '=' && j < KEYSIZE) { // key auslesen
				if (line[i] != ' ') // leerzeichen streichen
					key[j++] = line[i];
				i++;
			}
			key[j] = '\0';
			i++;
			j = 0;
			while (line[i] != ';' && j < VALUESIZE && i < lineSize-1) { // value auslesen
				if (line[i] != ' ') // leerzeichen streichen
					value[j++] = line[i];
				i++;
			} 
			value[j] = '\0';
			// eintrag mit key und value erzeugen und hinten an liste anfuegen
			newEntry = (configentry *) malloc (sizeof(configentry));
			newEntry->key = key;
			newEntry->value = value;
			newEntry->next = data;
			data = newEntry;
		}
	}
	fclose(file);
	return(data);
}

void conf_write_data(configentry *data, const char *filename) {
	FILE *filestream;
	filestream = fopen(filename, "w");
	while (data != NULL) {
		fprintf(filestream, "%s = %s;\n", data->key, data->value);
		data = data->next;
	}
	fclose(filestream);
}

void conf_print_data(configentry *data) {
	while (data != NULL) {
		printf("key: %s, value: %s \n", data->key, data->value);
		data = data->next;
	}
}

const char *conf_get_string(configentry *data, const char *key) {
	configentry *entry = data;
	while (entry != NULL && !strstr(key, entry->key)) {
		entry = entry->next;
	}
	return (entry->value);
}


int conf_get_int(configentry *data, const char *key) {
	return (atoi(conf_get_string(data, key)));
}

int conf_get_double(configentry *data, const char *key) {
	return (atof(conf_get_string(data, key)));
}

int conf_set_string(configentry *data, const char *key, const char *value) {
	configentry *entry = data;
	while (entry != NULL && !strstr(key, entry->key)) {
		entry = entry->next;
	}
	if (entry != NULL) {
		entry->value = value;
		return 0;
	} else {
		return -1;
	}
}

int conf_set_int(configentry *data, const char *key, int valueInt) {
	char value [LINESIZE];
	sprintf(value,"%d",valueInt);
	return conf_set_string(data, key, value);
}

int conf_set_double(configentry *data, const char *key, double valueDouble) {
	char value [LINESIZE];
	sprintf(value,"%f",valueDouble);
	return conf_set_string(data, key, value);
}

/*
int main () {
	configentry *data;
	data = NULL;
	data = conf_load_data(data, "configtest");
	int zahl = conf_get_int(data, "ljasflka");
	printf("%d\n", zahl);
	conf_set_double(data, "iko", 8.345e-3);
	conf_write_data(data, "configtest_out");


	return 0;
}*/
