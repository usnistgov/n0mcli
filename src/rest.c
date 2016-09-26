#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

CURLcode do_restful_post(char *url, char *bearer, char *pbody, int bodylen, long checkpeer, long checkhostname )
{
	CURL *curl;
	CURLcode res = CURLE_FAILED_INIT ;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();

	if(curl) {
	    struct curl_slist *slist = NULL;
	    char tmp[128] ;


	    sprintf(tmp, "Authorization: Bearer %s", bearer) ;
	    slist = curl_slist_append(slist, "Accept: */*");
	    slist = curl_slist_append(slist, "Content-Type: Application/xml");
	    slist = curl_slist_append(slist, tmp);

	    res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

	    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	    curl_easy_setopt(curl, CURLOPT_USERAGENT,  "N0M  libcurl");

	    curl_easy_setopt(curl, CURLOPT_URL, url);
	    curl_easy_setopt(curl, CURLOPT_POST, 1);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, checkpeer);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, checkhostname);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pbody);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return (res) ;
}

char * get_xmlbuf(char *pfname, int *plen)
{
	char *p = NULL ;
	FILE *f = fopen(pfname, "rb") ;
	long fsize = 0 ;

	if (f) {
		fseek(f, 0, SEEK_END) ;
		fsize = ftell(f) ;
		fseek(f, 0, SEEK_SET);
		if (fsize > 0) {
			if ((p = malloc(fsize+1)) != NULL) {
				if (fread(p, fsize, 1, f) > 0) {
					p[fsize] = '\0' ;
				}
				else {
					free(p) ;
					p = NULL ;
				}
			}
		}
		fclose(f) ;
		if (plen) {
			*plen = (fsize > 0) ? fsize : 0 ;
		}
	}

	return p ;
}

void free_xmlbuf(char *p)
{
	if (p)
		free(p) ;
}
