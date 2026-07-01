//----------------------------------------------------------------------------

#ifndef ServerMethodsUnit1H
#define ServerMethodsUnit1H
//----------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <DataSnap.DSServer.hpp>
//----------------------------------------------------------------------------
class DECLSPEC_DRTTI TServerMethods1 : public TComponent
{
private:	// User declarations
public:		// User declarations
    System::UnicodeString EchoString(System::UnicodeString value);
    System::UnicodeString  ReverseString(System::UnicodeString value);
};
#endif
