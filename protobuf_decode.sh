#!/bin/bash

protoc -Iprotobuf --decode=ActionCore action.proto < encoded_actioncore.bin