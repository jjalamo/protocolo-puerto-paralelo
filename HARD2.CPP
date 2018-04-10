#include <graphics.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <io.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


//-------------------- CONSTANTES
#define MAXNBYTES 100   //Numero max de bytes seguidos que se pueden enviar

#define REG_DATOS 0     //Desplazamiento del registro de datos
#define REG_ESTADO 1    //Desplazamiento del registro de estado
#define REG_CONTROL 2   //Desplazamiento del registro de control

#define SIG_BYTE 1      //Codigo de control siguiente byte a enviar
#define ULTIMO_BYTE 0   //Codigo de control ultimo byte a enviar
#define DISPONIBLE 3    //Codigo de control el byte esta disponible

#define PREPARADO 1     //Codigo de control preparado para recibir
#define RECIBIDO 3      //Codigo de control byte recibido

#define QUIERO_ENVIAR 2 //Codigo de control quiero enviar
#define PUEDES_ENVIAR 0 //Codigo de control puedes enviar

#define NOSC 0          //Codigo de estado no seccion critica
#define SC_ENVIAR 1     //Codigo de estado ejecutando seccion critica enviar
#define SC_RECIBIR 2    //codigo de estado ejecutando seccion critica recibir
#define SC_QENVIAR 3    //Codigo de estado ejecutando seccion critica quiero enviar
//----------------------------------------------------------------------


//-------------------- ESTRUCTURAS DE DATOS
struct tamfich_rep {//Almacena el tama¤o del fichero en 4 bytes
  unsigned int t[4];
}; typedef struct tamfich_rep tamfich;
//----------------------------------------------------------------------


//-------------------- CABECERAS DE FUNCIONES
void InG();
void CG();
void Ventana (int xini,int yini, int xfin, int yfin,int color);
void Cuadro(int xini,int yini, int xfin, int yfin,int color);
void imprimir(int i,int color);
void mostrar_tpc(int tp,int pos);
void enviar(unsigned int dbase,int reg,unsigned char dato);
unsigned char recibir (unsigned int dbase,int reg);
unsigned int puerto();
void modo_enviar(unsigned int dbase);
void modo_recibir(unsigned int dbase);
void q_enviar(char *nfich);
void proto_enviar();
void proto_recibir();
unsigned char decod_controlsignal(unsigned char x);
unsigned char cod_controlsignal(unsigned char x);
unsigned char decod_estadosignal(unsigned char x);
unsigned char cod_estadosignal(unsigned char x);
void p_enviar();
tamfich filesize(FILE *stream);
long tamafichero(tamfich t);
void env_inf_fich(tamfich t, char *nomb);
tamfich rec_inf_fich();
void connect();
//----------------------------------------------------------------------


//-------------------- VARIABLES GLOBALES
FILE *fentrada;             //Fichero a enviar
FILE *fsalida;              //Fichero a recibir
long tam_fich_env;          //Tama¤o del fichero a enviar
long tam_fich_rec;          //Tama¤o del fichero a recibir
long prog_env;              //progresion del fichero a enviar
long prog_rec;              //progresion del fichero a recibir
long bytes_env;             //bytes enviados del fichero a enviar
long bytes_rec;             //bytes recibidos del fichero a recibir
long bxl_env;               //bytes por linea de la barra de progreso del fichero a enviar
long bxl_rec;               //bytes por linea de la barra de progreso del fichero a recibir
long cont_env;              //contador de bytes enviados hasta llegar a bxl del fichero a enviar
long cont_rec;              //contador de bytes recibidos hasta llegar a bxl del fichero a recibir
long lineas_env;            //numero de lineas dibujadas de la barra de progreso del fichero a enviar
long lineas_rec;            //numero de lineas dibujadas de la barra de progreso del fichero a recibir
int enviando=0;             //indica si estamos o no enviando un fichero
int recibiendo=0;           //indica si estamos o no recibiendo un fichero
int quiero_enviar=0;        //indica si queremos o no enviar un fichero
int dbase;                  //direccion base del puerto paralelo
int sc_qenv=0;              //indica si estamos ejecutando la seccion critica quiero enviar
int tpc_env;                //% enviado del fichero a enviar
int tpc_rec;                //% recibido del fichero a recibir
unsigned char byte_enviar;  //siguinte byte a enviar
unsigned char byte_recibir; //ultimo byte recibido
char ruta_rec[80];          //ruta del fichero que se esta recibiendo
char nombrefich[80];        //nombre del fichero a enviar
char num[11];               //Alamcena en forma caracter los numeros del 0 al 9
//----------------------------------------------------------------------


