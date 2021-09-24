#!/bin/bash

../src/extern/nanopb/generator/nanopb_generator.py -I../src/protobuf_files -D../src/protobuf_files/c_files action.proto  blockchain.proto
