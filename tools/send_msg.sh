#!/bin/bash

PORT=/dev/ttyUSB2
PHONE=15899962740

echo "Send a message to ${PHONE}"
echo "Content: $1"

echo -e "ATI\r\n" > ${PORT}
sleep 1
echo -e "AT+CPIN?\r\n" > ${PORT}
sleep 1
echo -e "AT+CCID\r\n" > ${PORT}
sleep 1
echo -e "AT+CSQ\r\n" > ${PORT}
sleep 1
echo -e "AT+CMGF=1\r\n" > ${PORT}
sleep 1
echo -e "AT+CSCS=\"GSM\"\r\n" > ${PORT}
sleep 1
echo -e "AT+CMGS=\"${PHONE}\"\r\n" > ${PORT}
sleep 2
if [ -n "$1" ]; then
    echo -e "$1\x1a" > ${PORT}
else
    echo -e "Hello, World! From EC20\x1a" > ${PORT}
fi
sleep 1
echo "Done!"

