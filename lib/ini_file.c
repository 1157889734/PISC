

#include "../include/include.h"


#define ETC_MAXLINE	255





/****************************** ETC file support ******************************/

static char* get_section_name (char *section_line)
{
    char* current;
    char* tail;
    char* name;

    if (!section_line)
        return NULL;

    current = section_line;

    while (*current == ' ' ||  *current == '\t') current++; 

    if (*current == ';' || *current == '#')
        return NULL;

    if (*current++ == '[')
        while (*current == ' ' ||  *current == '\t') current ++;
    else
        return NULL;

    name = tail = current;
    while (*tail != ']' && *tail != '\n' &&
          *tail != ';' && *tail != '#' && *tail != '\0')
          tail++;
    *tail = '\0';
    while (*tail == ' ' || *tail == '\t') {
        *tail = '\0';
        tail--; 
    }

    return name;
}

static int get_key_value (char *key_line, char **mykey, char **myvalue)
{
    char* current;
    char* tail;
    char* value;

    if (!key_line)
        return -1;

    current = key_line;

    while (*current == ' ' ||  *current == '\t') current++; 

    if (*current == ';' || *current == '#')
        return -1;

    if (*current == '[')
        return 1;

    if (*current == '\n' || *current == '\0')
        return -1;

    tail = current;
    while (*tail != '=' && *tail != '\n' &&
          *tail != ';' && *tail != '#' && *tail != '\0')
          tail++;

    value = tail + 1;
    if (*tail != '=')
        *value = '\0'; 

    *tail-- = '\0';
    while (*tail == ' ' || *tail == '\t') {
        *tail = '\0';
        tail--; 
    }
        
    tail = value;
    while (*tail != '\n' && *tail != '\0') tail++;
    *tail = '\0'; 

    if (mykey)
        *mykey = current;
    if (myvalue)
        *myvalue = value;

    return 0;
}


/* This function locate the specified section in the etc file. */
static int etc_LocateSection(FILE* fp, const char* pSection, FILE* bak_fp)
{
    char szBuff[ETC_MAXLINE + 1];
    char *name;

    while (1) {
        if (!fgets(szBuff, ETC_MAXLINE, fp)) {
            if (feof (fp))
                return -1;
            else
                return -1;
        }
        else if (bak_fp && fputs (szBuff, bak_fp) == EOF)
            return -1;
        
        name = get_section_name (szBuff);
        if (!name)
            continue;

        if (strcmp (name, pSection) == 0)
            return 0; 
    }

    return -1;
}

/* This function locate the specified key in the etc file. */
#if 0
static int etc_LocateKeyValue(FILE* fp, const char* pKey, 
                               int bCurSection, char* pValue, int iLen,
                               FILE* bak_fp, char* nextSection)
#else
int etc_LocateKeyValue(FILE* fp, const char* pKey, 
                               int bCurSection, char* pValue, int iLen,
                               FILE* bak_fp, char* nextSection)
#endif
{
    char szBuff[ETC_MAXLINE + 1 + 1];
    char* current;
    char* value;
    int ret;

    while (1) {
        int bufflen;

        if (!fgets(szBuff, ETC_MAXLINE, fp))
            return -1;
        bufflen = strlen (szBuff);
        if (szBuff [bufflen - 1] == '\n')szBuff [bufflen - 1] = '\0';
	if (szBuff [bufflen - 2] == '\r')szBuff [bufflen - 2] = '\0';
	
        ret = get_key_value (szBuff, &current, &value);
        if (ret < 0)
            continue;
        else if (ret > 0) {
            fseek (fp, -bufflen, SEEK_CUR);
            return -1;
        }
            
        if (strcmp (current, pKey) == 0) {
            if (pValue)
                strncpy (pValue, value, iLen);

            return 0; 
        }
        else if (bak_fp && *current != '\0')
            fprintf (bak_fp, "%s=%s\n", current, value);
    }

    return -1;
}



/* Function: GetValueFromEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey, char* pValue, int iLen);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 *     pValue:   The buffer will store the value of the key.
 *     iLen:     The max length of value string.
 * Return:
 *     int               meaning
 *     -1           The etc file not found. 
 *     -1        The section is not found. 
 *     ETC_EKYNOTFOUND        The Key is not found.
 *     0            OK.
 */