//-------------------- PROGRAMA PRINCIPAL
void main() {
  int salir=0;        //Centinela de fin de programa
  int opc=0;          //ultimo caracter pulsado en el teclado
  int pos=1;          //indica en que opcion del menu nos encontramos
  int escribiendo=0;  //indica si estamos o no escribiendo la ruta de un fichero
  int ruta_escrita=0; //Indica si esta escrita o no la ruta del fichero a enviar
  int nuevo=0;        //Indica si comenzamos a escribir la ruta de un nuevo fichero
  int i=0;            //contador, variable auxiliar
  int esc=0;          //indica si se ha pulsado o no la tecla ESC
  int intro=0;        //Indica si se ha pulsado o no la tecla INTRO
  int estado;         //Almacena el contenido del registro de estado
  int espera_env=0;   //indica si debemos hacer o no una espera en el envio del fichero
  int espera_rec=0;   //indica si debemos hacer o no una espera en la recepcion del fichero
  int nb_env=0;       //contador de bytes enviados hasta MAXNBYTES
  int nb_rec=0;       //contador de bytes recibidos hasta MAXNBYTES
  int tpc;            //% a mostrar en pantalla
  char ruta_env[80];  //Almacena la ruta del ficher a enviar
  char car[1];        //caracter capturado del teclado ha mostrar en pantalla

  InG();//iniciamos del modo grafico
  dbase=puerto(); //obtenemos la direccion base del puerto paralelo

  //Dibujamos la interface del programa
  Ventana(0,0,639,327,1);     // FONDO DEL ORDENADOR A
  Cuadro(0,0,639,237,15);     // CUADRO DEL ORDENADOR A
  Cuadro(250,0,390,25,15);    // CUADRO TITULO DEL ORDENADOR A

  Ventana(0,240,639,479,1);   // FONDO DEL ORDENADOR B
  Cuadro(0,240,639,479,15);   // CUADRO DEL ORDENADOR B
  Cuadro(250,240,390,265,15); // CUADRO TITULO DEL ORDENADOR B

  setcolor(12);
  outtextxy(298,10,"ENVIAR");  // TITULO DEL ORDENADOR A
  outtextxy(275,250,"  RECIBIR"); // TITULO DEL ORDENADOR B

  Cuadro(20,197,619,217,15); // BARRA DE PROGRESO DEL ORDENADOR A
  Cuadro(20,439,619,459,15); // BARRA DE PROGRESO DEL ORDENADOR B

  //Menu del progroma
  outtextxy(15,55,"FICHERO ");
  setcolor(7);
  outtextxy(15,75,"ENVIAR");
  outtextxy(15,95,"SALIR");
  outtextxy(15,300,"FICHERO");

  outtextxy(500,10,"CONECTANDO... ");
  strcpy(num,"0123456789");
  connect(); //iniciamos el registro de control
  setcolor(1);
  outtextxy(500,10,"ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ");

  while ( !salir ) {

    if(kbhit() ) { //comprobamos si se ha pulsado alguna tecla
      opc=getch(); //capturamos la tecla pulsada
      if(!escribiendo) { //comprobamos si estamos escribiendo alguna ruta o no
	setcolor(1);
	outtextxy(30,150,"ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ");
	//comprobamos si la tecla pulsada corresponde con moverse hacia arriba
	//en el menu
	if ( ( opc == 72 ) && ( pos > 1 ) ) {  // ARRIBA
	  pos--;
	  imprimir(pos+1,7);
	  imprimir(pos,15);
	}
	//comprobamos si la tecla pulsada corresponde con moverse hacia abajo
	//en el menu
	if ( ( opc == 80 ) && ( pos < 3 ) ) {  // ABAJO
	  pos++;
	  imprimir(pos-1,7);
	  imprimir(pos,15);
	}
	//comprobamos si la tecla pulsada ha sido INTRO
	if ( ( opc == 13) && ( ! escribiendo ) ){
	  //Si la tecla pulsada ha sido INTRO, comprobamos si la opcion
	  //activa del menu es la 1, en cuyo caso activamos los indicadores
	  //de escribiendo ruta de fichero y el nueva ruta
	  if(pos==1) {  // ESCRIBE UNA FRASE
	    if (!enviando) {
	      escribiendo=1;
	      nuevo=1;
	      Ventana(22,199,617,215,1);
	      imprimir(7,1);
	      setcolor(1);
	      outtextxy(550,179+pos,"ÛÛÛÛÛÛÛ");
	    }
	  }
	  //Si la tecla pulsada ha sido INTRO, comprobamos si la opcion
	  //activa del menu es la 2 enviar fichero, en cuyo caso
	  //comprobamos si la ruta del ficherio esta escrita y no estamos
	  //enviando
	  if(pos==2) {  // ENVIAR EL FICHERO
	    if (ruta_escrita && !enviando) {
	      //si la ruta esta escrita y no estamos enviando, habrimos el
	      //fichero para comprobar que existe.
	      if ((fentrada = fopen(ruta_env, "r"))== NULL) {
		//si el fichero no existe se muestra el mensaje en pantalla
		//y se desactiva el indicador de peticion de envio
		quiero_enviar=0;
		imprimir(10,12);
	      } else {
		//si existe el fichero, activamos el indicador de peticion de
		//envio y cerramos el fichero
		quiero_enviar=1;
		fclose(fentrada);
	      }
	    }
	  }
	  //Si la tecla pulsada ha sido INTRO, comprobamos si la opcion
	  //activa del menu es la 3 salir del programa, en cuyo caso
	  //comprobamos si estamos enviando y/o recibiendo un fichero
	  //si no estamos enviando ni recibiendo, activamos el indicador
	  //de terminar programa
	  if(pos==3) {  // SALIR DEL PROGRAMA
	    if(!enviando && !recibiendo) {
	      salir=1;
	    }
	  }
	}
      }
    }

    //comprobamos si estamos o no escribiendo la ruta del fichero a enviar
    if ( escribiendo ) {         // ESCRIBIR LA RUTA DEL FICHERO
      //comprobamos si comenzamos a escribir la ruta
      if ( nuevo ) {
	//iniciamos variables y borramos la zona de pantalla correspondiente
	i=0;
	esc=0;
	intro=0;
	setcolor(1);
	outtextxy(80,55,"ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ");
	setcolor(15);
	outtextxy(82,57,"þ");
	nuevo=!nuevo;
	opc='\x1';
      }

      //coprobamos la tecla pulsada
      switch (opc) {
	//si la tecla pulsada es INTRO
	case '\r':
	  intro=1;//activamos el indicador de INTRO
	  escribiendo=0;//desactivamos el indicador de escribiendo ruta de fichero
	  break;

	//si la tecla pulsada es ESC
	case '\x1B':
	  esc=1;//activamos el indicador de ESC
	  //borramos la zona de pantalla correspondiente al nombre del fichero
	  setcolor(1);
	  outtextxy(80,55,"ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ");
	  setcolor(15);
	  escribiendo=0;//desactivamos el indicador de escribiendo ruta del fichero
	  break;

	//si la tecla pulsada es DELETE
	case '\b':
	  //borramos el ultimo caracter pulsado y lo eliminamos de la ruta
	  //del fichero a enviar
	  if ( i>=1 ) {
	    setcolor(1);
	    outtextxy(i*8+72,55,"ÛÛ");
	    setcolor(15);
	    outtextxy(i*8+74,57,"þ");
	    i--;
	    ruta_env[i]=' ';
	    opc='\x1';
	  }
	  break;

	//si la tecla es otra
	default:
	  //comprobamos si la tecla pulsada es un caracter ASCII y no
	  //un caracter de control
	  if ( ! iscntrl(opc) ) {
	    //mostramos en pantalla el caracter pulsado y lo a¤adimos al
	    //final de la ruta del ficher
	    strcpy(car,"");
	    if ( i<=48 ) i++;
	    ruta_env[i-1]=opc;
	    setcolor(1);
	    outtextxy(i*8+72,55,"Û");
	    setcolor(15);
	    car[0]=opc;
	    outtextxy(i*8+72,55,car);
	    setcolor(15);
	    outtextxy((i+1)*8+74,57,"þ");
	    opc='\x1';
	  }
      }
      //si la tecla pulsada es ESC, borramos la ruta del fichero,
      //y desactivamos el indicador de escribiendo y ruta escrita
      if ( esc ) {
	ruta_env[0]='\x0';
	escribiendo=0;
	esc=0;
	opc='\1';
	ruta_escrita=0;
      }
      //Si la tecla pulsada es INTRO, cerramos la ruta del fichero,
      //desactivamnos el indicador de escribiendo ruta del fichero
      //y activamos el indicador de ruta escrita
      if ( intro ) {
	ruta_env[i]='\x0';
	setcolor(1);
	outtextxy((i+1)*8+72,55,"Û");
	escribiendo=0;
	ruta_escrita=1;
	imprimir(7,1);
      }
    }

    //--------- PUEDES ENVIAR

    //Comprobamos si no estamos recibiendo
    if(!recibiendo) {
      //leemos del registro de estado la se¤al de control enviada por el otro PC
      estado=recibir(dbase,REG_ESTADO);
      estado=decod_controlsignal(estado);
      //comprobamos si la se¤al de control recibida es quiero enviar
      //en cuyo caso pasamos a darle permiso al otro PC para que envie
      //el fichero
      if(estado==QUIERO_ENVIAR) {
	p_enviar();
      }
    }

    //-------- QUIERO_ENVIAR

    //Comprobamos si esta activo el indicador de solicitud de envio
    if(quiero_enviar==1) {
      //leemos del registro de estado la se¤al de estado enviada por el
      //otro PC
      estado=recibir(dbase,REG_ESTADO);
      estado=decod_estadosignal(estado);
      //comprobamos si el otro PC esta ejecutando o no alguna
      //seccion critica
      if(estado==NOSC) {
	//si el otro PC no esta ejecutando ninguna seccion critica
	//le enviamos la se¤al de estado seccion critica quiero enviar.
	//Volvemos a leer la se¤al de estado del otro PC
	enviar(dbase,REG_CONTROL,cod_estadosignal(SC_QENVIAR));
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_estadosignal(estado);
	//volvemos a comprobar si la se¤al de estado del otro PC es
	//no seccion critica, de esta manera, evitamos interbloqueos
	if(estado==NOSC) {
	  //ejecutamos la seccion critica quiero enviar
	  q_enviar(ruta_env);
	  //borramos la barra de progreso del fichero a enviar y la zona
	  //de mensajes
	  Ventana(22,199,617,215,1);
	  imprimir(7,1);
	  imprimir(6,10);

	  sc_qenv=0;//desactivamos el indicador de seccion critica quiero enviar
	  //enviamos al otro PC la se¤al de estado no seccion critica
	  enviar(dbase,REG_CONTROL,cod_estadosignal(NOSC));
	} else {
	  //si el otro PC esta en alguna seccion critica le enviamos
	  //la se¤al de estado no seccio critica y hacemos un retardo
	  enviar(dbase,REG_CONTROL,cod_estadosignal(NOSC));
	  delay(10);
	}
      }
    }

    //--------- ENVIAR

    //comprobamos si estamos enviando y si no estamos en espera de envio
    if(enviando==1 && !espera_env) {
      //leemos del registro de estado la se¤al de estado enviada por el otro PC
      estado=recibir(dbase,REG_ESTADO);
      estado=decod_estadosignal(estado);
      //comprobamos si el otro PC esta ejecutando su seccion critica
      //enviar o su seccion critica quiero enviar
      if(estado!=SC_ENVIAR && estado!=SC_QENVIAR) {
	//enviamos al otro PC la se¤al de estado seccion critica enviar
	enviar(dbase,REG_CONTROL,cod_estadosignal(SC_ENVIAR));
	//volvemos a leer del registro de estado la se¤al de estado
	//enviada por el otro PC
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_estadosignal(estado);
	//volvemos a comprobar si el otro PC esta ejecutando su seccion
	//critica enviar o su seccion critica quiero enviar
	if(estado!=SC_ENVIAR && estado!=SC_QENVIAR) {
	  //ejecutamos nuestra seccion critica enviar
	  proto_enviar();
	  bytes_env++;//incrementamos el numero de bytes enviados

	  //actualizamos la barra de progreso del fichero a enviar
	  if ( tam_fich_env > 595 ) {
	    cont_env++;
	    if ( cont_env == bxl_env ) {
	      cont_env=0;
	      lineas_env++;
	      if (lineas_env <= 595 ) {
		setcolor(14);
		line(lineas_env+21,199,lineas_env+21,215);
	      }
	    }
	  } else {
	    if ( prog_env * bxl_env+21 <=  595) {
	      setcolor(14);
	      for(cont_env=1;cont_env<=bxl_env;cont_env++)
		line(prog_env*bxl_env+cont_env+21,199,prog_env*bxl_env+cont_env+21,215);
	    }
	  }
	  prog_env++;

	  //Actualizamos el indicador de % del fichero a enviar
	  tpc=(bytes_env * 100) / tam_fich_env;
	  if(tpc!=tpc_env) {
	    mostrar_tpc(tpc,0);
	    tpc_env=tpc;
	  }


	  //comprobamos si el numero de bytes enviados de forma seguida ha
	  //alcanzado MAXNBYTES en cuyo caso activamos el indicador
	  //de espera de envio, dsando al otro PC oportunidad de enviar
	  if(recibiendo) {
	    nb_env++;
	    if(nb_env>MAXNBYTES) {
	      espera_env=1;
	      nb_env=0;
	    }
	  } else {
	    espera_env=0;
	  }
	  nb_rec=espera_rec=0;
	  enviar(dbase,REG_CONTROL,cod_estadosignal(NOSC));
	} else {
	  enviar(dbase,REG_CONTROL,cod_estadosignal(NOSC));
	  delay(10);
	}
      }
    }

    //---------RECIBIR
    //comprobamos si estamos recibiendo,si no queremos ejecutar la seccion
    //critica quiero enviar y si no estamos en espera de recepcion
    if(recibiendo==1 && !sc_qenv && !espera_rec) {
      //leemos del registro de estado la se¤al de estado enviada por el otro PC
      estado=recibir(dbase,REG_ESTADO);
      estado=decod_estadosignal(estado);
      //comprobamos si el otro PC esta ejecutando su seccion critica recibir
      //o su seccion critica quiero enviar
      if(estado!=SC_RECIBIR && estado!=SC_QENVIAR) {
	//enviamos al otro PC la se¤al de estado seccion critica recibir
	enviar(dbase,REG_CONTROL,cod_estadosignal(SC_RECIBIR));
	//volvemos a leer del registro de estado la se¤al de estado
	//enviada por el otro PC
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_estadosignal(estado);
	//volvemos a comprobar si el otro PC esta ejecutando su seccion
	//critica recibir o su seccion critica quiero enviar. de esta manera
	//evitamos interbloqueos
	if(estado!=SC_RECIBIR && estado!=SC_QENVIAR) {
	  //ejecutamos nuestra seccion critica recibir
	  proto_recibir();
	  bytes_rec++;//incrementamos el numero de bytes recibidos
	  //actualizamos la barra de progreso del fichero a recibir
	  if ( tam_fich_rec > 595 ) {
	    cont_rec++;
	    if ( cont_rec == bxl_rec ) {
	      cont_rec=0;
	      lineas_rec++;
	      if (lineas_rec <= 595 ) {
		setcolor(14);
		line(lineas_rec+21,441,lineas_rec+21,457);
	      }
	    }
	  } else {
	    if ( prog_rec * bxl_rec+21 <=  595) {
	      setcolor(14);
	      for(cont_rec=1;cont_rec<=bxl_rec;cont_rec++)
		line(prog_rec*bxl_rec+cont_rec+21,441,prog_rec*bxl_rec+cont_rec+21,457);
	    }
	  }
	  prog_rec++;
	  //actualizamos el indicador de % del fichero a recibir
	  tpc=(bytes_rec * 100) / tam_fich_rec;
	  if(tpc!=tpc_rec) {
	    mostrar_tpc(tpc,1);
	    tpc_rec=tpc;
	  }

	  //comprobamos si el numero de bytes recibidos de forma seguida ha
	  //alcanzado MAXNBYTES en cuyo caso activamos el indicador
	  //de espera de recepcion, dando al otro PC oportunidad de recibir
	  if(enviando) {
	    nb_rec++;
	    if(nb_rec>MAXNBYTES) {
	      espera_rec=1;
	      nb_rec=0;
	    }
	  } else {
	    espera_rec=0;
	  }
	  nb_env=espera_env=0;

	  if(!sc_qenv) {
	    enviar(dbase,REG_CONTROL,cod_estadosignal(NOSC));
	  }

	} else {
	  enviar(dbase,REG_CONTROL,cod_estadosignal(NOSC));
	  delay(10);
	}
      }
    }
  }
  //cerramos el modo grafico
  CG();
}
//----------------------------------------------------------------------


