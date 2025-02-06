#!/bin/bash
exec 3<>/dev/tcp/localhost/6667
    sleep 1
    echo "PASS RPR" >&3
	sleep 1
    echo "NICK GAB" >&3
	sleep 1
	echo "USER 0 * :realname GAB" >&3
	sleep 1
    echo "JOIN #BIKE" >&3
	
# Ler saída do servidor em background
cat <&3 &

# Permitir entrada interativa
while true; do
    read -r input
    echo "$input" >&3
done