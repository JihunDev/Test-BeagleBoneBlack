#include <stdio.h>

int main( )
{
	FILE *pFile = NULL;
	pFile = fopen( "df.CSV", "r" );
	
  if( pFile != NULL )
	{
		char strTemp[255];
		char *pStr;
    char *ptr;
    int count=0;
		while( !feof( pFile ) )
		{
			pStr = fgets( strTemp, sizeof(strTemp), pFile );
			printf( "pStr => %s\n", pStr );
      
      ptr = strtok(pStr, ","); 
      while(ptr != NULL)
      {
        if (count == 0) {
					count++;
          break;
				}
        printf( "\t%s", ptr); 
        ptr = strtok( NULL, ",");
      }
		}

		fclose( pFile );
	} else {
		printf("Csv File Road err");
	}

	return 0;
}