//-------------------- DESARROLLO DE FUNCIONES
//-----------------------
void InG() {
// INICIA EL PROGRAMA EN MODO GRAFICO

  int gdriver = DETECT, gmode, errorcode;

  initgraph(&gdriver, &gmode, "");
  errorcode = graphresult();
  if (errorcode != grOk)  {
    clrscr();
    textcolor(4);
    printf("Error grafico: %s\n", grapherrormsg(errorcode));
    printf("Presione una tecla para salir");
    getch();
    exit(1);
  }
}
//-----------------------

//-----------------------
void CG() {
// RESTAURA EL MODO GRAFICO QUE HABIA
// ANTERIOR AL USADO ACTUALMENTE
  restorecrtmode();
}
//-----------------------

//-----------------------
void Ventana (int xini,int yini, int xfin, int yfin,int color) {
// DIBUJA EN MODO GRAFICO UNA VENTANA INTRODUCIENDOLE
// LAS COORDENADAS Y EL COLOR

  int i;

  setcolor(color);
  for (i=xini;i<=xfin;i++) line(i, yini,i,yfin);
}
//-----------------------

//-----------------------
void Cuadro(int xini,int yini, int xfin, int yfin,int color) {
// DIBUJA UN CUADRO EN MODO GRAFICO INTRODUCIENDOLE
// LAS COORDENADAS Y EL COLOR

  setcolor(color);
  line(xini, yini,xini,yfin);
  line(xini,yini,xfin,yini);
  line(xfin,yini,xfin,yfin);
  line(xfin,yfin,xini,yfin);
}
//-----------------------

