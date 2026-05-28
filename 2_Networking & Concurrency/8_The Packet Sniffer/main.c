#include <winsock2.h>
#include <stdio.h>
#include <mstcpip.h> // Requiered for SIO_RCVALL (Promiscuous mode)

#pragma comment(lib, "ws2_32.lib")

// --- Die Envelopes (Header) ---

typedef struct IPHeader {
    unsigned char  iph_ihl:4, iph_ver:4; // Version and Header Length
    unsigned char  iph_tos;             // Type of Service
    unsigned short iph_len;             // Total Length
    unsigned short iph_id;              // Identification
    unsigned short iph_offset;          // Flags and Offset
    unsigned char  iph_ttl;             // Time to Live
    unsigned char  iph_protocol;        // Protocol (TCP = 6, UDP = 17, ICMP = 1)
    unsigned short iph_chksum;          // Checksum
    unsigned int   iph_sourceip;        // Source IP address
    unsigned int   iph_destip;          // Destination IP address
} IP_HDR;

int main() {
    WSADATA wsa;
    SOCKET sniffer;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    // the socket:
    // SOCK_RAW: raw data, no filters!
    // IPPROTO_IP: Tells the socket to use the ip layer protocol
    sniffer = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

    if(sniffer == INVALID_SOCKET) {
        printf("Failed to create raw socket. Error: %d\n", WSAGetLastError());
        return 1;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = inet_addr("10.241.4.1"); // Your IP
    local.sin_port = htons(0);

    bind(sniffer, (struct sockaddr *)&local, sizeof(local));

    int optval = 1; // This is a flag, 1 for turning the sniffer on 

    // WSAIoctl function requires a place to store the number of bytes it sends back to you
    DWORD dwBytesRet = 0; // DWORD = Windows-specific type (an unsigned 32-bit integer)

    // WSAIoctl = Windows Sockets Input/Output Control
    // It is a multipurpose tool used to change the behavior of a socket
    // This turns on the "Promiscuous Mode" (The Big Ear)
    if(WSAIoctl(sniffer, SIO_RCVALL, &optval, sizeof(optval), NULL, 0, &dwBytesRet, NULL, NULL) == SOCKET_ERROR) {
        printf("WSAIoctl failed. Error: %d\n", WSAGetLastError());
        return 1;
    }

    char buffer[65536]; // Max size of an IP packet
    struct sockaddr_in source;
    int size = sizeof(source);

    printf("IP SNIFFER ACTIVE: Tracking source and destination IPs...\n");

    while(1) {
        // Capture the raw data
        int bytes = recvfrom(sniffer, buffer, sizeof(buffer), 0, (struct sockaddr *)&source, &size);

        if(bytes > 0) {
            // Map our struct to the buffer
            // Type Punned Pointers
            IP_HDR *iph = (IP_HDR *)buffer;
            
            // Extract the Source IP
            struct in_addr src;
            src.s_addr = iph->iph_sourceip;

            // Extract the Destination IP
            struct in_addr dest;
            dest.s_addr = iph->iph_destip;

            // Printing the "Clean" data
            // We only print who sent the packet and who should receive it
            printf("[IP PACKET] From: %s -> To: %s | Size: %d bytes | Proto: %d\n", 
                    inet_ntoa(src), inet_ntoa(dest), ntohs(iph->iph_len), iph->iph_protocol);
        }
    }
    return 0;
}