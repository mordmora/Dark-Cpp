#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::udp; // protocolo UDP Para la coneccion

int main()
{
    // Contexto I/O para BOOST.ASIO
    boost::asio::io_context io_context;

    // crea un socket udp y lo vincula al puerto 8888
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 8888));

    // crea un buffer para mantener los datos entrantes
    boost::array<char, 1024> recv_buf;

    // crea un objeto de tipo enpoint para mantener la direccion saliente
    udp::endpoint remote_endpoint;

    while (true) 
    {
        boost::system::error_code error;
        size_t recv_len = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 0, error); //recive el mensaje y guarda la direccion del remitente en remote_endpoint

        if (error && error != boost::asio::error::message_size) 
        {
            std::cerr << "Error receiving data: " << error.message() << std::endl;
            continue;
        }

        // Convierte el buffer a string para mostrar la informacion por la salida estandar (la pantalla)
        std::string message(recv_buf.data(), recv_len);
        std::cout << "Received message: " << message << std::endl;
    }

    return 0;
}