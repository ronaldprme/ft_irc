#!/bin/bash
NICK="GIO"
exec 3<>/dev/tcp/localhost/6667
sleep 1
# Enviar comandos iniciais
    echo "PASS pass" >&3
	sleep 1
    echo "NICK $NICK" >&3
	sleep 1
	echo "USER $NICK 0 * :$NICK" >&3
	sleep 1
    echo "JOIN #PNL" >&3
	sleep 1
	echo "PRIVMSG #PNL :LEMBREM-SE DE QUE O MAPA NAO É O TERRITORIO" >&3
	echo "JOIN #NS" >&3
	echo "PRIVMSG #NS :E AI PAI, TUDO BEM?" >&3
	echo "PRIVMSG #PNL :LEMBREM-SE DE QUE O MAPA NAO É O TERRITORIO" >&3
# Ler saída do servidor em background
cat <&3 &

# Permitir entrada interativa
while true; do
    read -r input
    echo "$input" >&3
done