//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
//  Abstract:
//
//  This provides a simple sockets protocol that encapsulates the basic
//  things that a client would do with a socket.
//
//  It is designed to be asynchronous, although there are 2 different methods for
//  the asynchronicity.
//
//  1) Initialize() (setting up the network), and Connect() (connecting a socket) are made
//     asynchronous by having separate calls, IsInitialized() and IsConnected(),
//	 respectively, that allow for checking the status of the network/connection
//
//  2) Send() and Recv() will return 0 if the network is not ready to send/receive more data.
//
//  While this might seem complicated, it appears to me to be the simplest way to allow
//  for a design that supports asynchronous access, but also does not require it -- any
//  of the calls can be made synchronous as well.


#ifndef _SIMPLE_SOCKET_PROTOCOL_
#define _SIMPLE_SOCKET_PROTOCOL_

#ifndef _SIMPLE_SOCKET_DRIVER_H_
#include "Efi.h"
#endif

#define SIMPLE_SOCKET_PROTOCOL_GUID  \
  { 0xcd171cf, 0xf5d8, 0x4937, 0xb3, 0x3a, 0xb6, 0xdd, 0x82, 0xd0, 0xd6, 0x26 }

EFI_FORWARD_DECLARATION (SIMPLE_SOCKET_PROTOCOL);

//
// This is designed to be a simple, asynchronous, sockets client protocol.
//
// It may later be extended with the "socket server" calls.
//
typedef UINT32 SIMPLE_SOCKET;
typedef UINT16 SIMPLE_SOCKET_PORT;

//
// Initialize()
//
// Initialize the sockets API. This will connect to some network, typically via
// the SimpleNetwork Protocol, acquire a DHCP lease if necessary, etc.
//
// In particular, it will "acquire" the Simple Network API, either directly
// or indirectly.
//
// The intent behind this call is that it can be made multiple times to "poll"
// for completion of initialization (non-blocking). See the return codes.
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this succeeds
// EFI_ALREADY_STARTED	 may also be returned if the network has been successfully
//						 started once, and this is a subsequent call
// EFI_NOT_READY		 is returned if this is started initializing, but is
//						 not yet complete
// EFI_NOT_FOUND		 The Simple Network Protocol driver could not be found,
//						 or no network itself could be found
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_INITIALIZE) (
	IN SIMPLE_SOCKET_PROTOCOL		*This
	);

//
// IsInitialized()
//
// check the initialization status of the network
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this call succeeds. The network is initialized
// EFI_NOT_STARTED		 network initialize not attempted
// EFI_NOT_READY		 is returned if this call succeeds, but the network is not
//						 yet up.
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//

typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_IS_INITIALIZED) (
	IN SIMPLE_SOCKET_PROTOCOL		*This
	);

//
// Cleanup()
//
// Shutdown the network, release networking resources, etc.
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this succeeds
// EFI_NOT_STARTED		 The network was not yet started
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_CLEANUP) (
	IN SIMPLE_SOCKET_PROTOCOL		*This
	);

//
// Open()
//
// open a socket handle. (socket() call). Doesn't do anything with it.
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this succeeds
// EFI_NOT_STARTED		 The network was not yet started
// EFI_INVALID_PARAMETER network initialization not complete, or some other parameter error
// EFI_DEVICE_ERROR		 Some other lower-level error returned
// EFI_OUT_OF_RESOURCES	 Have hit the maximum number of open sockets
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_OPEN) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	OUT SIMPLE_SOCKET				    *Socket
	);

//
// Close()
//
// close an open socket handle, and disconnect
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this succeeds
// EFI_INVALID_PARAMETER The socket passed in is not a valid one
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_CLOSE) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN SIMPLE_SOCKET				    Socket
	);

//
// Connect()
//
// connect a socket to a destination. This should initiate a connection,
// IsConnected() should be called to see the status of the connection. Also,
// it is legal to make a call immediately to Send() or Recv() and have either
// return an EFI_NOT_READY status
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this call succeeds, and the socket is
//						 completely connected
// EFI_NOT_READY		 is returned if this call succeeds, but the socket
//						 connection is not complete
// EFI_INVALID_PARAMETER The socket passed in is not a valid one, or is
//						 already connected.
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_CONNECT) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN SIMPLE_SOCKET				    Socket,
	IN EFI_IP_ADDRESS				    *Address,
	IN SIMPLE_SOCKET_PORT			  Port
	);

