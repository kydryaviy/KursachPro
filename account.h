/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <CORBA.h>
#include <mico/throw.h>

#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__




class Account;
typedef Account *Account_ptr;
typedef Account_ptr AccountRef;
typedef ObjVar< Account > Account_var;
typedef ObjOut< Account > Account_out;

class Bank;
typedef Bank *Bank_ptr;
typedef Bank_ptr BankRef;
typedef ObjVar< Bank > Bank_var;
typedef ObjOut< Bank > Bank_out;



enum Currency {
  UAH = 0,
  RUB,
  USD
};

typedef Currency& Currency_out;


/*
 * Base class and common definitions for interface Account
 */

class Account : 
  virtual public CORBA::Object
{
  public:
    virtual ~Account();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef Account_ptr _ptr_type;
    typedef Account_var _var_type;
    #endif

    static Account_ptr _narrow( CORBA::Object_ptr obj );
    static Account_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Account_ptr _duplicate( Account_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static Account_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual void deposit( CORBA::Float amount ) = 0;
    virtual void withdraw( CORBA::Float amount ) = 0;
    virtual CORBA::Float balance() = 0;
    virtual void login( const char* card, CORBA::UShort pin ) = 0;
    virtual void send( const char* card ) = 0;
    virtual void convert( ::Currency currency ) = 0;

  protected:
    Account() {};
  private:
    Account( const Account& );
    void operator=( const Account& );
};

// Stub for interface Account
class Account_stub:
  virtual public Account
{
  public:
    virtual ~Account_stub();
    void deposit( CORBA::Float amount );
    void withdraw( CORBA::Float amount );
    CORBA::Float balance();
    void login( const char* card, CORBA::UShort pin );
    void send( const char* card );
    void convert( ::Currency currency );

  private:
    void operator=( const Account_stub& );
};

#ifndef MICO_CONF_NO_POA

class Account_stub_clp :
  virtual public Account_stub,
  virtual public PortableServer::StubBase
{
  public:
    Account_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~Account_stub_clp ();
    void deposit( CORBA::Float amount );
    void withdraw( CORBA::Float amount );
    CORBA::Float balance();
    void login( const char* card, CORBA::UShort pin );
    void send( const char* card );
    void convert( ::Currency currency );

  protected:
    Account_stub_clp ();
  private:
    void operator=( const Account_stub_clp & );
};

#endif // MICO_CONF_NO_POA


/*
 * Base class and common definitions for interface Bank
 */

class Bank : 
  virtual public CORBA::Object
{
  public:
    virtual ~Bank();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef Bank_ptr _ptr_type;
    typedef Bank_var _var_type;
    #endif

    static Bank_ptr _narrow( CORBA::Object_ptr obj );
    static Bank_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static Bank_ptr _duplicate( Bank_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static Bank_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual ::Account_ptr create( const char* card, CORBA::UShort pin ) = 0;

  protected:
    Bank() {};
  private:
    Bank( const Bank& );
    void operator=( const Bank& );
};

// Stub for interface Bank
class Bank_stub:
  virtual public Bank
{
  public:
    virtual ~Bank_stub();
    ::Account_ptr create( const char* card, CORBA::UShort pin );

  private:
    void operator=( const Bank_stub& );
};

#ifndef MICO_CONF_NO_POA

class Bank_stub_clp :
  virtual public Bank_stub,
  virtual public PortableServer::StubBase
{
  public:
    Bank_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~Bank_stub_clp ();
    ::Account_ptr create( const char* card, CORBA::UShort pin );

  protected:
    Bank_stub_clp ();
  private:
    void operator=( const Bank_stub_clp & );
};

#endif // MICO_CONF_NO_POA

#ifndef MICO_CONF_NO_POA

class POA_Account : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~POA_Account ();
    Account_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static POA_Account * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual void deposit( CORBA::Float amount ) = 0;
    virtual void withdraw( CORBA::Float amount ) = 0;
    virtual CORBA::Float balance() = 0;
    virtual void login( const char* card, CORBA::UShort pin ) = 0;
    virtual void send( const char* card ) = 0;
    virtual void convert( ::Currency currency ) = 0;

  protected:
    POA_Account () {};

  private:
    POA_Account (const POA_Account &);
    void operator= (const POA_Account &);
};

class POA_Bank : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~POA_Bank ();
    Bank_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static POA_Bank * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual ::Account_ptr create( const char* card, CORBA::UShort pin ) = 0;

  protected:
    POA_Bank () {};

  private:
    POA_Bank (const POA_Bank &);
    void operator= (const POA_Bank &);
};

#endif // MICO_CONF_NO_POA

extern CORBA::StaticTypeInfo *_marshaller_Currency;

extern CORBA::StaticTypeInfo *_marshaller_Account;

extern CORBA::StaticTypeInfo *_marshaller_Bank;

#endif
