#!/bin/bash

HOST=""
USER="pi"
PASS=""
SERVICES=()
REMOTE_DIR="~/deployment_build"
ARCHIVE_NAME="ccu_source.tar.gz"

usage() {
	echo "Usage: $0 -h <ip> -p <password> -s <service1> [service2 ...]"
	exit 1
}


while [[ $# -gt 0 ]]; do
	case $1 in
		-h|--host)
			HOST="$2"
			shift 2
			;;
		-p|--password)
			PASS="$2"
			shift 2
			;;
		-u|--user)
			USER="$2"
			shift 2
			;;
		-s|--services)
			shift
			while [[ $# -gt 0 ]] && [[ ! "$1" =~ ^- ]]; do
				SERVICES+=("$1")
				shift
			done
			;;
		*)
			echo "ERR : Unknown option: $1"
			usage
			;;
	esac
done


if [ -z "$HOST" ] || [ -z "$PASS" ] || [ ${#SERVICES[@]} -eq 0 ]; then
	echo "ERR : Host, password and services are required."
	usage
fi

echo "DEPLOY TO $USER@$HOST ..."
echo "CLEAN BUILD LOCALLY ..."
make distclean >/dev/null

echo "TAR SOURCE FILES ..."
tar --exclude='.git' --exclude='build' --exclude='*.o' --exclude='*.tar.gz' -czf "$ARCHIVE_NAME" .

if [ ! -f "$ARCHIVE_NAME" ]; then
	echo "ERR : Cannot create source archive."
	exit 1
fi

echo "SCP FILES TO REMOTE"
sshpass -p "$PASS" scp -o StrictHostKeyChecking=no "$ARCHIVE_NAME" "$USER@$HOST:~/$ARCHIVE_NAME"

rm "$ARCHIVE_NAME"


echo "REMOTE DEPLOYMENT ..."
# 1. On supprime des eventuels résidus de déploiements précédents, on crée le dossier de déploiement
# 2. On décompresse l'archive dans le dossier de déploiement
# 3. On supprime l'archive
# 4. On se place dans le dossier de déploiement
# 5. On rend le script d'installation exécutable
# 6. On exécute le script d'installation avec sudo et les services en arguments
# 7. On espère que ça marche.
REMOTE_CMD="
echo '$PASS' | sudo -S rm -rf $REMOTE_DIR && \
mkdir -p $REMOTE_DIR && \
tar -xzf ~/$ARCHIVE_NAME -C $REMOTE_DIR && \
rm ~/$ARCHIVE_NAME && \
cd $REMOTE_DIR && \
chmod +x install.sh && \
echo '$PASS' | sudo -S ./install.sh ${SERVICES[*]}
"

sshpass -p "$PASS" ssh -t -o StrictHostKeyChecking=no "$USER@$HOST" "$REMOTE_CMD"