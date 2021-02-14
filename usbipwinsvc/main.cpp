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

#include "Win32_Service.h"
using namespace cli;
using namespace std;

void run_telnet_server(void)
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
}

class Service : public WindowsService {
	using WindowsService::WindowsService;
private:
        bool b_server_running = false;
        MainScheduler* scheduler;
        CliTelnetServer* server;
        Cli* cli;

protected:
	virtual DWORD WINAPI worker(LPVOID) {
        std::string debugmsg = "USPIPSVC: worker called";
        OutputDebugString(debugmsg.c_str());
        this->scheduler->Run();
                //if(false == this->b_server_running)
                //{
                //        this->b_server_running = true;
                //        run_telnet_server();
                //}
                
		//  Periodically check if the service has been requested to stop
		while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {

			// Pause on pauseEvent
			if (WaitForSingleObject(pauseEvent, 0) != WAIT_OBJECT_0) {

				/*
				* Perform main service functionality here
				*/

				//  Simulate work
				//Sleep(3000);
                                
			}
			else {
				confirm_pause();
				// Wait for continue to be thrown
				WaitForSingleObject(continueEvent, INFINITE);
				confirm_continue();
			}
		}

		return ERROR_SUCCESS;
	}

	// Gets called during startup.
	virtual void on_startup() {
        //__debugbreak();
		/*
		 * Perform tasks necessary to start the service here.
		 */
                //run_telnet_server();
        //this->scheduler.Run();
	}

	// Gets called when pause is thrown and pause/continue is enabled.
	virtual void on_pause() {
		/*
		 * Perform tasks necessary to pause the service here.
		 */
	}

	// Gets called when continue is thrown and pause/continue is enabled.
	virtual void on_continue() {
		/*
		 * Perform tasks necessary to continue the service here.
		 */
	}

	// Gets called when the windows service controller tells service to stop, but BEFORE stopEvent is thrown.
	virtual void on_stop() {
		/*
		 * Perform tasks necessary to stop the service loop here.
		 */
	}

	// Gets called all the way at the end of the liecycle.
	virtual void on_exit() {
		/*
		 * Perform tasks necessary for cleanup.
		 */
	}

	// What to do when the service fails to regiser.
	// By default it runs test_startStop();
	virtual void on_failedRegistration() {
		/*
		 * what to do when registration fails...
		 */
		 test_startStop();
	}

public:
    Service(std::string _name, bool _canPauseContinue)
        : WindowsService(_name, _canPauseContinue)
    {
        // setup cli

        auto rootMenu = make_unique< Menu >("cli");
        rootMenu->Insert(
            "hello",
            [](std::ostream& out) { out << "Hello, world\n"; },
            "Print hello world");
        rootMenu->Insert(
            "hello_everysession",
            [](std::ostream&) { Cli::cout() << "Hello, everybody" << std::endl; },
            "Print hello everybody on all open sessions");
        rootMenu->Insert(
            "answer",
            [](std::ostream& out, int x) { out << "The answer is: " << x << "\n"; },
            "Print the answer to Life, the Universe and Everything ");
        rootMenu->Insert(
            "color",
            [](std::ostream& out) { out << "Colors ON\n"; SetColor(); },
            "Enable colors in the cli");
        rootMenu->Insert(
            "nocolor",
            [](std::ostream& out) { out << "Colors OFF\n"; SetNoColor(); },
            "Disable colors in the cli");

        auto subMenu = make_unique< Menu >("sub");
        subMenu->Insert(
            "hello",
            [](std::ostream& out) { out << "Hello, submenu world\n"; },
            "Print hello world in the submenu");
        subMenu->Insert(
            "demo",
            [](std::ostream& out) { out << "This is a sample!\n"; },
            "Print a demo string");

        auto subSubMenu = make_unique< Menu >("subsub");
        subSubMenu->Insert(
            "hello",
            [](std::ostream& out) { out << "Hello, subsubmenu world\n"; },
            "Print hello world in the sub-submenu");
        subMenu->Insert(std::move(subSubMenu));

        rootMenu->Insert(std::move(subMenu));

        // create a cli with the given root menu and a persistent storage
       // you must pass to FileHistoryStorage the path of the history file
       // if you don't pass the second argument, the cli will use a VolatileHistoryStorage object that keeps in memory
       // the history of all the sessions, until the cli is shut down.

        this->cli = new Cli(std::move(rootMenu), std::make_unique<FileHistoryStorage>(".cli"));
        //Cli cli(std::move(rootMenu), std::make_unique<FileHistoryStorage>(".cli"));
        // global exit action
        this->cli->ExitAction([](auto& out) { out << "Goodbye and thanks for all the fish.\n"; });
        // std exception custom handler
        this->cli->StdExceptionHandler(
            [](std::ostream& out, const std::string& cmd, const std::exception& e)
            {
                out << "Exception caught in cli handler: "
                    << e.what()
                    << " handling command: "
                    << cmd
                    << ".\n";
            }
        );

        this->scheduler = new MainScheduler();
        //CliLocalTerminalSession localSession(  , scheduler, std::cout, 200);
        //localSession.ExitAction(
        //    [this](auto& out) // session exit action
        //    {
        //        out << "Closing App...\n";
        //        this->scheduler->Stop();
        //    }
        //);

        // setup server
        //CliTelnetServer _server( *(scheduler.get()), 5000, *(cli) );
        this->server = new CliTelnetServer( *scheduler, 5000, *cli );
        // exit action for all the connections
        this->server->ExitAction([](auto& out) { out << "Terminating this session...\n"; });

        //scheduler.Run();

    }
};

int main()
{
    Service test("test", true);
	return test.run();
        
        //run_telnet_server();
        //return 0;
}