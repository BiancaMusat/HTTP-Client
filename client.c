#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "json-c/json.h" /* json parsing */
#include "helpers.h"
#include "requests.h"

char jwl[10000];  // authentification token

/*
    Function that solves the first task.
    It returns the response from the server that is going to
be used in the next task.
*/
char* task1() {
	char *msg;  // message to send
    char *response;  // response from site
    int sockfd;
	sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    msg = compute_get_request(IP_SERVER, 0, "/task1/start", NULL, NULL, 0, NULL);
    send_to_server(sockfd, msg);
    printf("%s\n", msg);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);
    return response;
}
/*
    Function that solves the second task.
    @param  response - the response from the previous task
    It returns the response from the server that is going to
be used in the next task.
*/
char* task2(char *response) {
	char *msg;  // message to send
	char *token;  // used to split the initial response
    int sockfd;
    char data[10000];  // used to compose the message
    int i = 0;
    // structures nedeed when parsing the json message
    struct json_object *parser;
    struct json_object *enunt;
    struct json_object *url;
    struct json_object *method;
    struct json_object *type;
    struct json_object *user_data;
    struct json_object *parser_data;
    struct json_object *user;
    struct json_object *password;

    // token will contain only the json data
    token = strtok(response,"\n\n");
    while (token!= NULL) {
        if (i == 9) {
            break;
        }
        token = strtok(NULL, "\n\n");
        ++i;
    }
    // parsing the json data
    parser = json_tokener_parse(token);
    json_object_object_get_ex(parser, "enunt", &enunt);
    json_object_object_get_ex(parser, "url", &url);
    json_object_object_get_ex(parser, "method", &method);
    json_object_object_get_ex(parser, "type", &type);
    json_object_object_get_ex(parser, "data", &user_data);  
    parser_data = json_tokener_parse(json_object_get_string(user_data));
    json_object_object_get_ex(parser_data, "username", &user);
    json_object_object_get_ex(parser_data, "password", &password);

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    strcat(data, "user=");
    strcat(data, json_object_get_string(user));
    strcat(data, "pass=");
    strcat(data, json_object_get_string(password));
    // sending a POST request with the credentials in order to log in on the site
    msg = compute_post_request(IP_SERVER, 0, (char*)json_object_get_string(url), data, NULL, 0, NULL);
    // send the response to server
    send_to_server(sockfd, msg);
    printf("%s\n", msg);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);
    return response;
}
/*
    Function that solves the third & forth tasks.
    @param  response - the response from the previous task
    It returns the response from the server that is going to
be used in the next task.
*/
char* task3and4(char *response) {
	char *msg;  // message to send
	char *token;
	char *cookies;  // token for getting the cookies string task 3
    char *cookies2;  // token for getting the cookies string task 4
    int sockfd;
    char data[10000];  // used to compose the message
    char response_aux[1000];  // copy of the response task 3
    char response_aux2[1000];  // copy of the response task 4
    char cookie_vect[10][300];  // vector of cookies
    int i = 0, j = 0;
    // structures nedeed when parsing the json message
	struct json_object *parser;
	struct json_object *enunt;
    struct json_object *url;
    struct json_object *method;
    struct json_object *type;
    struct json_object *user_data;
    struct json_object *parser_data;
    struct json_object *token1;
    struct json_object *query;
    struct json_object *parser_query;
    struct json_object *id;
    struct json_object *parser2;
    struct json_object *enunt2;
    struct json_object *url2;
    struct json_object *method2;

//  Taks 3
    // adding the cookies to the vector of cookies
    strcpy(response_aux, response);
    cookies = strtok(response_aux, "\n");
    while(cookies) {
        char aux_cookies[1000];
        strcpy(aux_cookies, cookies);
        char *aux = strstr(aux_cookies, "Cookie: ");

        if (aux != NULL) {
            strcpy(cookie_vect[j], aux);
            ++j;
        }
        cookies = strtok(NULL, "\n");
    }

    // parsing the response so that token will contain only the json message
    i = 0;
    token = strtok(response,"\n\n");
    while (token!= NULL) {
        if (i == 9) {
            break;
        }
        token = strtok(NULL, "\n\n");
        ++i;
    }
    // parsing the json message
    parser = json_tokener_parse(token);
    json_object_object_get_ex(parser, "enunt", &enunt);
    json_object_object_get_ex(parser, "url", &url);
    json_object_object_get_ex(parser, "method", &method);
    json_object_object_get_ex(parser, "type", &type);
    json_object_object_get_ex(parser, "data", &user_data);

    parser_data = json_tokener_parse(json_object_get_string(user_data));
    json_object_object_get_ex(parser_data, "token", &token1);
    json_object_object_get_ex(parser_data, "queryParams", &query);

    parser_query = json_tokener_parse(json_object_get_string(query));
    json_object_object_get_ex(parser_query, "id", &id);
    // data will contain the 2 responses and the id
    strcpy(data, "raspuns1=omul&raspuns2=numele&id=");
    strcat(data, json_object_get_string(id));
    // jwl will contain the authentification token
    strcpy (jwl, (char*)json_object_get_string(token1));
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    // send a GET request containing the responses, cookies and the authentification token
    msg = compute_get_request(IP_SERVER, PORT_SERVER, (char*)json_object_get_string(url), data, cookie_vect, j, jwl);
    send_to_server(sockfd, msg);
    printf("%s\n", msg);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);

