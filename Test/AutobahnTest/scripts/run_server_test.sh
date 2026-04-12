#!/bin/bash
#
# Test the WebSocket ECHO SERVER running on Windows.
#
# Prerequisites:
#   1. Docker is running in WSL2 (run start_docker.sh first)
#   2. EchoServer.exe is running on Windows, listening on port 9001
#
# Autobahn acts as a fuzzing CLIENT: it connects to your server,
# sends crafted frames, and checks the server's responses.
#
# Results: open reports/server/index.html in a browser.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TEST_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== Autobahn|Testsuite: Testing your WebSocket SERVER ==="
echo ""
echo "Make sure EchoServer.exe is running on Windows (port 9001)."
echo ""

sudo docker run --rm \
    --network host \
    -v "$TEST_DIR/config:/config:ro" \
    -v "$TEST_DIR/reports/server:/reports/server" \
    crossbario/autobahn-testsuite \
    wstest -m fuzzingclient -s /config/fuzzingclient.json

echo ""
echo "Done. Open reports/server/index.html to view results."
