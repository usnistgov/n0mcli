#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inifile.h"
#include "diag.h"
#include "config.h"
#include <libgen.h>

char proc_dir[MAXPATH] ;
static char inifile[MAXPATH] = "n0mcli.ini" ;
static 	char inipath[MAXPATH] ;

char seriodev[32] = "/dev/ttyS0" ;
int  baud;
char user[32] = "netzero" ;

char pw[128] = { 0xa6,0xa6,0xa6,0xa6, 0xa6,0xa6,0xa6,0xa6, 0xa6,0xa6,0xa6,0xa6,
	0xa6,0xa6,0xa6,0xa6, 0xa6,0xa6,0xa6,0xa6 } ;
int pw_size = 20 ;

char tablist[128];
int tables[128] ;
int ntables = 0 ;;

int verbose = 0 ;
int dosave = 0 ;
int doxml = 0 ;
int pwhex = 1 ;
int force_post = 1 ;

extern int lp_blks_to_read ;
extern int lp_price ;
extern int lp_channels ;
extern double lp_t15RatioF1 ;
extern double lp_t15RatioP1 ;
extern double lp_t12Multiplier ;
extern double lp_LPMultiplier ;
extern double lp_LPDivisor ;
extern double lp_LPValueOffset ;

char outdir[MAXPATH] = "./" ;
char xmlfn[MAXPATH] = "" ;

char post_url[MAXPATH] = "https://localhost" ;
char post_bearer[128] = "unknown" ;
long checkpeer_flag = 1 ;
long checkhostname_flag = 1 ;

char post_file[MAXPATH] ;

char retail_customer[16] = "9B6C7088" ;
char usage_point[16] = "5446AF3F" ;

char meterinfo_uuid[64] = "3FF8DFDE-C18C-433F-A10F-0B90E9FFEC8C" ;
char timeinfo_uuid[64] = "7F2A4CAA-DD8A-4D9E-92BE-B8F8286D8DA0" ;
char readinginfo_uuid[64] = "7F2A4CAA-DD8A-4D9E-92BE-B8F8286D8DA0" ;
char datainfo_uuid[64] = "7F2A4CAA-DD8A-4D9E-92BE-B8F8286D8DA0" ;

/*
 * look for the specified key in a section at given index.
 * If found, copies the key value to a specified buffer and returns
 * length of it. Otherwise returns 0
 */
static int retrieve_key_value(dbdesc_t *pdb, int s_idx, char *pkey, char *pbuf)
{
	int k_idx ;
	int rc = 0 ;

	k_idx = inidb_find_key(pdb, s_idx, pkey) ;
	if (k_idx >= 0)
	{
		rc = inidb_get_key_value(pdb, s_idx, k_idx, pbuf) ;
	}

	return rc ;
}

static int parse_baudrate(char *p)
{
	int n = 0 ;

	if (p) {
		n = atoi(p) ;
		switch (n) {
			case 1200:
			case 2400:
			case 4800:
			case 9600:
			case 19200:
			case 38400:
			case 57600:
			case 115200:
				break ;
			default:
				n = 0 ;
				break ;
		}
	}
	return n ;
}

static int parse_hexpw(char *pout, char *pin)
{
	int i ;
	unsigned v ;
	char *p ;
	int n ;
	int size ;

	n = strlen(pin) / 2 ;
	for (i =0, p = pin; i < n; i++, p += 2) {
		sscanf(p, "%2X", &v) ;
		pw[i] = (char) v ;
	}
	size = n ;

	return size ;
}

