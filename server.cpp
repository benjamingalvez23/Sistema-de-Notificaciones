// server.cpp
#include <iostream>
#include <cstring>
#include <string>
#include <csignal>
#include <thread>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // inet_ntop
#include <unistd.h>       // close

using namespace std;

vector<int> clients;

void atenderCliente(int client_fd, sockaddr_in cli);

int main() {
    const int PORT = 8080;

    //1. Crear socket
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);

    //2. Configurar dirección y puerto, y hacer bind
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    addr.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    //3. Escuchar conexiones entrantes
    listen(server_fd, 8);

    cout << "[SERVIDOR] Escuchando en 0.0.0.0:" << PORT << " ...\n";

    sockaddr_in cli{};
    socklen_t cli_len = sizeof(cli);

    //4. Aceptar una conexión entrante


    int client_fd=0;
    ///////////// CREAR HILO PARA ATENDER CLIENTES (OPCIONAL) /////////////
    vector<thread> hilos;

    for(int i = 0; i < 2; i++)
    {
           client_fd = accept(server_fd, (sockaddr*)&cli, &cli_len);
           clients.emplace_back(client_fd);
           hilos.emplace_back(atenderCliente, client_fd, cli);
    }

    for(auto& th : hilos) th.join();

    // Cierre ordenado
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    close(server_fd);
    std::cout << "[SERVIDOR] Conexión finalizada.\n";
    return 0;
}


void atenderCliente(int client_fd, sockaddr_in cli) {
    char ipstr[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &(cli.sin_addr), ipstr, sizeof(ipstr));
    std::cout << "[SERVIDOR] Cliente conectado desde " << ipstr
              << ":" << ntohs(cli.sin_port) << "\n";

    // Mensaje de bienvenida
    // const char* hello = "\n=== Bienvenido al Notificaciones Chat. ===\n";
    // send(client_fd, hello, std::strlen(hello), MSG_NOSIGNAL);

    // Bucle de eco
    char buf[1024];
    while (true) {
        ssize_t n = recv(client_fd, buf, sizeof(buf)-1, 0);
        if (n < 0) { perror("recv"); break; }
        if (n == 0) {
            std::cout << "[SERVIDOR] Cliente " << client_fd << " se desconectó.\n";
            break;
        }

        string buffer;
        // string msg = "[Cliente" + std::to_string(client_fd) + "] ";
        // buffer += msg;
        buffer += buf;
        buffer += '\0';
        
        std::cout << buffer;
        // Echo
        for (int i : clients) {
            if (i != client_fd) ssize_t s = send(i, buffer.c_str(), sizeof(buffer)-1, MSG_NOSIGNAL);
        }
    }

}