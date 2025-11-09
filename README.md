# Network-Based File Sharing System (Server–Client Model)
- Capstone Project Report
- Author: Sandeep Kumar Samal
- Batch: 4  
- Year: 2025

---

## Overview
Small TCP-based client/server file sharing example. The server stores files and supports multiple clients that can list, download, and upload files using a simple line-based protocol.

Storage directory (created by the server): `server_files/`

---

## Quick facts
- Language: C++17
- Build system: CMake
- Platform: Cross-platform (Linux / WSL / macOS / Windows)

---

## Repository layout
```
network-file-share/
├── CMakeLists.txt          # top-level
├── README.md
├── README_fixed.md         # this cleaned README copy
├── common/
│   ├── protocol.hpp        # socket helpers (sendLine/recvLine, sendAll/recvAll)
│   └── CMakeLists.txt
├── server/
│   ├── server.cpp
│   └── CMakeLists.txt      # builds server_app
├── client/
│   ├── client.cpp
│   └── CMakeLists.txt      # builds client_app
├── build/                  # optional build output
└── server_files/           # runtime storage (created by server)
```

---

## Build
From a shell (Windows cmd.exe, PowerShell, or bash/WSL):

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Notes:
- On Unix-like systems you can run `make -j$(nproc)` after `cmake ..`.
- On Windows with MSVC look under `build\\Release\\` or `build\\Debug\\` for executables.

After a successful build you should have `server_app` and `client_app` (or `*.exe` on Windows) in the build output.

---

## Run
Start the server (default port 8080; optional port argument supported):

```bash
# from the build output folder or adjust path accordingly
./server_app 8080
```

Start the client (connects to 127.0.0.1:8080 by default):

```bash
./client_app
```

If you used an out-of-source build, run the executables from the build subfolders (for example `build\\server\\server_app.exe` on Windows).

---

## Client usage (menu)
After connecting the client shows a short menu:

1) LIST — list files on server
2) DOWNLOAD — download a file (prompts for filename)
3) UPLOAD — upload a local file (prompts for filename)
4) QUIT — disconnect

---

## Protocol (text-based)
Client → Server commands (newline-terminated single-line commands):

- `LIST`
- `DOWNLOAD <filename>`
- `UPLOAD <filename>` (client then sends `SIZE <bytes>` followed by raw bytes)
- `QUIT`

Server → Client responses:

- file lines then `END`
- `OK`, `ERR <message>`, `SIZE <bytes>` then raw bytes

Notes: filenames are sanitized on the server (no `..` or path separators).

---

## Troubleshooting
- "connect: Connection refused" — ensure server is running and listening on the correct port.
- Download fails — verify filename using `LIST`.
- Upload fails — verify local file path and read permissions.

---

## Development notes / next steps
- Make the client accept host/port CLI args (currently hard-coded to 127.0.0.1:8080).
- Add TLS and authentication for production use.
- Add resume support and per-user storage.

---

If you'd like, I can: add a small demo/run script, or implement client CLI args next.
