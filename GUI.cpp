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

//#include <iostream>
//#include <optional>
//#include <boost/asio.hpp>

/*class session : public std::enable_shared_from_this<session>
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
}*/
/*
 * Copyright (C) 2008 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGlobal.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WTabWidget.h>
#include <Wt/WTable.h>
#include <Wt/WFileResource.h>
#include <Wt/WBootstrapTheme.h>
/*
 * A simple hello world application class which demonstrates how to react
 * to events, read input, and give feed-back.
 */
class HelloApplication : public Wt::WApplication
{
public:
    HelloApplication(const Wt::WEnvironment &env);
};

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
HelloApplication::HelloApplication(const Wt::WEnvironment &env)
    : WApplication(env)
{
    setTitle("WebChat"); // application title

    auto theme = std::make_shared<Wt::WBootstrapTheme>();
    theme->setVersion(Wt::WBootstrapTheme::Version::v3);
    setTheme(theme); // resources folder must be where the .wt file is. I guess the dir structure is hard coded there

    auto container = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto tabW = container->addNew<Wt::WTabWidget>();
    auto table = std::make_unique<Wt::WTable>();

    table->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Tescik00!"));
    table->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Tescik01!"));
    table->elementAt(1, 0)->addWidget(std::make_unique<Wt::WText>("Tescik10!"));
    table->elementAt(1, 1)->addWidget(std::make_unique<Wt::WText>("Tescik11!"));
    auto contactsTab = tabW->addTab(std::move(table), "Contacts", Wt::ContentLoading::Eager);

    tabW->addTab(Wt::cpp14::make_unique<Wt::WTextArea>("The contents of the tabs are pre-loaded in"
                                                       " the browser to ensure swift switching."),
                 "Preload", Wt::ContentLoading::Eager);
    tabW->addTab(Wt::cpp14::make_unique<Wt::WTextArea>("You could change any other style attribute of the"
                                                       " tab widget by modifying the style class."
                                                       " The style class 'trhead' is applied to this tab."),
                 "Style", Wt::ContentLoading::Eager);
}

int main(int argc, char **argv)
{
    /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */
    return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
        /*
     * You could read information from the environment to decide whether
     * the user has permission to start a new application
     */
        return std::make_unique<HelloApplication>(env);
    });
}