
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.hpp"

using namespace std;

static void flushInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    string host = "127.0.0.1";
    int port = 8080;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }

    cout << "Connected to server " << host << ":" << port << "\n";

    while (true) {

        cout << "\n===================================\n";
        cout << "1) LIST\n";
        cout << "2) DOWNLOAD\n";
        cout << "3) UPLOAD\n";
        cout << "4) QUIT\n";
        cout << "===================================\n";
        cout << "Choice: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            continue;  // ask again
        }

        int choice;
        try {
            choice = stoi(input);
        } catch (...) {
            cout << "Invalid input. Enter 1–4.\n";
            continue;
        }

        // ==========================
        //   LIST
        // ==========================
        if (choice == 1) {
            sendLine(sockfd, "LIST");
            string line;
            cout << "\n--- Server Files ---\n";
            while (recvLine(sockfd, line)) {
                if (line == "END") break;
                cout << "  " << line << "\n";
            }
            cout << "---------------------\n";
        }

        // ==========================
        //   DOWNLOAD
        // ==========================
        else if (choice == 2) {
            cout << "Enter filename to download: ";
            string name;
            getline(cin, name);

            if (name.empty()) {
                cout << "Invalid filename\n";
                continue;
            }

            sendLine(sockfd, "DOWNLOAD " + name);

            string line;
            if (!recvLine(sockfd, line)) {
                cout << "No response from server.\n";
                continue;
            }

            if (line.rfind("ERR", 0) == 0) {
                cout << "Server error: " << line << "\n";
                continue;
            }

            if (line != "OK") {
                cout << "Unexpected response: " << line << "\n";
                continue;
            }

            if (!recvLine(sockfd, line) || line.rfind("SIZE ", 0) != 0) {
                cout << "Missing SIZE header.\n";
                continue;
            }

            uint64_t size = stoull(line.substr(5));
            vector<char> buf(size);

            if (!recvAll(sockfd, buf.data(), size)) {
                cout << "Failed to receive file.\n";
                continue;
            }

            FILE* f = fopen(name.c_str(), "wb");
            if (!f) {
                cout << "Cannot save file.\n";
                continue;
            }
            fwrite(buf.data(), 1, size, f);
            fclose(f);

            cout << "Downloaded successfully: " << name << " ("<<size<<" bytes)\n";
        }

        // ==========================
        //   UPLOAD
        // ==========================
        else if (choice == 3) {
            cout << "Enter local filename to upload: ";
            string name;
            getline(cin, name);

            if (name.empty()) {
                cout << "Invalid filename\n";
                continue;
            }

            FILE* f = fopen(name.c_str(), "rb");
            if (!f) {
                cout << "File not found.\n";
                continue;
            }

            fseek(f, 0, SEEK_END);
            uint64_t size = ftell(f);
            fseek(f, 0, SEEK_SET);

            vector<char> buf(size);
            fread(buf.data(), 1, size, f);
            fclose(f);

            sendLine(sockfd, "UPLOAD " + name);
            sendLine(sockfd, "SIZE " + to_string(size));

            if (!sendAll(sockfd, buf.data(), size)) {
                cout << "Failed to send file.\n";
                continue;
            }

            string resp;
            if (!recvLine(sockfd, resp)) {
                cout << "No response.\n";
                continue;
            }

            cout << "Server: " << resp << "\n";
        }

        // ==========================
        //   QUIT
        // ==========================
        else if (choice == 4) {
            sendLine(sockfd, "QUIT");
            cout << "Goodbye.\n";
            break;
        }

        else {
            cout << "Invalid input. Enter 1–4.\n";
        }
    }

    close(sockfd);
    return 0;
}
