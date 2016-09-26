/* implements a read-only functionality for initialization
 * files. Initialization file is a text file split by sections --
 * special lines of form ['section-name'].
 * Each section can contain any numbers of lines containing key/value pairs:
 *
 *   Key = Value
 *
 * spaces and tabs surrounding the '=' are ignored.
 * lines beginning with '#' or ';' are ignored
 */

#include <sys/io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include "inifile.h"
#include "diag.h"

static const char defsection[] = DEFAULT_SECTION_NAME ;
static const char emptystring[] = DEFAULT_EMPTY_STRING ;

/*----------------------------------------------------------------------------\
//	Returns pointer to the next non-whitespace character.
//---------------------------------------------------------------------------*/

static char *skipws(char *p)
{
	while ((p != NULL) && ((*p == ' ') || (*p == '\t') || (*p == '\r') /*|| (*p == '\n')*/))
	{
		++p ;
	}

	return (p) ;
}


/*----------------------------------------------------------------------------\
//	Returns pointer to the next line that follow LF character.
//---------------------------------------------------------------------------*/

static char *skipline(char *p)
{
	while ((p != NULL) && (*p != '\0') && (*p != '\n'))
	{
		++p ;
	}

	if (*p == '\n')
		++p ;

	return (p) ;
}

/*----------------------------------------------------------------------------\
//	Returns length of section name, if ']' found, -1 if section name is empty,
//  0 otherwise
//---------------------------------------------------------------------------*/

static int parse_section_name(char *p)
{
	int len = -1 ;                       /* start with supposition that section name is absent */

	while ((*p != ']') && (len != 0))    /* while NOT ']' AND length is NOT zero DO ... */
	{
		switch (*p)
		{
			case '\n':                   /* if LF */
			case '\0':                   /* or EOL (end of line) */
				len = 0 ;                /* section name is malformed; set zero length */
				break ;

			case '\r':                   /* if CR */
				++p ;                    /* just skip it; the pointer will be set to LF */
				break ;

			default:
				if (len == -1)           /* if some other character */
					len = 0 ;            /* start counting chars */
				++len ;                  /* continue counting */
				++p ;
				break ;
		}
	}

	return len ;
}


/*----------------------------------------------------------------------------\
//	Returns length of right trimmed value substring.
//  Value substring is considered to be terminated with either LF or CR or EOL;
//  TABs and SPACEs are ignored.
//---------------------------------------------------------------------------*/

static int parse_key_value(char *p)
{
	char *q ;

	for (q = p; (*q != '\n') && (*q != '\r') && (*q != '\0'); ++q)
		;

	if (q > p)
	{
		while (((*q) <= ' ') && (q > p))      /* TABs and SPACEs are ignored */
			--q ;

		if (*q > ' ')                         /* SPACEs are not counted */
			++q ;
	}

	return (q - p) ;
}

/*----------------------------------------------------------------------------\
//	Returns length of right trimmed name substring or 0 if '=' is not
//  encountered. Name substring is considered to be terminated with either LF
//  or CR or EOL or '=" character. TABs and SPACEs are ignored.
//---------------------------------------------------------------------------*/

static int parse_key_name(char *p)
{
	char *q  ;
	int len ;

	for (q = p; (*q != '=') && (*q != '\n') && (*q != '\r') && (*q != '\0'); ++q)
		;

	if (*q == '=')
	{
		if (q > p)
		{
			--q ;

			while (((*q) <= ' ') && (q > p))
				--q ;

			if (*q > ' ')
				++q ;

			len = q - p ;
		}
		else
			len = 0 ;
	}
	else
		len = 0 ;

	return len ;
}





/*---------------------------------------------------------------------------\
//	Parses ini-file and sets references to section names, key names,
//  and key values.
//		pdb - pointer to the database descriptor
//		pfb - pointer to the file text buffer
//--------------------------------------------------------------------------*/

