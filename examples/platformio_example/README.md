# Iotex-arduino PlatformIO example

## Summary

Minimal example of how using the library to create a new account and query info from the blockchain

## Usage

1. Create a file named `secrets.h` inside the `src` directory
2. Add the required definitions to it. Eg:

```
#ifndef SECRETS_H
#define SECRETS_H

#define PRIVATE_KEY "thePrivateKeyAsAHexString"
#define WIFI_SSID   "WifiSsid"
#define WIFI_PASS   "WifiPassword"

#endif
```

3. Build and upload using PlatformIO VSCode extension, or Platformio cli
