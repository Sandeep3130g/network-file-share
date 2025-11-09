# Network-Based File Sharing System (Server–Client Model)
Capstone Project Report  
Author: Sandeep Kumar Samal
Batch : 4  
Year: 2025  

---

## 1. Title
Network-Based File Sharing System (Server–Client Model)

---

## 2. Introduction
This capstone project implements a distributed file-sharing system using a Client–Server architecture over TCP sockets.  
The server stores files and allows multiple clients to:
- List available files
- Download files
- Upload files
## Network File Share — Server/Client (C++ / CMake)

A simple TCP-based file sharing example (capstone-style). The server stores files in a local directory and multiple clients can list, download, and upload files using a small line-based protocol.

Author: Your Name (replace this)  
Date: 2025

---

## Quick facts
- Language: C++17
- Build: CMake
- Platform: cross-platform (Linux/WSL/macOS/Windows — see notes)
- Storage directory (server): `server_files/` (created automatically by the server)

---

## What this repo contains

- `client/` — client implementation (`client.cpp`) and `CMakeLists.txt`
- `server/` — server implementation (`server.cpp`) and `CMakeLists.txt`
- `common/` — shared headers (socket helpers in `protocol.hpp`)
- `CMakeLists.txt` — top-level project configuration

---

## Build (recommended, cross-platform)

From a cmd.exe (Windows) or a bash shell (Linux/WSL/macOS):

1) Create and change into a build directory:

```
mkdir build
cd build
```

2) Configure with CMake (default generator).

```
cmake ..
```

3) Build the project.

- Cross-platform (recommended):

```
cmake --build . --config Release
```

- Unix-like with Makefiles (after `cmake ..`):

```
make -j$(nproc)
```

Notes for Windows users:
- If CMake chooses the Visual Studio generator the binaries may be in `build/Debug/` or `build/Release/` (or `build/<generator>/...`). Search for `server_app.exe` / `client_app.exe` inside `build/`.
- For MinGW or MSYS you can use `-G "MinGW Makefiles"` when running `cmake ..`.

After a successful build you should find the executables named `server_app` and `client_app` (or with `.exe` on Windows) somewhere under `build/`.

---

## Run

Open two terminals.

Start the server (optionally pass a port, default 8080):

Windows cmd example (from project root):

```
build\server_app.exe 8080
```

Unix-like example:

```
./build/server_app 8080
```

If your build placed binaries in `build/Release/` use that path instead.

Start a client (no args required):

Windows cmd example:

```
build\client_app.exe
```

Unix-like:

```
./build/client_app
```

The client connects to `127.0.0.1:8080` by default. If you need to connect to a remote server update the source or run a client built to accept host/port (current client is hard-coded to localhost:8080).

---

## Protocol (text-based)

Client → Server commands (single-line, newline-terminated):

- `LIST` — server responds with file names, one per line, then `END`
- `DOWNLOAD <filename>` — server replies `OK` then `SIZE <bytes>` followed by raw bytes of the file
- `UPLOAD <filename>` — client sends `SIZE <bytes>` then raw bytes; server replies `OK` or `ERR <reason>`
- `QUIT` — server closes the session

Server → Client responses:

- `OK`, `ERR <message>`, `SIZE <bytes>`, file lines, `END`

Notes:
- Filenames are sanitized on the server (no `..`, no slashes).  
- Transfers are raw byte streams after the `SIZE` header; the protocol uses helper functions in `common/protocol.hpp` for line- and length-based IO.

---

## Storage

The server stores files under `server_files/` next to the project root. It creates this directory automatically.

---

## Example client workflow

1) Run client.  
2) Choose `1) LIST` to see available files.  
3) Choose `2) DOWNLOAD` and enter a filename to download.  
4) Choose `3) UPLOAD` and enter a local filename to upload to server.  
5) Choose `4) QUIT` to disconnect.

---

## Security / Limitations

- No authentication or encryption (plaintext TCP). Do not expose this server to untrusted networks.  
- No rate limiting or per-user isolation.  
- Filenames are sanitized but additional checks may be needed for production use.  

---

## Troubleshooting

- "connect: Connection refused" — ensure the server is running and listening on the configured port.  
- "File not found" during download — double-check the exact filename reported by `LIST`.  
- On Windows, if you get missing header errors when building, ensure you build using a compiler that supports C++17 (MSVC 2017+, MinGW-w64, or use WSL).

---

## Contributing / Next steps

- Add command-line host/port args to the client.  
- Add TLS (OpenSSL) or start using a secure transport.  
- Add authentication and per-user directories.  
- Add resume support for large files.

---

If you'd like, I can also:

- add a short `README` section that shows exact `cmake` commands for Visual Studio generators on Windows, or
- wire the client to accept host/port from the command line.

---
