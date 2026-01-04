#!/bin/bash

PROJECT_NAME="ccu"
LIB_DEST="/usr/lib/$PROJECT_NAME"
CONF_DEST="/etc/$PROJECT_NAME"
SYSTEMD_DEST="/etc/systemd/system"
LOG_DEST="/var/log/$PROJECT_NAME"
FORCE_CONFIG_OVERWRITE=false
SERVICES=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -f|--force)
            FORCE_CONFIG=true
            shift
            ;;
        *)
            SERVICES+=("$1")
            shift
            ;;
    esac
done


if [ "$EUID" -ne 0 ]; then
  echo -e "ERR : Ce script doit être exécuté en tant que root. Utilisez sudo."
  exit 1
fi

if [ ${#SERVICES[@]} -eq 0 ]; then
    echo -e "ERR : Aucun service spécifié. Usage: ./install [-f] service1 service2 ..."
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
    if [ ! -f "$DEST_FILE" ] || [ "$FORCE_CONFIG" = true ]; then
        cp "$FILE" "$DEST_FILE"
        echo "COPY CONF FILE $(basename "$FILE")"
    else
        echo "SKIP EXISTING CONF FILE $(basename "$FILE") (Use -f to overwrite)"
    fi
done
echo "CONF OK"

echo -e "LOG DIR IN $LOG_DEST ..."
mkdir -p "$LOG_DEST"
echo "LOG DIR OK"

echo -e "INSTALL SERVICES IN $SYSTEMD_DEST ..."

for SERVICE in "${SERVICES[@]}"; do
    SERVICE_FILE="services/${SERVICE}.service"
    BIN_SOURCE="bin/$SERVICE"
    BIN_DEST="/usr/local/bin/$SERVICE"
    
    if [ -f "$SERVICE_FILE" ] && [ -f "$BIN_SOURCE" ]; then
        echo "--- HANDLING $SERVICE ---"

        if systemctl is-active --quiet "$SERVICE"; then
            echo "Stopping existing service $SERVICE..."
            systemctl stop "$SERVICE"
        fi

        echo "Installing binary to $BIN_DEST..."
        cp "$BIN_SOURCE" "$BIN_DEST"

        echo "Installing service file..."
        cp "$SERVICE_FILE" "$SYSTEMD_DEST/"

        systemctl daemon-reload
        systemctl enable "$SERVICE"
        systemctl start "$SERVICE"
        
        if systemctl is-active --quiet "$SERVICE"; then
             echo "SUCCESS: Service $SERVICE STARTED"
        else
             echo "WARNING: Service $SERVICE does not seem to have started."
             journalctl -u "$SERVICE" -n 5 --no-pager
        fi
    else
        echo "ERROR: Service file ($SERVICE_FILE) or Binary ($BIN_SOURCE) not found."
    fi
done

echo "INSTALLATION DONE."
exit 0