// #include <iostream>
// #include <wx/wx.h>
// #include <wx/grid.h>
// class MyApp : public wxApp
// {
// public:
//     virtual bool OnInit();
// };

// class Simple : public wxFrame
// {
// public:
//     Simple(const wxString &title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 600), wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
//     {
//         Centre();
//     }
// };

// IMPLEMENT_APP(MyApp)

// bool MyApp::OnInit()
// {
//     Simple *simple = new Simple(wxT("Simple"));
//     simple->Show(true);
//     //wxStaticText *staticText = new wxStaticText(simple, wxID_ANY, "Hejka");
//     //staticText->Show(true);
//     //staticText->SetLabelText("Hejka\nco tam?");
//     wxTextCtrl *textCtrl = new wxTextCtrl(simple, wxID_ANY, wxEmptyString, wxPoint(1, 300), wxSize(598, 250));
//     textCtrl->Show(true);
//     wxGrid *grid = new wxGrid(simple, wxID_ANY, wxPoint(1, 1), wxSize(598, 250));
//     grid->Show(true);
//     return true;
// }
// // I think I will ditch GUI.. it just takes too much time to get the API etc
// // Gonna try with qt and qt creator to create xml files, etc

#include <iostream>
#include <optional>
#include <boost/asio.hpp>

class session : public std::enable_shared_from_this<session>
{
public:
    session(boost::asio::ip::tcp::socket &&socket)
        : socket(std::move(socket))
    {
    }

    void start()
    {
        boost::asio::async_read_until(socket, streambuf, '\n', [self = shared_from_this()](boost::system::error_code error, std::size_t bytes_transferred) {
            std::cout << std::istream(&self->streambuf).rdbuf();
        });
    }

private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf streambuf;
};

class server
{
public:
    server(boost::asio::io_context &io_context, std::uint16_t port)
        : io_context(io_context), acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
    }

    void async_accept()
    {
        socket.emplace(io_context);

        acceptor.async_accept(*socket, [&](boost::system::error_code error) {
            std::make_shared<session>(std::move(*socket))->start();
            async_accept();
        });
    }

private:
    boost::asio::io_context &io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    std::optional<boost::asio::ip::tcp::socket> socket;
};

int main()
{
    boost::asio::io_context io_context;
    server srv(io_context, 15001);
    srv.async_accept();
    io_context.run();
    return 0;
}