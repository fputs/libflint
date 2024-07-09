set -e

#./testrunner
./tcptest &
tcpout=$(echo "hello" | nc localhost 18632)
echo "tcpout: $tcpout" 
if [ "$tcpout" != "TEST SEND" ]; then
 echo "Error: \"$tcpout\" != \"TEST SEND\""
 exit 1
fi

exit 0
