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

char *compute_get_request(char *host, int port, char *url, char *url_params,
                            char cookies_vect[][300], int cookies, char* jwt)
{   
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (url_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    }
    else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    if (port == 0) {
        sprintf(line, "Host: %s", host);
        compute_message(message, line);
    } else {
        sprintf(line, "Host: %s:%d", host, port);
        compute_message(message, line);
    }
    for (int i = 0; i < cookies; ++i) {
        char *token;
        token = strtok(cookies_vect[i], ";");
        sprintf(line, "%s;", token);
        compute_message(message, line);
    }
    if (jwt != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    sprintf(line, "");
    compute_message(message, line);

    return message;
}
char *compute_post_request(char *host, int port, char *url, char *data,
                            char cookies_vect[][300], int cookies, char* jwt) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    if (port == 0) {
        sprintf(line, "Host: %s", host);
        compute_message(message, line);
    } else {
        sprintf(line, "Host: %s:%d", host, port);
        compute_message(message, line);
    }
    if (port == 0) {
        sprintf(line, "Content-Type: application/x-www-form-urlencoded");
        compute_message(message, line);
    } else {
        sprintf(line, "Content-Type: application/json");
        compute_message(message, line);
    }
    for (int i = 0; i < cookies; ++i) {
        char *token;
        token = strtok(cookies_vect[i], ";");
        sprintf(line, "%s;", token);
        compute_message(message, line);
    }
    if (jwt != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    sprintf(line, "Content-Length: %ld", strlen(data));
    compute_message(message, line);
    sprintf(line, "");
    compute_message(message, line);
    sprintf(line, "%s", data);
    compute_message(message, line);

    return message;
}

char *compute_delete_request(char *host, char *url) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    sprintf(line, "Content-Type: application/x-www-form-urlencoded");
    compute_message(message, line);
    sprintf(line, "");
    compute_message(message, line);

    return message;
}