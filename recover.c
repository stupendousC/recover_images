// EVERYTHING WORKS 100%  :-D
#include <stdio.h>
#include <stdlib.h>

int BLOCK = 512;

int checkJPGheader(unsigned char* bufferPtr, FILE * inptr)
// if JPG header detected, return 1/T, otherwise return 0/F
// jpeg signature: byte1 = 0xff, byte2 = 0xd8, byte3 = 0xff, byte 4 = 0xf?  <- the ? here can be 0 to f
{
    if (bufferPtr[0] == 0xff && bufferPtr[1] == 0xd8 && bufferPtr[2] == 0xff && (bufferPtr[3] & 0xf0) == 0xe0)  //jPG header
    {
        //printf("JPG header found!\n\n");
        //printf ("bufferPtr[0] is 0x%x\n", bufferPtr[0]);
        //printf ("bufferPtr[1] is 0x%x\n", bufferPtr[1]);
        //printf ("bufferPtr[2] is 0x%x\n", bufferPtr[2]);
        //printf ("bufferPtr[3] is 0x%x\n", bufferPtr[3]);
        return 1;
    }
    return 0;
}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Error! Looking for ./recover RAW_FILE_NAME.raw\n");
        return 1;
    }

    char * infile = argv[1];
    FILE *inptr = fopen(infile, "r");

    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    //printf("READY TO READ inptr %s", argv[1]);

    //Set up buffer storage
    unsigned char * bufferPtr = malloc(BLOCK);     // unsigned char b/c 0xff won't fit into a signed char!!! got thsi from stackoverflow.com, dont' really understand it either
    if (bufferPtr == NULL)
    {
        printf ("no memory space!\n");
        return 1;
    }

    // open output file                  //sprintf(stringptr, string format)
    int jpgImageNum = 0;
    char jpgFilename[8];
    sprintf(jpgFilename, "%03i.jpg", jpgImageNum);            //%03i means an integer w/ up to 3 leading zeros
    FILE *outptr = fopen(jpgFilename, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", jpgFilename);
        return 3;
    }

    int veryFirstJPG = 0;
    int reachedEOF = 0;
    while (reachedEOF == 0)
    {
        int cursorBeginning = ftell(inptr);
        fread(bufferPtr, BLOCK, 1, inptr);
        int cursorEnd = ftell(inptr);

        if (cursorEnd < cursorBeginning + 512)
        {
            //printf("END OF FILE!!!\n");
            reachedEOF = 1;
        }

        if (checkJPGheader(bufferPtr, inptr) == 1)      //jpg found & cursor returned to beginning of jpg
        {
            if (veryFirstJPG == 0)
            {
                fwrite(bufferPtr, BLOCK, 1, outptr);
                //printf("writing 1st line...\n");
                veryFirstJPG = 1;
            }
            else
            {
                //printf("START NEXT FILE!\n");
                fclose(outptr);
                jpgImageNum += 1;
                sprintf(jpgFilename, "%03i.jpg", jpgImageNum);            //%03i means an integer w/ up to 3 leading zeros
                outptr = fopen(jpgFilename, "w");
                fwrite(bufferPtr, BLOCK, 1, outptr);
                //printf("writing 1st line...\n");
            }
        }
        else if (veryFirstJPG != 0)
        {
            if (reachedEOF == 0)
            {
                //printf("writing\n");
                fwrite(bufferPtr, BLOCK, 1, outptr);
            }
            else
            {
                printf("throw away this incomplete chunk of code\n");
            }
        }
        else
        {
            //printf("No JPG found yet\n");
        }

    }

    // close infile & outfile
    fclose(inptr);
    fclose(outptr);

    //free memory leaks
    free(bufferPtr);

    return 0;
}


