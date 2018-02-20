#include "load_config.h"

void load_config(object_t* obj) {

	configentry *data;
	data = NULL;
	data = conf_load_data(data, "config.txt");

	if (data == NULL) {
		printf("Warning: Unable to load config file. Use Default Settings.");
		free(data);
		return;
	}
	// Ball.Speed = conf_get_double(data, "BallSpeed");
	

	free(data);
}
