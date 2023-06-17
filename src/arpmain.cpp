/*
Ar.P. - Artificial Planetoids
Copyright (c) 2003-2004 Markus Liehmann

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*
	arpmain.cpp

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: Contains main-function

*/

#include <stdlib.h>
#include <stdio.h>
#include "ArPEngine.h"

int main(int argc, char *argv[])
{
	CArPEngine* engine;

	// reset error-file

	FILE* errorlog;
	errorlog = fopen("error.txt","w");
    fprintf(errorlog, "No errors");
	fclose(errorlog);

	// reset log-file

	FILE* log;
	log = fopen("log.txt","w");
    fprintf(log, "Log file\n\n");
	fclose(log);

	try
	{
		// create new Engine and enter the message-loop

		engine = new CArPEngine("arp.cfg");
		engine->EnterMessageLoop();
	}
	catch (char* sz)
	{
		FILE* errorlog;
		errorlog = fopen("error.txt","w");
        fprintf(errorlog, sz);
		fclose(errorlog);
        exit(1);
	}

	delete engine;
    return 0;
}