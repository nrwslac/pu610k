#include <epicsExport.h>
#include <epicsTypes.h>
#include <aSubRecord.h>
#include <registryFunction.h>
#include <string.h>

long PU610KErrorInit(struct aSubRecord *psub)
{
    psub->dpvt = 0;
    return 0;
}

long PU610KErrorParse(struct aSubRecord *psub)
{
    char *name     = ((char *) (psub->a));
    epicsInt32 *id  = ((epicsInt32 *) (psub->vala));
    char *out      = ((char *) (psub->valb));
    if (!strncmp(name, "OK,", 3)) {
        strcpy(out, "OK");
        *id = 0;
    } else {
        strcpy(out, name+8);
        *id = atoi(name);
    }
    return 0;
}

long PU610KTimeInit(struct aSubRecord *psub)
{
    psub->dpvt = 0;
    return 0;
}

long PU610KTimeParse(struct aSubRecord *psub)
{
    int sec, min, hrs;
    epicsInt32 time  = *((epicsInt32 *) (psub->a));
    char *name     = ((char *) (psub->vala));
    sec = time % 60;
    time = (time - sec) / 60;
    min = time % 60;
    hrs = (time - min) / 60;
    sprintf(name, "%d:%02d:%02d", hrs, min, sec);
    return 0;
}


epicsRegisterFunction(PU610KErrorInit);
epicsRegisterFunction(PU610KErrorParse);
epicsRegisterFunction(PU610KTimeInit);
epicsRegisterFunction(PU610KTimeParse);

