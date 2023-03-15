/********************************************************************************
* main.c: Demonstration av inbyggt system innefattande PWM-styrning av
*         lysdioder anslutna till pin 8 - 10 (PORTB0 - PORTB2) via en
*         potentiometer ansluten till analog pin A0 (PORTB). Maximalt fem
*         Watchdog-timeouts (sker efter 8192 ms utan Watchdog reset) får ske
*         innan systemet låses.
*
*         För att genomgöra Watchdog reset kan användaren trycka på en
*         tryckknapp ansluten till pin 13 (PORTB5). Efter fem timeouts låses
*         systemet, där det enda som sker är att en lysdiod ansluten till
*         pin 8 (PORTB0) blinkar var 50:e millisekund via Timer 1.
*
*         Utskrift sker via seriell överföring efter varje Watchdog timeout,
*         vid Watchdog reset samt vid låsning av systemet. För att undvika
*         multipla avbrott orsakat av kontaktstudsar inaktiveras PCI-avbrott
*         på I/O-port B i 300 millisekunder efter nedtryckning, implementerat
*         via Timer 0.
********************************************************************************/
#include "header.h"

/* Deklaration av globala objekt: */
struct led l1;
struct button b1;
struct timer t0, t1;

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
*        6. Initierar Watchdog-timern med en timeout på 8192 ms. Avbrott
*           aktiveras så att timeout medför avbrott. Avbrottsvektorn för
*           motsvarande avbrottsrutin är WDT_vect.
********************************************************************************/
void setup(void)
{
   led_init(&l1, 8);
   button_init(&b1, 13);
   button_enable_interrupt(&b1);

   timer_init(&t0, TIMER_SEL_0, 300);
   timer_init(&t1, TIMER_SEL_1, 50);

   serial_init(9600);

   wdt_init(WDT_TIMEOUT_8192_MS);
   wdt_enable_interrupt();
   return;
}

/********************************************************************************
* main: Initierar systemet vid start. Watchdog timeout sker sedan kontinuerligt
*       var 8192:e millisekund om inte användaren under denna tid återställer
*       Watchdog-timern, vilket åstadkommes genom att trycka ned tryckknapp
*       b1 ansluten till pin 13 (PORTB5). Efter fem timeouts låses systemet.
*       Lysdiod l1 ansluten till pin 8 (PORTB0) kommer då kontinuerligt blinka
*       var 50:e millisekund tills en total systemåterställning genomförs.
********************************************************************************/
int main(void)
{
   setup();
   
   while (1)
   {
      
   }

   return 0;
}