int GetValueFromEtcFile(const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue, int iLen)
{
    FILE* fp;
    char tempSection [ETC_MAXLINE + 2];

    if (!(fp = fopen(pEtcFile, "r")))
         return -1;

    if (pSection)
         if (etc_LocateSection (fp, pSection, NULL) != 0) {
             fclose (fp);
             return -1;
         }

    if (etc_LocateKeyValue (fp, pKey, pSection != NULL, 
                pValue, iLen, NULL, tempSection) != 0) {
         fclose (fp);
         return -1;
    }

    fclose (fp);
    return 0;
}



/* Function: GetIntValueFromEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 * Return:
 *     int                      meaning
 *     -1             The etc file not found. 
 *     -1          The section is not found. 
 *     ETC_EKYNOTFOUND              The Key is not found.
 *     0                       OK.
 */
 #if 0
int GetIntValueFromEtcFile(const char* pEtcFile, const char* pSection,
                               const char* pKey, int* value)
{
    int ret;
    char szBuff [51];

    ret = GetValueFromEtcFile (pEtcFile, pSection, pKey, szBuff, 50);
    if (ret < 0)
        return ret;

    *value = strtol (szBuff, NULL, 0);
    if ((*value == LONG_MIN || *value == LONG_MAX) && errno == ERANGE)
        return -1;

    return 0;
}
#endif
static int etc_CopyAndLocate (FILE* etc_fp, FILE* tmp_fp, 
                const char* pSection, const char* pKey, char* tempSection)
{
    if (pSection && etc_LocateSection (etc_fp, pSection, tmp_fp) != 0)
        return -1;

    if (etc_LocateKeyValue (etc_fp, pKey, pSection != NULL, 
                NULL, 0, tmp_fp, tempSection) != 0)
        return -1;

    return 0;
}

static int etc_FileCopy (FILE* sf, FILE* df)
{
    char line [ETC_MAXLINE + 1];
    
    while (fgets (line, ETC_MAXLINE + 1, sf) != NULL)
        if (fputs (line, df) == EOF) {
            return -1;
        }

    return 0;
}

/* Function: SetValueToEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey, char* pValue);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 *     pValue:   Value.
 * Return:
 *     int                      meaning
 *     -1         The etc file not found.
 *     -1        Create tmp file failure.
 *     0                   OK.
 */
int SetValueToEtcFile (const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue)
{
    FILE* etc_fp;
    FILE* tmp_fp;
    int rc;
    char tempSection [ETC_MAXLINE + 2];

#ifndef HAVE_TMPFILE
    char tmp_nam [256];

    sprintf (tmp_nam, "/tmp/mg-etc-tmp-%x", (int)time(NULL));
    if ((tmp_fp = fopen (tmp_nam, "w+")) == NULL)
        return -1;
#else
    if ((tmp_fp = tmpfile ()) == NULL)
        return -1;
#endif

    if (!(etc_fp = fopen (pEtcFile, "r+"))) {
        fclose (tmp_fp);
#ifndef HAVE_TMPFILE
        unlink (tmp_nam);
#endif
        if (!(etc_fp = fopen (pEtcFile, "w"))) {
            return -1;
        }
        fprintf (etc_fp, "[%s]\n", pSection);
        fprintf (etc_fp, "%s=%s\n", pKey, pValue);
        fclose (etc_fp);
        return 0;
    }

    switch (etc_CopyAndLocate (etc_fp, tmp_fp, pSection, pKey, tempSection))
    {
    case -1:
        fprintf (tmp_fp, "\n[%s]\n", pSection);
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        break;

    default:
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        break;
    }

    if ((rc = etc_FileCopy (etc_fp, tmp_fp)) != 0)
        goto error;
    
    // replace etc content with tmp file content
    // truncate etc content first
    fclose (etc_fp);
    if (!(etc_fp = fopen (pEtcFile, "w"))) {
        fclose (tmp_fp);
#ifndef HAVE_TMPFILE
        unlink (tmp_nam);
#endif
        return -1;
    }
    
    rewind (tmp_fp);
    rc = etc_FileCopy (tmp_fp, etc_fp);

error:
    fclose (etc_fp);
    fclose (tmp_fp);
#ifndef HAVE_TMPFILE
    unlink (tmp_nam);
#endif
    return rc;
}