//  Task 4
    // adding the cookies to the vector of cookies
    j = 0;
    strcpy(response_aux2, response);
    cookies2 = strtok(response_aux2, "\n");
    while(cookies2) {
        char aux_cookies[1000];
        strcpy(aux_cookies, cookies2);
        char *aux = strstr(aux_cookies, "Cookie: ");

        if (aux != NULL) {
            strcpy(cookie_vect[j], aux);
            ++j;
        }
        cookies2 = strtok(NULL, "\n");
    }
    // parsing the response so that token will contain only the json message
    i = 0;
    token = strtok(response,"\n\n");
    while (token!= NULL) {
        if (i == 9) {
            break;
        }
        token = strtok(NULL, "\n\n");
        ++i;
    }
    // parsing the json message
    parser2 = json_tokener_parse(token);
    json_object_object_get_ex(parser2, "enunt", &enunt2);
    json_object_object_get_ex(parser2, "url", &url2);
    json_object_object_get_ex(parser2, "method", &method2);

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    // GET request containing the authentification token and the cookies
    msg = compute_get_request(IP_SERVER, PORT_SERVER, (char*)json_object_get_string(url2), NULL, cookie_vect, j, jwl);
    send_to_server(sockfd, msg);
    printf("%s\n", msg);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);
    return response;
}
/*
    Function that solves the fifth task.
    @param  response - the response from the previous task
*/
void task5(char* response) {
    char *msg;  // message to send
    char *cookies;
    char *token;  // will contain the json message
    char *token2;  // used for parsing the url
    char *response_site;  // used for parsing the url
    char *weather;  // will contain the information about the weather
    char final_data[10000];  // contains the query params
    char response_aux[1000];  // copy of the response
    char url_copy[1000];  // copy of the url
    char ip[INET_ADDRSTRLEN];
    char cookie_vect[10][300]; // vector of cookies
    int sockfd;
    int i = 0, j = 0;
    // structures nedeed when parsing the json message
    struct json_object *parser;
    struct json_object *enunt;
    struct json_object *url;
    struct json_object *method;
    struct json_object *type;
    struct json_object *data;
    struct json_object *parser_data;
    struct json_object *url_data;
    struct json_object *query_params;
    struct json_object *data_type;
    struct json_object *parsed_query;
    struct json_object *method_data;
    struct json_object *query_q;
    struct json_object *query_APPID;
    struct addrinfo hints, *result, *p;

    // adding the cookies to the vector of cookies
    j = 0;
    strcpy(response_aux, response);
    cookies = strtok(response_aux, "\n");
    while(cookies) {
        char aux_cookies[1000];
        strcpy(aux_cookies, cookies);

        char *aux = strstr(aux_cookies, "Cookie: ");

        if (aux != NULL) {
            strcpy(cookie_vect[j], aux);
            ++j;
        }
        cookies = strtok(NULL, "\n");
    }
    // parsing the response so that token will contain only the json message
    i = 0;
    token = strtok(response,"\n\n");
    while (token!= NULL) {
        if (i == 9) {
            break;
        }
        token = strtok(NULL, "\n\n");
        ++i;
    } 
    // parsing the json messaje
    parser = json_tokener_parse(token);
    json_object_object_get_ex(parser, "enunt", &enunt);
    json_object_object_get_ex(parser, "url", &url);
    json_object_object_get_ex(parser, "method", &method);
    json_object_object_get_ex(parser, "type", &type);
    json_object_object_get_ex(parser, "data", &data); 

    parser_data = json_tokener_parse(json_object_get_string(data));
    json_object_object_get_ex(parser_data, "url", &url_data);
    json_object_object_get_ex(parser_data, "type", &data_type);
    json_object_object_get_ex(parser_data, "queryParams", &query_params);
    json_object_object_get_ex(parser_data, "method", &method_data);

    parsed_query = json_tokener_parse(json_object_get_string(query_params));
    json_object_object_get_ex(parsed_query, "q", &query_q);
    json_object_object_get_ex(parsed_query, "APPID", &query_APPID); 
    // copy the url and extract the part that we need for the DNS request
    strcpy(url_copy, json_object_get_string(url_data));
    token2 = strtok((char*)json_object_get_string(url_data), "/");

    response_site = strchr(url_copy, '/');  // will contain second part of url

    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_TCP;

    getaddrinfo(token2, NULL, &hints, &result);
    // DNS call for getting the ip address of the site
    for (p = result; p != NULL; p = p->ai_next) {
         if (p->ai_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*) p->ai_addr;
            if (inet_ntop(p->ai_family, &(addr->sin_addr), ip, sizeof(ip)) != NULL) {
                break;
            }
         } 
    }
    freeaddrinfo(result);

    sockfd = open_connection(ip, 80, AF_INET, SOCK_STREAM, 0);
    strcpy(final_data,response_site);
    strcat(final_data, "?q=");
    strcat(final_data, json_object_get_string(query_q));
    strcat(final_data, "&APPID=");
    strcat(final_data, json_object_get_string(query_APPID));
    // GET request on the url we obtained containing the queryParams
    msg = compute_get_request(ip, 80, (char*)final_data, NULL, cookie_vect, 0, NULL);
    send_to_server(sockfd, msg);
    printf("%s\n", msg);
    // get the response from server containing the weather information
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);
    // open new conection with the server
	sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    // get only the weather information
    weather = strchr(response,'{');
    // send the information along with the cookies and the authentification token
    msg = compute_post_request(IP_SERVER, PORT_SERVER, (char*)json_object_get_string(url), weather, cookie_vect, j, jwl);
    send_to_server(sockfd, msg);
    printf("%s\n", msg);
    // get the final response from server
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // close connection
    close_connection(sockfd);
    // free memory
    free(msg);
}

int main(int argc, char *argv[]) {
    char *response;
    response = task1();
    response = task2(response);
    response = task3and4(response);
    task5(response);
    return 0;
}