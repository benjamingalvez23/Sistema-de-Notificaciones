// server.cpp
#include <iostream>
#include <cstring>
#include <string>
#include <csignal>
#include <thread>
#include <vector>
#include <chrono>
#include <ctime>
#include <mutex>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // inet_ntop
#include <unistd.h>       // close

using namespace std;

struct Clients
{
    int fd;
    string username;
    chrono::system_clock::time_point lastMessage;
};

vector<Clients> clients;
mutex clientsMutex;

void atenderCliente(int client_fd, sockaddr_in cli);
void monitorActivity();

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

    thread monitorThread(monitorActivity);
    sockaddr_in cli{};
    socklen_t cli_len = sizeof(cli);

    //4. Aceptar una conexión entrante


    int client_fd=0;
    ///////////// CREAR HILO PARA ATENDER CLIENTES (OPCIONAL) /////////////
    vector<thread> hilos;

    for(int i = 0; i < 2; i++)
    {
        client_fd = accept(server_fd, (sockaddr*)&cli, &cli_len);
        // clients.emplace_back(client_fd);
        hilos.emplace_back(atenderCliente, client_fd, cli);
    }

    for(auto& th : hilos) th.join();
    monitorThread.detach();

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

    {
        lock_guard<mutex> lock(clientsMutex);
        clients.push_back({client_fd, string(ipstr), chrono::system_clock::now()});
    }

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
        
        {
            lock_guard<mutex> lock(clientsMutex);
            for (auto& c : clients) {
                if (c.fd == client_fd) {
                    c.lastMessage = chrono::system_clock::now();
                    break;
                }
            }
        }
        
        std::cout << buffer;
        // Echo
        for (auto& i : clients) {
            if (i.fd != client_fd) ssize_t s = send(i.fd, buffer.c_str(), sizeof(buffer)-1, MSG_NOSIGNAL);
        }
    }

    {
        lock_guard<mutex> lock(clientsMutex);
        clients.erase(remove_if(clients.begin(), clients.end(), [client_fd](const Clients& c) { 
            return c.fd == client_fd; 
        }), clients.end());
    }
    close(client_fd);
}

void monitorActivity() {
    int inactivityTime = 15;  // Cuanto tiempo tiene que pasar para que este inactivo, esta en segundos
    int interval = 60;   // Para que envie una notificacion de que cuanto tiempo lleva inactivo
        
    while (true) {
        this_thread::sleep_for(chrono::seconds(interval));
        
        auto now = chrono::system_clock::now();
        
        {
            lock_guard<mutex> lock(clientsMutex);
            for (const auto& client : clients) {
                auto elapsed = chrono::duration_cast<chrono::seconds>(now - client.lastMessage).count();
                    
                // tiempo de la ultima actividad
                if (elapsed > inactivityTime) {
                    string notification = "[NOTIFICACION] Usuario " + client.username + " esta inactivo hace " + to_string(elapsed / 60) + " minutos\n";
                        
                    // Enviar a otros clientes
                    for (const auto& other : clients) {
                        if (other.fd != client.fd) {
                            send(other.fd, notification.c_str(), notification.size(), MSG_NOSIGNAL);
                        }
                    }
                    cout << notification;
                }
            }
        }
    }
}