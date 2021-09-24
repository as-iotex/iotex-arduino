#!/bin/bash

protoc -Isrc/protobuf_files --decode=ActionCore action.proto < encoded_actioncore.bin