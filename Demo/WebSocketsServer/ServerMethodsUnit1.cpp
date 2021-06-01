//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ServerMethodsUnit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TServerMethods1::TServerMethods1(TComponent* Owner)
    : TDataModule(Owner)
{
}
//----------------------------------------------------------------------------
System::UnicodeString TServerMethods1::EchoString(System::UnicodeString value)
{
    return value;
}
//----------------------------------------------------------------------------
System::UnicodeString TServerMethods1::ReverseString(System::UnicodeString value)
{
    return ::ReverseString(value);
}
//----------------------------------------------------------------------------

