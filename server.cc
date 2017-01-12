#include <CORBA.h>
#include <coss/CosNaming.h>
#include "account.h"
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;
class Account_impl;
void rewriteFile(Account_impl*);
/* реализация интерфейса Account (описание его функциональности) */
class Account_impl :
virtual public POA_Account /* класс, сгенерированый из idl-файла */
{
public:
  Account_impl (const char*, CORBA::UShort, CORBA::Long);
	~Account_impl ();

  void deposit (CORBA::Long);
  void withdraw (CORBA::Long);
  CORBA::Long getBalance ();
	CORBA::Boolean login (CORBA::UShort);
	char* getCardNum ();

private:
  CORBA::Long balance;
	char* cardNum;
	CORBA::UShort pin;
};

/* конструктор */
Account_impl::Account_impl (const char* _cardNum, CORBA::UShort _pin, CORBA::Long _balance)
{
	balance = _balance;
	cardNum = new char[20];
	strcpy(cardNum,_cardNum);
	//cardNum = (char*)_cardNum;
	pin = _pin;
}

Account_impl::~Account_impl ()
{
	delete cardNum;
}

/* функция внесения депозита */
void Account_impl::deposit (CORBA::Long amount)
{
  balance += amount;
  rewriteFile(this);
}

/* функция снятия наличных */
void Account_impl::withdraw (CORBA::Long amount)
{
  balance -= amount;
  rewriteFile(this);
}

CORBA::Long Account_impl::getBalance ()
{
  return balance;
}

CORBA::Boolean Account_impl::login (CORBA::UShort _pin)
{
  if (pin == _pin)
		return true;
	return false;
}

char* Account_impl::getCardNum ()
{
	return cardNum;
}

/* реализация интерфейса Bank */
class Bank_impl : virtual public POA_Bank
{
public:
	Bank_impl (const char*);
	~Bank_impl ();

  Account_ptr getAccount (const char*);
	void createAccount (const char*, CORBA::UShort, CORBA::Long);
	char* getName ();
private:
	char* bankName;
  vector<Account_impl*> accounts;
};

Bank_impl::Bank_impl (const char* _bankName)
{
	bankName = new char[20];
	strcpy(bankName,_bankName);
}

Bank_impl::~Bank_impl ()
{
	delete bankName;
}

Account_ptr Bank_impl::getAccount (const char* cardNum)
{
	Account_ptr account = nullptr;
	for_each(accounts.begin(),accounts.end(),[cardNum, &account](Account_impl* acc)
	{
		if (!strcmp(acc->getCardNum(), cardNum))
		{

			account = acc->_this ();
			//account = acc;
		}
	});

	return account;
}

void Bank_impl::createAccount(const char* cardNum, CORBA::UShort pin, CORBA::Long balance)
{

  /* создание нового объекта Account, удалить который невозможно*/
  Account_impl * ai = new Account_impl(cardNum, pin, balance);

/*получить ссылку на новый объект используя метод _this() -  активизация службы подсчёта ссылок для этого объекта */
  //Account_ptr aref = ai->_this ();
  //assert (!CORBA::is_nil (aref));
  /* возврат ссылки на объект */
  accounts.push_back(ai);
}

char* Bank_impl::getName ()
{
	return bankName;
}

vector<string> splitString(string str)
{
	vector<string> v;
	istringstream f(str);
	string s;
	while (getline(f,s,' '))
	{
		v.push_back(s);
	}
  return v;
}

void addAccIfThisBank(Bank_impl* bank, string data)
{
	vector<string> v = splitString(data);
	if (!strcmp(bank->getName(),v[0].c_str()))
		bank->createAccount(v[1].c_str(),(unsigned short) strtoul(v[2].c_str(), NULL, 0),stol(v[3]));
}

vector<string> readFile()
{
	vector<string> stringsFromFile;
	string str;
	ifstream file("accounts.txt");
	while (getline(file, str))
	{
		stringsFromFile.push_back(str);
	}
	return stringsFromFile;
}

void rewriteFile(Account_impl* acc)
{
	vector<string> stringsFromFile = readFile();
	for_each(stringsFromFile.begin(),stringsFromFile.end(),[acc](string &s)
	{
		vector<string> v = splitString(s);
		if (!strcmp(acc->getCardNum(),v[1].c_str()))
		{
			v[3] = to_string(acc->getBalance());
			s = "";
			for (int i = 0; i < 4; ++i)
      {
				s += v[i];
        if (i != 3)
          s += " ";
      }
		}
	});

  ofstream file("accounts.txt");
  for (int i = 0; i < stringsFromFile.size(); ++i)
  {
    file << stringsFromFile[i];
    if (i != stringsFromFile.size() - 1)
      file << endl;
  }
}

int main (int argc, char *argv[])
{
  /* инициализация ORB, получение Root POA объекта и регистрация */
  CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);

  /* получение ссылки на RootPOA и его менеджера */
  CORBA::Object_var poaobj = orb->resolve_initial_references ("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
  PortableServer::POAManager_var mgr = poa->the_POAManager();

	cout << "Input bank name" << endl;
	char* bankName = new char[30];
	cin >> bankName;
  /* создание объекта Bank */

   Bank_impl *micocash = new Bank_impl(bankName);



	vector<string> stringsFromFile = readFile();
	for_each(stringsFromFile.begin(),stringsFromFile.end(),[&micocash](string s)
	{
		addAccIfThisBank(micocash, s);
	});




  /* активация данного объекта */
  PortableServer::ObjectId_var oid = poa->activate_object (micocash);
  CORBA::Object_var ref = poa->id_to_reference (oid.in());


  /* получение ссылки на Службу Именования */
  CORBA::Object_var nsobj = orb->resolve_initial_references ("NameService");
  CosNaming::NamingContext_var nc = CosNaming::NamingContext::_narrow (nsobj);

  if (CORBA::is_nil (nc))
  {
    cerr << "oops, I cannot access the Naming Service!" << endl;
    exit (1);
  }

  /* регистрация имени для банка в Службе Именования */
  CosNaming::Name name;
  name.length (1);
  name[0].id = CORBA::string_dup ("Bank");
  name[0].kind = CORBA::string_dup ("");

  /* сохранение ссылки на объект банка в Службе Именования */
  cout << "Binding Bank in the Naming Service ... " << flush;
  nc->rebind (name, ref);
  cout << "done." << endl;

  /* активация POA и начало обслуживания клиентских запросов */
  printf ("Running.\n");

  mgr->activate ();
  orb->run();

  /* выключение */
  poa->destroy (TRUE, TRUE);
  delete micocash;
	delete bankName;
	cout << "EXITING" << endl;
  return 0;
}
