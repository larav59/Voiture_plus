#!/bin/bash
trap "kill 0" SIGINT SIGTERM EXIT

echo "Démarrage des ponts SOCAT..."

echo "[-] Création /dev/ttyAMA0_PY <-> /dev/ttyAMA0"
socat -d -d pty,raw,echo=0,link=/dev/ttyAMA0_PY,mode=777 pty,raw,echo=0,link=/dev/ttyAMA0,mode=777 &

echo "[-] Création /dev/ttyARDUINO <-> /dev/serial0"
socat -v -x pty,raw,echo=0,link=/dev/ttyARDUINO,mode=777 pty,raw,echo=0,link=/dev/serial0,mode=777 &

echo "CTRL+C pour arrêter."

wait