static int process_ini_file(char *path)
{
	int rc = 1 ;
	int s_idx ;
	char tmpbuf[MAXPATH] ;
	char *p ;
	int len, n ;

	dbdesc_t *pdb ;

	//VTRACE(".. processing %s\n", path) ;

	if ( (pdb = inidb_open(path)) != NULL)
	{
		p = "Settings" ;
		if ((s_idx = inidb_find_section(pdb, p)) >= 0)
		{
			if ((len = retrieve_key_value(pdb, s_idx, "serialport", tmpbuf)) > 0)
			{
				if ((n = access(tmpbuf, R_OK|W_OK)) != -1)
				{
					strncpy(seriodev, tmpbuf, sizeof(seriodev)-1) ;
					VTRACE("INI: seriodev=%s\n", seriodev) ;
				}
				else {
					DIAG_PRINT("Warning: %s doesn't exist or not accessible, using %s\n", tmpbuf, seriodev) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "baudrate", tmpbuf)) > 0)
			{
				n = parse_baudrate(tmpbuf) ;
				if (n != 0) {
					baud = n ;
					VTRACE("INI: baud=%d\n", baud) ;
				}
				else {
					DIAG_PRINT("Warning: %s -- bad baudrate, using %d\n", tmpbuf, baud) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "user", tmpbuf)) > 0)
			{
				strncpy(user, tmpbuf, sizeof(user)-1) ;
				VTRACE("INI: user=%s\n", user) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "pwhex", tmpbuf)) > 0)
			{
				n = tmpbuf[0] & 0xff ;
				if ((n == '0') || (n == 'n') || (n == 'N')) {
					pwhex = 0 ;
				}
				VTRACE("INI: pwhex=%d\n", pwhex) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "pwascii", tmpbuf)) > 0)
			{
				n = tmpbuf[0] & 0xff ;
				if ((n == '1') || (n == 'y') || (n == 'Y')) {
					pwhex = 0 ;
				}
				VTRACE("INI: pwhex=%d\n", pwhex) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "password", tmpbuf)) > 0)
			{
				if (pwhex) {
					pw_size = parse_hexpw(&pw[0], &tmpbuf[0]) ;
				}
				else {
					strncpy(pw, tmpbuf, sizeof(pw)-1) ;
					pw_size = strlen(pw) ;
				}
				VTRACE("INI: pwd=%s, size=%d\n", tmpbuf, pw_size) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "outdir", tmpbuf)) > 0)
			{
				if ((n = access(tmpbuf, R_OK|W_OK)) != -1)
				{
					strncpy(outdir, tmpbuf, sizeof(outdir)-1) ;
					VTRACE("INI: outdir=%s\n", outdir) ;
				}
				else {
					DIAG_PRINT("Warning: %s doesn't exist or not accessible, using %s\n", tmpbuf, outdir) ;
//					return rc ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "savedata", tmpbuf)) > 0)
			{
				n = tmpbuf[0] & 0xff ;
				if ((n == '1') || (n == 'y') || (n == 'Y')) {
					dosave = 1 ;
				}
				VTRACE("INI: dosave=%d\n", dosave) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "savexml", tmpbuf)) > 0)
			{
				n = tmpbuf[0] & 0xff ;
				if ((n == '1') || (n == 'y') || (n == 'Y')) {
					doxml = 1 ;
				}
				VTRACE("INI: doxml=%d\n", doxml) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "verbose", tmpbuf)) > 0)
			{
				/***
				n = tmpbuf[0] & 0xff ;
				if ((n == '1') || (n == 'y') || (n == 'Y')) {
					verbose = 1 ;
				}
				***/
				verbose = atoi(tmpbuf) ;
				VTRACE("INI: verbose=%d\n", verbose) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "xtratables", tmpbuf)) > 0)
			{
				strncpy(tablist, tmpbuf, sizeof(tablist)-1) ;
				VTRACE("INI: tablist=%s\n", tablist) ;
			}
		}

		p = "LoadProfile" ;
		if ((s_idx = inidb_find_section(pdb, p)) >= 0)
		{
			if ((len = retrieve_key_value(pdb, s_idx, "blockstoread", tmpbuf)) > 0)
			{
				lp_blks_to_read = atoi(tmpbuf) ;
				VTRACE("INI: lp_blks_to_read=%d\n", lp_blks_to_read) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "price", tmpbuf)) > 0)
			{
				lp_price = atoi(tmpbuf) ;
				VTRACE("INI: lp_price=%d\n", lp_price) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "channels", tmpbuf)) > 0)
			{
				lp_channels = atoi(tmpbuf) ;
				VTRACE("INI: lp_channels=%d\n", lp_channels) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "t15RatioF1", tmpbuf)) > 0)
			{
				lp_t15RatioF1 = atof(tmpbuf) ;
				VTRACE("INI: lp_t15RatioF1=%f\n", lp_t15RatioF1) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "t15RatioP1", tmpbuf)) > 0)
			{
				lp_t15RatioP1 = atof(tmpbuf) ;
				VTRACE("INI: lp_t15RatioP1=%f\n", lp_t15RatioP1) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "t12Multiplier", tmpbuf)) > 0)
			{
				lp_t12Multiplier = atof(tmpbuf) ;
				VTRACE("INI: lp_t12Multiplier=%f\n", lp_t12Multiplier) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "LPMultiplier", tmpbuf)) > 0)
			{
				lp_LPMultiplier = atof(tmpbuf) ;
				VTRACE("INI: lp_LPMultiplier=%f\n", lp_LPMultiplier) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "LPDivisor", tmpbuf)) > 0)
			{
				lp_LPDivisor = atof(tmpbuf) ;
				VTRACE("INI: lp_LPDivisor=%f\n", lp_LPDivisor) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "LPValueOffset", tmpbuf)) > 0)
			{
				lp_LPValueOffset = atof(tmpbuf) ;
				VTRACE("INI: lp_LPValueOffset=%f\n", lp_LPValueOffset) ;
			}
		}

		p = "REST" ;
		if ((s_idx = inidb_find_section(pdb, p)) >= 0)
		{
			if ((len = retrieve_key_value(pdb, s_idx, "post_url", tmpbuf)) > 0)
			{
				if (len >= sizeof(post_url)) {
					len = sizeof(post_url)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(post_url, tmpbuf) ;
					VTRACE("INI: post_url=%s\n", post_url) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "post_bearer", tmpbuf)) > 0)
			{
				if (len >= sizeof(post_bearer)) {
					len = sizeof(post_bearer)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(post_bearer, tmpbuf) ;
					VTRACE("INI: post_bearer=%s\n", post_bearer) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "post_retailcustomer", tmpbuf)) > 0)
			{
				if (len >= sizeof(retail_customer)) {
					len = sizeof(retail_customer)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(retail_customer, tmpbuf) ;
					VTRACE("INI: post_retailcustomer=%s\n", retail_customer) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "post_usagepoint", tmpbuf)) > 0)
			{
				if (len >= sizeof(usage_point)) {
					len = sizeof(usage_point)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(usage_point, tmpbuf) ;
					VTRACE("INI: post_usagepoint=%s\n", usage_point) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "meterinfo_uuid", tmpbuf)) > 0)
			{
				if (len >= sizeof(meterinfo_uuid)) {
					len = sizeof(meterinfo_uuid)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(meterinfo_uuid, tmpbuf) ;
					VTRACE("INI: meterinfo_uuid=%s\n", meterinfo_uuid) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "timeinfo_uuid", tmpbuf)) > 0)
			{
				if (len >= sizeof(timeinfo_uuid)) {
					len = sizeof(timeinfo_uuid)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(timeinfo_uuid, tmpbuf) ;
					VTRACE("INI: timeinfo_uuid=%s\n", timeinfo_uuid) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "readinginfo_uuid", tmpbuf)) > 0)
			{
				if (len >= sizeof(readinginfo_uuid)) {
					len = sizeof(readinginfo_uuid)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(readinginfo_uuid, tmpbuf) ;
					VTRACE("INI: readinginfo_uuid=%s\n", readinginfo_uuid) ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "datainfo_uuid", tmpbuf)) > 0)
			{
				if (len >= sizeof(datainfo_uuid)) {
					len = sizeof(datainfo_uuid)-1 ;
					tmpbuf[len] = '\0' ;
				}
				if (len>0) {
					strcpy(datainfo_uuid, tmpbuf) ;
					VTRACE("INI: datainfo_uuid=%s\n", datainfo_uuid) ;
				}
			}

			if ((len = retrieve_key_value(pdb, s_idx, "ssl_verify_peer", tmpbuf)) > 0)
			{
				n = tmpbuf[0] & 0xff ;
				if ((n == '1') || (n == 'y') || (n == 'Y')) {
					checkpeer_flag = 1 ;
				}
				else {
					checkpeer_flag = 0 ;
				}
				VTRACE("INI: checkpeer_flag=%d\n", (int) checkpeer_flag) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "ssl_verify_hostname", tmpbuf)) > 0)
			{
				n = tmpbuf[0] & 0xff ;
				if ((n == '1') || (n == 'y') || (n == 'Y')) {
					checkhostname_flag = 1 ;
				}
				else {
					checkhostname_flag = 0 ;
				}
				VTRACE("INI: checkhostname_flag=%d\n", (int) checkhostname_flag) ;
			}
		}

		inidb_close(pdb) ;
		pdb = NULL ;
	}
	else {
		DIAG_PRINT("%s -- not found or bad\n", path) ;
		rc = 0 ;
	}

	return rc ;
}
static void usage(char *p)
{
	TRACE(" Usage: \n");
	TRACE("  %s [<flags>] [parameters]\n", p) ;
	TRACE("  flags :=\n") ;
	TRACE("    -x -- produce the .XML file named 'lpibYYYYMMDDHHNNSS.xml' and exit\n") ;
	TRACE("    -X -- produce the .XML file named 'lpibYYYYMMDDHHNNSS.xml' and POST it via https://\n") ;
	TRACE("    -s -- save the above tables in corresponding 'tablexxx.dat' files\n") ;
	TRACE("    -f<xml-file> -- POST the existing XML file instead of reading the LP\n") ;
	TRACE("    -h -- help\n") ;
	TRACE("    -v <level> -- verbose\n") ;
	TRACE("  parameters :=\n") ;
	TRACE("    -c <path> -- alternative configuration file. default is n0mcli.ini\n") ;
	TRACE("    -n <count> -- read <count> meter LP blocks. Default is 10. 0 means 'all available'. Negative means 'n last blocks'\n") ;
	TRACE("    -d <outdir> -- specifies the directory for output files. Current directory is used by default.\n") ;
	TRACE("    -l <devname> -- use posix devname for communication. Default is /dev/ttyACM0\n") ;
	TRACE("    -b <baud> -- set specific COM port baudrate. Default is 9600\n") ;
	TRACE("    -u <string> -- set user id. Default is ''\n") ;
	TRACE("    -p <string> -- set C12.19 password. Default is '00000000000000000000'\n") ;
	TRACE("    -t <list> -- read tables listed in comma-separated <list>, (tab0, tab61-64 always read)\n");
}

