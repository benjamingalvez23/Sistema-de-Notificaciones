// client.cpp
#include <iostream>
#include <cstring>
#include <string>
#include <csignal>
#include <thread>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // inet_pton
#include <unistd.h>      // close

void sendMsg(int fd);
void recvMsg(int fd);

using namespace std;

string username;
bool connected = true;
bool notification = true;
bool isActive = true;

int main(int argc, char* argv[]) {
    // Uso: ./c 127.0.0.1 8080 [USERNAME]
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <IP> <PUERTO> <USUARIO>\n";
        return 1;
    }

    std::signal(SIGPIPE, SIG_IGN);

    const char* server_ip = argv[1];
    int port = std::stoi(argv[2]);
    username = argv[3];

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

    thread sendT(sendMsg, fd);
    thread recvT(recvMsg, fd);

    sendT.join();
    recvT.join();

    shutdown(fd, SHUT_RDWR);
    close(fd);
    std::cout << "\n> Conexión finalizada.\n";
    return 0;
}

void sendMsg(int fd) {
    cout << "> Escribe líneas y presiona Enter para enviar mensajes\n> Escriba '/leave' para desconectarse.\n> Use '/help' para ver los comandos. (Coming soon...) \n> Escribir '/on o /off' para activar o desactivar las notificaciones respectivamente\n";
    string line;
    while (getline(std::cin, line)) {
        if (line == "/leave") {
            cout << "Desconectado\n";
            connected = false;

            shutdown(fd, SHUT_RDWR);
            close(fd);

            break;
        }

        if(line == "/off") {
            notification = false; 
            cout << "Notificaciones apagadas\n";
            continue;
        }
        if(line == "/on") {
            notification = true; 
            cout << "Notificaciones activadas\n";
            continue;
        }
        line.push_back('\n');
        string msg = "[" + username + "] " + line;
        if (send(fd, msg.c_str(), msg.size(), MSG_NOSIGNAL) < 0) {
            perror("send");
            
            break;
        }
    }
}

void recvMsg(int fd) {
    char buf[1024];
    while (connected)
    {

        string buf_string(buf);
        ssize_t r = recv(fd, buf, sizeof(buf)-1, 0);
        
        if(buf_string.find("[NOTIFICACION]") != string::npos && !notification) continue;
        if (r <= 0) {
            if (r < 0) perror("recv");
            else if(connected)std::cout << "Servidor cerró.\n";
            break;
        }

        buf[r] = '\0';
        std::cout << buf;
    }
}