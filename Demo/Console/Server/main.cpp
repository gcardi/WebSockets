#include <windows.h>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <tchar.h>
#include <clocale>
#include <cstdio>
#include <memory>

#include <IdContext.hpp>
#include <IdCustomHTTPServer.hpp>
#include <IdHTTPServer.hpp>

#include "WebSockets.h"

using std::make_unique;

namespace {

constexpr int DefaultPort = 8080;
constexpr int ReadTimeoutMs = 10000;

void InitConsole()
{
    std::setlocale( LC_ALL, ".UTF-8" );
    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );
}

int ParsePort( int argc, _TCHAR* argv[] )
{
    if ( argc <= 1 ) {
        return DefaultPort;
    }

    int const Port = StrToInt( argv[1] );
    if ( Port <= 0 || Port > 65535 ) {
        throw Exception(
            _D( "Invalid TCP port: %s" ),
            ARRAYOFCONST( ( String( argv[1] ) ) )
        );
    }
    return Port;
}

std::unique_ptr<TStringList> BuildAllowedOrigins( int Port )
{
    auto Origins = make_unique<TStringList>();
    String const PortText = IntToStr( Port );

    Origins->Add( _D( "http://localhost:" ) + PortText );
    Origins->Add( _D( "http://127.0.0.1:" ) + PortText );
    Origins->Add( _D( "http://[::1]:" ) + PortText );
    Origins->Add( _D( "https://localhost:" ) + PortText );
    Origins->Add( _D( "https://127.0.0.1:" ) + PortText );
    Origins->Add( _D( "https://[::1]:" ) + PortText );

    return Origins;
}

String PeerText( TIdContext* Context )
{
    if ( Context == nullptr || Context->Connection == nullptr ||
         Context->Connection->Socket == nullptr ||
         Context->Connection->Socket->Binding == nullptr )
    {
        return _D( "<unknown>" );
    }

    return Format(
        _D( "%s:%d" ),
        ARRAYOFCONST( (
            Context->Connection->Socket->Binding->PeerIP,
            Context->Connection->Socket->Binding->PeerPort
        ) )
    );
}

class TConsoleWebSocketServer : public TIdHTTPServer {
public:
    explicit __fastcall TConsoleWebSocketServer( int Port )
        : TIdHTTPServer( nullptr )
        , allowedOrigins_( BuildAllowedOrigins( Port ) )
    {
        DefaultPort = Port;
        KeepAlive = true;

        handshakeOptions_.AllowedOrigins = allowedOrigins_.get();
        handshakeOptions_.RequireOrigin = true;
        handshakeOptions_.RejectExtensions = true;
    }

protected:
    void __fastcall DoCommandGet(
        TIdContext* AContext,
        TIdHTTPRequestInfo* ARequestInfo,
        TIdHTTPResponseInfo* AResponseInfo ) override
    {
        if ( ARequestInfo->Document != _D( "/websocket" ) ) {
            AResponseInfo->ContentType = _D( "text/plain; charset=utf-8" );
            AResponseInfo->ContentText =
                _D( "WebSocket echo server. Connect to /websocket.\n" );
            return;
        }

        HandleWebSocket( AContext, ARequestInfo, AResponseInfo );
    }

private:
    std::unique_ptr<TStringList> allowedOrigins_;
    SvcApp::WebSockets::Server::HandshakeOptions handshakeOptions_;

    void HandleWebSocket(
        TIdContext* AContext,
        TIdHTTPRequestInfo* ARequestInfo,
        TIdHTTPResponseInfo* AResponseInfo )
    {
        using namespace SvcApp::WebSockets;

        Server::WebSocket WS(
            AContext, ARequestInfo, AResponseInfo, &handshakeOptions_
        );

        if ( !WS.IsWebSocket() ) {
            std::printf(
                "Rejected %ls: HTTP %d %ls\n",
                PeerText( AContext ).c_str(),
                AResponseInfo->ResponseNo,
                AResponseInfo->ResponseText.c_str()
            );
            return;
        }

        std::printf( "WebSocket connected: %ls\n", PeerText( AContext ).c_str() );

        Opcode Type {};
        TBytes Data;
        CloseStatus CloseReason { CloseStatus::Normal };
        String CloseText;

        while ( WS.ReadMessage( Type, Data, CloseReason, CloseText, ReadTimeoutMs ) ) {
            if ( Type == Opcode::Text ) {
                String const Text = TEncoding::UTF8->GetString( Data );
                std::printf( "Received text(%d): %ls\n", Text.Length(), Text.c_str() );
                WS.SendFrame( Text );
            }
            else if ( Type == Opcode::Binary ) {
                std::printf( "Received binary(%lld bytes)\n", Data.Length );
                WS.SendFrame( Data );
            }
            else {
                std::printf(
                    "Received %ls frame\n",
                    SvcApp::WebSockets::ToString( Type ).c_str()
                );
            }
        }

        std::printf(
            "WebSocket closed: %ls (%d %ls)\n",
            PeerText( AContext ).c_str(),
            static_cast<int>( CloseReason ),
            CloseText.c_str()
        );

        WS.SendCloseFrame( CloseReason, CloseText );
        AContext->Connection->Disconnect();
    }
};

} // anonymous namespace

int _tmain( int argc, _TCHAR* argv[] )
{
    InitConsole();

    try {
        int const Port = ParsePort( argc, argv );
        auto Server = make_unique<TConsoleWebSocketServer>( Port );

        Server->Active = true;

        std::printf( "WebSocket echo server listening on http://127.0.0.1:%d/websocket\n", Port );
        std::printf( "Press Enter to stop.\n" );
        std::getchar();

        Server->Active = false;
        Server->Bindings->Clear();
    }
    catch ( Exception const & E ) {
        std::printf( "Failed: %ls\n", E.Message.c_str() );
        return 1;
    }

    return 0;
}
