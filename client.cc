#include <CORBA.h>
#include <coss/CosNaming.h>
#include "account.h"
#include <string>
#include <regex>

using namespace std;

void begin();
char* bankName;
vector<string> bankNames;
vector<Bank_var> banks;

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

long inputMoney()
{
	string amount;
	regex regMoney("^[0-9]+$");
	cout << "Input amount of money:" << endl;
	cin >> amount;
	if (!regex_match(amount, regMoney))
	{
		return 0l;
	}
	else
	{
		return stol(amount);
	}
}

void work(Account_var account)
{
	system("clear");
	cout << "1.Check balance\n2.Withdraw money\n3.Deposit money\n0.Exit\n";
	short option;
	cin >> option;
	long amount = 0l;
	switch(option)
	{
	case 1:
		system("clear");
		cout << account->getBalance() << endl;
		cin.ignore().get();
		work(account);
		break;
	case 2:
		amount = inputMoney();
		account->withdraw(amount);
		work(account);
		break;
	case 3:
		amount = inputMoney();
		account->deposit(amount);
		work(account);
		break;
	case 0:
		begin();
		break;
	default:
		work(account);
		break;
	}
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

void begin()
{
	system("clear");
	string sCardNum;
	cout << "Input your card number:" << endl;
	cin >> sCardNum;
	regex regCard("^[0-9]{16}$");
	if (!regex_match(sCardNum, regCard))
	{
		cout << "Invalid card number" << endl;
		cin.ignore().get();
		begin();
	}
	else
	{
		Account_var account;
		account = banks[0]->getAccount(sCardNum.c_str());
		if (account == nullptr)
		{
			cout << "There is no such card in this bank" << endl;
			cin.ignore().get();
			begin();
		}
		else
		{
			if (login(account))
				work(account);
			else
				begin();
		}
	}
}
void addBanksToVector(string bName, CosNaming::NamingContext_var nc)
{
	CosNaming::Name name;
  name.length (1);
  name[0].id = CORBA::string_dup (bName.c_str());
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
  banks.push_back(Bank::_narrow (obj));
}
int main (int argc, char *argv[])
{
	bankNames.push_back("Privat");
	bankNames.push_back("Sberbank");
	bankNames.push_back("ProstoBank");
	cout << "Input bank name" << endl;
	bankName = new char[30];
	cin >> bankName;
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

  for (int i = 0; i < bankNames.size(); ++i)
	{
		addBanksToVector(bankNames[i].c_str(), nc);
	}

  /* получение объекта Account */
	begin();



  return 0;
}
