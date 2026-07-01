//----------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.StrUtils.hpp>
#pragma hdrstop

#include "ServerMethodsUnit1.h"
//----------------------------------------------------------------------------
#pragma package(smart_init)
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

