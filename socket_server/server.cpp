#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

#define PORT 6438
#define BUFFER_SIZE 1024

std::vector<std::thread> threads;
std::mutex threadsMutex;
std::mutex clientsMutex;
std::vector<int> clients;

void forwardMessage(const char* message, int sendSocket) {
    std::cout << "Forwarding message: " << message << std::endl;
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (int socket : clients) {
        if (socket != sendSocket) {
            if (write(socket, message, strlen(message)) < 0) {
                std::cerr << "Error forwarding message to socket " << socket << ": " << strerror(errno) << std::endl;
            }
        }
    }
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(clientSocket);
    }

    while ((bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received message from client: " << buffer << std::endl;

        forwardMessage(buffer, clientSocket);
    }

    if (bytesRead == 0) {
        std::cout << "Client disconnected." << std::endl;
    } else if (bytesRead < 0) {
        std::cerr << "Read error: " << strerror(errno) << std::endl;
    }

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
    }

    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            close(serverSocket);
            return 1;
        }

        std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        {
            std::lock_guard<std::mutex> lock(threadsMutex);
            threads.emplace_back(handleClient, clientSocket);
        }
    }

    close(serverSocket);

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    return 0;
}