//-----------------------
void imprimir(int i,int color) {
//MUESTRA EN PANTALLA UNA SERIE DE MENSAJES.
//EL MENSAJE A MOSTRAR ESTA INDICADO POR EL PARAMETRO i.
//EL PARAMETRO color INDICA EL COLOR EN QUE SE MOSTRARA EL MENSAJE

  setcolor(color);
  switch (i) {
    case 1:
      outtextxy(15,55,"FICHERO ");
      break;
    case 2:
      outtextxy(15,75,"ENVIAR");
      break;
    case 3:
      outtextxy(15,95,"SALIR");
      break;
    case 6:
      outtextxy(30,170,"ENVIANDO FICHERO...");
      break;
    case 7:
      outtextxy(30,170,"ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ");
      break;
    case 8:
      outtextxy(200,170,"TRASNFERENCIA FINALIZADA CON EXITO");
      break;
    case 10:
      outtextxy(30,150,"ERROR AL ABRIR EL FICHERO");
      break;
    case 11:
      outtextxy(30,421,"RECIBIENDO FICHERO...");
      break;
    case 12:
      outtextxy(200,421,"TRANSFERENCIA FINALIZADA CON EXITO");
      break;
    case 13:
      outtextxy(85,300,"ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ");
      break;
    case 14:
      outtextxy(30,421,"ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ");
      break;
  }
}
//-----------------------

