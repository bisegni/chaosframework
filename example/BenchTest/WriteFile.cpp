#include <stdio.h>
#include <string.h>

#include "WriteFile.h"

WriteFile::WriteFile()
{
	f_y=fopen("y.txt","w");
	f_u=fopen("u.txt","w");
	f_d=fopen("d.txt","w");
	f_delay=fopen("delay.txt","w");
	
}


WriteFile::WriteFile(char *name_system)
{	
	
  char str1[40];
  char str2[40];
  char str3[40];
  char str4[40];
  strcpy (str1,name_system);
  strcat (str1,"_y.txt");
  strcpy (str2,name_system);
  strcat (str2,"_u.txt");
  strcpy (str3,name_system);
  strcat (str3,"_d.txt");
  strcpy (str4,name_system);
  strcat (str4,"_delay.txt");
   
  f_y=fopen(str1,"w");
  f_u=fopen(str2,"w");
  f_d=fopen(str3,"w");
  f_delay=fopen(str4,"w");	
}

WriteFile::~WriteFile()
{
	fclose(f_y);
	fclose(f_u);
	fclose(f_d);
	fclose(f_delay);

}


void WriteFile::write_y(double y[])
{
	
	fprintf(f_y,"%4.8f\t %4.8f\n",y[0],y[1]);	
    
}

void WriteFile::write_u(double u[])
{
	
	fprintf(f_u,"%4.8f\t %4.8f\n",u[0],u[1]);	
    
}

void WriteFile::write_d(double d[])
{
	
	fprintf(f_d,"%4.8f\t %4.8f\n",d[0],d[1]);	
    
}

void WriteFile::write_delay(int64_t delay)
{
	
	fprintf(f_delay,"%d\n",delay);	
    
}	





