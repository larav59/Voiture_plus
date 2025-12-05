#!/bin/bash

PROJECT_NAME="ccu"
LIB_DEST="/usr/lib/$PROJECT_NAME"
CONF_DEST="/etc/$PROJECT_NAME"
LOG_DEST="/var/log/$PROJECT_NAME"


if [ "$EUID" -ne 0 ]; then
  echo -e "ERR : Ce script doit être exécuté en tant que root. Utilisez sudo."
  exit 1
fi

if [ $# -eq 0 ]; then
	echo -e "ERR : Aucun service spécifié. Usage: ./install service1 service2 ..."
	exit 1
fi

echo "UNINSTALL ${PROJECT_NAME}..."

if [ $# -gt 0 ]; then
	for SERVICE in "$@"; do
		echo "UNINSTALL SERVICE $SERVICE"
		systemctl stop "$SERVICE" 2>/dev/null
		systemctl disable "$SERVICE" 2>/dev/null
		rm "/etc/systemd/system/$SERVICE.service" 2>/dev/null
		
		rm "/usr/local/bin/$SERVICE" 2>/dev/null
	done
	systemctl daemon-reload
else
	echo -e "ERR : Aucun service spécifié. Usage: ./uninstall service1 service2 ..."
	exit 1
fi

echo "CLEAN FILES..."

rm -rf "$LIB_DEST"
rm -rf "$CONF_DEST"

rm -rf "$LOG_DEST"

echo "UNINSTALLATION DONE."