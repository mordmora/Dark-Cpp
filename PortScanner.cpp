#include<WinSock2.h>
#include<iostream>
#include<WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
	std::string target = "127.0.0.1";
	unsigned short target_port = 80;

	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		std::cerr << "Winsock no funciona" << std::endl;
		return 1;
	}

	//creacion del socket tcp
	SOCKET socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_fd == INVALID_SOCKET) {
		std::cerr << "Erro al crear el socket" << std::endl;
		WSACleanup();
		return 1;
	}

	//Puerto y direccion de destino
	sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(target_port);

	//Convertir la direccion destino en estructura de direccion binaria
	wchar_t dest_ip_wide[16];
	MultiByteToWideChar(CP_UTF8, 0, target.c_str(), -1, dest_ip_wide, 16);
	InetPton(AF_INET, dest_ip_wide, &(dest_addr.sin_addr));


	//connectar a la direccion ip y puerto de destino
	if (connect(socket_fd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
		std::cerr << target << ":" << target_port << " ---> close" << std::endl;
		closesocket(socket_fd);
		WSACleanup();
		return 1;
	}

	//imprimir que el puerto esta abierto
	std::cout << target << ":" << target_port << " ---> open" << std::endl;
	closesocket(socket_fd);
	WSACleanup();
	return 0;
}