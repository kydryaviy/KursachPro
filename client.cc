#include <CORBA.h>
#include <coss/CosNaming.h>
#include "account.h"
#include <string>
#include <regex>

using namespace std;

bool yesOrNo()
{
	char c;
	cout << "Y/N?" << endl;
	cin >> c;
	switch(c)
	{
	case 'y':	
	case 'Y':
		return true;
	case 'n':
	case 'N':
		return false;
	default:
		cout << "Try again"	<< endl;
		yesOrNo();
	}
}

void work(Account_var account)
{
	system("clear");
}

bool login(Account_var account)
{
	system("clear");
	string sPin;
	cout << "Input your PIN" << endl;
	cin >> sPin;
	regex regPin("^[0-9]{4}$");
	if (!regex_match(sPin, regPin))
	{
		cout << "Invalid pin, try again?" << endl;
		if (yesOrNo())
			login(account);
	}
	else
	{
		if (account->login((unsigned short)strtoul(sPin.c_str(),NULL,0)))
			return true;
		else
		{
			cout << "Wrong pin, try again?" << endl;
			if (yesOrNo())
				login(account);
			else
				return false;
		}
	}
}

void begin(Bank_var bank, string startStr = "")
{
	system("clear");
	if (startStr.length() != 0)
	{
		cout << startStr << endl;
	}
	string sCardNum;
	cout << "Input your card number:" << endl;
	cin >> sCardNum;
	regex regCard("^[0-9]{16}$");
	if (!regex_match(sCardNum, regCard))
		begin(bank, "Invalid card number");
	else
	{	
		Account_var account;
		account = bank->getAccount(sCardNum.c_str());
		if (account == nullptr)
			begin(bank, "There is no such card in this bank");
		else
		{
			if (login(account))
				work(account);
			else
				begin(bank);
		}
	}
}

int main (int argc, char *argv[])
{
  // инициализация ORB
  CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);
  
  /* получение ссылки на Службу Именования */
  CORBA::Object_var nsobj = orb->resolve_initial_references ("NameService");
  CosNaming::NamingContext_var nc = CosNaming::NamingContext::_narrow (nsobj);

  if (CORBA::is_nil (nc)) 
  {
    cerr << "oops, I cannot access the Naming Service!" << endl;
    exit (1);
  }

  /* создание имени Службы Именования для объекта “банк” */
  CosNaming::Name name;
  name.length (1);
  name[0].id = CORBA::string_dup ("Bank");
  name[0].kind = CORBA::string_dup ("");
  
  /* попытка получения узла в дереве Службы Именования */
  CORBA::Object_var obj;

  cout << "Looking up Bank ... " << flush;

#ifdef HAVE_EXCEPTIONS
  try {
    obj = nc->resolve (name);
  }
  catch (CosNaming::NamingContext::NotFound &exc) {
    cout << "NotFound exception." << endl;
    exit (1);
  }
  catch (CosNaming::NamingContext::CannotProceed &exc) {
    cout << "CannotProceed exception." << endl;
    exit (1);
  }
  catch (CosNaming::NamingContext::InvalidName &exc) {
    cout << "InvalidName exception." << endl;
    exit (1);
  }
#else
  obj = nc->resolve (name);
#endif

  cout << "done." << endl;

/* Служба именования вовзращает ссылку как CORBA::Object. Для приведения типов используется метод _narrow() */
  Bank_var bank = Bank::_narrow (obj);

  /* получение объекта Account */	
	begin(bank);
	
	

  return 0;
}
