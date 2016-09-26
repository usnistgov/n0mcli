#ifndef _REST_H_
#define _REST_H_

#include <curl/curl.h>

extern char post_url[] ;
extern char post_bearer[] ;
extern long checkpeer_flag ;
extern long checkhostname_flag ;

extern CURLcode do_restful_post(char *url, char *bearer, char *pbody, int bodylen, long checkpeer, long checkhostname ) ;

extern char * get_xmlbuf(char *pfname, int *plen) ;
extern void free_xmlbuf(char *p) ;

#endif