static void parse_ini_file(dbdesc_t *pdb, char *pfb)
{
	section_row_t *psects ;                 /* pointer to the beginning of section descriptor table */
	key_row_t *pkeys ;                   /* pointer to the beginning of key descr table */

	char *psecname = (char *) defsection ;   /* default section name */
	char *pname = NULL ;
	char *pval = NULL ;
	char *p ;                                /* pointer to file contents buffer */
	int  nsects = 0 ;                        /* running number of sections in the file */
	int	 nkeys = 0 ;                         /* running number of keys in the file */
	int  len = 0 ;

	section_row_t *pcursect = NULL ;
	int i ;


	// at the beginning pdb->nsections and pdb->nkeys contain estimated maximum No of sections and keys

	psects = pdb->psections ;         /* pointer to the table of section descriptors */
	pkeys = pdb->pkeys ;

	p =pfb ;
	/* p initially points to the beginning of ini-file contents in a buffer */

	do {

		p = skipws(p) ;                       /* skip SPACEs, TABs, and CR */
		switch(*p)                            /* see what we get */
		{
			case '\n':                        /* if LF, skip it */
			case '\r':                        /* if CR, skip it */
				++p ;
				break ;

			case '[':                         /* if initial section delimiter */
				++p ;                             /* skip it */
				len = parse_section_name(p) ;     /* get the length of section name */

				if (len > 0)	                  /* if section name found */
				{
					psecname = p ;
					*(psecname + len) = '\0' ;
					++len ;
					p += len ;	                  /* skip past the parsed section name */

					for (i=0; i < nsects; i++)
					{
						if (strcasecmp(psects->psname,psecname) == 0)
							break ;
					}
					if (i >= nsects)
					{
						(psects + nsects)->psname = psecname ;
						pcursect = (psects + nsects) ;
						++nsects ;
					}
					else
						pcursect = (psects + i) ;
				}
				p = skipline(p) ;
				break ;

			case ';':                         /* comment line */
			case '#':                         /* comment line */
				p = skipline(p) ;
				break ;

			default:                          /* it a a key/value string then */
				len = parse_key_name(p) ;      /* look for a key name */

				if (len > 0)	                  /* key name found */
				{
					pname = p ;
					p += len ;	                  /* skip past the parsed string name */
					if (*p != '=')                /* if NOT equality sign... */
					{
						p = skipws(p) ;           /* skip SPACEs, TABs, CRs */
					}
					++p ;                         /* skip equality sign */
					p = skipws(p) ;               /* then skip SPACEs, TABs, CRs */

					*(pname + len) = '\0' ;
					(pkeys + nkeys)->pkname = pname ;

					len = parse_key_value(p) ; /* look for a value name */

					if (len > 0)	              /* value name found */
					{
						pval = p ;
						*(pval + len) = '\0' ;
						++len ;
						p += len ;

						(pkeys + nkeys)->pkvalue = pval ;
					}
					else	                      /* value name not found */
					{
						(pkeys + nkeys)->pkvalue = (char *) emptystring ;
					}

          /* apply_default_section_if_needed */
					if (nsects == 0)
					{
						(psects + nsects)->psname = (char *) (&defsection[0]) ;
						pcursect = (psects + nsects) ;
						++nsects ;
					}

					// point to the container section
					(pkeys + nkeys)->psect = pcursect ;

					// if this is a first key for a section -- store its address
					if (pcursect->ppairs == NULL)
						pcursect->ppairs = (pkeys + nkeys) ;

					(pkeys + nkeys)->psect->npairs += 1 ;

					++nkeys ;

					if (*p == '\n')
						p = skipline(p) ;
				}
				else	                          /* key name not found */
				{
					p = skipline(p) ;
				}
				break ;
		}

	} while ((*p != '\0') && ((p - pfb) < pdb->bufsize)) ;

	pdb->nkeys = nkeys ;
	pdb->nsections = nsects ;
}


/*----------------------------------------------------------------------------\
//	Parses text in the file buffer and fills in the database structures.
//	Returns 1 in case of success, 0 otherwise.
//		pdb - pointer to the database descriptor
//		pfb - pointer to the file text buffer
//---------------------------------------------------------------------------*/

