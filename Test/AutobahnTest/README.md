# Autobahn Test Runbook

This directory contains the Windows and WSL2 setup that was validated against
the current WebSockets library and Autobahn configuration on 2026-04-17.

The setup assumes:

- Windows host path: `C:\Users\Giuliano\Documents\Embarcadero\Studio\Projects\WebSockets`
- WSL distro: `Ubuntu-22.04`
- WSL repo path: `/mnt/c/Users/Giuliano/Documents/Embarcadero/Studio/Projects/WebSockets`
- RAD Studio environment script:
  `C:\Program Files (x86)\Embarcadero\Studio\37.0\bin\rsvars.bat`
- MSBuild executable:
  `C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe`

## 1. One-time Windows WSL networking setup

Edit `C:\Users\Giuliano\.wslconfig` so it contains:

```ini
[wsl2]
networkingMode = mirrored
dnsTunneling = true
```

Then restart WSL from Windows PowerShell:

```powershell
wsl --shutdown
```

This is what made the WSL Docker container reachable from Windows through
`localhost`.

## 2. One-time Docker setup inside WSL

Open Ubuntu and install Docker:

```bash
sudo apt-get update
sudo apt-get install -y docker.io
```

Create the systemd override that works in this WSL environment:

```bash
sudo mkdir -p /etc/systemd/system/docker.service.d
sudo tee /etc/systemd/system/docker.service.d/override.conf > /dev/null <<'EOF'
[Service]
ExecStart=
ExecStart=/usr/bin/dockerd --containerd=/run/containerd/containerd.sock --iptables=false --ip6tables=false --bridge=none
EOF
```

Important: if `/etc/docker/daemon.json` also sets `bridge`, `iptables`, or
`ip6tables`, Docker can fail to start because the same options are being passed
twice. Either remove those keys or move the file out of the way.

Enable the daemon and pull the Autobahn image:

```bash
sudo systemctl daemon-reload
sudo systemctl enable docker
sudo systemctl start docker
sudo docker pull crossbario/autobahn-testsuite
```

Verify the stable daemon state:

```bash
systemctl is-active docker
ps -ef | grep '[d]ockerd'
docker info --format '{{.ServerVersion}}'
docker ps
```

Expected behavior:

- `systemctl is-active docker` prints `active`
- `dockerd` is running with `--iptables=false --ip6tables=false --bridge=none`
- `docker ps` is empty unless you started a test container yourself

Only the Docker daemon is persistent. The Autobahn containers do not auto-start.

## 3. Build the release test binaries from Windows

Run these from Windows PowerShell in the repository root:

```powershell
cmd /c "\"C:\Program Files (x86)\Embarcadero\Studio\37.0\bin\rsvars.bat\" && \"C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe\" Test\AutobahnTest\EchoServer\TestEchoServer.cbproj /t:Build /p:Config=Release /p:Platform=Win64x"
cmd /c "\"C:\Program Files (x86)\Embarcadero\Studio\37.0\bin\rsvars.bat\" && \"C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe\" Test\AutobahnTest\TestClient\TestClient.cbproj /t:Build /p:Config=Release /p:Platform=Win64x"
```

Outputs:

- `Test\AutobahnTest\EchoServer\Win64x\Release\TestEchoServer.exe`
- `Test\AutobahnTest\TestClient\Win64x\Release\TestClient.exe`

## 4. Server-role Autobahn run

This tests the Windows echo server. Autobahn acts as the client.

1. Start the echo server from Windows PowerShell:

```powershell
.\Test\AutobahnTest\EchoServer\Win64x\Release\TestEchoServer.exe
```

1. In another Windows PowerShell window, start Docker if needed:

```powershell
wsl -d Ubuntu-22.04 bash -lc "cd /mnt/c/Users/Giuliano/Documents/Embarcadero/Studio/Projects/WebSockets/Test/AutobahnTest && ./scripts/start_docker.sh"
```

1. Run the server test suite:

```powershell
wsl -d Ubuntu-22.04 bash -lc "cd /mnt/c/Users/Giuliano/Documents/Embarcadero/Studio/Projects/WebSockets/Test/AutobahnTest && ./scripts/run_server_test.sh"
```

Reports are written to:

- `Test\AutobahnTest\reports\server\index.html`
- `Test\AutobahnTest\reports\server\index.json`

## 5. Client-role Autobahn run

This tests the Windows client. Autobahn acts as the server.

1. In Windows PowerShell, start the Autobahn fuzzing server:

```powershell
wsl -d Ubuntu-22.04 bash -lc "cd /mnt/c/Users/Giuliano/Documents/Embarcadero/Studio/Projects/WebSockets/Test/AutobahnTest && ./scripts/run_client_test.sh"
```

1. In another Windows PowerShell window, run the release client:

```powershell
.\Test\AutobahnTest\TestClient\Win64x\Release\TestClient.exe
```

1. After the client finishes, stop the WSL fuzzing server with `Ctrl+C` in the
first PowerShell window.

Reports are written to:

- `Test\AutobahnTest\reports\client\index.html`
- `Test\AutobahnTest\reports\client\index.json`

## 6. Known-good results from the validated run

The latest successful run produced:

- Server role: `247 total`, `240 OK`, `4 NON-STRICT`, `3 INFORMATIONAL`,
  `0 FAILED`
- Client role: `247 total`, `240 OK`, `4 NON-STRICT`, `3 INFORMATIONAL`,
  `0 FAILED`

Both validated profiles are limited to categories `1.*` through `7.*` and
`10.*`. Performance tests (`9.*`) and compression-extension tests (`12.*`,
`13.*`) are intentionally excluded.

## 7. Notes

- `config/fuzzingclient.json` and `config/fuzzingserver.json` are both pinned to
  `ws://localhost:9001`
- generated files under `reports/client/` and `reports/server/` are ignored by
  git; keep or refresh them locally as needed
- `scripts/start_docker.sh` prefers `systemctl start docker` and only falls back
  to a manual `dockerd` launch if needed
- If Docker starts failing again after a configuration change, check for a
  duplicate `bridge`, `iptables`, or `ip6tables` setting in
  `/etc/docker/daemon.json`
