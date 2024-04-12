# UDP to TCP Relay

This program is a UDP to TCP relay that accepts data blocks via UDP and sends them via TCP to a server. It is designed to handle specific requirements and provides reliable data transfer between UDP and TCP protocols.

## Features

- Accepts data blocks via UDP and sends them via TCP to a server
- Each UDP packet can contain only one data block ranging from 16 to 128 bytes in length
- Adds a prefix of four characters (specified as a parameter) to each data block before sending it to the server
- Maintains a permanent TCP connection with the server and automatically re-establishes the connection in case of failure
- Discards UDP data if it arrives when the TCP connection is not established
- Ignores any data received from the TCP server
- Logs important events, such as establishing or breaking TCP connections, data transmission or reception errors, etc.

## Requirements

- Linux operating system
- GCC compiler
- Make build system

## Building the Program

To build the program, navigate to the project directory and run the following command:

```
make
```

This will compile the source files and generate the executable `udp_tcp_relay`.

## Running the Program

To run the program, use the following command:

```
./udp_tcp_relay <udp_ip:port> <tcp_ip:port> <log_file> <prefix>
```

- `<udp_ip:port>`: The IP address and port number for receiving UDP data blocks.
- `<tcp_ip:port>`: The IP address and port number of the TCP server to send modified data blocks to.
- `<log_file>`: The path to the log file where important events will be logged.
- `<prefix>`: The four characters to be added as a prefix to the transmitted data blocks.

## Test Server Programs

The repository also includes test server programs located in the `test_servers` folder. These programs can be used to simulate the UDP client and TCP server for testing purposes.

To build the program 

To run the test servers, open a new terminal window, navigate to the `test_servers` folder, and run the following command:

```
make
```

This will build TCP server and UDP server


```
make run
```

This will start the TCP server and run the UDP client simultaneously.

To stop the test servers, use the following command:

```
make stop
```

This will terminate the running TCP server process.

## Folder Structure

- `include/`: Contains the header files
- `src/`: Contains the source code files
- `obj/`: Contains the object files generated during compilation
- `test_servers/`: Contains the test server programs (UDP client and TCP server)