//-----------------------
void mostrar_tpc(int tp,int pos) {
//MUESTRA EN PANTALLA EL VALOR DEL PARAMETRO tp (%)
//EL PARAMETRO pos INDICA DONDE SE MOSTRARA tp
//SI pos=0 EL MENSAJE SE MOSTRARA ENCIMA DE LA BARRA DE PROGRESO DEL
//FICHERO A ENVIAR, EN CUALQUIER OTRO CASO, SE MOSTRARA ENCIMA DE LA BARRA
//DE PROGRESO DEL FICHER A RECIBIR

  char num_car[1];
  int n;

  if(pos) {
    pos=242;
  }
  strcpy(num_car,"");
  num_car[1]='\x0';
  setcolor(1);
  outtextxy(550,180+pos,"ÛÛÛÛÛÛÛ");
  setcolor(15);
  if(tp==100) {
    outtextxy(550,180+pos,"100 % ");
  }
  if(tp<100 && tp>9) {
    n=tp / 10;
    num_car[0]=num[n];
    outtextxy(550,180+pos,num_car);
    n=tp - (n * 10);
    num_car[0]=num[n];
    outtextxy(558,180+pos,num_car);
    outtextxy(566,180+pos," % ");
  }
  if(tp<10) {
    num_car[0]=num[tp];
    outtextxy(550,180+pos,num_car);
    outtextxy(558,180+pos," % ");
  }
}
//-----------------------
void enviar(unsigned int dbase,int reg,unsigned char dato) {
//ENVIA EL BYTE dato AL REGISTRO DEL PUERTO PARALELO INDICADO POR EL
//PARAMETRO reg. EL PARAMETRO dbase INDICA LA DIRECCION BASE DEL
//PUERTO PARALELO
  outportb(dbase+reg,dato);
}
//-----------------------

