#include <CORBA.h>
#include <coss/CosNaming.h>
#include "account.h"
#include <vector>
#include <algorithm>

using namespace std;

/* реализация интерфейса Account (описание его функциональности) */
class Account_impl : 
virtual public POA_Account /* класс, сгенерированый из idl-файла */
{
public:
  Account_impl (char*, CORBA::UShort, CORBA::Float, char*);
	~Account_impl ();

  void deposit (CORBA::Float, char*);
  void withdraw (CORBA::Float, char*);
  CORBA::Float balance () const;
	CORBA::Boolean login (char*, CORBA::UShort) const;
	char* getCardNum ();

private:
  CORBA::Float balance;
	char* cardNum
	CORBA::UShort pin;
	char* bankName
};

/* конструктор */
Account_impl::Account_impl (char* _cardNum, CORBA::UShort _pin, CORBA::Float _balance, char* _bankName)
{
	balance = _balance;
	cardNum = new char[12];
	cardNum = _cardNum;
	pin = _pin;
	bankName = _bankName;
}

Account_impl::~Account_impl ()
{
	delete cardNum;
}

/* функция внесения депозита */ 
void Account_impl::deposit (CORBA::Float amount, char* _bankName)
{
	float multiplier = (bankName == _bankName)?1:0.95f;
  balance += amount * multiplier;
}

/* функция снятия наличных */ 
void Account_impl::withdraw (CORBA::Float amount, char* _bankName)
{
	float multiplier = (bankName == _bankName)?1:0.95f;
  balance -= amount * multiplier;
}

CORBA::Float Account_impl::balance ()
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
	Bank_impl (char*);
	~Bank_impl ();

  Account_ptr getAccount (char*);
	void createAccount (char*, CORBA::UShort, CORBA::Float);
private:
	char* bankName;
  vector<Account_ptr> accounts;
};

Bank_impl::Bank_impl (char* _bankName)
{
	int length = strlen(_bankName);
	bankName = new char[length];
	bankName = _bankName;
}

Bank_impl::~Bank_impl ()
{
	delete bankName;
}

Account_ptr Bank_impl::getAccount (char* cardNum)
{
	for_each(accounts.begin(),accounts.end(),[](Account_ptr acc)
	{
		if (!strcmp(acc->getCardNum(), cardNum))
			return acc; 
	});
	return nullptr;
}

void createAccount(char cardNum, CORBA::UShort pin, CORBA::Float balance)
{

  /* создание нового объекта Account, удалить который невозможно*/
  Account_impl * ai = new Account_impl(cardNum, pin, balance, bankName);

/*получить ссылку на новый объект используя метод _this() -  активизация службы подсчёта ссылок для этого объекта */
  Account_ptr aref = ai->_this ();
  assert (!CORBA::is_nil (aref));

  /* возврат ссылки на объект */
  accounts.push_back(aref);
}

int main (int argc, char *argv[])
{
  /* инициализация ORB, получение Root POA объекта и регистрация */  
  CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);
  
  /* получение ссылки на RootPOA и его менеджера */
  CORBA::Object_var poaobj = orb->resolve_initial_references ("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
  PortableServer::POAManager_var mgr = poa->the_POAManager();

  /* создание объекта Bank */
  Bank_impl * micocash = new Bank_impl;

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

  return 0;
}