static int create_db_structure(dbdesc_t *pdb, char *pfb)
{
	section_row_t *psects ;          /* pointer to the begining of section descriptor table */
	int nsects = 0 ;                  /* number of sections in DDB-file */
	int length_of_section_table = 0 ; /* length of table for section descriptors */

	key_row_t *ppair ;             /* pointer to the k/v pair structure table */
	int npairs = 0 ;                   /* current number of k/v pairs */
	int length_of_key_table = 0 ;

	int rc = 0 ;


	// estimate number of sections
	nsects = (pdb->bufsize) / strlen("[x]\n") ;

	// allocate memory for the table of section descriptors
	length_of_section_table = sizeof(section_row_t) * nsects ;

	if ((psects = (section_row_t *) malloc(length_of_section_table)) == NULL)
	{
		return rc ;                 /* error allocating memory */
	}

	memset(psects, 0, length_of_section_table) ;      // clear the allocated memory

	// estimate number of keys defined in a file
	npairs = (pdb->bufsize) / strlen("a=b\n") ;

	length_of_key_table = npairs * sizeof(key_row_t) ;

	if ((ppair = (key_row_t *) malloc(length_of_key_table)) == NULL)
	{
		free(psects) ;
		return rc ;                 /* error allocating memory */
	}

	memset(ppair, 0, length_of_key_table) ;  /* clear the allocated memory */

	pdb->psections = psects ;       /* pointer to section descriptor table */
	pdb->nsections = nsects ;       /* set number of sections  */

	pdb->nkeys = npairs ;
	pdb->pkeys = ppair ;

	/* set pointers to section names, key names and key values */
	parse_ini_file(pdb, pfb) ;          /* do final file parsing */

	if (pdb->nkeys != 0)
	{
		pdb->pkeys = realloc(pdb->pkeys, pdb->nkeys * sizeof(key_row_t)) ;

		pdb->psections = realloc(pdb->psections, sizeof(section_row_t) * pdb->nsections) ;
		rc = 1 ;
	}
	else
	{
		free(pdb->pkeys) ; pdb->pkeys=NULL ;
		free(pdb->psections) ; pdb->psections=NULL ; pdb->nsections=0 ;
	}

	return rc ;
}


/*----------------------------------------------------------------------------\
//	Check if a file is available and opens it, if so
//  Returns fd on success, -1 otherwise.
//---------------------------------------------------------------------------*/

static int ini_file_open(char *path)
{
	int fd ;
	int rc = -1 ;


	if ((fd = open(path, O_RDONLY /*| O_BINARY*/)) != -1)
	{
		rc = fd ;
	}
	return rc ;
}


/*----------------------------------------------------------------------------\
//	Checks if a ini-file is valid
//  Returns file size in case of success, 0 otherwise.
//---------------------------------------------------------------------------*/

static int get_file_size(int fd)
{
	int size = 0;

	if (fd >= 0)
	{
		size = (int) lseek(fd, 0 ,SEEK_END ) ;
		lseek(fd, 0 ,SEEK_SET ) ;
	}
	return size ;
}


/*----------------------------------------------------------------------------\
//	Creates a database for the given initialization file.
//  Needs the file handler (fileID) and file statistics structure (stat).
//  returns pointer to dbdesc_t structure on success, NULL otherwise.
//---------------------------------------------------------------------------*/

dbdesc_t * inidb_open(char *path)
{
	dbdesc_t *pdb ;                // pointer to the database descriptor
	char *pfb;                       // pointer to INI-file buffer
	long buflen ;                    // buffer size
	long nbytes = 0 ;
	int rc ;
	int fd ;

	if ((fd = ini_file_open(path)) < 0)
	{
		//perror(path) ;
		return NULL ;
	}

	// allocate memory for database descriptor
	if ((pdb = (dbdesc_t *) malloc(sizeof(dbdesc_t))) != NULL)
	{
		memset(pdb, 0, sizeof(dbdesc_t)) ;   // clear the allocated memory

		// allocate memory for file contents buffer
        // add 1 to file size for a trailing zero byte, just in case
		buflen = get_file_size(fd) ;

		if ((pfb = (char *) malloc(buflen + 1)) != NULL)
		{
			memset(pfb, 0, buflen + 1) ;        // clear the allocated memory
			pdb->pbuf = pfb ;
			pdb->bufsize = buflen ;

			// read the contents of ini-file into the buffer
			if ((nbytes = read(fd, pfb, buflen)) == buflen)
			{
				close(fd) ;		/* don't need the file open anymore .. */
				pdb->fd = -1 ;

				if ((rc = create_db_structure(pdb, pfb)) != 0)   /* if success */
				{
					return pdb ;
				}
				else
					TRACE("can't create db structure\n") ;
			}

			close(fd) ;
			free(pfb) ;
		}
		else
		{
			TRACE("can't allocate inifile buffer\n") ;

			close(fd) ;
		}

		free(pdb) ;
	}
	else
		close(fd) ;

	return NULL ;
}


//----------------------------------------------------------------------------
//	Delete the existing database by deallocating all memory blocks.
//----------------------------------------------------------------------------

