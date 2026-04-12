#!/bin/bash
# Start the Docker daemon in WSL2.
# Docker Engine was installed without systemd, so we start it manually.
# Run this once after each WSL2 restart.

if sudo docker info > /dev/null 2>&1; then
    echo "Docker is already running."
else
    echo "Starting Docker daemon..."
    sudo nohup dockerd --iptables=false --ip6tables=false --bridge=none \
        > /tmp/dockerd.log 2>&1 &
    sleep 3

    if sudo docker info > /dev/null 2>&1; then
        echo "Docker started successfully."
    else
        echo "Failed to start Docker. Check /tmp/dockerd.log"
        exit 1
    fi
fi