//
// IsConnected()
//
// check the connection status of a socket. The following possibilities exist:
//
// 1) Connect() was never called on this socket:
//    returns EFI_INVALID_PARAMETER
// 2) Connect() was called, the socket connection is started, but not complete
//    returns EFI_NOT_READY
// 3) Connect() was called, the socket connection is completed
//    returns EFI_SUCCESS
// 4) Connect() was called, the socket connection was closed by the other end
//    returns EFI_ABORTED
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this call succeeds, and the socket is
//						 connected, or the socket was closed by the other end.
// EFI_NOT_READY		 is returned if this call succeeds, but the socket
//						 connection is not complete.
// EFI_ABORTED			 Connection closed at the other end
// EFI_INVALID_PARAMETER The socket passed in is not a valid one, no connection
//						 attempt has been made, or it was closed
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//

typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_IS_CONNECTED) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN SIMPLE_SOCKET            Socket
	);

//
// Send()
//
// will send up to the # of bytes passed in. It will return the number of bytes
// actually sent. If it sends 0 bytes, it should return EFI_NOT_READY as opposed
// to EFI_SUCCESS
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this call succeeds.
// EFI_NOT_READY		 Not ready to send on the socket.
// EFI_INVALID_PARAMETER The socket passed in is not a valid one, or is not connected
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_SEND) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN SIMPLE_SOCKET            Socket,
	IN UINT32                   ToSend,
	IN UINT8                    *Data,
	OUT UINT32                  *Sent
	);

//
// Recv()
//
// will receive up to the # of bytes passed in. It will return the number of bytes
// actually received. If it receives 0 bytes, it should return EFI_NOT_READY as
// opposed to EFI_SUCCESS
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this call succeeds (some # of bytes
//						 received).
// EFI_NOT_READY		 Not ready to receive on the socket.
// EFI_INVALID_PARAMETER The socket passed in is not a valid one, or is not connected
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_RECV) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN SIMPLE_SOCKET            Socket,
	IN UINT32                   Size,
	OUT UINT8                   *Data,
	OUT UINT32                  *Received
	);

//
// GetIPAddress()
//
// will retrieve an IP Address for a host. May return EFI_NOT_READY; a subsequent
// call will eventually retrieve the IP address, or return failure
//
// Return codes:
//
// EFI_SUCCESS			 is returned if this call succeeds (some # of bytes
//						 received).
// EFI_NOT_READY		 Not ready to receive on the socket. This could
//						 be because the socket is not yet connected, or
//						 it could be because the socket is not ready to receive
//						 some data.
// EFI_NOT_FOUND		 The name could not be resolved.
// EFI_INVALID_PARAMETER The name passed in does not match DNS requirements
// EFI_DEVICE_ERROR		 Some other lower-level error returned
//
typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_GET_IP_ADDRESS) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN CHAR8                    *Name,
	OUT EFI_IP_ADDRESS          *Address
	);


typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_SET_DNS_SERVER) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN EFI_IP_ADDRESS						*PrimaryServerIP,
	IN EFI_IP_ADDRESS				    *SecondaryServerIP OPTIONAL
	);

typedef
EFI_STATUS
(EFIAPI *SIMPLE_SOCKET_SET_IO_TIMEOUT) (
	IN SIMPLE_SOCKET_PROTOCOL		*This,
	IN SIMPLE_SOCKET				    Socket,
	IN UINT32				            Timeout //  us
	);
//
// SIMPLE_SOCKET PROTOCOL
//
typedef struct _SIMPLE_SOCKET_PROTOCOL {
	SIMPLE_SOCKET_INITIALIZE		  Initialize;
	SIMPLE_SOCKET_IS_INITIALIZED	IsInitialized;
	SIMPLE_SOCKET_CLEANUP			    Cleanup;
	SIMPLE_SOCKET_OPEN				    Open;
	SIMPLE_SOCKET_CLOSE				    Close;
	SIMPLE_SOCKET_CONNECT			    Connect;
	SIMPLE_SOCKET_IS_CONNECTED		IsConnected;
	SIMPLE_SOCKET_SEND				    Send;
	SIMPLE_SOCKET_RECV				    Recv;
	SIMPLE_SOCKET_GET_IP_ADDRESS	GetIPAddress;
  SIMPLE_SOCKET_SET_DNS_SERVER  SetDNSServer;
  SIMPLE_SOCKET_SET_IO_TIMEOUT  SetIOTimeout;
	EFI_HANDLE                    ImgHandle;
} SIMPLE_SOCKET_PROTOCOL;

extern EFI_GUID               gSimpleSocketProtocolGuid;

#endif