void inidb_close(dbdesc_t *pdb)
{
	dbdesc_t *p ;

	/* if the file was opened, close it (12/04/2009 - am) */
	if ((pdb->fd) > 0)
	{
		close(pdb->fd) ;
		pdb->fd = -1 ;
	}

	p = pdb ;                       /* get the database handle */
	if (p)                               /* if database was created */
	{
		if (p->pkeys)
		{
			free(p->pkeys) ;         /* deallocate the key descr table */
		}

		if (p->psections)
		{
			free(p->psections) ;         /* deallocate the section descr table */
		}

		if (p->pbuf)
			free(p->pbuf) ;
		free(p) ;                        /* deallocate the database descriptor */
	}
}


//----------------------------------------------------------------------------
//	Deallocate all memory blocks except for database descriptor.
//----------------------------------------------------------------------------

void inidb_clear(dbdesc_t *pdb)
{
	dbdesc_t *p ;

	p = pdb ;

	if (p)
	{
		if (p->pkeys != NULL)
		{
			free(p->pkeys) ;
		}
		if (p->psections != NULL)
		{
			free(p->psections) ;         /* deallocate the section descr table */
		}
	}
}



//----------------------------------------------------------------------------
// Retrieves the number of sections in DB
//----------------------------------------------------------------------------

int inidb_get_number_of_sections(dbdesc_t *pdb)
{
	if (pdb)
		return (pdb->nsections) ;             // number of sections

	return 0 ;
}



//----------------------------------------------------------------------------
// Retrieves the length of the name of a section with given index
//----------------------------------------------------------------------------

int inidb_get_section_name_length(dbdesc_t *pdb, int sidx)
{
	dbdesc_t *p ;                     /* pointer to the database descriptor */
	section_row_t *psect ;             /* pointer to section table */

	if ((p = pdb) != NULL)
	{
		psect = (p->psections) + sidx ;
		return (strlen(psect->psname)) ;    /* returns length of section name */
	}

	return 0 ;
}


//----------------------------------------------------------------------------
// Retrieves the the name of a section with given index
//----------------------------------------------------------------------------

int inidb_get_section_name( dbdesc_t *pdb, int idx, char *psn)
{
	dbdesc_t *p ;                     /* pointer to the database descriptor */
	section_row_t *psect ;             /* pointer to section table */
    char *psrc, *pdst ;
	int len = 0 ;

	if ((p = pdb) != NULL)
	{
		psect = (p->psections) + idx ;
    	psrc = psect->psname ;
    	pdst = psn ;
		len = 0 ;

    	while ((*pdst++ = *psrc++) != '\0')
    	{
 			++len ;
    	}
	}
	return len ;
}


//----------------------------------------------------------------------------
// Retrieves the number of key/value pairs for the section given by index
//----------------------------------------------------------------------------

int inidb_get_number_of_keys(dbdesc_t *pdb, int idx)
{
	dbdesc_t *p ;                 // pointer to the database descriptor
	section_row_t *psect ;         // pointer to section table

	if ((p = pdb) != NULL)
	{
		psect = (p->psections) + idx ;
		return (psect->npairs) ;      // return number of key/value pairs
	}

	return 0 ;
}


//----------------------------------------------------------------------------
// Retrieves the length of key value string for the given key value row index
//----------------------------------------------------------------------------

int inidb_get_key_value_length(dbdesc_t *pdb, int sidx, int kidx)
{
	dbdesc_t *p ;                 /* pointer to the database descriptor */
	section_row_t *psect ;         /* pointer to section table */
	key_row_t *pstrs ;          /* pointer to string table for a section */
	int len = 0 ;
	int i, j ;

	if ((p = pdb) != NULL)
	{
		psect = (p->psections) + sidx ;
		if (kidx < psect->npairs)
		{
			pstrs = NULL ;
			j = 0 ;
			for (i = psect->ppairs - p->pkeys ; i < p->nkeys; i++)
			{
				if ((p->pkeys + i)->psect == psect)
				{
					if (j == kidx)
					{
						pstrs = p->pkeys + i ;
						break ;
					}
					++j ;
				}
			}

			if (pstrs != NULL)
				len = strlen(pstrs->pkvalue) ;
		}
	}
	return len ;                    /* return value name length  */
}


//----------------------------------------------------------------------------
// Retrieves the key value string for the given key row index
//----------------------------------------------------------------------------

