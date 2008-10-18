/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of IO.IN Research.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"

const char *charset = "0123456789abcdef";
int         charsetlen = 0;

void GenerateRandom(CrissCross::Data::DArray<char *> &data)
{
	char *string = NULL;

	charsetlen = strlen(charset);

	CrissCross::System::SeedRandom();
	data.setSize(DATASET_SIZE);
	for (int i = 0; i < DATASET_SIZE; i++) {
		string = new char[ENTRY_LENGTH];
		data.insert(string);

		string[ENTRY_LENGTH - 1] = 0;
		for (int j = 0; j < ENTRY_LENGTH - 1; j++) {
			string[j] = charset[CrissCross::System::RandomNumber() % charsetlen];
		}

		/* printf ( "%s\n", string ); */
		string = NULL;
	}
}
