#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h> // gettimeofday()

#define IP_MAXPACKET 65535
// ICMP header len for echo req
#define ICMP_HDRLEN 8

// Checksum algo
unsigned short calculate_checksum(unsigned short *paddress, int len);

//our ip --> raz's ip computer
#define SOURCE_IP "10.0.2.15"
// i.e the gateway or ping to google.com for their ip-address
//we took the Google Public DNS IP addresses (IPv4) from this web:
// https://developers.google.com/speed/public-dns/docs/using
#define DESTINATION_IP "8.8.8.8"

int main() {
    struct icmp icmphdr; // ICMP-header
    char data[IP_MAXPACKET] = "This is the ping.";
    int datalen = strlen(data) + 1;

    //===================
    // ICMP header
    //===================
    // Message Type (8 bits): ICMP_ECHO_REQUEST; ICMP Type: 8 is request, 0 is reply.
    icmphdr.icmp_type = ICMP_ECHO;

    // Message Code (8 bits): echo request
    icmphdr.icmp_code = 0;

    // Identifier (16 bits): some number to trace the response.
    // It will be copied to the response packet and used to map response to the request sent earlier.
    // Thus, it serves as a Transaction-ID when we need to make "ping"
    icmphdr.icmp_id = 18; // hai

    // Sequence Number (16 bits): starts at 0
    icmphdr.icmp_seq = 0;

    // ICMP header checksum (16 bits): set to 0 not to include into checksum calculation
    icmphdr.icmp_cksum = 0;

    // Combine the packet
    char packet[IP_MAXPACKET];

    // First, ICMP header
    memcpy((packet), &icmphdr, ICMP_HDRLEN);

    // After ICMP header, add the ICMP data.
    memcpy(packet + ICMP_HDRLEN, data, datalen);

    // Calculate the ICMP header checksum
    icmphdr.icmp_cksum = calculate_checksum((unsigned short *) (packet), ICMP_HDRLEN + datalen);
    memcpy(packet, &icmphdr, ICMP_HDRLEN);

    struct sockaddr_in dest_in;
    memset(&dest_in, 0, sizeof(struct sockaddr_in));
    dest_in.sin_family = AF_INET;

    // The port is irrelant for Networking and therefore was zeroed.
    dest_in.sin_addr.s_addr = inet_addr(DESTINATION_IP);

    // Create raw socket for IP-RAW (make IP-header by yourself)
    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) { // IPPROTO_RAW
        fprintf(stderr, "socket() failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        return -1;
    }



    ////////////////////////our code//////////////////////////
    ///////////////myping sends ICMP ECHO REQUEST/////////////
    char buf[IP_MAXPACKET]; //array for the buffer

    //timeval: A time value that is accurate to the nearest microsecond but also has a range of years.
    struct timeval start, end;

    //get the current time of day
    gettimeofday(&start, NULL);

    // Send the packet using sendto() for sending datagrams.
    if (sendto(sock, packet, ICMP_HDRLEN + datalen, 0, (struct sockaddr *) &dest_in, sizeof(dest_in)) == -1) {
        fprintf(stderr, "sendto() failed with error: %d", errno);
        return -1;
    }
    else{
        ///printing the src and the dest of th packets
        printf("sent successfully!\n");
        printf("IP source: %s\n",SOURCE_IP);
        printf("IP destination: %s\n",DESTINATION_IP);
    }

    ///////////////receives ICMP-ECHO-REPLY:///////////////////

    // socklen_t which is an unsigned opaque integral type of length of at least 32 bits
    socklen_t socklen = sizeof(dest_in);
    int check_recv = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *) &dest_in, (&socklen));
    //checking the recvfrom:
        if (check_recv < 0) {
            perror("**ERROR** type_error: receive error\n");
            return -1; //there is an error
        } else{
            printf("Received successfully!\n");
        }

    // this web help us to understand the gettimeofday use:
    //https://linuxhint.com/gettimeofday_c_language/
    gettimeofday(&end, NULL);

    ///printing the RTT times
    double microseconds = end.tv_usec - start.tv_usec;
    printf("The RTT time: %f [microseconds]\n",microseconds);
    //1 microsecond=0.001 milliseconds
    double milliseconds = microseconds/1000.;
    printf("The RTT time: %f  [milliseconds]\n",milliseconds);

    // Close the raw socket descriptor.
    close(sock);
    printf("FINISH!\n");
    return 0;
}

// Compute checksum (RFC 1071).
unsigned short calculate_checksum(unsigned short *paddress, int len) {
    int nleft = len;
    int sum = 0;
    unsigned short *w = paddress;
    unsigned short answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *((unsigned char *) &answer) = *((unsigned char *) w);
        sum += answer;
    }

    // add back carry outs from top 16 bits to low 16 bits
    sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
    sum += (sum >> 16);                 // add carry
    answer = ~sum;                      // truncate to 16 bits

    return answer;
}
