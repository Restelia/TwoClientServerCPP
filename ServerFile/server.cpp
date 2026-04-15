#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <cstring>

void relay(int from_fd, int to_fd)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(from_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0)
        {
            std::cout << "A client disconnected." << std::endl;
            break;
        }
        send(to_fd, buffer, bytes, 0);
    }
}

int main(){

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    std::cout << "Socket Created!" << std::endl;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Failed to bind." << std::endl;
        return 1;
    }

    std::cout << "Bound to port 8080" << std::endl;

    if (listen(server_fd, 2) < 0)
    {
        std::cerr << "Listen failed." << std::endl;
        return 1;
    }

    std::cout << "Server is listening..." << std::endl;

    int client_fd [2];
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    for (size_t i {0}; i < 2; i++)
    {
        client_fd[i] = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd[i] < 0)
        {
            std::cerr << "Accept failed." << std::endl;
            return 1;
        }
        std::cout << "Client " << i + 1 << " connected." << std::endl;
    }

    std::thread t1(relay, client_fd[0], client_fd[1]);
    std::thread t2(relay, client_fd[1], client_fd[0]);

    t1.join();
    t2.join();

    return 0;
}