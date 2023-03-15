/********************************************************************************
* header.h: Innehållande diverse definitioner samt deklarationer för
*           implementering av det inbyggda systemet.
********************************************************************************/
#ifndef HEADER_H_
#define HEADER_H_

/* Inkluderingsdirektiv: */
#include "led.h"
#include "button.h"
#include "timer.h"
#include "serial.h"
#include "eeprom.h"
#include "wdt.h"

/* Makrodefinitioner: */
#define TIMEOUT_MAX 5       /* Maximalt antal timeouts innan programmet låses. */

/* Deklaration av globala objekt: */
extern struct led l1;
extern struct button b1;
extern struct timer t0, t1;

/********************************************************************************
* setup: Initierar systemet enligt följande:

*        1. Initierar lysdiod l1 ansluten till pin 8 (PORTB0).
*
*        2. Initierar tryckknapp b1 ansluten till pin 13 (PORTB5) och
*           aktiverar avbrott vid nedtryckning/uppsläppning.
*           Avbrottsvektor för avbrottsrutinen är PCINT0_vect.
*
*        3. Initierar timer t0 till den 8-bitars timerkretsen Timer 0, som
*           löper ut efter 300 millisekunder efter aktivering i Normal Mode.
*           Avbrottsvektor för avbrottsrutinen är TIMER0_OVF_vect.
*
*        4. Initierar timer t1 till den 16-bitars timerkretsen Timer 1, som
*           löper ut efter 50 millisekunder efter aktivering i CTC Mode
*           (räknar upp till 256 och fungerar därmed som en 8-bitars timer).
*           Avbrottsvektor för avbrottsrutinen är TIMER0_COMPA_vect.
*
*        5. Initierar seriell överföring med en baud rate på 9600 kbps för
*           att möjliggöra utskrift till seriell terminal.
*
*        6. Skriver startvärdet 0 till adressen 100 i EEPROM-minnet. Denna
*           adress används för att lagra antalet passerade Watchdog timeouts.
*
*        7. Initierar Watchdog-timern med en timeout på 8192 ms. Avbrott
*           aktiveras så att timeout medför avbrott. Avbrottsvektorn för
*           motsvarande avbrottsrutin är WDT_vect.
********************************************************************************/
void setup(void);

#endif /* HEADER_H_ */