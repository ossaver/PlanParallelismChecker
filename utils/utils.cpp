#include "utils.h"
#include <ctype.h>
#include <string.h>

/********************************************************/
/* Oscar Sapena Vercher - DSIC - UPV                    */
/* April 2022                                           */
/********************************************************/
/* Constants and utilities.								*/
/********************************************************/

bool SIGNIFICATIVE_LANDMARKS = false;

#ifdef DEBUG_TO_FILE_NOT_CONSOLE
ofstream* debugFile = nullptr;
#else
ostream* debugFile = nullptr;
#endif

// Compare two strings
bool compareStr(std::string s1, std::string s2) {
	unsigned int l = s1.size();
	if (l != s2.size())
		return false;
	for (unsigned int i = 0; i < l; i++)
		if (tolower(s1[i]) != s2[i])
			return false;
	return true;
}

#ifdef DEBUG_TO_FILE_NOT_CONSOLE
void createDebugFile()
{
	debugFile = new ofstream();
	debugFile->open("C:\\Laptop\\Research\\AIPlan4EU\\debug.txt");
}

void closeDebugFile()
{
	if (debugFile != nullptr) {
		debugFile->close();
		debugFile = nullptr;
	}
}
#else
void createDebugFile()
{
	debugFile = &cout;
}

void closeDebugFile()
{
	debugFile = nullptr;
}
#endif