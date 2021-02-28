#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
int get_valid_id(void);
#define IP_SERVER "3.8.116.10"

int main(int argc, char *argv[])
{
    bool access_to_login = false;
    bool access_to_enter_library = false;

    char *message;
    char *response;
    char *cookie;
    char *temp;
    char *temp2;
    char *jwt;
    char *route;
    int sockfd;

    string username;
    string password;
    string command;

    string title;
    string author;
    string genre;
    string publisher;
    int page_count;

    char *content_type = (char*)"application/json";
    json j;
    json add_book;

    int id;
    while(1) {
        cin >> command;
        if (command == "register") {
            while(1) {
                cout << "username=";
                cin >> username;

                cout << "password=";
                cin >> password;

                // formarea json ului cu credentiale care va fi trimis la server pt inregistrare
                j["username"] = username;
                j["password"] = password;
                message = compute_post_request ((char*)IP_SERVER, (char*)"/api/v1/tema/auth/register", content_type, j.dump(), NULL);

                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);
                close_connection(sockfd);
    
                printf("%s\n", response);


                temp = strstr(response, "error");
                temp2 = strstr(response, "429 Too Many Requests");
                if (temp2 == NULL) {
                    if (temp == NULL) {
                        printf("\nSuccesfull register\n");
                        break;
                    }
                    else {
                        printf("\nThe username is already taken. Try again!\n");
                    }
                }
                else {
                    printf("\nToo many requests. Register failed! Please try again later\n");
                    break;
                }
                free(message);
                free(response);
            }
            
        } else if (command == "login") {
            if (access_to_login == false) {
                cout << "username=";
                cin >> username;

                cout << "password=";
                cin >> password;

                j["username"] = username;
                j["password"] = password;

                message = compute_post_request ((char*)IP_SERVER, (char*)"/api/v1/tema/auth/login", content_type, j.dump(), NULL);

                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);
                close_connection(sockfd);

                temp = strstr(response,  "connect.sid=");
                cookie = strtok(temp, " ");
                printf("%s\n", response);

                //printf("%s\n", cookie);
                access_to_login = true;
                temp2 = strstr(response, "HTTP/1.1 400 Bad Request");
                if (temp2 != NULL) {
                    printf("\nAuthentification failed! Username or password are wrong\n");
                } else {
                    printf("\nSuccesfull authentification.\n");
                }

                free(message);
                free(response);
            } else {
                printf("\nYou're already logged.\nType \'logout\' before trying to login\n");
            }
            
        }
        else if (command == "enter_library") {
            if (access_to_login == true) {

                message = compute_get_request((char*)IP_SERVER, (char*)"/api/v1/tema/library/access", NULL, cookie);

                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);
                

                temp = strstr(response, "{\"token");
                temp2 = strstr(temp, ":");
                temp = strstr(temp2, "\"");
                jwt = strtok(temp, "\"}");
                printf("%s\n", response);
                
                access_to_enter_library = true;

                printf("\nAcces to library granted\n");

                free(message);
                free(response);

            } else {
                printf("\nYou need to login(authentificate yourself) before entering the library.\nType \'login\'.\n");
            }
           
        }
        else if (command == "get_books") {
            if (access_to_enter_library == true) {
                message = compute_get_request_jwt((char*)IP_SERVER, (char*)"/api/v1/tema/library/books", NULL, jwt);

                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                printf("%s\n", response);
                printf("\nList of books shown succesfull!\n");
                free(message);
                free(response);
            } else {
                printf("\nYou don't have access to the library.\nType \'enter_library\'\n");
            }
            
        }
        else if (command == "get_book") {
            if (access_to_enter_library == true) {
                cout << "id=";
                id = get_valid_id();
                auto s = "/api/v1/tema/library/books/" + to_string(id);
                
                route = const_cast<char*> (s.c_str());
                
                message = compute_get_request_jwt((char*)IP_SERVER, route , NULL, jwt);
                printf("%s\n", message);

                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);
                printf("%s\n", response);

                temp = strstr(response, "HTTP/1.1 404 Not Found");
                if (temp != NULL) {
                    printf("\nThe book you are searching for doesn't exist.\nType again the \'get_book\'command with a valid ID.\n");
                } else {
                    printf("\nBook got with success!\n");
                }

                free(message);
                free(response);
            } else {
                printf("\nYou don't have access to the library.\nType \'enter_library\'\n");
            }
           
        }
        else if (command == "add_book") {
            if (access_to_enter_library == true) {
                cout << "title=";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                getline(cin, title, '\n');

                cout << "author=";
                getline(cin, author, '\n');

                cout << "genre=";
                getline(cin, genre, '\n');

                cout << "publisher=";
                getline(cin, publisher, '\n');

                cout << "page_count=";
                page_count = get_valid_id();

                add_book["title"] = title;
                add_book["author"] = author;
                add_book["genre"] = genre;
                add_book["page_count"] = page_count;
                add_book["publisher"] = publisher;

                message = compute_post_request_jwt((char*)IP_SERVER, (char*)"/api/v1/tema/library/books", content_type, add_book.dump(), NULL, jwt);

                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                printf("%s\n", response);
                printf("\n Success!\n");
                free(message);
                free(response);
            }
            else {
                printf("\nYou don't have access to the library.\nType \'enter_library\'\n");
            }
        }

        else if (command == "delete_book") {
            if (access_to_enter_library == true) {
                cout << "id=";
                id = get_valid_id();
                auto s = "/api/v1/tema/library/books/" + to_string(id);
                route = const_cast<char*> (s.c_str());

                message = compute_delete_request_jwt((char*)IP_SERVER, route , NULL, jwt);
                
                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                printf("%s\n", response);

                temp = strstr(response, "HTTP/1.1 404 Not Found");
                if (temp != NULL) {
                    printf("\n The book you are searching for doesn't exist.\nType again the \'delete_book\'command with a valid ID.\n");
                } else {
                    printf("\nDeleted with success!\n");
                }

                free(message);
                free(response);
            } else {
                printf("\nYou don't have access to the library.\nType \'enter_library\'\n");
            }
        }
        else if (command == "logout") {
            if (access_to_login == true) {
                message = compute_get_request((char*)IP_SERVER, (char*)"/api/v1/tema/auth/logout", NULL, cookie);

                sockfd = open_connection((char*)IP_SERVER, 8080, AF_INET, SOCK_STREAM, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                printf("%s\n", response);
                printf("\nLogout succesfull!\n");

                access_to_login = false;
                jwt =  NULL;
                cookie = NULL;

                free(message);
                free(response);
            } else {
                printf("\nYou can't logout if you don't login first.\n");
            }
        }
        else if (command == "exit") {
            printf("\nSuccess\n");
            close_connection(sockfd);
            break;
        }
        else {
            printf("\nUnknown command. Valid commmands are:\nregister\nlogin\nenter_library\nget_books\nget_book\nadd_book\ndelete_book\nlogout\nexit\n");
        }
    }
    
    return 0;
}

int get_valid_id(void) {
    std::string theInput;
    int inputAsInt;
    std::cin.clear();
    cin >> theInput;
    while(std::cin.fail() || std::cin.eof() || theInput.find_first_not_of("0123456789") != std::string::npos) {

        std::cout << "Invalid argument. Argument should be numeric, not string and not negative.Try again!" << std::endl;

        if( theInput.find_first_not_of("0123456789") == std::string::npos) {
            std::cin.clear();
            std::cin.ignore(256,'\n');
        }

        //std::getline(std::cin, theInput);
        cin >> theInput;
    }

    std::string::size_type st;
    inputAsInt = std::stoi(theInput,&st);
    
  
    //std::cout << inputAsInt << std::endl;
    return inputAsInt;
}