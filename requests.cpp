#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"


char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookie)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0 , LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    memset(line, 0, LINELEN);
    sprintf(line, "Cookie: %s", cookie);
    compute_message(message, line);

   /* if (cookies != NULL) {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       char append[100];
       for (size_t i = 0; i < cookies_count; i++) {
           sprintf(append, "%s;", cookies[i]);
           strcat(line, append);
       }
       compute_message(message, line);
    }
    // Step 4: add final new line
    */
    free(line);
    compute_message(message, "");
    return message;
}

char *compute_get_request_jwt(char *host, char *url, char *query_params,
                            char *jwt)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0 , LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    memset(line, 0, LINELEN);
    sprintf(line, "Authorization: Bearer %s", jwt);
    compute_message(message, line);

   /* if (cookies != NULL) {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       char append[100];
       for (size_t i = 0; i < cookies_count; i++) {
           sprintf(append, "%s;", cookies[i]);
           strcat(line, append);
       }
       compute_message(message, line);
    }
    // Step 4: add final new line
    */
    free(line);
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, string body_data, char *cookie)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host

    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    memset(line, 0, LINELEN);
    sprintf(line, "Content-type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", body_data.size());
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Cookie: %s", cookie);
    compute_message(message,line);


    memset(line, 0, LINELEN);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    char* c = const_cast <char*> (body_data.c_str()); //TODO sa verific daca genereaza memory leak
    strcpy(line, c);
    compute_message(message, line);

    free(line);
    return message;
}

char *compute_post_request_jwt(char *host, char *url, char* content_type, string body_data, char *cookie, char *jwt)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host

    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    memset(line, 0, LINELEN);
    sprintf(line, "Content-type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", body_data.size());
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Cookie: %s", cookie);
    compute_message(message,line);

    memset(line, 0, LINELEN);
    sprintf(line, "Authorization: Bearer %s", jwt);
    compute_message(message, line);


    memset(line, 0, LINELEN);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    char* c = const_cast <char*> (body_data.c_str()); //TODO sa verific daca genereaza memory leak
    strcpy(line, c);
    compute_message(message, line);

    free(line);
    return message;
}

char *compute_delete_request_jwt(char *host, char *url, char *query_params,
                            char *jwt)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0 , LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    memset(line, 0, LINELEN);
    sprintf(line, "Authorization: Bearer %s", jwt);
    compute_message(message, line);

   /* if (cookies != NULL) {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       char append[100];
       for (size_t i = 0; i < cookies_count; i++) {
           sprintf(append, "%s;", cookies[i]);
           strcat(line, append);
       }
       compute_message(message, line);
    }
    // Step 4: add final new line
    */
    free(line);
    compute_message(message, "");
    return message;
}