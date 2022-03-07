#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<netinet/ip_icmp.h>
#include<netinet/ip.h>
#include<net/ethernet.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#define  IP_MAXPACKET  65535
int cntMessages=1; //counter for the num of the icmp messages

//We've been helped by codes from the practice.
int main() {
    struct sockaddr sock_addr;
    char buf[IP_MAXPACKET];  //array for the buffer that collect packets

    // create a raw socket
    int sock = socket(AF_PACKET, SOCK_RAW,htons(ETH_P_ALL));
        if (socket<0) { //check the creation of the socket
            perror("**ERROR** type_error: socket error\n");
            return 1; //there is an error
        }

    //Begins to capture packets
    while(1){
        int check_recv, sock_add_len=sizeof(sock_addr);
        bzero(buf, sizeof(buf));
        check_recv = recvfrom(sock, buf, sizeof(buf), 0, &sock_addr, &sock_add_len);
        //checking the recvfrom:
            if(check_recv < 0) {
                perror("**ERROR** type_error: socket error\n");
                return 1; //there is an error
            }
            if(check_recv >= 0) {
                struct iphdr *iph = (struct iphdr *) (buf +sizeof(struct ethhdr));//construct to the IP header
                if (iph->protocol ==1) { //checking if it's Icmp protocol( he needs to be 1 )
                    struct sockaddr_in src,dest; //temporary structures that specify a transport address and port for the AF_INET address family


                    ///printing the num of icmp messages
                    printf("\n#####################\n");
                    printf("Message number: %d\n", cntMessages++);

                    //The  memset()  function  fills  the  first n bytes of the memory area pointed to by s with the constant byte c.
                    memset(&src, 0, sizeof(src));
                    src.sin_addr.s_addr = iph->saddr;
                    memset(&dest, 0, sizeof(dest));
                    dest.sin_addr.s_addr = iph->daddr;

                    ///printing the IP_SRC, IP_DST
                    //inet_ntoa is Convert Internet number in IN to ASCII representation.  The return value is a pointer to an internal array containing the string.
                    printf("IP src: %s\n", inet_ntoa(src.sin_addr));
                    printf("IP destination: %s\n", inet_ntoa(dest.sin_addr));

                    ///printing the  ICMP TYPE
                    struct icmphdr *icmph = (struct icmphdr *) ((char *) iph + (4 * iph->ihl));
                    //Check the type of the ICMP Protocol
                    unsigned int type=icmph->type;
                        if (type==0) //reply
                            printf("ICMP type: reply\n");
                        if (type==8) //request
                            printf("ICMP type: request\n");

                    ///printing the CODE
                    printf("Code: %d\n", icmph->code);
                    printf("\n#####################\n");
                }
                else{
                    break;
                }
            }
    }
    close(sock);
    printf("FINISH!\n");
    return 0;
}

