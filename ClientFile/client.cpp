#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>

void recieve(int sock_fd)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0)
        {
            std::cout << "Server disconnected." << std::endl;
            break;
        }
        std::cout << "Friend: " << buffer << std::endl;
    }
}

int main()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Server connection failed." << std::endl;
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    std::thread t(recieve, sock_fd);
    std::string message;
    while (std::getline(std::cin, message))
    {
        send(sock_fd, message.c_str(), message.size(), 0);
    }

    t.join();
    close(sock_fd);

    return 0;
}