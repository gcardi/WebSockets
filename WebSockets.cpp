//---------------------------------------------------------------------------

#pragma hdrstop

#include <algorithm>
#include <iterator>
#include <memory>

#include <IdHashSHA.hpp>
#include <IdCoderMIME.hpp>

#include <System.Math.hpp>

#include "WebSockets.h"

using std::copy;
using std::begin;
using std::end;
using std::make_unique;
using std::min;

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
namespace SvcApp {
//---------------------------------------------------------------------------
namespace WebSockets {
//---------------------------------------------------------------------------

static constexpr LPCTSTR SecWSKeyGUID = _D( "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );

//---------------------------------------------------------------------------

bool ToCloseStatus( uint16_t Code, CloseStatus& Status )
{
    switch ( Code ) {
        case 1000: Status = CloseStatus::Normal;            return true;
        case 1001: Status = CloseStatus::GoingAway;         return true;
        case 1002: Status = CloseStatus::ProtocolError;     return true;
        case 1003: Status = CloseStatus::UnhandledDataType; return true;
        case 1004: Status = CloseStatus::Reserved;          return true;
        case 1005: Status = CloseStatus::ReservedNoStatus;  return true;
        case 1006: Status = CloseStatus::ReservedAbnormal;  return true;
        case 1007: Status = CloseStatus::InconsistentData;  return true;
        case 1008: Status = CloseStatus::PolicyError;       return true;
        case 1009: Status = CloseStatus::ToBigMessage;      return true;
        case 1010: Status = CloseStatus::MissingExtenstion; return true;
        case 1011: Status = CloseStatus::UnExpectedError;   return true;
        case 1015: Status = CloseStatus::ReservedTLSError;  return true;
        default:   return false;
    }
}
//---------------------------------------------------------------------------

String ToString( Opcode Type )
{
    switch ( Type ) {
        case Opcode::Continuation:     return _D( "Continuation" );
        case Opcode::Text:             return _D( "Text" );
        case Opcode::Binary:           return _D( "Binary" );
        case Opcode::Close:            return _D( "Close" );
        case Opcode::Ping:             return _D( "Ping" );
        case Opcode::Pong:             return _D( "Pong" );
        default:                          return _D( "Unknown" );
    }
}
//---------------------------------------------------------------------------

bool WebSocket::ReadFrameHeader( TIdIOHandler& IOHandler, TBytes& InBuffer,
                                 size_t& PayloadLen, size_t& PayloadPos,
                                 CloseStatus& CloseReason, String& CloseText )
{
    IOHandler.ReadBytes( InBuffer, 2 );

    size_t Pos { 2 };

    switch ( GetLen( InBuffer ) ) {
        case 126:
            IOHandler.ReadBytes( InBuffer, 2, true );
            PayloadLen =
                ntohs( *reinterpret_cast<uint32_t*>( &InBuffer[Pos] ) );
            Pos += 2;
            break;
        case 127:
            {
                IOHandler.ReadBytes( InBuffer, 8, true );
                uint64_t const FLen =
                    ntohll( *reinterpret_cast<uint64_t*>( &InBuffer[Pos] ) );
                if ( FLen > 1073741824 ) {
                    CloseReason = CloseStatus::ToBigMessage;
                    CloseText = _D( "Payload data size exceeds 1 GiB" );
                    return false;
                }
                Pos += 8;
                PayloadLen = static_cast<size_t>( FLen );
            }
            break;
        default:
            PayloadLen = GetLen( InBuffer );
            break;
    }

    switch ( GetOpcode( InBuffer) ) {
        case Opcode::Continuation:
        case Opcode::Text:
        case Opcode::Binary:
            break;
        case Opcode::Close:
        case Opcode::Ping:
        case Opcode::Pong:
            if ( !GetFin( InBuffer ) ) {
                CloseReason = CloseStatus::ProtocolError;
                CloseText = _D( "Control frames must have FIN bit set" );
                return false;
            }
            if ( PayloadLen > 125 ) {
                CloseReason = CloseStatus::ToBigMessage;
                CloseText =
                    _D( "Control frames must have payload data size less than 126" );
                return false;
            }
            break;
        default:
            CloseReason = CloseStatus::ProtocolError;
            CloseText = _D( "Unknown opcode" );
            return false;
    }

    PayloadPos = Pos;

    return true;
}
//---------------------------------------------------------------------------

TBytes WebSocket::BuildFrameHeader( Opcode Type, int DataLen, bool Fin, bool Masked )
{
    TBytes Header;
    size_t Pos {};

    if ( DataLen < 126 ) {
        Header.Length = 2 + ( Masked ? 4 : 0 );
        Header[Pos++] = static_cast<System::Byte>( Type ) | ( Fin ? 128 : 0 );
        Header[Pos++] =
            static_cast<System::Byte>( DataLen ) | ( Masked ? 128 : 0 );
    }
    else if ( DataLen < 65536 ) {
        Header.Length = 4 + ( Masked ? 4 : 0 );
        Header[Pos++] = static_cast<System::Byte>( Type ) | ( Fin ? 128 : 0 );
        Header[Pos++] = Masked ? 126 + 128 : 126;
        Header[Pos++] = static_cast<System::Byte>( DataLen >> 8 );
        Header[Pos++] = static_cast<System::Byte>( DataLen & 255 );
    }
    else {
        Header.Length = 10 + + ( Masked ? 4 : 0 );
        Header[Pos++] = static_cast<System::Byte>( Type ) | ( Fin ? 128 : 0 );
        Header[Pos++] = Masked ? 127 + 128 : 127;
        Header[Pos++] = static_cast<System::Byte>( 0 );
        Header[Pos++] = static_cast<System::Byte>( 0 );
        Header[Pos++] = static_cast<System::Byte>( 0 );
        Header[Pos++] = static_cast<System::Byte>( 0 );
        Header[Pos++] = static_cast<System::Byte>( DataLen >> 24 );
        Header[Pos++] = static_cast<System::Byte>( DataLen >> 16 );
        Header[Pos++] = static_cast<System::Byte>( DataLen >> 8 );
        Header[Pos++] = static_cast<System::Byte>( DataLen & 255 );
    }

    return Header;
}
//---------------------------------------------------------------------------

bool WebSocket::DoReadFragmentedData( TBytes& AData, CloseStatus& CloseReason,
                                      String& CloseText )
{
    TBytes Buffer;
    size_t PayloadLen {};
    size_t PayloadPos {};
    //auto& IOHandler = GetIOHandler();

    static constexpr size_t ChunckSize = 4096;

    TBytes Data;
    Data.Length = ChunckSize;
    size_t DataLen {};

    for ( ;; ) {
        Buffer.Length = 0;

        if ( DoReadFrame( Buffer, PayloadLen, PayloadPos, CloseReason, CloseText ) ) {
            switch ( GetOpcode( Buffer ) ) {
                case Opcode::Ping:
                    SendPongFrame( Buffer.CopyRange( PayloadPos, PayloadLen ) );
                    break;
                case Opcode::Pong:
                    // A response to an unsolicited Pong frame is not expected.
                    break;
                case Opcode::Binary:
                case Opcode::Text:
                    CloseReason = CloseStatus::ProtocolError;
                    CloseText =
                        _D( "Data frame received: a continuation frame was expected." );
                    return false;
                case Opcode::Close:
                    CloseReason = CloseStatus::ProtocolError;
                    CloseText =
                        _D( "Close frame received: a continuation frame was expected." );
                    return false;
                case Opcode::Continuation:
                    if ( DataLen + PayloadLen > Data.Length ) {
                        // slarga il buffer
                        Data.Length = Data.Length + ChunckSize;
                    }
                    copy(
                        &Buffer[0] + PayloadPos,
                        &Buffer[0] + PayloadPos + PayloadLen,
                        &Data[0] + DataLen
                    );

                    DataLen += PayloadLen;

                    if ( GetFin( Buffer ) ) {
                        auto const DstPos = AData.Length;
                        AData.Length = DstPos + DataLen;
                        copy(
                            &Data[0],
                            &Data[0] + DataLen,
                            &AData[0] + DstPos
                        );
                        return true;
                    }
                    break;
            }
        }
        else {
            SendCloseFrame( CloseReason, CloseText );
            break;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

bool WebSocket::DoReadMessage( Opcode& AType, TBytes& AData,
                               CloseStatus& CloseReason, String& CloseText )
{
    TBytes Buffer;
    size_t PayloadLen {};
    size_t PayloadPos {};

    for ( ;; ) {
        Buffer.Length = 0;

        if ( DoReadFrame( Buffer, PayloadLen, PayloadPos, CloseReason, CloseText ) ) {
            switch ( auto const Type = GetOpcode( Buffer ) ) {
                case Opcode::Ping:
                    SendPongFrame( Buffer.CopyRange( PayloadPos, PayloadLen ) );
                    break;
                case Opcode::Pong:
                    // A response to an unsolicited Pong frame is not expected.
                    break;
                case Opcode::Binary:
                case Opcode::Text:
                    AData = Buffer.CopyRange( PayloadPos, PayloadLen );
                    if ( GetFin( Buffer ) ) {
                        AType = Type;
                        return true;
                    }
                    else {
                        // go to FRAGDATA state
                        if ( DoReadFragmentedData( AData, CloseReason, CloseText ) ) {
                            AType = Type;
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                    break;
                case Opcode::Close:
                    AType = Type;
                    if ( PayloadLen >= 2 ) {
                        uint16_t const Code =
                            ( static_cast<uint16_t>( Buffer[PayloadPos] ) << 8 ) |
                            static_cast<uint16_t>( Buffer[PayloadPos + 1 ] );
                        ToCloseStatus( Code, CloseReason );
                    }
                    if ( PayloadLen > 2 ) {
                        CloseText =
                            TEncoding::UTF8->GetString(
                                Buffer.CopyRange(
                                    PayloadPos + 2,
                                    PayloadLen - 2
                                )
                            );
                    }
                    return false;
                case Opcode::Continuation:
                    CloseReason = CloseStatus::ProtocolError;
                    CloseText = _D( "Continuation frame received at start position" );
                    return false;
            }
        }
        else {
            SendCloseFrame( CloseReason, CloseText );
            break;
        }
    }
    return false;
}
//---------------------------------------------------------------------------

void WebSocket::SendCloseFrame( CloseStatus CloseReason, TBytes const & CloseData )
{
    TBytes Data;

    Data.Length = 2 + CloseData.Length;
    Data[0] = static_cast<System::Byte>( static_cast<uint16_t>( CloseReason ) >> 8 );
    Data[1] = static_cast<System::Byte>( static_cast<uint16_t>( CloseReason ) & 255 );
    copy(
        begin( const_cast<TBytes&>( CloseData ) ),
        end( const_cast<TBytes&>( CloseData ) ),
        &Data[2]
    );
    SendFrame( Opcode::Close, Data, true );
}
//---------------------------------------------------------------------------

void WebSocket::DoTryToProcessControlFrame( TBytes& Buffer )
{
    auto& IOHandler = GetIOHandler();
    IOHandler.CheckForDataOnSource();
    IOHandler.CheckForDisconnect();
    if ( !IOHandler.InputBufferIsEmpty() ) {
        IOHandler.ReadBytes( Buffer, IOHandler.InputBuffer->Size );

    }
}
//---------------------------------------------------------------------------

void WebSocket::DoSendMessage( Opcode& Type, TBytes const & Data,
                               CloseStatus& CloseReason, String& CloseText,
                               size_t MaxChunkSize )
{
    if ( Data.Length > MaxChunkSize ) {
        SendFrame( Type, Data.CopyRange( 0, MaxChunkSize ), false );

        TIdBytes CtrlBuffer;

        for ( size_t Idx = MaxChunkSize ; Idx < Data.Length ; Idx += MaxChunkSize ) {
            DoTryToProcessControlFrame( CtrlBuffer );
            auto const ChunkSize = min( Data.Length - Idx, MaxChunkSize );
            SendFrame(
                    Opcode::Continuation,
                Data.CopyRange( Idx, ChunkSize ),
                Idx + MaxChunkSize >= Data.Length
            );
        }
    }
    else {
        SendFrame( Type, Data, true );
    }
}
//---------------------------------------------------------------------------

bool WebSocket::ReadTextFrame( int Timeout, String& Text, String& CloseText,
                               CloseStatus& CloseReason )
{
    TBytes Buffer;
    size_t PayloadLen {};
    size_t PayloadPos {};
    bool Masked {};
    bool Fin {};

    Buffer.Length = 0;
    Opcode Type {};

    if ( ReadFrame( Buffer, PayloadLen, PayloadPos, CloseReason, CloseText, Timeout ) ) {
        Text = TEncoding::UTF8->GetString( Buffer, PayloadPos, PayloadLen );
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

String WebSocket::ReadTextFrame( int Timeout )
{
    String Text;
    String CloseText;
    CloseStatus CloseReason;

    if ( ReadTextFrame( Timeout, Text, CloseText, CloseReason  ) ) {
        return Text;
    }
    else {
        throw Exception(
            _D( "Closed while reading text frame: %d %s\n" ),
            ARRAYOFCONST( (
                static_cast<int>( CloseReason ),
                CloseText
            ) )
        );
    }
}

//---------------------------------------------------------------------------
namespace Client {
//---------------------------------------------------------------------------

WebSocket::WebSocket( TIdHTTP& IdHTTP, String URL,
                      Idheaderlist::TIdHeaderList* AdditionalCustomHeader )
  : idHTTP_( IdHTTP )
{
    TryToUpgrade( URL, AdditionalCustomHeader );
}
//---------------------------------------------------------------------------

TIdIOHandler& WebSocket::DoGetIOHandler() const
{
    return *idHTTP_.IOHandler;
}
//---------------------------------------------------------------------------

String WebSocket::GenerateSecretKey()
{
    auto SecKeySB = make_unique<TStringBuilder>();
    for ( int i = 0 ; i < 16 ; ++i ) {
        SecKeySB->Append( Char( Random( 127 - 32 ) + 32 ) );
    }
    return TIdEncoderMIME::EncodeString( SecKeySB->ToString() );
}
//---------------------------------------------------------------------------

void WebSocket::TryToUpgrade( String URL, Idheaderlist::TIdHeaderList* AdditionalCustomHeader )
{
    Randomize();

    auto SecWSKey = GenerateSecretKey();

    // base64 encoded
    idHTTP_.Request->CustomHeaders->Clear();
    idHTTP_.Request->CustomHeaders->AddValue(
        _D( "Upgrade" ), _D( "websocket" )
    );
    idHTTP_.Request->CustomHeaders->AddValue(
        _D( "Sec-WebSocket-Version" ), _D( "13" )
    );
    idHTTP_.Request->CustomHeaders->AddValue(
        _D( "Sec-WebSocket-Key" ), SecWSKey
    );

    idHTTP_.Request->CustomHeaders->AddValue(
        _D( "Connection" ), _D( "Upgrade" )
    );

    if ( AdditionalCustomHeader ) {
        AdditionalCustomHeader->AddStrings( AdditionalCustomHeader );
    }

    idHTTP_.Get( URL );

    auto& Response = *idHTTP_.Response;

    // https://www.indyproject.org/2018/10/15/new-tidhttp-functionality-for-protocol-upgrading/

    if ( Response.ResponseCode != 101 ) {
        throw Exception(
            _D( "Error while upgrading: %d: %s" ),
            ARRAYOFCONST( (
                idHTTP_.Response->ResponseCode,
                idHTTP_.Response->ResponseText
            ) )
        );
    }

    auto const ResponseConnTxt = idHTTP_.Response->Connection;
    if ( !SameText( ResponseConnTxt, _D( "upgrade" ) ) ) {
        throw Exception(
            _D( "Connection not upgraded: %s" ),
            ARRAYOFCONST( ( ResponseConnTxt ) )
        );
    }

    auto const UpgradeTxt =
        idHTTP_.Response->RawHeaders->Values[ResponseConnTxt];
    if ( !SameText( UpgradeTxt, _D( "websocket" ) ) ) {
        throw Exception(
            _D( "Not upgraded to websocket: %s" ),
            ARRAYOFCONST( ( UpgradeTxt ) )
        );
    }

    auto Hash = make_unique<TIdHashSHA1>();

    String MatchingSecWSKey =
        TIdEncoderMIME::EncodeBytes(
            Hash->HashString(
                SecWSKey +
                SecWSKeyGUID
            )
        );

    auto SecWebSocketAccept =
        Response.RawHeaders->Values[_D( "sec-websocket-accept" )];
    if ( !SameText( SecWebSocketAccept, MatchingSecWSKey ) ) {
        throw Exception(
            _D( "Key handshake failed: \'%s\' vs \'%s\'" ),
            ARRAYOFCONST( ( SecWebSocketAccept, MatchingSecWSKey ) )
        );
    }
}
//---------------------------------------------------------------------------

bool WebSocket::DoReadFrame( TBytes& InBuffer, size_t& PayloadLen,
                             size_t& PayloadPos, CloseStatus& Reason,
                             String& CloseText )
{
    auto& IOHandler = GetIOHandler();

    if ( ReadFrameHeader( IOHandler, InBuffer, PayloadLen, PayloadPos, Reason,
                          CloseText ) )
    {
        if ( !GetMasked( InBuffer ) ) {
            IOHandler.ReadBytes( InBuffer, PayloadLen );
            return true;
        }
        else {
            Reason = CloseStatus::ProtocolError;
            CloseText = _D( "Received a masked frame" );
        }
    }
    return false;
}
//---------------------------------------------------------------------------

void WebSocket::DoSendEmptyFrame( Opcode Type )
{
    TBytes Header;

    Header.Length = 6;
    Header[0] = static_cast<System::Byte>( Type ) | 128;
    Header[1] = 128;
    auto& Mask = *reinterpret_cast<MaskData * const>( &Header[2] );
    Mask.d_ = GenerateMask();
    GetIOHandler().Write( Header );
}
//---------------------------------------------------------------------------

void WebSocket::DoSendFrame( Opcode Type, TBytes const & Data, bool Fin )
{
    TBytes Frame = BuildFrameHeader( Type, Data.Length, Fin, true );

    auto const DataIdx = Frame.Length;
    auto const MaskIdx = DataIdx - 4;

    auto& Mask =
        *reinterpret_cast<MaskData * const>( &Frame[0] + MaskIdx );
    Mask.d_ = GenerateMask();

    if ( auto const PayloadLen = Data.Length ) {
        Frame.Length = Frame.Length + Data.Length;
        for( auto Idx = 0; Idx < PayloadLen ; ++Idx ) {
            Frame[DataIdx + Idx] = Data[Idx] ^ Mask.a_[Idx & 3];
        }
    }
    GetIOHandler().Write( Frame );
}
//---------------------------------------------------------------------------

uint32_t WebSocket::GenerateMask() const
{
    return RandomRange( 0, INT32_MAX );
}

//---------------------------------------------------------------------------
} // End of namespace Client
//---------------------------------------------------------------------------
namespace Server {
//---------------------------------------------------------------------------

TIdIOHandler& WebSocket::DoGetIOHandler() const
{
    return *thread_.Connection->IOHandler;
};
//---------------------------------------------------------------------------

bool WebSocket::DoReadFrame( TBytes& InBuffer, size_t& PayloadLen,
                             size_t& PayloadPos, CloseStatus& CloseReason,
                             String& CloseText )
{
    auto& IOHandler = GetIOHandler();

    if ( ReadFrameHeader( IOHandler, InBuffer, PayloadLen, PayloadPos,
                          CloseReason, CloseText ) )
    {
        if ( GetMasked( InBuffer ) ) {
            IOHandler.ReadBytes( InBuffer, PayloadLen + 4, true );
            auto& Mask =
                *reinterpret_cast<MaskData* const>( &InBuffer[PayloadPos] );
            PayloadPos += 4;
            for( auto Idx = 0; Idx < PayloadLen ; ++Idx ) {
                InBuffer[Idx + PayloadPos] ^= Mask.a_[Idx & 3];
            }
            return true;
        }
        else {
            CloseReason = CloseStatus::ProtocolError;
            CloseText = _D( "unmasked frame received" );
        }
    }
    return false;
}
//---------------------------------------------------------------------------

void WebSocket::DoSendEmptyFrame( Opcode Type )
{
    TBytes Header;

    Header.Length = 2;
    Header[0] = static_cast<System::Byte>( Type ) | 128;
    Header[1] = 128;
    GetIOHandler().Write( Header );
}
//---------------------------------------------------------------------------

void WebSocket::DoSendFrame( Opcode Type, TBytes const & Data, bool Fin )
{
    TBytes Frame = BuildFrameHeader( Type, Data.Length, Fin, false );
    if ( Data.Length ) {
        auto const Len = Frame.Length;
        Frame.Length = Len + Data.Length;
        copy(
            begin( const_cast<TBytes&>( Data ) ),
            end( const_cast<TBytes&>( Data ) ),
            &Frame[Len]
        );
    }
    GetIOHandler().Write( Frame );
}
//---------------------------------------------------------------------------

bool WebSocket::TryToUpgrade()
{
    auto& RequestInfo = GetRequestInfo();
    auto& ResponseInfo = GetResponseInfo();

    if ( !RequestInfo.IsVersionAtLeast( 1, 1 ) ||
         !SameText( RequestInfo.RawHeaders->Values[_D( "Upgrade" )], _D( "websocket" ) ) ||
         ToLower( RequestInfo.Connection ).Pos( _D( "upgrade" ) ) == 0 )
    {
        GetResponseInfo().ResponseNo = 426;
        GetResponseInfo().ResponseText = _D( "upgrade required" );
        return false;
    }

    if ( RequestInfo.RawHeaders->Values[_D("Sec-WebSocket-Version")] != _D( "13" ) ) {
        ResponseInfo.ResponseNo = 400;
        ResponseInfo.ResponseText = _D( "invalid Sec-WebSocket-Version, expected 13" );
        return false;
    }

    String SecWSKey = GetRequestInfo().RawHeaders->Values[_D("Sec-WebSocket-Key")];

    if ( SecWSKey.IsEmpty() ) {
        ResponseInfo.ResponseNo = 400;
        ResponseInfo.ResponseText = _D( "secret key is empty" );
        return false;
    }

    // validate Origin, Sec-WebSocket-Protocol, and Sec-WebSocket-Extensions as needed...

    ResponseInfo.ResponseNo         = 101;
    ResponseInfo.ResponseText       = _D( "Switching Protocols" );
    ResponseInfo.CloseConnection    = false;
    ResponseInfo.Connection         = _D( "Upgrade" );
    ResponseInfo.CustomHeaders->Values[_D( "Upgrade" )] = _D( "websocket" );

    auto Hash = make_unique<TIdHashSHA1>();

    auto const StrValue =
        TIdEncoderMIME::EncodeBytes(
            Hash->HashString( SecWSKey + SecWSKeyGUID )
        );

    ResponseInfo.CustomHeaders->Values[_D( "Sec-WebSocket-Accept" )] = StrValue;

    ResponseInfo.WriteHeader();

    return true;
}

//---------------------------------------------------------------------------
} // End of namespace Server
//---------------------------------------------------------------------------
} // End of namespace WebSockets
//---------------------------------------------------------------------------
} // End of namespace SvcApp
//---------------------------------------------------------------------------
