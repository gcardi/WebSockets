#!/bin/bash
#
# Test the WebSocket CLIENT running on Windows.
#
# Prerequisites:
#   1. Docker is running in WSL2 (run start_docker.sh first)
#
# Autobahn acts as a fuzzing SERVER: it listens on port 9001,
# your client connects to it and echoes messages back.
#
# Steps:
#   1. Run this script (it starts the fuzzing server and waits)
#   2. Run TestClient.exe on Windows
#   3. After TestClient finishes, press Ctrl+C here to stop the server
#
# Results: open reports/client/index.html in a browser.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TEST_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== Autobahn|Testsuite: Testing your WebSocket CLIENT ==="
echo ""
echo "Starting Autobahn fuzzing server on port 9001..."
echo "After it starts, run TestClient.exe on Windows."
echo "Press Ctrl+C when done to stop the server."
echo ""

sudo docker run --rm \
    --network host \
    -v "$TEST_DIR/config:/config:ro" \
    -v "$TEST_DIR/reports/client:/reports/client" \
    crossbario/autobahn-testsuite \
    wstest -m fuzzingserver -s /config/fuzzingserver.json
