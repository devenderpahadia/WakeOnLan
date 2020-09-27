//
//  main.c
//  Wake On Lan
//
//  Created by Garvit Chaudhary on 31/08/20.
//  Copyright © 2020 Garvit Chaudhary. All rights reserved.
//

#include <stdio.h>//included for funtions like main(),fprintf(),printf(),malloc(),

#include "WakeOnLan.h"//header file declared containing neccessary macros and the function prototypes
#include <stdlib.h>//pointer : stderr ; function used : strtol(),exit(),
#include <unistd.h> // variables : optarg, optopt, optind ;function used : getopt()
#include <ctype.h>//functions used : isprint(),
#include <string.h>//function used : strncpy(),strtok()
#include <sys/socket.h>//macros used : AF_INET,SOCK_DGRAM,IPPROTO_UDP ;fuction used : socket(),setsockopt(),
#include <arpa/inet.h>//structure used : sockaddr_in ; function used : inet_aton()
#include <errno.h>//function used : strerror()
/*
 Function Definations
 */
mac_address_t *nextAddressFromArguments( char **argument, int length )
{
    static int i=0;//static variable used so that we can access it after the function ends
  mac_address_t *currentMacAddress = (mac_address_t *) malloc( sizeof( mac_address_t ));//dynamic memory allocation of variable using malloc function

  if( currentMacAddress == NULL )//checking for whether it is possible to use the memory for the variable current mac address or the memory is full
  {
    fprintf( stderr, "Memory full : %s ...!\n", strerror( errno ));//for printing the error message for the memory full
    exit( EXIT_FAILURE );//to exit returning 8 value
  }

  while( i < length )
  {
    if( packMacAddress( argument[i], currentMacAddress ) < 0 )//checking whether structure to be formed is not null and exist on the memoery
    {
      fprintf( stderr, "Invalid MAC Address : %s ...!\n", argument[i] );//error display
      i++;
      continue;
    }
    i++;
    return currentMacAddress;//returning currentMacAddress if it is valid
  }

  return NULL;
}

mac_address_t *nextAddressFromFile( char **filenames, int length )
{
  static FILE *fp = NULL;//declaring a file pointer which will be used to read Mac Address from the file
  static int fileNo = 0;//static variable which will be used to iterate over to read from file
  mac_address_t *currentMacAddress = (mac_address_t *) malloc( sizeof( mac_address_t ));//allocating memory for currentMacAddress on heap using malloc
  char *currentInputMacAddress  = (char *) malloc( MAC_ADDRESS_STR_MAX * sizeof( char ));//allocating memory on heap for currentInputMacAddress using malloc

  if( currentMacAddress == NULL || currentInputMacAddress == NULL )//checking for empty mac address
  {
    fprintf( stderr, "Memory Full: %s ...!\n", strerror( errno ));
    exit( EXIT_FAILURE );
  }

  while( fileNo < length )
  {
    if( fp == NULL )
    {
      if(( fp = fopen( filenames[fileNo], "r" )) == NULL )//checking if file exists or not for reading purposes
      {
        fprintf( stderr, "Cannot open file %s: %s ...!\n", filenames[fileNo], strerror( errno ));//displaying error message
        exit( EXIT_FAILURE );//exit program with a failure
      }
      printf( "Read from file %s:\n", filenames[fileNo] );//if file exist reading the mac Address from the file
    }

    if( fgets( currentInputMacAddress, MAC_ADDRESS_STR_MAX, fp ) != NULL )//taking input from file till the end is not reached
    {
      if( currentInputMacAddress[0] == '#' )//Mac Address doesnt contains # sign
      {
        continue;
      }

      currentInputMacAddress[strlen( currentInputMacAddress ) - 1] = '\0';//Assinging '\0' to the end of currentInputMacAddress
      if( packMacAddress( currentInputMacAddress, currentMacAddress ) < 0 )//check for null Mac address and invalid mac address
      {
        fprintf( stderr, "MAC Address ist not valid: %s ...!\n", currentInputMacAddress );
        continue;
      }
      return currentMacAddress;
    }
    else
    {
      fclose( fp );//closing the file
      fp = NULL;
      fileNo++;
      puts( "" );
    }
  }

  return NULL;
}

int packMacAddress( const char *mac, mac_address_t *packedMac )//take two arguments mac and packedMac and copy the value of mac to packedMac
{
  char *tempMacAddress    = (char *) malloc( strlen( mac ) * sizeof( char ));//temp variable declared
  char *delimiter = (char *) ":";//delimiter declared which is to be further used in the strtok() function
  char *token;//to store the tokens generated by strtok()
  
  if( tempMacAddress == NULL )//to check for Null Mac address
  {
    fprintf( stderr, "Cannot allocate memory for mac address: %s ...!\n", strerror( errno ));//displaying error message
    return -1;//returning -1 indicating error occured which is further used in other functions in the program
  }

  strncpy( tempMacAddress, mac, strlen( mac ));//copying mac to tempMacAddress
  token = strtok( tempMacAddress, delimiter );//to get the tokens from the tempMacAddress seprated by delimiter ":"

  for(int i = 0; i < MAC_ADDRESS_MAX; i++ )//loop to check for null mac address and assinging value of mac_address of packedMac structure
  {
    if( token == NULL )
    {
      return -1;
    }

    packedMac->mac_address[i] = (unsigned char) strtol( token, NULL, BASE );//strtol function converts the token char to long int of base 16 that is hexadecimal value
    token = strtok( NULL, delimiter );//assinging token to null value
  }

  strncpy( packedMac->mac_address_str, mac, MAC_ADDRESS_STR_MAX );//to copy mac to mac_address_str of the packedMac struct type variable
  return 0;//indicating success in the function to be used in other function definations
}

