#!/bin/bash
# Ensure the Docker daemon is running in WSL2.
# Prefer the persistent systemd service; fall back to a manual start only if
# systemd is unavailable or the service cannot be started.

if sudo docker info > /dev/null 2>&1; then
    echo "Docker is already running."
    exit 0
fi

if command -v systemctl > /dev/null 2>&1; then
    echo "Starting Docker service..."
    sudo systemctl start docker

    if sudo docker info > /dev/null 2>&1; then
        echo "Docker service started successfully."
        exit 0
    fi
fi

echo "Falling back to manual dockerd startup..."
sudo nohup dockerd --iptables=false --ip6tables=false --bridge=none \
    > /tmp/dockerd.log 2>&1 &
sleep 3

if sudo docker info > /dev/null 2>&1; then
    echo "Docker started successfully."
else
    echo "Failed to start Docker. Check /tmp/dockerd.log"
    exit 1
fi
