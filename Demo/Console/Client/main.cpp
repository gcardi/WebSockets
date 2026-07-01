#include <windows.h>
#include <System.SysUtils.hpp>
#include <tchar.h>
#include <clocale>
#include <cstdio>
#include <IdHTTP.hpp>
#include <IdURI.hpp>

#include <memory>

#include "WebSockets.h"

using std::make_unique;

__attribute__((constructor)) void the_console_init(void)
{
    std::setlocale( LC_ALL, ".UTF-8" );
    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );
}

String BuildOriginHeader( String const & URL )
{
    auto URI = make_unique<Iduri::TIdURI>( URL );

    if ( URI->Protocol.IsEmpty() || URI->Host.IsEmpty() ) {
        throw Exception(
            _D( "Cannot build Origin header from URL: %s" ),
            ARRAYOFCONST( ( URL ) )
        );
    }

    String Origin = URI->Protocol + _D( "://" ) + URI->Host;
    if ( !URI->Port.IsEmpty() ) {
        Origin += _D( ":" ) + URI->Port;
    }
    return Origin;
}

std::unique_ptr<Idheaderlist::TIdHeaderList> BuildWebSocketHeaders(
    String const & URL )
{
    auto Headers = make_unique<Idheaderlist::TIdHeaderList>(
        Idglobalprotocols::QuoteHTTP
    );
    Headers->Values[_D( "Origin" )] = BuildOriginHeader( URL );
    return Headers;
}

int _tmain( int argc, _TCHAR* argv[] )
{
    try {
        using namespace SvcApp::WebSockets;

        String URL = _D( "http://127.0.0.1:8080/websocket" );
        if ( argc > 1 ) {
            URL = argv[1];
        }

        auto HTTP = std::make_unique<TIdHTTP>( nullptr );
        auto Headers = BuildWebSocketHeaders( URL );
        Client::WebSocket WS( *HTTP, URL, Headers.get() );

        String const Message = _D( "Hi, hello, world! 🌍 Éléphant 🦣 桁 (けた) ∮ E⋅da = Q" );
        //String const Message = _D( "Hello from a C++Builder console client" );
        WS.SendFrame( Message );

        String Reply;
        String CloseText;
        CloseStatus CloseReason { CloseStatus::Normal };

        if ( WS.ReadTextFrame( 10000, Reply, CloseText, CloseReason ) ) {
            std::printf( "Received(%d): %ls\n", Reply.Length(), Reply.c_str() );
        }
        else {
            std::printf(
                "Closed while waiting for reply: %d %ls\n",
                static_cast<int>( CloseReason ),
                CloseText.c_str()
            );
        }

        WS.SendCloseFrame( CloseStatus::Normal, _D( "done" ) );
    }
    catch ( Exception const & E ) {
        std::printf( "Failed: %ls\n", E.Message.c_str() );
        return 1;
    }

    return 0;
}