int startupSocket( )
{
  int sock;
  int optval = 1;

  if(( sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0 )//AF_INET : internetwork: UDP, TCP, etc.,SOCK_DGRAM : Datagram Socket,IPPROTO_UDP : user datagram protocol
  {
    fprintf( stderr, "Error openning the socket: %s ...!\n", strerror( errno ));//display error if condition is not matched
    return -1;//To indicate failure
  }

  if( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char *) &optval, sizeof( optval )) < 0 )//to setup the socket
  {
    fprintf( stderr, "Cannot set socket options: %s ...!\n", strerror( errno ));//error if socket is not setup
    return -1;//to indicate failure
  }

  return sock;//finally sock is returned if none of the error conditions are matched
}

int sendWakeOnLanMessage( const wakeOnLan_header_t *wakeOnLan_header, const int sock )
{
  struct sockaddr_in address;
  unsigned char packet[PACKET_BUFFER];//magic packet
  int i, j;

  address.sin_family = AF_INET;//initializing type of port communication to AF_INET
  address.sin_port   = htons( REMOTE_PORT );//initializing the port to  be used that is port 9
  if( inet_aton( wakeOnLan_header->remote_address, &address.sin_addr ) == 0 )
  {
    fprintf( stderr, "Invalid remote ip address given: %s ...!\n", wakeOnLan_header->remote_address );//error message
    return -1;
  }

  for( i = 0; i < 6; i++ )
  {
    packet[i] = 0xFF;//initializing magic packet
  }

  for( i = 1; i <= 16; i++ )
  {
    for( j = 0; j < 6; j++ )
    {
      packet[i * 6 + j] = wakeOnLan_header->mac_address->mac_address[j];//assinging magic packet with Mac Address
    }
  }

  if( sendto( sock, packet, sizeof( packet ), 0, (struct sockaddr *) &address, sizeof( address )) < 0 )//checking if magic packet is not sent successfully
  {
    fprintf( stderr, "Cannot send Magic Packet: %s ...!\n", strerror( errno ));//error message
    return -1;//failure
  }

  printf( "Successful sent Wake On Lan magic packet to %s ...!\n", wakeOnLan_header->mac_address->mac_address_str );//success message
  return 0;//success
}

int main(int argc, const char * argv[]) {
    // insert code here...
      int sock;
      mac_address_t *( *funcp )( char **args, int length ) = nextAddressFromArguments;
      wakeOnLan_header_t *currentWOLHeader = (wakeOnLan_header_t *) malloc( sizeof( wakeOnLan_header_t ));
      char **args = (char **) malloc( argc * ARGS_BUFFER_MAX * sizeof( char ));
      int length = argc;
      int argument;

      strncpy( currentWOLHeader->remote_address, REMOTE_ADDRESS, ADDRESS_LENGTH );//copy Macro REMOTE_ADDRESS to remote_address of currentWOLHeader

      while(( argument = getopt( argc, argv, "r:f" )) != -1 )//reading the arguments from command line
      {
        if( argument == 'f' )
        {
          funcp = nextAddressFromFile;//MAC Address from the file
        }
        else if( argument == 'r' )
        {
          strncpy( currentWOLHeader->remote_address, optarg, ADDRESS_LENGTH );//IP address copy
        }
        else if( argument == '?' )
        {
          if( isprint( optopt ))//check if the last argument is printable or not if it is then display the error of unknown option
          {
            fprintf( stderr, "Unknown option: %c ...!\n", optopt );//erro
          }
        }
        else
        {
          fprintf( stderr, USAGE, *argv );
        }
      }

      if( argc < 2 )//check if the arguments are less than two beacuse we need two arguments one for the MAC address and other for ip address
      {
        fprintf( stderr, USAGE, *argv );//error display
        exit( EXIT_FAILURE );//exit from main showing that code exit by returning 9
      }

      args   = &argv[optind];
      length = argc - optind;

      if(( sock = startupSocket( )) < 0 )
      {
        exit( EXIT_FAILURE ); // Log is done in startupSocket( )
      }

      while(( currentWOLHeader->mac_address = funcp( args, length )) != NULL )
      {
        if( sendWakeOnLanMessage( currentWOLHeader, sock ) < 0 )//check for magic packet is sent or not
        {
          fprintf( stderr, "Error occured during sending the WOL magic packet for mac address: %s ...!\n", currentWOLHeader->mac_address->mac_address_str );
        }
        free( currentWOLHeader->mac_address );//deleting memory allocated on the free store
      }

      close( sock );//closing socket
      return EXIT_SUCCESS;
    }
    

