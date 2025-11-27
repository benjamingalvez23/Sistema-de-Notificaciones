// server.cpp
#include <iostream>
#include <cstring>
#include <string>
#include <csignal>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // inet_ntop
#include <unistd.h>       // close

using namespace std;

int main() {
    const int PORT = 8080;

    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 8) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "[SERVIDOR] Escuchando en 0.0.0.0:" << PORT << " ...\n";

    sockaddr_in cli{};
    socklen_t cli_len = sizeof(cli);
    int client_fd = accept(server_fd, (sockaddr*)&cli, &cli_len);
    if (client_fd < 0) { perror("accept"); return 1; }

    char ipstr[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &(cli.sin_addr), ipstr, sizeof(ipstr));
    std::cout << "[SERVIDOR] Cliente conectado desde " << ipstr
              << ":" << ntohs(cli.sin_port) << "\n";

    // Mensaje de bienvenida
    const char* hello = "Bienvenido al Echo Server (escribe y te devuelvo lo mismo).\n";
    send(client_fd, hello, std::strlen(hello), MSG_NOSIGNAL);

    // Bucle de eco
    char buf[1024];
    while (true) {
        ssize_t n = recv(client_fd, buf, sizeof(buf)-1, 0);
        if (n < 0) { perror("recv"); break; }
        if (n == 0) {
            std::cout << "[SERVIDOR] Cliente cerró la conexión.\n";
            break;
        }
        buf[n] = '\0';
        std::cout << "[SERVIDOR] Recibido: " << buf;
        // Echo
        ssize_t s = send(client_fd, buf, n, MSG_NOSIGNAL);
        if (s < 0) { perror("send"); break; }
    }

    // Cierre ordenado
    close(client_fd);
    close(server_fd);
    
    std::cout << "[SERVIDOR] Conexión finalizada.\n";
    return 0;
}