//-----------------------
unsigned char recibir (unsigned int dbase,int reg) {
//DEVUELVE EL BYTE LEIDO DEL REGISTRO INDICADO EN EL PARAMETRO reg.
//EL PARAMETRO dbase ALMACENA LA DIRECCION BASE DEL PUERTO PARALELO
  unsigned char dato;

  dato=inportb(dbase+reg);
  return dato;
}
//-----------------------

//-----------------------
unsigned int puerto() {
  //DEVUELVE LA DIRECCION BASE DEL PUERTO PARALELO
  int desp=5;
  int encontrado=0;

  while ( ! encontrado ) {
    desp=desp+3;
    encontrado=peek(0x0040,desp);
  }
//  return (0x0378);  // PARA MI ORDENADOR
  return (encontrado);
}
//-----------------------

//-----------------------
void modo_enviar(unsigned int dbase) {
//PONE EL PUERTO PARALELO EN MODO ENVIAR
  unsigned char val;

  val=inportb(dbase+2);
  outportb(dbase+2,val & 0xDF);
}
//-----------------------

//-----------------------
void modo_recibir(unsigned int dbase) {
//PONE EL PUERTO PARALELO EN MODO RECIBIR
  unsigned char val;

  val=inportb(dbase+2);
  outportb(dbase+2,val | 0x20);
}
//-----------------------

//-----------------------
void q_enviar(char *nfich) {
//REALIZA UNA SOLICITUD DE ENVIO DE FICHERO AL OTRO PC
//EL PARAMETRO nfich ES LA RUTA DEL FICHERO A ENVIAR

  tamfich tama;//tama¤o del fichero a enviar
  unsigned char estado=0;//almacena el valor del registro de estado

  //enviamos la se¤al de control quiero_enviar
  enviar(dbase,REG_CONTROL,cod_controlsignal(QUIERO_ENVIAR));
  estado=2;
  //esperamos hasta que el otro PC conteste con la se¤al de control puedes_enviar
  while(estado!=PUEDES_ENVIAR) {
    estado=recibir(dbase,REG_ESTADO);
    estado=decod_controlsignal(estado);
  }

  if(estado==PUEDES_ENVIAR) {
    //abrimos el fichero a enviar
    if( (fentrada=fopen(nfich,"rb"))==NULL) {
      exit(1);
    }
    //leemos el tama¤o en bytes del ficher a enviar
    tama=filesize(fentrada);
    tam_fich_env=tamafichero(tama);

    //calculamos el numero de bytes por linea de cada linea de la barra
    //de progreso
    if ( tam_fich_env > 595 ){
      bxl_env=tam_fich_env/595;
    } else {
      bxl_env= 595/tam_fich_env+1;
    }
    //iniciamos variables
    prog_env=0;
    cont_env=0;
    lineas_env=0;
    quiero_enviar=0;
    enviando=1;
    bytes_env=0;
    tpc_env=0;
    imprimir(6,10);
    mostrar_tpc(0,0);
    //enviamos el tama¤o y nombre del fichero al otro PC
    env_inf_fich(tama,nfich);

  }
}
//-----------------------

//-----------------------
void p_enviar() {
//DA PERMISO PARA ENVIAR UN FICHERO AL OTRO PC

  unsigned char estado;//almacena el byte del registro de estado
  tamfich tama;//almacena el tama¤o del fichero a recibir

  //enviamos al otro PC la se¤al de control puedes enviar
  enviar(dbase,REG_CONTROL,cod_controlsignal(PUEDES_ENVIAR));
  //recibimos el tama¤o y nombre del fichero a recibir
  tama=rec_inf_fich();
  tam_fich_rec=tamafichero(tama);
  recibiendo=1;//activamos el indicador de recibiendo fichero

  //creamos el fichero
  if( (fsalida=fopen(ruta_rec,"wb"))==NULL) {
    exit(1);
  }

  //calculamos el numero de bytes por linea de las lineas de la barra de
  //progreso del fichero a recibir
  if ( tam_fich_rec > 595 ){
    bxl_rec=tam_fich_rec/595;
  } else {
    bxl_rec= 595/tam_fich_rec+1;
  }
  //iniciamos variables y actualizamos informacion del ficheroen pantalla
  prog_rec=0;
  cont_rec=0;
  lineas_rec=0;
  bytes_rec=0;
  tpc_rec=0;
  imprimir(14,1);
  imprimir(11,10);
  imprimir(13,1);
  Ventana(22,441,617,457,1);
  setcolor(15);
  outtextxy(85,300,nombrefich);
  mostrar_tpc(0,1);
}
//-----------------------

