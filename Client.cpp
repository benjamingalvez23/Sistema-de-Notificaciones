// client.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <csignal>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // inet_pton
#include <unistd.h>      // close

int main(int argc, char* argv[]) {
    // Uso: ./client 127.0.0.1 8080
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <IP> <PUERTO>\n";
        return 1;
    }

    std::signal(SIGPIPE, SIG_IGN);

    const char* server_ip = argv[1];
    int port = std::stoi(argv[2]);

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &srv.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(fd, (sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect");
        return 1;
    }

    std::cout << "[CLIENTE] Conectado a " << server_ip << ":" << port << "\n";

    // Leer bienvenida (si hay)
    char buf[1024];
    ssize_t n = recv(fd, buf, sizeof(buf)-1, MSG_DONTWAIT);
    if (n > 0) {
        buf[n] = '\0';
        std::cout << buf;
    }

    std::cout << "Escribe líneas y presiona Enter (Ctrl+D para salir):\n";
    std::string line;
    while (std::getline(std::cin, line)) {
        // Enviar con salto de línea para que el servidor lo vea "como línea"
        line.push_back('\n');
        if (send(fd, line.c_str(), line.size(), MSG_NOSIGNAL) < 0) {
            perror("send");
            break;
        }
        // Recibir eco
        ssize_t r = recv(fd, buf, sizeof(buf)-1, 0);
        if (r <= 0) {
            if (r < 0) perror("recv");
            else std::cout << "[CLIENTE] Servidor cerró.\n";
            break;
        }
        buf[r] = '\0';
        std::cout << "[ECO] " << buf;
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);
    std::cout << "\n[CLIENTE] Conexión finalizada.\n";
    return 0;
}
