#include "own_config.h"

configentry *conf_load_data(const char *filename) {
	configentry *data = NULL;
	configentry *entry;
	configentry *entry_tmp;
	char line [LINESIZE];
	int32_t i, j, lineSize, is_string;
	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL)
		return NULL;
	while (fgets(line, LINESIZE, file) != NULL) { // line-weise auslesen bis dateiende
		lineSize = strlen(line);
		if (lineSize > 1 && line[0] != '#') { // wenn in line was steht
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
			
			is_string = 0;
			while (line[i] != '#' && j < VALUESIZE && i < lineSize-1) { // value auslesen
				
				if (line[i] == '"') {
					if (is_string == 0) {
						is_string = 1; // anfuehrungszeichen starten
					} else {
						is_string = 0; // anfuehrungszeichen beenden
					}
				}
					
				if (is_string == 1 || line[i] != ' ') // leerzeichen streichen wenn kein string
					value[j++] = line[i];
				i++;
			} 
			value[j] = '\0';
			// eintrag mit key und value erzeugen und hinten an liste anfuegen
			
			// get previous entry:
			entry_tmp = entry; 
			
			// create new entry:
			entry = (configentry *) malloc(sizeof(configentry));
			entry->key = key;
			entry->value = value;
			entry->next = NULL;
			
			
			if (data == NULL) {
				data = entry;            // first place in list
			} else {
				entry_tmp->next = entry; // last place in list
			}
			
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

void conf_free_data(configentry *data) {
	configentry *data_tmp;
	while (data != NULL) {
		free(data->key);
		free(data->value);
		data_tmp = data;
		data = data->next;
		free(data_tmp);
	}
}

char *conf_get_string(configentry *data, char *key) {
	configentry *entry = data;
	while (entry != NULL && !strstr(key, entry->key)) {
		entry = entry->next;
	}
	if (entry != NULL) {
		return(entry->value);
	} else {
		return(NULL);
	}
}


int32_t conf_get_int(configentry *data, char *key) {
	char *str = conf_get_string(data, key);
	if (str != NULL) {
		int32_t ret;
		sscanf(str, "%SCNd32", &ret);
		return(ret);
		//return(atoi(str));
	} else {
		return(-666);
	}
}

double conf_get_double(configentry *data, char *key) {
	char *str = conf_get_string(data, key);
	if (str != NULL) {
		return(atof(str));
	} else {
		return(-666.0);
	}
}

bool conf_set_string(configentry *data, char *key, char *value) {
	configentry *entry = data;
	while (entry != NULL && !strstr(key, entry->key)) {
		entry = entry->next;
	}
	if (entry != NULL) {
		entry->value = value;
		return(false);
	} else {
		return(true);
	}
}

bool conf_set_int(configentry *data, char *key, int32_t valueInt) {
	char value [LINESIZE];
	sprintf(value, "%d", valueInt);
	return(conf_set_string(data, key, value));
}

bool conf_set_double(configentry *data, char *key, double valueDouble) {
	char value [LINESIZE];
	sprintf(value, "%f", valueDouble);
	return(conf_set_string(data, key, value));
}

/*
int main () {
	configentry *data;
	data = NULL;
	data = conf_load_data(data, "configtest");
	int32_t zahl = conf_get_int(data, "ljasflka");
	printf("%d\n", zahl);
	conf_set_double(data, "iko", 8.345e-3);
	conf_write_data(data, "configtest_out");


	return 0;
}*/
