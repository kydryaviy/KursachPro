#include <CORBA.h>
#include <coss/CosNaming.h>
#include <regex>
#include "account.h"

using namespace std;

/* реализация интерфейса Account (описание его функциональности) */
class Account_impl : 
virtual public POA_Account /* класс, сгенерированый из idl-файла */
{
public:
  Account_impl (char*, CORBA::UShort, CORBA::Float);
	~Account_impl ();

  void deposit (CORBA::Float);
  void withdraw (CORBA::Float);
  CORBA::Float balance () const;
	CORBA::Boolean login (char*, CORBA::UShort) const;
	void send (char*);

private:
  CORBA::Float balance;
	char* cardNum
	CORBA::UShort pin;
};

/* конструктор */
Account_impl::Account_impl (char* _cardNum, CORBA::UShort _pin, CORBA::Float _balance)
{
	balance = _balance;
	cardNum = new char[12];
	cardNum = _cardNum;
	pin = _pin;
}

Account_impl::~Account_impl ()
{
	delete cardNum;
}

/* функция внесения депозита */ 
void Account_impl::deposit (CORBA::Float amount)
{
  balance += amount;
}

/* функция снятия наличных */ 
void Account_impl::withdraw (CORBA::Float amount)
{
  balance -= amount;
}
CORBA::Float Account_impl::balance ()
{
  return bal;
}
CORBA::Boolean login (char* cardNum, CORBA::UShort pin)
{
	regex reg("^[0-9]{12}$");
	if (!std::regex_match(cardNum, reg))
		return false;
}

/* реализация интерфейса Bank */
class Bank_impl : virtual public POA_Bank
{
public:
  Account_ptr create ();
};

Account_ptr Bank_impl::create ()
{

  /* создание нового объекта Account, удалить который невозможно*/
  Account_impl * ai = new Account_impl;

/*получить ссылку на новый объект используя метод _this() -  активизация службы подсчёта ссылок для этого объекта */
  Account_ptr aref = ai->_this ();
  assert (!CORBA::is_nil (aref));

  /* возврат ссылки на объект */
  return aref;
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
