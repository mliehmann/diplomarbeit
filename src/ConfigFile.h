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
	ConfigFile.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CConfigFile class represents a config file (simple text file). The ParseLine
		function reads one line and extracts the token (first word in line) and the value
		(second word in line, has to be separated by either spaces or tabs). If ParseLine reads
		'#' it switches in comment mode and ignores the rest of the line. If the token and value
		have been extracted, all other chars in the line will also be ignored.
*/

#ifndef __MM_CONFIGFILE_H__
#define __MM_CONFIGFILE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class CConfigFile
{
private:
	char m_caToken[256];
	char m_caValue[256];
	char m_caFilename[256];
	FILE* m_pFile;

public:
	CConfigFile();
	CConfigFile(const char* filename);
	void SetFilename(const char* filename);

	int OpenConfig();
	void CloseConfig();
	int ParseLine();
	char* GetToken();
	char* GetValue();
	char* GetFilename();
};

#endif