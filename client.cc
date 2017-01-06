#include <CORBA.h>
#include <coss/CosNaming.h>
#include "account.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <sstream>

#define X 0
#define Y 0
#define WIDTH 200
#define HEIGHT 200
#define WIDTH_MIN 200
#define HEIGHT_MIN 200
#define BORDER_WIDTH 5
#define TITLE "ATM"
#define ICON_TITLE "ATM"
#define PRG_CLASS "ATM"

using namespace std;

static void SetWindowManagerHints ( 
 Display * display, 		/*Указатель на структуру Display */
 char * PClass, 		/*Класс программы */
 char * argv[],   		/*Аргументы программы */
 int argc,    			/*Число аргументов */
 Window window,    		/*Идентификатор окна */
 int x,     			/*Координаты левого верхнего */
 int y,	   				/*угла окна */
 int win_wdt,			/*Ширина  окна */
 int win_hgt,  			/*Высота окна */
 int win_wdt_min,			/*Минимальная ширина окна */
 int win_hgt_min, 		/*Минимальная высота окна */
 char * ptrTitle,  		/*Заголовок окна */
 char * ptrITitle,	/*Заголовок пиктограммы окна */
 Pixmap pixmap 	/*Рисунок пиктограммы */
)
{
 XSizeHints size_hints; /*Рекомендации о размерах окна*/

 XWMHints wm_hints;
 XClassHint class_hint;
 XTextProperty windowname, iconname;

 if ( !XStringListToTextProperty (&ptrTitle, 1, &windowname ) ||
    !XStringListToTextProperty (&ptrITitle, 1, &iconname ) ) {
  puts ( "No memory!\n");
  exit ( 1 );
}

size_hints.flags = PPosition | PMinSize | PMaxSize;
size_hints.min_width = win_wdt_min;
size_hints.min_height = win_hgt_min;

size_hints.max_width = win_wdt_min;
size_hints.max_height = win_hgt_min;
XSetWMNormalHints(display, window, &size_hints);

wm_hints.flags = StateHint | IconPixmapHint | InputHint;
wm_hints.initial_state = NormalState;
wm_hints.input = True;
wm_hints.icon_pixmap= pixmap;
class_hint.res_name = argv[0];
class_hint.res_class = PClass;

XSetWMProperties ( display, window, &windowname,
  &iconname, argv, argc, &size_hints, &wm_hints,
  &class_hint );
  
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
  Account_var account = bank->create ();

  if (CORBA::is_nil (account)) 
  {
    printf ("oops: account is nil\n");
    exit (1);
  }

  /* выполнение операций с объектом Account и вывод результатов */
  account->deposit (700);
  account->withdraw (450);
  		 
  	 string balance = "Your balance is: ";	 
		 stringstream ss;
		 ss << account->balance();
		 balance += ss.str();
		 

	 Display * display;  /* Указатель на структуру Display */
 int ScreenNumber;    /* Номер экрана */
 GC gc;				/* Графический контекст */
 XEvent report;
 Window window;

 /* Устанавливаем связь с сервером */
 if ( ( display = XOpenDisplay ( NULL ) ) == NULL ) {
  puts ("Can not connect to the X server!\n");
  exit ( 1 );
 }

 /* Получаем номер основного экрана */
 ScreenNumber = DefaultScreen ( display );

 /* Создаем окно */
 window = XCreateSimpleWindow ( display,
     RootWindow ( display, ScreenNumber ),
     X, Y, WIDTH, HEIGHT, BORDER_WIDTH,
     BlackPixel ( display, ScreenNumber ),
     WhitePixel ( display, ScreenNumber ) );

 /* Задаем рекомендации для менеджера окон */
 SetWindowManagerHints ( display, PRG_CLASS, argv, argc,
   window, X, Y, WIDTH, HEIGHT, WIDTH_MIN,
   HEIGHT_MIN, TITLE, ICON_TITLE, 0 );

 /* Выбираем события,  которые будет обрабатывать программа */
 XSelectInput ( display, window, ExposureMask | KeyPressMask );

 /* Покажем окно */
 XMapWindow ( display, window );

 /* Создадим цикл получения и обработки ошибок */
 while ( 1 ) {
  XNextEvent ( display, &report );

  switch ( report.type ) {
    case Expose :
     /* Запрос на перерисовку */
     if ( report.xexpose.count != 0 )
      break;

     gc = XCreateGC ( display, window, 0 , NULL );
		 
     XSetForeground ( display, gc, BlackPixel ( display, 0) );
     XDrawString ( display, window, gc, 20,50,
       balance.c_str(), strlen ( balance.c_str() ) );
     XFreeGC ( display, gc );
     XFlush(display);
     break;

    case KeyPress :
     /* Выход нажатием клавиши клавиатуры */
     XCloseDisplay ( display );
     exit ( 0 );
  //printf ("Balance is %ld.\n", account->balance ());

  return 0;
}
}
}
