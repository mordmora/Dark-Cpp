#include <iostream>
#include <string>
#include <boost/asio.hpp> 

using boost::asio::ip::tcp; // Protocolo tcp para la comunicacion

std::string make_response()
{
    std::string response_body = "<html><body>You successfully made a request to the server</body></html>"; // HTML response body
    std::string status_line = "HTTP/1.1 200 OK\r\n"; // HTTP status 200 ok
    std::string content_type_header = "Content-Type: text/html\r\n"; // HTTP response content type header con HTML
    std::string content_length_header = "Content-Length: " + std::to_string(response_body.size()) + "\r\n"; // HTTP response contiene el tamaño del header con el tamaño del response body
    return status_line + content_type_header + content_length_header + "\r\n" + response_body; // retorna todo el HTTP Response
}

int main()
{
        boost::asio::io_context io_context; // crea I/O context para Boost.Asio

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 80)); // crea un objeto tcp acceptor que escucha en el puerto 80

        while (true) // acepta conecciones de forma indefinida
        {
            tcp::socket socket(io_context); // crea un objeto de tipo TCP Socket
            acceptor.accept(socket); // acepta la coneccion entrante y la vincula al socket

            std::cout << "Received request from: " << socket.remote_endpoint().address().to_string() << std::endl; // muestra la direccion ip del endpoint 

            std::string response = make_response(); // genera el HTTP response

            boost::asio::write(socket, boost::asio::buffer(response)); // envia la respuesta al cliente

            socket.shutdown(tcp::socket::shutdown_both); // termina operaciones de lectura y escritura
            socket.close(); // cierra el socket
        }

    return 0;
}