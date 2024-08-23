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

std::string generateGUID() {
    srand((unsigned) time(0));
    std::string guid = std::to_string(time(0)) + std::to_string(rand());
    return guid;
}

void header(){
    std::cout << "Bomdev Runtime Secret Management" << std::endl;
    std::cout << "© 2024 Bomdev, Developed by Guilherme Ferreira" << std::endl << std::endl;
}

int main() {
    // Gerar um GUID único usando data/hora e valor aleatório
    std::string guidStr = generateGUID();

    // Cabeçalho
    header();


    // Nome do pipe usando o GUID
    bool isProduction = false;
    std::string fifoName = "/tmp/" +  (isProduction ? guidStr : "DEFAULT") + "_BOMDEV_SECRET";

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

    // Fechar o pipe
    close(fd);

    // Destruir o pipe após o uso
    unlink(fifoName.c_str());

    // Limpar o console
    system("clear");
    
    header();
    std::cout << "\033[32m" << "The operation was succefully." << "\033[0m" << std::endl << std::endl; // \033[0m reseta a cor para padrão

    // Encerrar a aplicação
    exit(0);
}
