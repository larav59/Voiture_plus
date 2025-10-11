#!/bin/bash

# Usage function 
usage() {
  echo "Usage: $0 -h <host> -u <user> -k <key> -n <build_version> -m <build_description>"
  exit 1
}

# Parse arguments
while getopts ":h:u:k:n:m:" opt; do
  case $opt in
    h) REMOTE_HOST="$OPTARG"
    ;;
    u) REMOTE_USER="$OPTARG"
    ;;
    k) SSH_KEY="$OPTARG"
    ;;
    n) BUILD_NUMBER="$OPTARG"
    ;;
    m) BUILD_DESCRIPTION="$OPTARG"
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
        usage
    ;;
  esac
done

# Check if all parameters are provided
if [ -z "$REMOTE_HOST" ] || [ -z "$REMOTE_USER" ] || [ -z "$SSH_KEY" ] || [ -z "$BUILD_NUMBER" ] || [ -z "$BUILD_DESCRIPTION" ]; then
  echo "Error: Missing parameters" >&2
  usage
fi

# Variables
if [ -z "$APP_NAME" ]; then
  APP_NAME="album-api"
fi

PROJECT_DIR=$(pwd)
DIST_DIR="$PROJECT_DIR/dist"
ARCHIVE_FILE="$APP_NAME-${BUILD_NUMBER}.tar.gz"
REMOTE_DIR="/home/$REMOTE_USER/$APP_NAME"

# Clear build folder
echo "[$(date)] - Clearing dist folder..."
rm -rf $DIST_DIR

# Build TypeScript project
echo "[$(date)] - Building TypeScript project..."
npm run build
if [ $? -ne 0 ]; then
  echo "[$(date)] - Error during build. Exiting script."
  exit 1
fi
echo "[$(date)] - Build successful."

# Create ZIP archive with build files
echo "[$(date)] - Creating Archive..."
# Transfer dist, package.json, public,
tar -czf $ARCHIVE_FILE -C $PROJECT_DIR dist package.json public
if [ $? -ne 0 ]; then
  echo "[$(date)] - Error during archive creation. Exiting script."
  exit 1
fi
echo "[$(date)] - Archive created."

# 4. Transfert de l'archive vers le serveur distant via SCP
echo "[$(date)] - Transfer archive file to $REMOTE_USER@$REMOTE_HOST:$REMOTE_DIR..."
scp -i $SSH_KEY $ARCHIVE_FILE $REMOTE_USER@$REMOTE_HOST:$REMOTE_DIR
if [ $? -ne 0 ]; then
  echo "[$(date)] - Error during file transfer. Exiting script."
  exit 1
fi
echo "[$(date)] - File transfer successful."

# Connect to remote server and deploy
echo "[$(date)] - Connecting to remote server $REMOTE_USER@$REMOTE_HOST using SSH key $SSH_KEY..."
ssh -i $SSH_KEY $REMOTE_USER@$REMOTE_HOST << EOF
  cd $REMOTE_DIR

	# Extract archive
	echo "[$(date)] - Extract archive..."
	tar -xzf $ARCHIVE_FILE

  # Install dependencies
	rm -rf node_modules
    npm install --omit=dev
    echo "[$(date)] - Dependencies installed."

  # Clean up files
	echo "[$(date)] - Cleaning up files..."
    rm $ARCHIVE_FILE

	# Add build info to an Build.txt file
	echo "Build number: $BUILD_NUMBER
	Build description: $BUILD_DESCRIPTION" > Build.txt

	# Restart PM2 process
	echo "[$(date)] - Restarting PM2 process..."
	pm2 restart ../ecosystem.config.js --only $APP_NAME --env production
		
	echo "[$(date)] - Deployment successful."
EOF

# Clean up local archive
echo "[$(date)] - Cleaning up local archive..."
rm -f $ARCHIVE_FILE

echo "[$(date)] - Deployment successful."