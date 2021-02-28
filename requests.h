#ifndef _REQUESTS_
#define _REQUESTS_
#include <string>
using namespace std;
// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							char *cookies);
char *compute_get_request_jwt(char *host, char *url, char *query_params,
							char *jwt);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, string body_data, char* cookie);

char *compute_post_request_jwt(char *host, char *url, char* content_type, string body_data, char *cookie, char *jwt);

char *compute_delete_request_jwt(char *host, char *url, char *query_params,
							char *jwt);

#endif
