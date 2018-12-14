#!/bin/bash

freeze_iperf() {
        server_pid=$1
        client_pid=$2

        # freeze
        echo "echo 1 > /proc/$server_pid/freeze"
        echo "echo 1 > /proc/$client_pid/freeze"
        echo 1 > /proc/$server_pid/freeze
        echo 1 > /proc/$client_pid/freeze
        
        sleep 8 

        # unfreeze
        echo "echo 0 > /proc/$server_pid/freeze"
        echo "echo 0 > /proc/$client_pid/freeze"
        echo 0 > /proc/$server_pid/freeze
        echo 0 > /proc/$client_pid/freeze
}

TIME=40
NUM_PAUSE=10
BEFORE_PAUSE=10
rm Time${TIME}.client Time${TIME}.server

tc qdisc add dev lo root handle 1: htb default 12
tc class add dev lo parent 1:1 classid 1:12 htb rate 100mbit burst 1k
tc qdisc add dev lo parent 1:12 netem delay 1ms

setsid iperf3 -s --logfile Time${TIME}.server &
server_pid=$!

# server enter virtual time
echo "echo 1000 > /proc/$server_pid/dilation"
echo 1000 > /proc/$server_pid/dilation
sleep 1

setsid iperf3 -c 127.0.0.1 -t $TIME --logfile Time${TIME}.client &
#setsid iperf3 -c 127.0.0.1 -t $TIME -b 100m --logfile Time${TIME}.client &
client_pid=$!

# client enter virtual time
echo "echo 1000 > /proc/$client_pid/dilation"
echo 1000 > /proc/$client_pid/dilation
sleep $BEFORE_PAUSE

#freeze_iperf $server_pid $client_pid
#sleep 2
#freeze_iperf $server_pid $client_pid
#sleep 2
#freeze_iperf $server_pid $client_pid
#sleep 2
#freeze_iperf $server_pid $client_pid
#sleep 2

for ((i = 0; i < $NUM_PAUSE; ++i)) {
        freeze_iperf $server_pid $client_pid
        sleep 2
}

wait $client_pid
kill -s SIGKILL $server_pid

tc qdisc del dev lo root

cat Time${TIME}.client

