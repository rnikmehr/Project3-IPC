#!/usr/bin/env bash

# executable variables
#
SERVER="csv-server"
#
CLIENT="csv-client"

# execution parameters
#
FILE="test_input.csv"
SEARCH_STR1="17 + 51 + 44"
SEARCH_STR2="66400 + 3.78 + 79700"

# output variables
#
SERVER_LOG="test2_server.txt"
SERVER_FILES="$SERVER $SERVER_LOG"
#
CLIENT_LOG="test2_client.txt"
CLIENT_FILES="$CLIENT $CLIENT_LOG"

echo "Test 2"

# start server
#
./$SERVER &> $SERVER_LOG &
SERVER_PID=$!  # remember PID to kill later
sleep 1  # allow 1 second for initialization (much too long)


# client test 1
#
# echo "./$CLIENT $SOCKET_NAME $FILE $SEARCH_STR1 &> $CLIENT_LOG"
./$CLIENT $SOCKET_NAME $FILE $SEARCH_STR1 &> $CLIENT_LOG
sleep 1  # allow 1 second for interaction (much too long)


# client test 2
#
# echo "./$CLIENT $SOCKET_NAME $FILE $SEARCH_STR2 &>> $CLIENT_LOG"
./$CLIENT $SOCKET_NAME $FILE $SEARCH_STR2 &>> $CLIENT_LOG
sleep 1  # allow 1 seconds for interaction (much too long)


ps -p $SERVER_PID > /dev/null  # ask ps is server running
if [ $? ]  # kill server, because it should be running
then
  kill $SERVER_PID
fi

