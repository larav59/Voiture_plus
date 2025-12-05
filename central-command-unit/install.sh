#!/bin/bash

PROJECT_NAME="ccu"
LIB_DEST="/usr/lib/$PROJECT_NAME"
CONF_DEST="/etc/$PROJECT_NAME"
SYSTEMD_DEST="/etc/systemd/system"
LOG_DEST="/var/log/$PROJECT_NAME"


if [ "$EUID" -ne 0 ]; then
  echo -e "ERR : Ce script doit être exécuté en tant que root. Utilisez sudo."
  exit 1
fi

if [ $# -eq 0 ]; then
	echo -e "ERR : Aucun service spécifié. Usage: ./install service1 service2 ..."
	exit 1
fi

echo "INSTALL ${PROJECT_NAME}..."
if make; then
	echo "MAKE OK"
else
	echo -e "ERR : Erreur lors de la compilation."
	exit 1
fi

echo -e "INSTALL LIBS IN $LIB_DEST ..."
mkdir -p "$LIB_DEST"
cp -r lib/* "$LIB_DEST/"
echo "LIBS OK"

echo -e "INSTALL CONF IN $CONF_DEST ..."
mkdir -p "$CONF_DEST"

# On ne copie uniquement les fichiers de configuration qui n'existe pas déjà (évite d'écraser les configs personnalisées)
for FILE in config_model/*; do
	DEST_FILE="$CONF_DEST/$(basename "$FILE")"
	if [ ! -f "$DEST_FILE" ]; then
		cp "$FILE" "$DEST_FILE"
		echo "COPY CONF FILE $(basename "$FILE")"
	else
		echo "SKIP EXISTING CONF FILE $(basename "$FILE")"
	fi
done

echo "CONF OK"

echo -e "LOG DIR IN $LOG_DEST ..."
mkdir -p "$LOG_DEST"
echo "LOG DIR OK"

echo -e "INSTALL SERVICES IN $SYSTEMD_DEST ..."

for SERVICE in "$@"; do
	SERVICE_FILE="services/${SERVICE}.service"
	cp "bin/$SERVICE" "/usr/local/bin/$SERVICE"
	
	if [ -f "$SERVICE_FILE" ]; then
		echo "INSTALL SERVICE $SERVICE"
		cp "$SERVICE_FILE" "$SYSTEMD_DEST/"

		systemctl daemon-reload
		systemctl enable "$SERVICE"
		systemctl restart "$SERVICE"
		
		if systemctl is-active --quiet "$SERVICE"; then
			 echo "SERVICE $SERVICE STARTED"
		else
			 echo "WARNING: Service $SERVICE does not seem to have started."
		fi

	else
		echo "ERROR: The file $SERVICE_FILE does not exist."
	fi
done

echo "INSTALLATION DONE."
exit 0