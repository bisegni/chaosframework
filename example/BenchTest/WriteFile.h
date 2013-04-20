#ifndef ChaosFramework_writefile
#define ChaosFramework_writefile
#include <stdint.h> 
#include <stdio.h>

class WriteFile {
	
	FILE *f_y, *f_u, *f_d, *f_delay;
	
	public:
	WriteFile();
	WriteFile(char *name_system);
	~WriteFile();
	void write_y(double []);
	void write_u(double []);
	void write_d(double []);
	void write_delay(int64_t delay);
};

#endif

