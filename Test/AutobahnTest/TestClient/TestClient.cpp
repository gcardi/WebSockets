//---------------------------------------------------------------------------
// TestClient - Autobahn|Testsuite client tester
//
// Connects to an Autobahn fuzzing server and runs through all test cases.
// For each case, it echoes back any received message (text or binary).
// Ping/pong is handled automatically by the WebSockets library.
//
// Usage: TestClient.exe [host] [port]
//        Default: localhost 9001
//
// To test with Autobahn|Testsuite:
//   1. Start the fuzzing server from WSL2/Docker (see run_client_test.sh)
//   2. Run this test client
//   3. Open reports/client/index.html for results
//---------------------------------------------------------------------------

#pragma hdrstop

#include <tchar.h>
#include <cstdio>
#include <memory>

#include <IdHTTP.hpp>

#include "WebSockets.h"

//---------------------------------------------------------------------------

using SvcApp::WebSockets::Opcode;
using SvcApp::WebSockets::CloseStatus;
using std::unique_ptr;

static String const AgentName = _D( "WebSocketsCppBuilder-Client" );

//---------------------------------------------------------------------------

static int GetCaseCount( String const & BaseURL )
{
    unique_ptr<TIdHTTP> HTTP( new TIdHTTP( nullptr ) );

    SvcApp::WebSockets::Client::WebSocket WS(
        *HTTP, BaseURL + _D( "/getCaseCount" )
    );

    String CountText = WS.ReadTextFrame( 10000 );
    WS.SendCloseFrame();

    return CountText.Trim().ToInt();
}

//---------------------------------------------------------------------------

static void RunCase( String const & BaseURL, int CaseNum )
{
    unique_ptr<TIdHTTP> HTTP( new TIdHTTP( nullptr ) );

    try {
        String URL = Format(
            _D( "%s/runCase?case=%d&agent=%s" ),
            ARRAYOFCONST( ( BaseURL, CaseNum, AgentName ) )
        );

        SvcApp::WebSockets::Client::WebSocket WS( *HTTP, URL );

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

        try {
            WS.SendCloseFrame( CloseReason, CloseText );
        }
        catch ( ... ) {
        }
    }
    catch ( Exception const & E ) {
        // Some test cases intentionally trigger protocol errors.
        std::printf( "  exception: %ls\n", E.Message.c_str() );
    }
    catch ( ... ) {
        std::printf( "  unknown exception\n" );
    }
}

//---------------------------------------------------------------------------

static void UpdateReports( String const & BaseURL )
{
    unique_ptr<TIdHTTP> HTTP( new TIdHTTP( nullptr ) );

    try {
        SvcApp::WebSockets::Client::WebSocket WS(
            *HTTP, BaseURL + _D( "/updateReports?agent=" ) + AgentName
        );

        // The fuzzing server closes the connection after generating reports.
        Opcode Type {};
        TBytes Data;
        CloseStatus CloseReason = CloseStatus::Normal;
        String CloseText;
        WS.ReadMessage( Type, Data, CloseReason, CloseText, 30000 );

        try {
            WS.SendCloseFrame();
        }
        catch ( ... ) {
        }
    }
    catch ( ... ) {
    }
}

//---------------------------------------------------------------------------

int _tmain( int argc, _TCHAR* argv[] )
{
    String Host = _D( "localhost" );
    int Port = 9001;

    if ( argc > 1 ) {
        Host = argv[1];
    }
    if ( argc > 2 ) {
        Port = _ttoi( argv[2] );
    }

    String BaseURL = Format(
        _D( "http://%s:%d" ),
        ARRAYOFCONST( ( Host, Port ) )
    );

    try {
        std::printf( "Connecting to Autobahn fuzzing server at %ls\n",
                     BaseURL.c_str() );

        int CaseCount = GetCaseCount( BaseURL );
        std::printf( "Total test cases: %d\n\n", CaseCount );

        for ( int i = 1; i <= CaseCount; ++i ) {
            std::printf( "Running case %d / %d ...\n", i, CaseCount );
            RunCase( BaseURL, i );
        }

        std::printf( "\nAll cases complete. Requesting report generation...\n" );
        UpdateReports( BaseURL );
        std::printf( "Done. Check reports/client/index.html for results.\n" );
    }
    catch ( Exception const & E ) {
        std::printf( "Fatal error: %ls\n", E.Message.c_str() );
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
