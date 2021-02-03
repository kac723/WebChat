#include <boost/asio/io_context.hpp>
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
#include <boost/enable_shared_from_this.hpp>
#include <regex>
#include <boost/bind.hpp>
using std::cout;
using std::endl;
using namespace boost::asio;
using ip::tcp;

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
{
public:
    typedef boost::shared_ptr<TCPConnection> pointer;

    static pointer create(io_context &io)
    {
        return pointer(new TCPConnection(io));
    }
    tcp::socket &socket()
    {
        return m_socket;
    }
    void start()
    {
        //m_message = "TestAsync";
        /*async_write(m_socket,
                    boost::asio::buffer(m_message),
                    boost::bind(&TCPConnection::handle_wrtie,
                                shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
    */
    }

private:
    TCPConnection(io_context &io) : m_socket(io) {}
    void handle_wrtie(const boost::system::error_code &e, size_t bytesTransfered)
    {
    }
    tcp::socket m_socket;
    std::string m_message;
};

const unsigned int maxBuffer = 2048;
class Server
{
private:
    tcp::acceptor m_acceptor;
    std::vector<std::pair<tcp::socket, int>> m_users;
    // std::vector<std::pair<Poco::Net::StreamSocket, int>> m_sockets;
    std::mutex _mutex;
    // Poco::Net::ServerSocket m_serverSocket;
    int _clientID = 0;
    io_context &m_io;
    void startAccept()
    {
        TCPConnection::pointer newConnection = TCPConnection::create(m_io);
        m_acceptor.async_accept(newConnection->socket(), boost::bind(&Server::handleAccept, this, newConnection, placeholders::error));
    }

    void handleAccept(TCPConnection::pointer newConnection, const boost::system::error_code &error)
    {
        if (!error)
        {
            newConnection->start();
        }
        //tcp::socket s = newConnection->socket();
        _mutex.lock();
        m_users.push_back(std::move(std::pair<tcp::socket, int>(
            std::move(newConnection->socket()), _clientID))); /// yeeey move
        // auto newSocket = m_serverSocket.acceptConnection();
        // newSocket.setReceiveTimeout(Poco::Timespan(1, 0));
        // m_sockets.push_back(std::pair<Poco::Net::StreamSocket,
        // int>(newSocket, _clientID));
        // newSocket.setReceiveTimeout(Poco::Timespan(1, 0));
        _mutex.unlock();
        cout << "New Client! ID: " << _clientID << endl;
        // Poco::Net::SocketStream ss(newSocket);
        auto strClientID = std::to_string(_clientID);
        // newSocket.sendBytes(strClientID.data(), strClientID.size());
        write(m_users[m_users.size() - 1].first,
              buffer(std::to_string(_clientID)));
        ++_clientID;
        startAccept();
    }

public:
    //constructor for accepting connection from client
    Server(io_context &io)
        : m_io(io), m_acceptor(io, ip::tcp::endpoint(
                                       ip::tcp::v4(), 4321))
    {
        startAccept();
        // m_serverSocket =
        // Poco::Net::ServerSocket(Poco::Net::SocketAddress("127.0.0.1:1234"));
    }
    void connect()
    {

        while (true)
        {
            tcp::socket s = m_acceptor.accept();
            _mutex.lock();
            m_users.push_back(std::move(std::pair<tcp::socket, int>(
                std::move(s), _clientID))); /// yeeey move
            // auto newSocket = m_serverSocket.acceptConnection();
            // newSocket.setReceiveTimeout(Poco::Timespan(1, 0));
            // m_sockets.push_back(std::pair<Poco::Net::StreamSocket,
            // int>(newSocket, _clientID));
            // newSocket.setReceiveTimeout(Poco::Timespan(1, 0));
            _mutex.unlock();
            cout << "New Client! ID: " << _clientID << endl;
            // Poco::Net::SocketStream ss(newSocket);
            auto strClientID = std::to_string(_clientID);
            // newSocket.sendBytes(strClientID.data(), strClientID.size());
            write(m_users[m_users.size() - 1].first,
                  buffer(std::to_string(_clientID)));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void onReceive()
    {
        while (true)
        {
            _mutex.lock();
            for (size_t i = 0; i < m_users.size(); ++i)
            {
                const auto noOfAvailableBytss = m_users[i].first.available();
                if (noOfAvailableBytss != 0 && noOfAvailableBytss <= maxBuffer)
                {
                    std::string data(noOfAvailableBytss, 0);
                    m_users[i].first.read_some(buffer(data));
                    cout << "Client " << m_users[i].second << data << endl;
                    std::regex r("\"");
                    Poco::JSON::Parser p;

                    auto parsed = p.parse(data);

                    auto obj = parsed.extract<Poco::JSON::Object::Ptr>();
                    auto receivingClientID = std::stoi(obj->get("ReceivingClientID").toString());
                    auto message = obj->get("Message").toString();
                    //cout << noOfAvailableBytss << " data available for Client !" << _sockets[i].second << endl;
                    for (size_t j = 0; j < m_users.size(); ++j)
                    {
                        if (j != i)
                        {
                            cout << "Sending message " << message << " to Client " << receivingClientID << endl;
                            write(m_users[j].first, buffer(message));
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
    try
    {
        io_context io;
        Server s(io);
        cout << "Waiting for clients..." << endl;
        io.run();
        // Server c(io);
        //std::thread t(&Server::connect, &c);
        //std::thread t2(&Server::onReceive, &c);
        //while (1)
        //{
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        //I'm in an IDIOT.
        //Without the sleep, this while loop was going on and on and
        // caused over 100% CPU usage :)
        //}
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}