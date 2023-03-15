/********************************************************************************
* main.c: Demonstration av inbyggt system innefattande PWM-styrning av
*         lysdioder anslutna till pin 8 - 10 (PORTB0 - PORTB2) via en
*         potentiometer ansluten till analog pin A0 (PORTB). Maximalt fem
*         Watchdog-timeouts (sker efter 8192 ms utan Watchdog reset) f�r ske
*         innan systemet l�ses.
*
*         F�r att genomg�ra Watchdog reset kan anv�ndaren trycka p� en
*         tryckknapp ansluten till pin 13 (PORTB5). Efter fem timeouts l�ses
*         systemet, d�r det enda som sker �r att en lysdiod ansluten till
*         pin 8 (PORTB0) blinkar var 50:e millisekund via Timer 1.
*
*         Utskrift sker via seriell �verf�ring efter varje Watchdog timeout,
*         vid Watchdog reset samt vid l�sning av systemet. F�r att undvika
*         multipla avbrott orsakat av kontaktstudsar inaktiveras PCI-avbrott
*         p� I/O-port B i 300 millisekunder efter nedtryckning, implementerat
*         via Timer 0.
********************************************************************************/
#include "header.h"

/* Deklaration av globala objekt: */
struct led l1;
struct button b1;
struct timer t0, t1;

/********************************************************************************
* setup: Initierar systemet enligt f�ljande:

*        1. Initierar lysdiod l1 ansluten till pin 8 (PORTB0).
*
*        2. Initierar tryckknapp b1 ansluten till pin 13 (PORTB5) och
*           aktiverar avbrott vid nedtryckning/uppsl�ppning.
*           Avbrottsvektor f�r avbrottsrutinen �r PCINT0_vect.
*
*        3. Initierar timer t0 till den 8-bitars timerkretsen Timer 0, som
*           l�per ut efter 300 millisekunder efter aktivering i Normal Mode.
*           Avbrottsvektor f�r avbrottsrutinen �r TIMER0_OVF_vect.
*
*        4. Initierar timer t1 till den 16-bitars timerkretsen Timer 1, som
*           l�per ut efter 50 millisekunder efter aktivering i CTC Mode
*           (r�knar upp till 256 och fungerar d�rmed som en 8-bitars timer).
*           Avbrottsvektor f�r avbrottsrutinen �r TIMER0_COMPA_vect.
*
*        5. Initierar seriell �verf�ring med en baud rate p� 9600 kbps f�r
*           att m�jligg�ra utskrift till seriell terminal.
*
*        6. Initierar Watchdog-timern med en timeout p� 8192 ms. Avbrott
*           aktiveras s� att timeout medf�r avbrott. Avbrottsvektorn f�r
*           motsvarande avbrottsrutin �r WDT_vect.
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
*       var 8192:e millisekund om inte anv�ndaren under denna tid �terst�ller
*       Watchdog-timern, vilket �stadkommes genom att trycka ned tryckknapp
*       b1 ansluten till pin 13 (PORTB5). Efter fem timeouts l�ses systemet.
*       Lysdiod l1 ansluten till pin 8 (PORTB0) kommer d� kontinuerligt blinka
*       var 50:e millisekund tills en total system�terst�llning genomf�rs.
********************************************************************************/
int main(void)
{
   setup();
   
   while (1)
   {
      
   }

   return 0;
}

