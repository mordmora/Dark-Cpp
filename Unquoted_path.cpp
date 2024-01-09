#include <windows.h>
#include <iostream>
#include <array>
#include <sstream>

// funcion para ejecutar un comando y obtener el resultado
std::string exec(const char* cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// funcion para determinar si una cadena está rodeada por comillas
bool isQuoted(const std::string& str) {
    return str.front() == '\"' && str.back() == '\"';
}

// Función para comprobar si la ruta apunta a un ejecutable
bool isExecutable(const std::string& str) {
    return str.find(".exe") != std::string::npos;
}

// Función para comprobar si la ruta contiene espacios.
bool hasSpaces(const std::string& str) {
    return str.find(' ') != std::string::npos;
}

int main() {
    // obtener todos los servicios
    std::string allServices = exec("wmic service get pathname");

    std::istringstream serviceStream(allServices);
    std::string line;

    while (std::getline(serviceStream, line)) {
        // ignora lineas con "Windows" and "Program Files"
        if (line.find("Windows") == std::string::npos && line.find("Program Files") == std::string::npos) {
            // ignora lineas que no apuntan a ningun ejecutable
            if (isExecutable(line)) {
                // Eliminar espacios en blanco iniciales y finales
                line.erase(0, line.find_first_not_of(" \n\r\t"));
                line.erase(line.find_last_not_of(" \n\r\t") + 1);

                // Comprueba si es una ruta sin comillas con espacios.
                if (!isQuoted(line) && hasSpaces(line)) {
                    std::cout << line << std::endl;
                }
            }
        }
    }

    return 0;
}