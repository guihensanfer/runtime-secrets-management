#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>  // Inclui funções da API do Windows
#include <sstream>    // Para ostringstream
#include <ctime>      // Para gerar o GUID único
#include <cstdlib>    // Para a função rand()

// Função personalizada para converter números em string
template <typename T>
std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Função para gerar um GUID único usando data/hora e valor aleatório
std::string generateGUID() {
    srand((unsigned)time(0));
    std::string guid = to_string(time(0)) + to_string(rand());
    return guid;
}

// Função para comparação segura de strings, prevenindo ataques de timing
bool secure_compare(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    volatile int result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}

// Função para zerar a memória de forma segura
void secure_zero(void* ptr, size_t len) {
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (len--) *p++ = 0;
}

// Função para verificar se um debugger está presente e encerrar a aplicação se estiver
void check_debugger() {
    if (IsDebuggerPresent()) {
        std::cerr << "Debugger detected! Exiting..." << std::endl;
        exit(1);
    }
}

// Cabeçalho de identificação
void header() {
    std::cout << "Bomdev Runtime Secret Management" << std::endl;
    std::cout << "2024 Bomdev, Developed by Guilherme Ferreira" << std::endl << std::endl;
}

int main() {
    SetConsoleTitle("Bomdev Secret Management Console");
    
    // Verificação de debugger
    check_debugger();

    // Gerar um GUID único
    std::string guidStr = generateGUID();

    // Cabeçalho
    header();

    // Nome do pipe usando o GUID
    bool isProduction = false;
    std::string pipeName = "\\\\.\\pipe\\" + (isProduction ? guidStr : "DEFAULT") + "_BOMDEV_SECRET";

    // Definir a senha (evitar hardcoding, ideal é usar um sistema de gerenciamento de segredos)
    const std::string password = "cUDo56INbp9PH39lODvfP9bm0drs41nhW0dSKklpNqHQEvhxOp";
    std::string pass = "";

    // Loop para solicitar a senha do usuário
    do {
        if (pass != "")
            std::cout << "\033[33m" << "Invalid password." << "\033[0m" << std::endl;

        std::cout << "Enter the password: ";
        std::getline(std::cin, pass);

        // Limpar o console após cada tentativa de senha
        system("cls");  // No Windows, use "cls" para limpar a tela

        // Mostrar cabeçalho novamente
        header();

    } while (pass == "" || !secure_compare(pass, password));

    // Zerar a senha da memória após validação
    secure_zero(&pass[0], pass.size());

    std::cout << "Pipe name: " << pipeName << std::endl;

    // Solicitar o JSON de configuração
    std::string jsonInput;
    std::cout << "\033[35m" << "Enter the project config content: " << "\033[0m"; // \033[0m reseta a cor para padrão
    std::getline(std::cin, jsonInput);

    // Criar o named pipe
    HANDLE hPipe = CreateNamedPipe(
        pipeName.c_str(),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        0,
        0,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error to create pipe: " << GetLastError() << std::endl;
        return 1;
    }

    // Conectar ao pipe
    if (!ConnectNamedPipe(hPipe, NULL)) {
        std::cerr << "Error to connect pipe: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Escrever o JSON no pipe
    DWORD bytesWritten = 0;
    if (!WriteFile(hPipe, jsonInput.c_str(), jsonInput.size(), &bytesWritten, NULL)) {
        std::cerr << "Error to write to pipe: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Zerar a memória que contém o JSON
    secure_zero(&jsonInput[0], jsonInput.size());

    // Fechar o pipe
    CloseHandle(hPipe);

    // Limpar o console
    system("cls");

    // Mostrar o cabeçalho novamente
    header();
    std::cout << "\033[32m" << "The operation was successfully." << "\033[0m" << std::endl << std::endl; // \033[0m reseta a cor para padrão

    // Encerrar a aplicação
    exit(0);
}
