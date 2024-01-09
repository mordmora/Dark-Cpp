
#include <iostream> 
#include <string> 
#include <cstdio> 
#include <memory> 
#include <array>

int main()
{

    std::string host = "google.com"; //objetivo
    
    std::string ping_cmd = "ping -n 1 " + host;// Construye el comando para enviar el ping (para Windows)

    // Crea un buffer para almacenar los datos
    std::array<char, 128> buffer;
    std::string result;

    // ejecuta el comando y lee la salida usando punteros inteligentes, cierra de forma automatica la pipeline
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(ping_cmd.c_str(), "r"), _pclose);

    // revisa que el comando se ejecutó de forma correcta
    if (!pipe)
    {
        std::cerr << "Error: failed to execute command" << std::endl;
        return EXIT_FAILURE;
    }

    // lee la salida del comando y la guarda en un string
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    //revisa si el string contiene un substring con "Reply from" para determinar si el host está vivo o no
    if (result.find("Reply from") != std::string::npos)
    {
        std::cout << "Host " << host << " is up" << std::endl;
    }
    else
    {
        std::cout << "Host " << host << " is down" << std::endl;
    }

    return EXIT_SUCCESS; 
}