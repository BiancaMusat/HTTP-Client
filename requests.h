#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request(char *host, int port, char *url, char *url_params, char cookies_vect[][300], int index, char *jwt);
char *compute_post_request(char *host, int port, char *url, char *data, char cookies_vect[][300], int index, char *jwt);
char *compute_delete_request(char *host, char *url);

#endif