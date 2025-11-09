#include <bits/stdc++.h>
#include <filesystem>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.hpp"

using namespace std;
namespace fs = std::filesystem;

static fs::path storage = "server_files";

string safeName(string s) {
    if (s.find("..") != string::npos) return "";
    if (s.find('/')  != string::npos) return "";
    if (s.find('\\') != string::npos) return "";
    return s;
}

void handleClient(int cfd) {
    string line;

    while (true) {
        if (!recvLine(cfd, line)) break;

        if (line == "LIST") {
            for (auto& p : fs::directory_iterator(storage)) {
                if (p.is_regular_file())
                    sendLine(cfd, p.path().filename().string());
            }
            sendLine(cfd, "END");
        }

        else if (line.rfind("DOWNLOAD ", 0) == 0) {
            string name = safeName(line.substr(9));
            if (name.empty()) {
                sendLine(cfd, "ERR badname");
                continue;
            }
            fs::path p = storage / name;
            if (!fs::exists(p)) {
                sendLine(cfd, "ERR missing");
                continue;
            }
            uintmax_t size = fs::file_size(p);
            sendLine(cfd, "OK");
            sendLine(cfd, "SIZE " + to_string(size));

            FILE* f = fopen(p.c_str(), "rb");
            if (!f) break;

            vector<char> buf(65536);
            uintmax_t left = size;
            while (left > 0) {
                size_t chunk = min(left, (uintmax_t)buf.size());
                fread(buf.data(), 1, chunk, f);
                if (!sendAll(cfd, buf.data(), chunk)) break;
                left -= chunk;
            }
            fclose(f);
        }

        else if (line.rfind("UPLOAD ", 0) == 0) {
            string name = safeName(line.substr(7));
            if (name.empty()) {
                sendLine(cfd, "ERR badname");
                continue;
            }
            if (!recvLine(cfd, line) || line.rfind("SIZE ", 0) != 0) {
                sendLine(cfd, "ERR sizehdr");
                continue;
            }

            uint64_t size = stoull(line.substr(5));
            fs::create_directories(storage);
            fs::path tmp = storage / (name + ".part");

            FILE* f = fopen(tmp.c_str(), "wb");
            if (!f) {
                sendLine(cfd, "ERR open");
                continue;
            }

            vector<char> buf(65536);
            uint64_t left = size;
            bool ok = true;

            while (left > 0) {
                size_t chunk = min((uint64_t)buf.size(), left);
                if (!recvAll(cfd, buf.data(), chunk)) {
                    ok = false; break;
                }
                fwrite(buf.data(), 1, chunk, f);
                left -= chunk;
            }
            fclose(f);

            if (!ok) {
                fs::remove(tmp);
                sendLine(cfd, "ERR fail");
                continue;
            }

            fs::rename(tmp, storage / name);
            sendLine(cfd, "OK");
        }

        else if (line == "QUIT") {
            break;
        }

        else {
            sendLine(cfd, "ERR unknown");
        }
    }

    close(cfd);
}

int main(int argc, char* argv[]) {
    fs::create_directories(storage);

    int port = 8080;
    if (argc >= 2) port = atoi(argv[1]);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sfd, (sockaddr*)&addr, sizeof(addr));
    listen(sfd, 20);

    cout << "Server listening on port " << port << "\n";

    while (true) {
        int cfd = accept(sfd, nullptr, nullptr);
        thread(handleClient, cfd).detach();
    }
}
