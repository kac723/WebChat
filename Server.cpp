#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/SocketAddress.h>
#include <regex>
using std::cout;
using std::endl;
using namespace boost::asio;
using ip::tcp;

const unsigned int maxBuffer = 2048;
class Server
{
private:
    tcp::acceptor _acceptor;
    std::vector<std::pair<tcp::socket, int>> _sockets;
    //std::vector<std::pair<Poco::Net::StreamSocket, int>> m_sockets;
    std::mutex _mutex;
    //Poco::Net::ServerSocket m_serverSocket;
    int _clientID = 0;

public:
    //constructor for accepting connection from client
    Server(boost::asio::io_service &io) : _acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1234))
    {
        //m_serverSocket = Poco::Net::ServerSocket(Poco::Net::SocketAddress("127.0.0.1:1234"));
    }
    void connect()
    {

        while (true)
        {
            tcp::socket s = _acceptor.accept();
            _mutex.lock();
            _sockets.push_back(std::move(std::pair<tcp::socket, int>(std::move(s), _clientID))); ///yeeey move
            //auto newSocket = m_serverSocket.acceptConnection();
            //newSocket.setReceiveTimeout(Poco::Timespan(1, 0));
            //m_sockets.push_back(std::pair<Poco::Net::StreamSocket, int>(newSocket, _clientID));
            //newSocket.setReceiveTimeout(Poco::Timespan(1, 0));
            _mutex.unlock();
            cout << "New Client! ID: " << _clientID << endl;
            //Poco::Net::SocketStream ss(newSocket);
            auto strClientID = std::to_string(_clientID);
            //newSocket.sendBytes(strClientID.data(), strClientID.size());
            write(_sockets[_sockets.size() - 1].first, buffer(std::to_string(_clientID)));
            ++_clientID;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void onReceive()
    {
        while (true)
        {
            _mutex.lock();
            for (size_t i = 0; i < _sockets.size(); ++i)
            {
                const auto noOfAvailableBytss = _sockets[i].first.available();
                if (noOfAvailableBytss != 0 && noOfAvailableBytss <= maxBuffer)
                {
                    std::string data(noOfAvailableBytss, 0);
                    _sockets[i].first.read_some(buffer(data));
                    cout << "Client " << _sockets[i].second << data << endl;
                    std::regex r("\"");
                    Poco::JSON::Parser p;

                    auto parsed = p.parse(data);

                    auto obj = parsed.extract<Poco::JSON::Object::Ptr>();
                    auto receivingClientID = std::stoi(obj->get("ReceivingClientID").toString());
                    auto message = obj->get("Message").toString();
                    //cout << noOfAvailableBytss << " data available for Client !" << _sockets[i].second << endl;
                    for (size_t j = 0; j < _sockets.size(); ++j)
                    {
                        if (j != i)
                        {
                            cout << "Sending message " << message << " to Client " << receivingClientID << endl;
                            write(_sockets[j].first, buffer(message));
                        }
                    }
                }
            }
            _mutex.unlock();

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};
int main()
{
    io_service io;

    cout << "Waiting for clients..." << endl;
    Server c(io);
    std::thread t(&Server::connect, &c);
    std::thread t2(&Server::onReceive, &c);
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //I'm in an IDIOT.
        //Without the sleep, this while loop was going on and on and
        // caused over 100% CPU usage :)
    }
    return 0;
}