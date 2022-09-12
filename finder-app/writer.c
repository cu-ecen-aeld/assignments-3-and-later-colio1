/*******************************************************************************
* INCLUDES
*******************************************************************************/
#include <stdio.h>
#include <syslog.h>

/*******************************************************************************
* DEFINITIONS
*******************************************************************************/
#define TRUE               1
#define FALSE              0
#define ERROR              1
#define OK                 0

/*******************************************************************************
* DATA TYPES
*******************************************************************************/

/*******************************************************************************
* GLOBALS
*******************************************************************************/

/*******************************************************************************
* PROTOTYPES
*******************************************************************************/

/*******************************************************************************
Function: main
Description: Main program starts here. This function will open a file with the
filename specified by the user and write the string specified by the user.
*******************************************************************************/
int main(int argc, char *argv[])
{
	FILE* userFP; // pointer userFP to FILE

	//Open SYSLOG
	// openlog("[COURSE:ECEA 5305][ASSIGNMENT:2]", LOG_NDELAY, LOG_USER);
	openlog(NULL, LOG_NDELAY, LOG_USER);

	if( argc != 3 )
	{
		//Write to SYSLOG
		syslog(LOG_ERR, "Error: Incorrect number of arguments supplied.\n");
		return 1;
	}

	// Creates a file with file access as write-plus mode
	userFP = fopen(argv[1], "w+");
	// Write to SYSLOG
	syslog(LOG_DEBUG, "Writing %s to %s\n", argv[2], argv[1]);
	// Write to file
	// fprintf(userFP, argv[2]);
	fprintf(userFP, "%s", argv[2]);
	// closes the file pointed by userFP
	fclose(userFP);

	return 0;
}