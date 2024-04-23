#include <iostream>
using namespace std;
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <limits>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

// Caesar cipher encryption function
string encryptCaesarCipher(const string& text, int shift) {
    string result = "";
    for (char character : text) {
        if (isalpha(character)) {
            char shiftedChar = character + shift;
            if (isupper(character) && shiftedChar > 'Z') {
                shiftedChar = 'A' + (shiftedChar - 'Z' - 1);
            } else if (islower(character) && shiftedChar > 'z') {
                shiftedChar = 'a' + (shiftedChar - 'z' - 1);
            }
            result += shiftedChar;
        } else {
            result += character; // Keep non-alphabetic characters unchanged
        }
    }
    return result;
}

// Function to create a new account
void createAccount(const string& username, const string& password) {
    ofstream credentialsFile("credentials.txt", ios::app);
    if (!credentialsFile.is_open()) {
        cerr << "Error: Unable to open credentials file." << endl;
        return;
    }

    // Encrypt username and password before saving
    string encryptedUsername = encryptCaesarCipher(username, 3);
    string encryptedPassword = encryptCaesarCipher(password, 3);

    credentialsFile << encryptedUsername << " " << encryptedPassword << endl;
    cout << "Account created successfully." << endl;
}

// Function to authenticate user
bool authenticate(const string& username, const string& password) {
    // Read encrypted credentials from file
    ifstream credentialsFile("credentials.txt");
    if (!credentialsFile.is_open()) {
        cerr << "Error: Unable to open credentials file." << endl;
        return false;
    }

    string storedEncryptedUsername, storedEncryptedPassword;
    while (credentialsFile >> storedEncryptedUsername >> storedEncryptedPassword) {
        // Decrypt and compare credentials
        string decryptedUsername = encryptCaesarCipher(storedEncryptedUsername, -3);
        string decryptedPassword = encryptCaesarCipher(storedEncryptedPassword, -3);

        if (username == decryptedUsername && password == decryptedPassword) {
            return true;
        }
    }

    return false;
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024] = {0};

    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    cout << "Connected to server!" << endl;
    cout << "---------------------" << endl;
    cout << "        _______         " << endl;
    cout << "       /       \\       " << endl;
    cout << "      |  *   *  |      " << endl;
    cout << "      |   \\_/   |      " << endl;
    cout << "       \\       /       " << endl;
    cout << "        \\_____/        " << endl;
    cout << endl;

    // Menu
    cout << "1. Log in" << endl;
    cout << "2. Create an account" << endl;
    int choice;
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        cout << "Enter username: ";
        string username;
        cin >> username;
        cout << "Enter password: ";
        string password;
        cin >> password;

        if (!authenticate(username, password)) {
            cerr << "Error: Invalid credentials." << endl;
            close(clientSocket);
            exit(EXIT_FAILURE);
        }

        cout << "Authentication successful. You can start chatting." << endl;
    } else if (choice == 2) {
        cout << "Enter username: ";
        string username;
        cin >> username;
        cout << "Enter password: ";
        string password;
        cin >> password;

        createAccount(username, password);
    } else {
        cerr << "Error: Invalid choice." << endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Clear input buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true) {
        // Get user input
        string message;
        cout << "You: ";
        getline(cin, message);

        // Send the message to server
        send(clientSocket, message.c_str(), message.length(), 0);

        // Receive reply from server
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        cout << "Server: " << buffer << endl;
    }

    close(clientSocket);
    return 0;
}
