#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cstring> // Para memset
#include <signal.h>
#include <stdexcept>

// Função para gerar um GUID único usando data/hora e valor aleatório
std::string generateGUID() {
    srand((unsigned) time(0));
    std::string guid = std::to_string(time(0)) + std::to_string(rand());
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
#ifdef _WIN32
    if (IsDebuggerPresent()) {
        std::cerr << "Debugger detected! Exiting..." << std::endl;
        exit(1);
    }
#endif
}

// Cabeçalho de identificação
void header(){
    std::cout << "Bomdev Runtime Secret Management" << std::endl;
    std::cout << "© 2024 Bomdev, Developed by Guilherme Ferreira" << std::endl << std::endl;
}

int main() {
    // Verificação de debugger
    check_debugger();

    // Gerar um GUID único
    std::string guidStr = generateGUID();

    // Cabeçalho
    header();

    // Nome do pipe usando o GUID
    bool isProduction = false;
    std::string fifoName = "/tmp/" +  (isProduction ? guidStr : "DEFAULT") + "_BOMDEV_SECRET";
    
    // Definir a senha (evitar hardcoding, ideal é usar um sistema de gerenciamento de segredos)
    const std::string password = "cUDo56INbp9PH39lODvfP9bm0drs41nhW0dSKklpNqHQEvhxOp";
    std::string pass = "";

    // Loop para solicitar a senha do usuário
    if(isProduction){
        do {
            if(pass != "")
                std::cout << "\033[33m" << "Invalid password." << "\033[0m" << std::endl;            

            std::cout << "Enter the password: ";
            std::getline(std::cin, pass);    

            // Limpar o console após cada tentativa de senha
            system("clear");

            // Mostrar cabeçalho novamente
            header();

        } while(pass == "" || !secure_compare(pass, password));
    }    

    // Zerar a senha da memória após validação
    secure_zero(&pass[0], pass.size());

    std::cout << "Pipe name: " << fifoName << std::endl;

    // Solicitar o JSON de configuração
    std::string jsonInput;
    std::cout << "\033[35m" << "Enter the project config content: " << "\033[0m"; // \033[0m reseta a cor para padrão
    std::getline(std::cin, jsonInput);

    // Verificar se o pipe já existe, e se existir, remover
    if (access(fifoName.c_str(), F_OK) != -1) {        
        unlink(fifoName.c_str());
    }

    // Criar o named pipe (FIFO)
    if (mkfifo(fifoName.c_str(), 0666) == -1) {
        std::cerr << "Error to create pipe: ";
        perror("");
        return 1;
    }

    // Abrir o pipe para escrita
    int fd = open(fifoName.c_str(), O_WRONLY);
    if (fd == -1) {
        std::cerr << "Error to open pipe: ";
        perror("");
        return 1;
    }    

    // Escrever o JSON no pipe
    write(fd, jsonInput.c_str(), jsonInput.size());

    // Zerar a memória que contém o JSON
    secure_zero(&jsonInput[0], jsonInput.size());

    // Fechar o pipe
    close(fd);

    // Destruir o pipe após o uso
    unlink(fifoName.c_str());

    // Limpar o console
    system("clear");
    
    // Mostrar o cabeçalho novamente
    header();
    std::cout << "\033[32m" << "The operation was successfully." << "\033[0m" << std::endl << std::endl; // \033[0m reseta a cor para padrão

    // Encerrar a aplicação
    exit(0);
}