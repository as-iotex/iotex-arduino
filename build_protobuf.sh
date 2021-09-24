#!/bin/bash

src/extern/nanopb/generator/nanopb_generator.py -Isrc/protobuf_files -Dsrc/protobuf_files/c_files action.proto  blockchain.proto
