#include <cli/clilocalsession.h> // include boost asio
#include <cli/remotecli.h>
// TODO. NB: remotecli.h and clilocalsession.h both includes boost asio,
// so in Windows it should appear before cli.h that include rang
// (consider to provide a global header file for the library)
#include <cli/cli.h>
#include <cli/filehistorystorage.h>

#define ENABLE_TELNET_SERVER

using namespace cli;
using namespace std;

#include "Win32_Service.h"

enum class TelnetStateFlags
{
    UNDEF   = 0,
    INIT    = 1 << 00,
    RUNNING = 1 << 01,
    STOPPED = 1 << 02,
};

class Service : public WindowsService {
	using WindowsService::WindowsService;
private:
        boost::asio::io_context* ios;
        CliTelnetServer* server;
        Cli* cli;
        TelnetStateFlags telnet_state;

protected:
	virtual DWORD WINAPI worker(LPVOID) {
        std::string debugmsg = "USBIPSVC: worker called";
        OutputDebugString(debugmsg.c_str());
        if (telnet_state == TelnetStateFlags::INIT) {
            this->ios->run();
            telnet_state = TelnetStateFlags::RUNNING;
        }
                
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
        std::string debugmsg = "USBIPSVC: on_startup";
        OutputDebugString(debugmsg.c_str());
        //__debugbreak();
		/*
		 * Perform tasks necessary to start the service here.
		 */
        telnet_state = TelnetStateFlags::INIT;
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
        this->ios->stop();
        telnet_state = TelnetStateFlags::STOPPED;
        std::string debugmsg = "USBIPSVC: on_stop";
        OutputDebugString(debugmsg.c_str());
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

        this->ios = new boost::asio::io_context();

        // setup server
        this->server = new CliTelnetServer(*ios, 5000, *cli);

        // exit action for all the connections
        this->server->ExitAction([](auto& out) { out << "Terminating this session...\n"; });
    }
};

int main()
{
    Service usb_ip_svc("USBIP Service", true);
	return usb_ip_svc.run();
}