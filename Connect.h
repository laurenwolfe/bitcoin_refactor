#ifndef _CONNECT_H_
#define _CONNECT_H_

#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <sys/socket.h>

#define DOMAIN "cse461.cs.washington.edu"
#define PORT 46114

using namespace std;

class Connect {
    public:
        bool GetStreamBytes(vector<uint8_t> &binary_data);

    private:
        const char *domain_ = DOMAIN;
        unsigned short port_ = PORT;
        size_t ip_addr_len_;
        int socket_fdesc_;

        bool ReadTransactions(int socket_fdesc, vector<uint8_t> &binary_data);
        bool ConnectToServer(const struct sockaddr_storage &ip_addr_,
                             const size_t &ip_addr_len_, int *ret_fdesc);
        bool OpenFile(const char *filepath, int *ret_fdesc);

        bool GetIPAddress(const char *dns, unsigned short port,
                          struct sockaddr_storage *ip_addr_, size_t *ip_addr_len_);
};

bool GetIPAddress(const char *dns, unsigned short port,
                  struct sockaddr_storage *ip_addr, size_t *ip_addr_len);

bool Connect(const struct sockaddr_storage &ip_addr,
             const size_t &ip_addr_len, int *ret_fdesc);


bool ReadTransactions(int socket_fdesc, vector<uint8_t> &binary_data);

vector<string> BinToHex(int start_idx, int length, const vector<uint8_t> &binary_data);
int BinToDecimal(int start_idx, int length, const vector<uint8_t> &binary_data);


#endif //_CONNECT_H_