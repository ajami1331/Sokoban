#!/bin/sh
CONTAINER_TAG=$1
OUTPUT_DIR=$2
docker create --name dummy $CONTAINER_TAG
docker cp dummy:/app/releases $OUTPUT_DIR
docker rm -f dummy