int inidb_get_key_value(dbdesc_t *pdb, int sidx, int kidx, char *pval)
{
	dbdesc_t *p ;                 /* pointer to the database descriptor */
	section_row_t *psect ;         /* pointer to section table */
	key_row_t *pstrs ;          /* pointer to string table for a section */
	int len = 0;
	int i, j ;

	if ((p = pdb) != NULL)
	{
		psect = (p->psections) + sidx ;
		if (kidx < psect->npairs)
		{
			pstrs = NULL ;
			j = 0 ;
			for (i = psect->ppairs - p->pkeys ; i < p->nkeys; i++)
			{
				if ((p->pkeys + i)->psect == psect)
				{
					if (j == kidx)
					{
						pstrs = p->pkeys + i ;
						break ;
					}
					++j ;
				}
			}

			if (pstrs != NULL)
			{
				len = strlen(pstrs->pkvalue) ;
				strcpy(pval, pstrs->pkvalue) ;
			}
		}
	}
	return len ;

}

//----------------------------------------------------------------------------
// Retrieves length of key name for the given section index and key
// row index
//----------------------------------------------------------------------------

int inidb_get_key_name_length(dbdesc_t *pdb, int sidx, int kidx)
{
	dbdesc_t *p ;                 /* pointer to the database descriptor */
	section_row_t *psect ;         /* pointer to section table */
	key_row_t *pstrs ;          /* pointer to string table for a section */
	int len = 0 ;
	int i, j ;

	if ((p = pdb) != NULL)
	{
		psect = (p->psections) + sidx ;
		if (kidx < psect->npairs)
		{
			pstrs = NULL ;
			j = 0 ;
			for (i = psect->ppairs - p->pkeys ; i < p->nkeys; i++)
			{
				if ((p->pkeys + i)->psect == psect)
				{
					if (j == kidx)
					{
						pstrs = p->pkeys + i ;
						break ;
					}
					++j ;
				}
			}

			if (pstrs != NULL)
				len = strlen(pstrs->pkname) ;
		}
	}

	return len ;                    /* return value name length */
}


//----------------------------------------------------------------------------
// Retrieves key name for the given section index and key/value row index
//----------------------------------------------------------------------------

int inidb_get_key_name(dbdesc_t *pdb, int sidx, int kidx, char *pKeyName)
{
	dbdesc_t *p ;                 /* pointer to the database descriptor */
	section_row_t *psect ;         /* pointer to section table */
	key_row_t *pstrs ;          /* pointer to string table for a section */
	int len = 0;
	int i, j ;

	if ((p = pdb) != NULL)
	{
		psect = (p->psections) + sidx ;
		if (kidx < psect->npairs)
		{
			pstrs = NULL ;
			j = 0 ;
			for (i = psect->ppairs - p->pkeys ; i < p->nkeys; i++)
			{
				if ((p->pkeys + i)->psect == psect)
				{
					if (j == kidx)
					{
						pstrs = p->pkeys + i ;
						break ;
					}
					++j ;
				}
			}

			if (pstrs != NULL)
			{
				len = strlen(pstrs->pkname) ;
				strcpy(pKeyName, pstrs->pkname) ;
			}
		}
	}
	return len ;
}

int inidb_find_section(dbdesc_t *pdb, char *pname)
{
	int i ;
	int rc = -1 ;
	dbdesc_t *p ;

	if ((p = pdb) != NULL)
	{
		for (i = 0; i < p->nsections; i++)
		{
			if ((strcasecmp(pname, ((pdb->psections)+i)->psname)) == 0)
			{
				rc = i ;
				break ;
			}
		}
	}

	return rc ;
}

int inidb_find_key(dbdesc_t *pdb, int sidx, char *pname)
{
	int i, j ;
	int rc = -1 ;
	dbdesc_t *p ;
	section_row_t *psec ;

	if ((p = pdb) != NULL)
	{
		if ((p->nsections > 0) && (sidx >= 0) && (sidx < p->nsections))
		{
			psec = (pdb->psections)+sidx ;

			j = 0 ;
			for (i = psec->ppairs - p->pkeys; i < (p->nkeys) ; i++)
			{
				if (psec == (p->pkeys + i)->psect)
				{
					if ((strcasecmp(pname, ((p->pkeys)+i)->pkname) ) == 0)
					{
						rc = j ;
						break ;
					}
					++j ;
				}
			}
		}
	}

	return rc ;
}


