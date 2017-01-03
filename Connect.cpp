#include <sys/socket.h>
#include <sys/types.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "Connect.h"

bool Connect::GetStreamBytes(vector<uint8_t> &binary_data) {
    struct sockaddr_storage ip_addr;

    if(!GetIPAddress(domain_, port_, &ip_addr, &ip_addr_len_)) {
        cerr << "IP address lookup failed" << endl;
        return false;
    }

    if(!ConnectToServer(ip_addr, ip_addr_len_, &socket_fdesc_)) {
        cerr << "connect() failed: " << strerror(errno) << endl;
        return false;
    }

    if(!ReadTransactions(socket_fdesc_, binary_data)) {
        cerr << "read() failed: " << strerror(errno) << endl;
        return false;
    }

    return true;
}

bool Connect::GetIPAddress(const char *dns, unsigned short port,
                           struct sockaddr_storage *ip_addr_, size_t *ip_addr_len_) {
    int status;
    struct addrinfo hints, *results;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(dns, nullptr, &hints, &results)) != 0) {
        cerr << "getaddrinfo failed: " << gai_strerror(status) << endl;
        return EXIT_FAILURE;
    }

    if(results->ai_family == AF_INET) {
        struct sockaddr_in *v4addr = (struct sockaddr_in *) results->ai_addr;
        v4addr->sin_port = htons(port);
    } else if(results->ai_family == AF_INET6) {
        struct sockaddr_in6 *v6addr = (struct sockaddr_in6 *) results->ai_addr;
        v6addr->sin6_port = htons(port);
    } else {
        cerr << "No valid IP address returned." << endl;
        freeaddrinfo(results);
        return EXIT_FAILURE;
    }


    if(results == nullptr) {
        return false;
    }

    memcpy(ip_addr_, results->ai_addr, results->ai_addrlen);
    *ip_addr_len_ = results->ai_addrlen;

    freeaddrinfo(results);
    return true;
}

bool Connect::ConnectToServer(const struct sockaddr_storage &ip_addr_,
                              const size_t &ip_addr_len_, int *ret_fdesc) {

    int socket_fdesc = socket(ip_addr_.ss_family, SOCK_STREAM, 0);

    if (socket_fdesc == -1) {
        cerr << "Socket not generated." << endl;
        return EXIT_FAILURE;
    }

    int result = connect(socket_fdesc,
                         reinterpret_cast<const struct sockaddr *>(&ip_addr_),
                         ip_addr_len_);

    if (result == -1) {
        OpenFile("transactionData-10000-3.bin", ret_fdesc);
    } else {
        *ret_fdesc = socket_fdesc;
    }

    return true;
}

bool Connect::OpenFile(const char *filepath, int *ret_fdesc) {
    *ret_fdesc = open(filepath, O_RDONLY);
    return true;
}

bool Connect::ReadTransactions(int socket_fdesc, vector<uint8_t> &binary_data) {
    char buffer[256];
    int res;

    while(1) {
        res = read(socket_fdesc, buffer, 256);

        if(res == -1) {
            if(errno == EINTR)
                continue;

            cerr << "socket read failure: " << strerror(errno) << endl;
            close(socket_fdesc);
            return false;
        } else if(res == 0) {
            close(socket_fdesc);
            return true;
        }

        //cout << "res: " << res << endl;
        binary_data.reserve(res);
        copy(buffer, buffer + res, back_inserter(binary_data));
    }
}