//-----------------------
void proto_enviar() {
//PROTOCOLO ENVIAR
//MARCA LAS REGLAS PARA ENVIAR UN BYTE AL OTRO ORDENADOR

  unsigned char estado;//almacena el contenido del registro de estado

  //leemos del fichero el byte a enviar
  fread(&byte_enviar,1,1,fentrada);

  //comprobamos si hemos llegado a final de fichero
  if(feof(fentrada)) {
    //si es final de fichero en viamos la se¤al de control ULTIMO_BYTE
    enviar(dbase,REG_CONTROL,cod_controlsignal(ULTIMO_BYTE));
    enviando=0;//desactivamos el indicador de enviado fichero
    fclose(fentrada);//cerramos el fichero
    imprimir(8,10);
  } else {
    //si no es el ultimo byte, enviamos la se¤al de control SIG_BYTE
    enviar(dbase,REG_CONTROL,cod_controlsignal(SIG_BYTE));
    estado=0;
    //esperamos hasta que se reciba la se¤al de control PREPARADO
    //indicando que el otro PC esta preparado para recibir el byte
    while(estado!=PREPARADO) {
      estado=recibir(dbase,REG_ESTADO);
      estado=decod_controlsignal(estado);
    }

    if(estado==PREPARADO) {
      //ponemos el puerto paralelo en modo enviar
      modo_enviar(dbase);
      //enviamos por el registro de datos el byte a enviar
      enviar(dbase,REG_DATOS,byte_enviar);
      //enviamos la se¤al de control DISPONIBLE, indicando al otro PC
      //que el byte ha sido enviado
      enviar(dbase,REG_CONTROL,cod_controlsignal(DISPONIBLE));
      estado=0;
      //esperamos hasta recibir la se¤al de control RECIBIDO, indicando
      //que el otro PC a recibido correctamente el byte
      while(estado!=RECIBIDO) {
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_controlsignal(estado);
      }
    }
  }
}
//-----------------------

//-----------------------
void proto_recibir() {
//PROTOCOLO RECIBIR
//MARCA LAS REGLAS PARA RECIBIR UN BYTE DEL OTRO ORDENADOR

  unsigned char estado=3;//alamcena el valor del registro de estado

  //esperamos hasta recibir la se¤al de control SIG_BYTE o ULTIMO_BYTE
  while(estado!=ULTIMO_BYTE && estado!=SIG_BYTE) {
    estado=recibir(dbase,REG_ESTADO);
    estado=decod_controlsignal(estado);
  }
  //comprobamos si la se¤al de control recibida es la de ULTIMO_BYTE
  if(estado==ULTIMO_BYTE) {
    //si es el ultimo byte, cerramos el fichero y desactivamos el indicador
    //de recibiendo fichero
    fclose(fsalida);
    recibiendo=0;
    imprimir(12,10);
  }else{
    //comprobamos si la se¤al de control recibida es SIG_BYTE
    if(estado==SIG_BYTE) {
      //enviamos la se¤al de control PREPARADO, indicando que estamos
      //preparados para recibir un byte
      enviar(dbase,REG_CONTROL,cod_controlsignal(PREPARADO));
      estado=0;
      //esperamos hasta recibir la se¤al de control DISPONIBLE
      while(estado!=DISPONIBLE) {
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_controlsignal(estado);
      }
      //al recibir la se¤al de control DISPONIBLE, ponemos el puerto paralelo
      //en modo recibir, leemos del registro de datos el byte y lo escribimos
      //en el fichero
      modo_recibir(dbase);
      byte_recibir=recibir(dbase,REG_DATOS);
      fwrite(&byte_recibir,1,1,fsalida);
      //comprobamos si queremos enviar un fichero
      if(quiero_enviar==1) {
	//si queremos enviar un fichero, enviamos la se¤al de estado
	//seccion critica quiero enviar y activamos el indicador de
	//seccion critica quiero enviar, forzando a entrar en la seccion
	//critica quiero enviar en la siguiente iteraccion de bucle
	enviar(dbase,REG_CONTROL,cod_estadosignal(SC_QENVIAR));
	sc_qenv=1;
      }
      //enviamos la se¤al de control RECIBIDO indicando que hemos recibido
      //el byte correctamente
      enviar(dbase,REG_CONTROL,cod_controlsignal(RECIBIDO));
    }
  }
}
//-----------------------

//-----------------------
unsigned char decod_controlsignal(unsigned char x) {
//DEVUELVE LA SE¥AL DE CONTROL ALMACENADA EN EL PARAMETRO x

  x=x>>4;
  x=x^3;
  x=x&0xf;
  x=x & 3;
  return(x);
}
//-----------------------

//-----------------------
unsigned char cod_controlsignal(unsigned char x) {
//DEVUELVE UN BYTE QUE CONTIENE LA SE¥AL DE CONTROL QUE INDICA EL PARAMETRO x

  unsigned int control;
  unsigned int cod;

  control=recibir(dbase,REG_CONTROL);
  cod=(control & 12) | x;
  return(cod);
}
//-----------------------

//-----------------------
unsigned char decod_estadosignal(unsigned char x) {
//DEVUELVE LA SE¥AL DE ESTADO ALMACENADA EN EL PARAMETRO x

  x=x>>6;
  return(x);
}
//-----------------------

//-----------------------
unsigned char cod_estadosignal(unsigned char x) {
//DEVUELVE UN BYTE QUE CONTIENE LA SE¥AL DE ESTADO QUE INDICA EL PARAMETRO x

  unsigned int estado;
  unsigned int cod;

  x=x<<2;
  estado=recibir(dbase,REG_CONTROL);
  cod=(estado & 243) | x;
  return(cod);
}
//-----------------------

