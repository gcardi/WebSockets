//---------------------------------------------------------------------------
// EchoServer - Autobahn|Testsuite echo server
//
// A minimal WebSocket echo server for RFC 6455 compliance testing.
// Echoes back any text or binary message verbatim.
// Ping/pong is handled automatically by the WebSockets library.
//
// Usage: EchoServer.exe [port]
//        Default port is 9001.
//
// To test with Autobahn|Testsuite:
//   1. Start this echo server
//   2. Run the fuzzing client from WSL2/Docker (see run_server_test.sh)
//   3. Open reports/server/index.html for results
//---------------------------------------------------------------------------

#pragma hdrstop

#include <tchar.h>
#include <cstdio>

#include <IdHTTPServer.hpp>
#include <IdContext.hpp>
#include <IdCustomHTTPServer.hpp>

#include "WebSockets.h"

//---------------------------------------------------------------------------

using SvcApp::WebSockets::Opcode;
using SvcApp::WebSockets::CloseStatus;

//---------------------------------------------------------------------------

class TEchoHandler : public TObject
{
public:
    void __fastcall OnCommandGet(
        Idcontext::TIdContext* AContext,
        Idcustomhttpserver::TIdHTTPRequestInfo* ARequestInfo,
        Idcustomhttpserver::TIdHTTPResponseInfo* AResponseInfo )
    {
        SvcApp::WebSockets::Server::WebSocket WS(
            AContext, ARequestInfo, AResponseInfo
        );

        if ( !WS.IsWebSocket() ) {
            AResponseInfo->ResponseNo = 400;
            AResponseInfo->ContentText = _D( "WebSocket connections only" );
            return;
        }

        Opcode Type {};
        TBytes Data;
        CloseStatus CloseReason = CloseStatus::Normal;
        String CloseText;

        while ( WS.ReadMessage( Type, Data, CloseReason, CloseText, 30000 ) ) {
            switch ( Type ) {
                case Opcode::Text:
                case Opcode::Binary:
                    WS.SendFrame( Type, Data, true );
                    break;
                default:
                    break;
            }
        }

        // Reply to close frame. ReadMessage returns false on Close or
        // on frame-level errors (where it already sent a close frame).
        // Wrapping in try/catch avoids issues with double-close.
        try {
            WS.SendCloseFrame( CloseReason, CloseText );
        }
        catch ( ... ) {
        }
    }
};

//---------------------------------------------------------------------------

int _tmain( int argc, _TCHAR* argv[] )
{
    int Port = 9001;
    if ( argc > 1 ) {
        Port = _ttoi( argv[1] );
    }

    auto Handler = new TEchoHandler();
    auto Server = new TIdHTTPServer( nullptr );
    try {
        Server->OnCommandGet = Handler->OnCommandGet;
        Server->DefaultPort = Port;
        Server->Active = true;

        std::printf(
            "Autobahn echo server listening on port %d\n"
            "Press Enter to stop...\n",
            Port
        );

        std::getchar();

        Server->Active = false;
    }
    catch ( Exception const & E ) {
        std::printf( "Error: %ls\n", E.Message.c_str() );
    }

    delete Server;
    delete Handler;

    return 0;
}
//---------------------------------------------------------------------------