int cnf_init(int argc, char **argv)
{
	int rc = 1 ;

    int tflag, bflag, lflag, cflag, sflag, xflag, Xflag ;
    int dflag, pflag, uflag, nflag, vflag ;
    int fflag ;

    char *ptables, *pbaud, *pline, *pcfg ;
    char *pdir, *ppwd, *puser, *pnumblk, *pverb ;
    char *pxmlfile ;
    int opt ;
    int n ;


    if (readlink ("/proc/self/exe", proc_dir, sizeof(proc_dir)-1) != -1)
    {
    	strcpy(inipath, proc_dir) ;
    	rc = strlen(inipath) ;
    	strncat(inipath, ".ini", MAXPATH - rc - 1) ;

        dirname(proc_dir);
        strcat(proc_dir, "/");
    }
    else
    {
    	strncpy(inipath, argv[0], MAXPATH-1) ;
    	strcpy(proc_dir, inipath) ;

    	rc = strlen(inipath) ;
    	strncat(inipath, ".ini", MAXPATH - rc - 1) ;

        dirname(proc_dir);
        strcat(proc_dir, "/");
    }

    sflag = xflag = vflag = Xflag = 0;
    tflag = bflag = lflag = cflag = 0;
    dflag = pflag = uflag = nflag = 0 ;
    fflag = 0 ;
    ptables = pbaud = pline = pcfg = NULL ;
    pdir = ppwd = puser = pnumblk = pverb = NULL ;
    pxmlfile = NULL ;

    while ((opt = getopt(argc, argv, "b:c:d:f:l:n:p:st:u:v:xX?")) != -1) {
        switch (opt) {
        	case 'b':
        		bflag = 1 ;
        		pbaud = optarg ;
        		break;
        	case 'c':
        		cflag = 1 ;
        		pcfg = optarg ;
        		strncpy(inifile, pcfg, sizeof(inifile)-1) ;
        	    if (inifile[0] == '/') {
        	    	strcpy(inipath, inifile) ;
        	    }
        	    else {
        	    	strcpy(inipath,proc_dir) ;
        	    	strcat(inipath, inifile) ;
        	    }
        	    break;
        	case 'd':
        		dflag = 1 ;
        		pdir = optarg ;
        		break;
        	case 'f':
        		fflag = 1 ;
        		pxmlfile = optarg ;
        		break;
        	case 'l':
        		lflag = 1 ;
        		pline = optarg ;
        		break;
        	case 'n':
        		nflag = 1 ;
        		pnumblk = optarg ;
        		break;
        	case 'p':
        		pflag = 1 ;
        		ppwd = optarg ;
        		break;
        	case 's':
        		sflag = 1 ;
        		break;
        	case 't':
        		tflag = 1 ;
        		ptables = optarg ;
        		break;
        	case 'u':
        		uflag = 1 ;
        		puser = optarg ;
        		break;
        	case 'v':
        		vflag = 1 ;
        		pverb = optarg ;
        		break;
        	case 'x':
        		xflag = 1 ;
        		break;
        	case 'X':
        		Xflag = 1 ;
        		break;
        	case '?':
        	default:
        		usage(argv[0]) ;
        		rc = 0 ;
        }
    }

    if (rc == 0) {
    	return rc ;
    }

    rc = process_ini_file(inipath) ;

    if (bflag) {
    	int tmp = parse_baudrate(pbaud) ;
    	if (tmp != 0)
    		baud = tmp ;
    }
    if (dflag)
    	strncpy(outdir, pdir, sizeof(outdir)-1) ;

    if (lflag)
    	strncpy(seriodev, pline, sizeof(seriodev)-1) ;

    if (nflag) {
    	lp_blks_to_read = atoi(pnumblk) ;
    }

    if (pflag) {
    	if (pwhex) {
    		pw_size = parse_hexpw(&pw[0], ppwd) ;
    	}
    	else {
    		strncpy(pw, ppwd, sizeof(pw)-1) ;
    		pw_size = strlen(pw) ;
    	}
    }

    if (sflag)
    	dosave = 1 ;

    if (tflag)
    	strncpy(tablist, ptables, sizeof(tablist)-1) ;

    if (uflag)
    	strncpy(user, puser, sizeof(user)-1) ;

    if (vflag) {
    	verbose = atoi(pverb) ;
    }

    if (xflag) {
    	doxml = 1 ;
    	force_post = 0 ;
    }

    if (Xflag) {
    	doxml = 1 ;
    	force_post = 1 ;
    }

    if (fflag) {
    	if (access(pxmlfile, R_OK) != 0) {
    		TRACE("*** %s doesn't exist or is not readable\n", pxmlfile) ;
    		post_file[0] = '\0' ;
    		rc = 0 ;
    	}
    	else {
    		strcpy(post_file, pxmlfile) ;
    	}
    }
    else {
    	post_file[0] = '\0' ;
    }

    n = strlen(outdir) ;
    if (n > 0) {
    	if (outdir[n-1] != '/') {
    		strcat(outdir, "/") ;
    	}
    }

////   printf("name argument = %s\n", argv[optind]);

	return rc ;
}
