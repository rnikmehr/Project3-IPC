#!/usr/bin/env bash

# executable variables
#
SERVER="csv-server"
#
CLIENT="csv-client"

# execution parameters
#
FILE="test_input.csv"
SEARCH_STR1="industrial x farm x repay"
SEARCH_STR2="professional x apartment x default"

# output variables
#
SERVER_LOG="test1_server.txt"
SERVER_FILES="$SERVER $SERVER_LOG"
#
CLIENT_LOG="test1_client.txt"
CLIENT_FILES="$CLIENT $CLIENT_LOG"

echo "Test 1"

# start server
#
./$SERVER &> $SERVER_LOG &
sleep 1  # allow 1 second for initialization (much too long)
SERVER_PID=$!  # remember PID to kill later

# client test 1
#
./$CLIENT $FILE $SEARCH_STR1 &> $CLIENT_LOG
sleep 1  # allow 1 seconds for interaction (much too long)


# client test 2
#
./$CLIENT $FILE $SEARCH_STR2 &>> $CLIENT_LOG
sleep 1  # allow 1 second for interaction (much too long)

ps -p $SERVER_PID > /dev/null  # ask ps is server running
if [ $? ]  # kill server, because it should be running
then
  kill $SERVER_PID
fi

