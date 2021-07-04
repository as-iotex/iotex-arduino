#!/bin/bash

extern/nanopb/generator/nanopb_generator.py -Iprotobuf -Dprotobuf/c_files action.proto  blockchain.proto
