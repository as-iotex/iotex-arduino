# IoTeX-client

A simple C++ API client implementation in for the IoTeX Blockchain
Compatible with some Arduino boards and Raspberry Pi/Unix environments

## Building

### Building from command line (desktop environment)

`./build.sh`

### Building in VSCode (desktop environemnt)

Press <kbd>F1</kbd> to open the terminal and write `Tasks: Run task`  
Select the task `Arduino - Run unit tests`

### Building with PlatformIO (Arduino environement)

See examples folder. Each example contains it's respective `platformio.ini` and `README.md` files

### Building with Arduino

Copy the library into your Arduino libraries directory. See https://www.arduino.cc/en/guide/libraries and restart Arduino  
You can open any of the examples from File->Examples->IoTeX-client, configure your board and build/upload

## VSCode

The project contains VSCode build and launch configurations in ./vscode
In order to launch in VSCode, C++ extensions need installed

## Running the tests

### Running the tests from command line

`./build/tests/iotex_unit_tests`

### Running the tests in VSCode

Use the provided `Unit tests - launch` configuration

## Library usage

See examples in the examples folder

The user flow is the following:

1. Create the connection:  
   `Connection<Api> connection(tIp, tPort, tBaseUrl);`
2. Now you are ready to interact with the blockchain by calling any api methods. Eg:  
   `ResultCode result = connection.api.wallets.getBalance(tAccount, balance);`
