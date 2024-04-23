#include <iostream>
using namespace std;
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

// Caesar cipher encryption function
string encryptCaesarCipher(const string& message, int shift) {
    string encryptedMessage = "";
    for (char character : message) {
        if (isalpha(character)) {
            char shiftedChar = character + shift;
            if (isupper(character) && shiftedChar > 'Z') {
                shiftedChar = 'A' + (shiftedChar - 'Z' - 1);
            } else if (islower(character) && shiftedChar > 'z') {
                shiftedChar = 'a' + (shiftedChar - 'z' - 1);
            }
            encryptedMessage += shiftedChar;
        } else {
            encryptedMessage += character; // Keep non-alphabetic characters unchanged
        }
    }
    return encryptedMessage;
}

void displayWelcomeAnimation() {
    cout << "***********************************************" << endl;
    cout << "*                                             *" << endl;
    cout << "*               WELCOME TO THE                *" << endl;
    cout << "*                  CHAT SERVER                *" << endl;
    cout << "*                                             *" << endl;
    cout << "***********************************************" << endl;

}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[1024] = {0};
    ofstream outputFile("messages.txt");

    // Create a socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Display welcome animation
    displayWelcomeAnimation();

    cout << "Server listening on port " << PORT << endl;

    socklen_t clientAddrLen = sizeof(clientAddr);

    // Accept incoming connection
    if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    cout << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << endl;

    while (true) {
        // Receive data from client
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        cout << "Client: " << buffer << endl;

        // Encrypt the received message
        string encryptedMessage = encryptCaesarCipher(buffer, -3);

        // Save the received message to file with encryption
        outputFile << "Client: " << encryptedMessage << endl;

        // Get user input
        string message;
        cout << "You: ";
        getline(std::cin, message);

        // Send the message to client
        send(clientSocket, message.c_str(), message.length(), 0);

        // Encrypt the sent message
        encryptedMessage = encryptCaesarCipher(message, -3);

        // Save the sent message to file with encryption
        outputFile << "You: " << encryptedMessage << endl;
    }

    close(serverSocket);
    return 0;
}
