#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gplaut04.h"

extern UserData clientData;
//////////////////////////////////////////////////////////////////////////////////
//
int 
readFM(const char* dFileName, const int size)
//
//////////////////////////////////////////////////////////////////////////////////
{
    int state = 0;

    char buffer[256];
    int branch, point, prevpoint, myid;

    float fl1, fl2;
    FILE * inFile;
    inFile = fopen(dFileName, "r");

    if (!inFile)
    {
        printf("Cannot open the diagnostic file. \n");
        state = 1;
        return state;
    }

    clientData.multipliers = new float[myBifNode.totalNumPoints][6][2];
    clientData.eigenvalues = new bool[myBifNode.totalNumPoints];

    char * next;

    long icounter = 1;
    int rowi = 0;
    prevpoint = -1;

    while ( (next = fgets(buffer, sizeof(buffer), inFile)) != NULL
        && icounter < size)
    {
        clientData.eigenvalues[icounter] =
            strstr(buffer, "Eigenvalue") != NULL;
        if(strstr(buffer, "Multiplier") != NULL ||
           clientData.eigenvalues[icounter])
        {
            rowi = 0;
            do
            {
                int ret;
                if(clientData.eigenvalues[icounter])
                    ret = sscanf(buffer, "%d%d Eigenvalue %d: %e%e",
                           &branch, &point, &myid, &fl1, &fl2);
                else
                    ret = sscanf(buffer, "%d%d Multiplier %d%e%e",
                           &branch, &point, &myid, &fl1, &fl2);
                if(ret != 5) { 
                    if (rowi == 0)
                        --icounter;
                    break;
                }
                if (point == prevpoint && rowi == 0)
                    --icounter;
                prevpoint = point;
                clientData.multipliers[icounter][rowi][0] = fl1;
                clientData.multipliers[icounter][rowi][1] = fl2;
                rowi++;
                clientData.numFM = (rowi>clientData.numFM ) ? rowi : clientData.numFM;
            }                 //end inner while
            while( fgets(buffer, sizeof(buffer), inFile) &&
                     (strstr(buffer, "Multiplier") != NULL ||
                      strstr(buffer, "Eigenvalue") != NULL) );
            ++icounter;
        }
    }                         // end outter while
    fclose(inFile);

    return state;
}


