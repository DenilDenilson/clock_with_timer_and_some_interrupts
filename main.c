#include "librerias_pic/config_device.h"
#include <stdbool.h>

#define LOOP_MAIN_TRUE          1
#define DIGITO_UNIDADES_ON()    LATCbits.LATC0 = 0
#define DIGITO_UNIDADES_OFF()   LATCbits.LATC0 = 1
#define DIGITO_DECENAS_ON()     LATEbits.LATE2 = 0
#define DIGITO_DECENAS_OFF()    LATEbits.LATE2 = 1
#define DIGITO_CENTENAS_ON()    LATEbits.LATE1 = 0
#define DIGITO_CENTENAS_OFF()   LATEbits.LATE1 = 1
#define DIGITO_UND_MILLAR_ON()  LATEbits.LATE0 = 0
#define DIGITO_UND_MILLAR_OFF() LATEbits.LATE0 = 1
#define DISPLAY_PUERTO_DATOS    LATD


void PORT_Init ( void );
void PUT_Number( int num );
void TMR0Temporizador_Init(void);
void INTERRUPT_GLOBAL_Config(void);
void INTERRUPT_INTx_Config(void);

uint8_t AnodoComun7Seg[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
volatile uint16_t temporizador = 0;
volatile bool start = false;


int main (void)
{
    OSCILADOR_Init();
    PORT_Init();
    TMR0Temporizador_Init();
    INTERRUPT_GLOBAL_Config();
    INTERRUPT_INTx_Config();
    //Inicio con los display desabilitados
    DIGITO_UNIDADES_OFF();
    DIGITO_DECENAS_OFF();
    DIGITO_CENTENAS_OFF();
    DIGITO_UND_MILLAR_OFF();
    
    while(LOOP_MAIN_TRUE)
    {
        if ( PORTBbits.RB2 == 0 ) {
            __delay_ms(10);
            while ( PORTBbits.RB0 == 0 );
            T0CONbits.TMR0ON = 1; // Activar el timer
            INTCONbits.TMR0IE = 1; // Activamos la interrupcion del timer 0
        }
        
        if ( PORTBbits.RB3 == 0 ) {
            __delay_ms(10);
            while ( PORTBbits.RB0 == 0 );
            T0CONbits.TMR0ON = 0; // Activar el timer
            INTCONbits.TMR0IE = 0; // Activamos la interrupcion del timer 0
        }
        
//        if ( PORTBbits.RB1 == 0 ) {
//            __delay_ms(10);
//            while ( PORTBbits.RB1 == 0 );
//            temporizador = temporizador + 100;
//        }
//        
//        if ( PORTBbits.RB2 == 0 ) {
//            __delay_ms(10);
//            while ( PORTBbits.RB2 == 0 );
//            temporizador = temporizador + 10;
//        }
        
//        if ( PORTBbits.RB3 == 0 ) {
//            __delay_ms(10);
//            while ( PORTBbits.RB3 == 0 );
//            temporizador = temporizador + 1;
//        }
        
        PUT_Number(temporizador);
        
        
        
    }
}

/* F U N C I O N E S */
void __interrupt() RutineServiceInterrupt(void) {
    if ( INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1 ) {
        // Tarea
        if ( temporizador == 0 ) {
            temporizador = 0;
        }
        else {
            temporizador--;
        }
        
       // Reinicamos carga y flag
        INTCONbits.TMR0IF = 0;
        TMR0 = 3036;
    }
    
    if ( INTCONbits.INT0IE == 1 && INTCONbits.INT0IF == 1 ) {
        // Tarea
        temporizador++;
        
        // Reinciamos flag
        INTCONbits.INT0IF = 0;
    }
    
    if ( INTCON3bits.INT1IE == 1 && INTCON3bits.INT1IF == 1 ) {
        // Tarea
        if ( temporizador == 0 ) {
            temporizador = 0;
        }
        else {
            temporizador--;
        }
        // Reiniciamos Flag
        INTCON3bits.INT1IF = 0; 
    }
}

//Desarrollo de funciones
void PORT_Init (void)
{
    //Config Puerto D
    ANSELD = 0x00; //Digitales
    TRISD  = 0x00; //Salidas
    
    //Config Puerto E
    ANSELEbits.ANSE0 = 0;
    ANSELEbits.ANSE1 = 0;
    ANSELEbits.ANSE2 = 0;
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
    
    // Botones
    ANSELB = 0x00;
    TRISB = 0b00001111;
    
    //Config Puerto C
    TRISCbits.TRISC0 = 0; //Salida
}

void PUT_Number( int num ) {
    int unidades, decenas, centenas, unidad_millar;
    // Extraer las unidades
    unidades = num % 10;

    // Extraer las decenas
    num /= 10;
    decenas = num % 10;

    // Extraer las centenas
    num /= 10;
    centenas = num % 10;

    // Extraer la unidad de millar
    num /= 10;
    unidad_millar = num % 10;
    
    DIGITO_UNIDADES_OFF();
    DIGITO_DECENAS_OFF();
    DIGITO_CENTENAS_OFF();
    DIGITO_UND_MILLAR_OFF();
    //Escribir en el puerto de datos
    DISPLAY_PUERTO_DATOS = AnodoComun7Seg[unidades];
    //Habilito el digito de visualizacion
    DIGITO_UNIDADES_ON();
    __delay_ms(2); //Tiempo de visualizacion

    //Desabilitar digitos
    DIGITO_UNIDADES_OFF();
    DIGITO_DECENAS_OFF();
    DIGITO_CENTENAS_OFF();
    DIGITO_UND_MILLAR_OFF();
    //Escribir en el puerto de datos
    DISPLAY_PUERTO_DATOS = AnodoComun7Seg[decenas];
    DIGITO_DECENAS_ON(); //Habilito el digito de visualizacion
    __delay_ms(2); //Tiempo de visualizacion

    //Desabilitar digitos
    DIGITO_UNIDADES_OFF();
    DIGITO_DECENAS_OFF();
    DIGITO_CENTENAS_OFF();
    DIGITO_UND_MILLAR_OFF();
    //Escribir en el puerto de datos
    DISPLAY_PUERTO_DATOS = AnodoComun7Seg[centenas];
    DIGITO_CENTENAS_ON(); //Habilito el digito de visualizacion
    __delay_ms(2); //Tiempo de visualizacion

    //Desabilitar digitos
    DIGITO_UNIDADES_OFF();
    DIGITO_DECENAS_OFF();
    DIGITO_CENTENAS_OFF();
    DIGITO_UND_MILLAR_OFF();
    //Escribir en el puerto de datos
    DISPLAY_PUERTO_DATOS = AnodoComun7Seg[unidad_millar];
    DIGITO_UND_MILLAR_ON();
    //Habilito el digito de visualizacion
    __delay_ms(2); //Tiempo de visualizacion
}


void TMR0Temporizador_Init(void) {
    // CONFIGURAMOS EL TIMER
    T0CONbits.T08BIT = 0; // 16 bits 0 ... +65k counts
    T0CONbits.T0CS = 0; // Contador de pulsos Fosc/4
    T0CONbits.PSA = 0; // Activa el preescaler
    T0CONbits.T0PS = 0b101; // Preescaler x64
    
    // ASIGNAMOS UNA CARGA INICIAL
    // Cálculos hechos para tener un interrup a 1 segundo
    TMR0 = 3036; // El timer 0 inicia su cuenta en 3036 ... 65535
    
    // Configuramos la interrupción por desbordamiento
    
    INTCONbits.TMR0IF = 0; // Ponemos en baja el flag del interrupt
    
}

void INTERRUPT_GLOBAL_Config(void) {
    RCONbits.IPEN = 0; // No establece prioridades en las interrupciones
    INTCONbits.GIE = 1; // Habilita las interrupciones
    INTCONbits.PEIE = 1; // Habilita interrupciones por periféricos dentro del micro

    
}

void INTERRUPT_INTx_Config(void) {
        
    /* INT0 */
    INTCONbits.INT0IE = 1; // INT0 activado
    INTCONbits.INT0IF = 0; // Iniciamos flag deactivate
    INTCON2bits.INTEDG0 = 0; // Config INT0 por flanco de bajada}
    
    /* INT1 */
    INTCON3bits.INT1IE = 1; // INT1 activado
    INTCON3bits.INT1IF = 0; // Iniciamos flag disable
    INTCON2bits.INTEDG1 = 0; // Config flanco de bajada
    
    /* INT2 */
//    INTCON3bits.INT2IE = 1; // INT2 activado
//    INTCON3bits.INT2IF = 0; // Iniciamos flag disable
//    INTCON2bits.INTEDG2 = 0; // Config flanco de bajada
    
}