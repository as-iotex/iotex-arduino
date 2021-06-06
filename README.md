# Iotex arduino

## Building

### Building from command line

`./build.sh`

### Building in VSCode

Press <kbd>F1<kbd> to open the terminal and write `Tasks: Run task`
Select the task `Arduino - Run unit tests`

## VSCode

The project contains VSCode build and launch configurations in ./vscode
In order to launch in VSCode, C++ extensions need installed

## Running the tests

### Running the tests from command line

`./build/tests/iotex_unit_tests`

### Running the tests in VSCode

Use the provided `Unit tests - launch` configuration

## Milestone 1 - JSON-RPC over HTTP/S

[x] JSON-RPC parser/serializer  
    - `Iotex::api::rpc::` contains helper methods that construct the RPC request for each of the supported api endpoints.  
    - `Iotex::ResponseTypes::` contains helper methods for parsing JSON responses into C++ structs
  
[x] *IConnection:* This is an Interface for a HTTP/HTTPs connection. Possibly a mock implementation or an implementation for one board (eg. ESP32)  
    Implemented in `Iotex::Connection`, which contains a platform specific `Iotex::Host` instance, selected at compile time  
    The proper implementation of `Iotex::Host` is compiled based on the current platform. This is currently done by checking for a compiler flag (see `#ifdef OS` in `/src/http/os/http.cpp`)  
    `/src/http/os/http.cpp` contains an implementation of the HTTP class which works on Linux OS by using curl  
    A different implementation will be provided later on for each of the supported boards (ESP32, ESP8266, etc.)  

[x] *Api:* This is a class/interface for the IoTeX JSON-RPC api. Will implement the following methods, which construct the payload using the JSON-RPC parser, send a request over HTTP using IConnection, parse the response and return it in an adequate struct/class. Implements the following methods (initially)  
    Implemented in `Iotex::Api`  
    The full api divided in "subsections". Eg. the methods created for this milestone are encapsulated inside the `Iotex::api::Wallets` class  

## Library usage

Sample scenarios for the endpoints supported in milestone one are provided in `tests/src/api/wallets/walletsTests.cpp`
The user flow is the following:

1. Create the connection:
   `Connection<Api> connection(tIp, tPort, tBaseUrl);`
2. Now you are ready to interact with the blockchain by calling any api methods. Eg:
   `ResultCode result = connection.api.wallets.getBalance(tAccount, balance);`