//-----------------------
tamfich filesize(FILE *stream) {
//DEVUELVE 4 BYTES EL TAMA¥O DEL FICHERO INDICADO POR EL PARAMETRO stream

   long curpos, length;
   int i;
   tamfich t;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   for(i=3;i>=0;i--) {
     t.t[i]=length & 255;
     length=length>>8;
   }
   return (t);
}
//-----------------------

//-----------------------
long tamafichero(tamfich t) {
//DEVUELVE EN FORMATO long EL VALOR DEL PARAMETRO t

  long aux=0;
  int i;

  for(i=0;i<=3;i++) {
    aux=aux | t.t[i];
    if (i!=3) {
      aux=aux<<8;
    }
  }
  return(aux);
}
//-----------------------

//-----------------------
void env_inf_fich(tamfich t, char *nomb) {
//ENVIA AL OTRO PC EL TAMA¥O Y NOMBRE DEL FICHERO A ENVIAR

    int i;
    unsigned int estado;
    int longcad;

    //enviamos el nombre del fichero
    i=longcad=strlen(nomb);
    while(i>0 && nomb[i-1]!='\\') {
      i--;
    }
    for(;i<=longcad;i++) {
      enviar(dbase,REG_CONTROL,cod_controlsignal(SIG_BYTE));
      estado=0;

      while(estado!=PREPARADO) {
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_controlsignal(estado);
      }

      if(estado==PREPARADO) {
	modo_enviar(dbase);
	enviar(dbase,REG_DATOS,nomb[i]);
	enviar(dbase,REG_CONTROL,cod_controlsignal(DISPONIBLE));
	estado=0;

	while(estado!=RECIBIDO) {
	  estado=recibir(dbase,REG_ESTADO);
	  estado=decod_controlsignal(estado);
	}
      }
    }

    //enviamos el tama¤o del fichero
    for(i=0;i<=3;i++) {
      enviar(dbase,REG_CONTROL,cod_controlsignal(SIG_BYTE));
      estado=0;

      while(estado!=PREPARADO) {
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_controlsignal(estado);
      }

      if(estado==PREPARADO) {
	modo_enviar(dbase);
	enviar(dbase,REG_DATOS,t.t[i]);
	enviar(dbase,REG_CONTROL,cod_controlsignal(DISPONIBLE));
	estado=0;

	while(estado!=RECIBIDO) {
	  estado=recibir(dbase,REG_ESTADO);
	  estado=decod_controlsignal(estado);
	}
      }
   }
}
//-----------------------

//-----------------------
tamfich rec_inf_fich() {
//RECIBE EL TAMA¥O Y NOMBRE DEL FICHERO A RECIBIR

  tamfich t;
  int i;
  unsigned char estado;
  char caracter[2];

  strcpy(ruta_rec,"c:\\");
  strcpy(nombrefich,"");
  strcpy(caracter,"a");

  //recibimos el nombre del fichero
  while(caracter[0]!='\x0') {
    estado=0;

    while(estado!=SIG_BYTE) {
      estado=recibir(dbase,REG_ESTADO);
      estado=decod_controlsignal(estado);
    }

    if(estado==SIG_BYTE) {
      enviar(dbase,REG_CONTROL,cod_controlsignal(PREPARADO));
      estado=0;

      while(estado!=DISPONIBLE) {
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_controlsignal(estado);
      }

      modo_recibir(dbase);
      caracter[0]=recibir(dbase,REG_DATOS);
      strcat(nombrefich,caracter);
      enviar(dbase,REG_CONTROL,cod_controlsignal(RECIBIDO));
    }
  }

  strcat(ruta_rec,nombrefich);

  //recibimos el tama¤o del fichero
  for(i=0;i<=3;i++) {
    estado=0;

    while(estado!=SIG_BYTE) {
      estado=recibir(dbase,REG_ESTADO);
      estado=decod_controlsignal(estado);
    }

    if(estado==SIG_BYTE) {
      enviar(dbase,REG_CONTROL,cod_controlsignal(PREPARADO));
      estado=0;

      while(estado!=DISPONIBLE) {
	estado=recibir(dbase,REG_ESTADO);
	estado=decod_controlsignal(estado);
      }

      modo_recibir(dbase);
      t.t[i]=recibir(dbase,REG_DATOS);
      enviar(dbase,REG_CONTROL,cod_controlsignal(RECIBIDO));
    }
  }
  return(t);
}
//-----------------------

//-----------------------
void connect() {
//INICIA EL REGISTRO DE CONTROL DEL PC Y POR CONSIGUIENTE EL REGISTRO DE
//ESTADO DEL OTRO PC. ESPERA A QUE EL OTRO PC REALIZE LO MISMO

  unsigned char estado;
  unsigned char control;

  enviar(dbase,REG_CONTROL,cod_estadosignal(NOSC));
  enviar(dbase,REG_CONTROL,cod_controlsignal(RECIBIDO));
  estado=control=1;
  while(estado!=NOSC && control!=RECIBIDO) {
    estado=control=recibir(dbase,REG_ESTADO);
    estado=decod_estadosignal(estado);
    control=decod_controlsignal(control);
  }
}
//-----------------------
//----------------------------------------------------------------------
