// TODO: NB boostasioscheduler.h includes boost asio
// so in Windows it should appear before cli.h and clilocalsession.h that include rang,
// because both include WinSock.h
// (consider to provide a global header file for the library)
#include <cli/boostasioscheduler.h>
#include <cli/boostasioremotecli.h>
namespace cli
{    
    using MainScheduler = BoostAsioScheduler;
    using CliTelnetServer = BoostAsioCliTelnetServer;
}

#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <cli/filehistorystorage.h>
#include <cli/clilocalsession.h>

using namespace cli;
using namespace std;


int main()
{
    // setup cli

    auto rootMenu = make_unique< Menu >( "cli" );
    rootMenu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, world\n"; },
            "Print hello world" );
    rootMenu -> Insert(
            "hello_everysession",
            [](std::ostream&){ Cli::cout() << "Hello, everybody" << std::endl; },
            "Print hello everybody on all open sessions" );
    rootMenu -> Insert(
            "answer",
            [](std::ostream& out, int x){ out << "The answer is: " << x << "\n"; },
            "Print the answer to Life, the Universe and Everything " );
    rootMenu -> Insert(
            "color",
            [](std::ostream& out){ out << "Colors ON\n"; SetColor(); },
            "Enable colors in the cli" );
    rootMenu -> Insert(
            "nocolor",
            [](std::ostream& out){ out << "Colors OFF\n"; SetNoColor(); },
            "Disable colors in the cli" );

    auto subMenu = make_unique< Menu >( "sub" );
    subMenu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, submenu world\n"; },
            "Print hello world in the submenu" );
    subMenu -> Insert(
            "demo",
            [](std::ostream& out){ out << "This is a sample!\n"; },
            "Print a demo string" );

    auto subSubMenu = make_unique< Menu >( "subsub" );
        subSubMenu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, subsubmenu world\n"; },
            "Print hello world in the sub-submenu" );
    subMenu -> Insert( std::move(subSubMenu));

    rootMenu -> Insert( std::move(subMenu) );

     // create a cli with the given root menu and a persistent storage
    // you must pass to FileHistoryStorage the path of the history file
    // if you don't pass the second argument, the cli will use a VolatileHistoryStorage object that keeps in memory
    // the history of all the sessions, until the cli is shut down.
    Cli cli( std::move(rootMenu), std::make_unique<FileHistoryStorage>(".cli") );
    // global exit action
    cli.ExitAction( [](auto& out){ out << "Goodbye and thanks for all the fish.\n"; } );
    // std exception custom handler
    cli.StdExceptionHandler(
        [](std::ostream& out, const std::string& cmd, const std::exception& e)
        {
            out << "Exception caught in cli handler: "
                << e.what()
                << " handling command: "
                << cmd
                << ".\n";
        }
    );

    MainScheduler scheduler;
    CliLocalTerminalSession localSession(cli, scheduler, std::cout, 200);
    localSession.ExitAction(
        [&scheduler](auto& out) // session exit action
        {
            out << "Closing App...\n";
            scheduler.Stop();
        }
    );

    // setup server

    CliTelnetServer server(scheduler, 5000, cli);
    // exit action for all the connections
    server.ExitAction( [](auto& out) { out << "Terminating this session...\n"; } );

    scheduler.Run();

    return 0;
}