//---------------------------------------------------------------------------


#pragma hdrstop

#include "FormMain.h"
#include "DataModHTTP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "System.Classes.TPersistent"
#pragma resource "*.dfm"

//TdmHTTP *dmHTTP;

//---------------------------------------------------------------------------

__fastcall TdmHTTP::TdmHTTP(TComponent* Owner, TfrmMain& Log )
    : TDataModule(Owner)
    , log_( Log )
{
    IdSSLIOHandlerSocketOpenSSL1->PassThrough = false;
    //IdLogEvent1->Active = true;
}
//---------------------------------------------------------------------------

void __fastcall TdmHTTP::IdHTTP1Connected(TObject *Sender)
{
    auto& HTTP = static_cast<TIdHTTP&>( *Sender );
    log_.LogMessage(
        _D( "\nConnected to %s:%d\n" ),
        ARRAYOFCONST( ( HTTP.IOHandler->Host, HTTP.IOHandler->Port ) )
    );
}
//---------------------------------------------------------------------------

void __fastcall TdmHTTP::IdHTTP1Disconnected(TObject *Sender)
{
    auto& HTTP = static_cast<TIdHTTP&>( *Sender );
    log_.LogMessage(
        _D( "\nDisconnected to %s:%d\n" ),
        ARRAYOFCONST( ( HTTP.IOHandler->Host, HTTP.IOHandler->Port ) )
    );

}
//---------------------------------------------------------------------------

void __fastcall TdmHTTP::IdLogEvent1Received(TComponent *ASender, const UnicodeString AText,
          const UnicodeString AData)
{
    log_.LogMessage( _D( "\n%s: %s\n" ), ARRAYOFCONST( ( AText, AData ) ), clBlue );
}
//---------------------------------------------------------------------------

void __fastcall TdmHTTP::IdLogEvent1Sent(TComponent *ASender, const UnicodeString AText,
          const UnicodeString AData)
{
    log_.LogMessage( _D( "\n%s: %s\n" ), ARRAYOFCONST( ( AText, AData ) ), clPurple );
}
//---------------------------------------------------------------------------


