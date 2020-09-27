//
//  WakeOnLan.h
//  Wake On Lan
//
//  Created by Garvit Chaudhary on 31/08/20.
//  Copyright © 2020 Garvit Chaudhary. All rights reserved.
//
#ifndef WakeOnLan_h
#define WakeOnLan_h
/*
 Macros Defination
 */
#define PACKET_BUFFER 17*6
#define MAC_ADDRESS_MAX 6
#define MAC_ADDRESS_STR_MAX 64
#define BASE 16
#define ADDRESS_LENGTH 16
#define REMOTE_ADDRESS "255.255.255.255"
#define REMOTE_PORT 9
#define ARGS_BUFFER_MAX 128
#define USAGE "Usage: %s [-r remoteaddr] [-f filename1, ...|mac1, ...]\n"
/*
 Structures Defination
 */
//Structure for mac address of the target pc to be turned ON
typedef struct
{
  unsigned char mac_address[MAC_ADDRESS_MAX];
  char mac_address_str[MAC_ADDRESS_STR_MAX];
} mac_address_t;
//Structure for IP Address
typedef struct
{
  char remote_address[ADDRESS_LENGTH];
  mac_address_t *mac_address;
} wakeOnLan_header_t;
/*
 Function Prototypes
 */
//This function sends the magic packet to the given mac address
//wakeOnLan_header_t : Wake on lan header Packet
//sock : The socket to send the magic packet
int sendWakeOnLanMessage( const wakeOnLan_header_t *wakeOnLan_header, const int sock );
//This function creates the UDP(User Datagram Protocol) socket to send the Wake on Lan magic Packet
int startupSocket();
//This function is to get the mac address from the command line arguments
mac_address_t *nextAddressFromArguments( char **argument, int length );
//This function is used to get the Mac address from the file this method is useful in the cases where you want to wake up a single computer again and again to prevent using command line arguments
mac_address_t *nextAddressFromFile( char **filenames, int length );
//This Function converts a mac address string to a mac address struct of type mac_address_t
//mac : The mac address to be converted
int packMacAddress( const char *mac, mac_address_t *packedMac );
#endif /* WakeOnLan_h */
