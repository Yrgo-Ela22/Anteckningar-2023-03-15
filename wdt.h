/********************************************************************************
* wdt.h: Inneh�ller drivrutiner f�r mikrokontrollerns Watchdog-timer, b�de i 
*        System Reset Mode, vilket inneb�r att mikrodatorn �terst�lls vid 
*        timeout, samt i Interrupt Mode, vilket inneb�r avbrott vid timeout.
*        Det �r ocks� m�jligt att kombinera tidigare n�mnda modes f�r att
*        avbrott sker, f�ljt av system�terst�llning.
*
*        Avbrottsvektorn f�r timeout-avbrott �r WDT_vect. Timeout-avbrott
*        m�ste �teraktiveras i motsvarande avbrottsrutin, vilket genomf�rs
*        via anrop av funktionen wdt_enable_interrupt.
********************************************************************************/
#ifndef WDT_H_
#define WDT_H_

/* Inkluderingsdirektiv: */
#include "misc.h"

/********************************************************************************
* wdt_timeout: Enumeration f�r val av timeout f�r mikrodatorns Watchdog-timer.
********************************************************************************/
enum wdt_timeout
{
   WDT_TIMEOUT_16_MS   = 0,                                       /* 16 ms. */
   WDT_TIMEOUT_32_MS   = (1 << WDP0),                             /* 32 ms. */
   WDT_TIMEOUT_64_MS   = (1 << WDP1),                             /* 64 ms. */
   WDT_TIMEOUT_128_MS  = (1 << WDP1) | (1 << WDP0),               /* 128 ms. */
   WDT_TIMEOUT_256_MS  = (1 << WDP2),                             /* 256 ms. */
   WDT_TIMEOUT_512_MS  = (1 << WDP2) | (1 << WDP0),               /* 512 ms. */
   WDT_TIMEOUT_1024_MS = (1 << WDP2) | (1 << WDP1),               /* 1024 ms. */
   WDT_TIMEOUT_2048_MS = (1 << WDP2) | (1 << WDP1) | (1 << WDP0), /* 2048 ms. */
   WDT_TIMEOUT_4096_MS = (1 << WDP3),                             /* 4096 ms. */
   WDT_TIMEOUT_8192_MS = (1 << WDP3) | (1 << WDP0)                /* 8192 ms. */
};

/********************************************************************************
* wdt_reset: �terst�ller Watchdog-timern, vilket m�ste ske kontinuerligt innan
*            timern l�per ut f�r att undvika system�terst�llning eller avbrott.
*
*            1. Inaktiverar avbrott via assemblerinstruktionen CLI.
*            2. �terst�ller Watchdog-timern via assemblerinstruktionen WDR.
*            3. Nollst�ller Watchdog reset-flagga WDRF i MCUSR.
*            4. �teraktiverar avbrott efter �terst�llningen �r slutf�rd. 
********************************************************************************/
static inline void wdt_reset(void)
{
   asm("CLI");
   asm("WDR");
   MCUSR &= ~(1 << WDRF);
   asm("SEI");
   return;
}

/********************************************************************************
* wdt_init: Initierar Watchdog-timern med angiven timeout m�tt i millisekunder.
*
*           1. Inaktiverar avbrott under bytet (vi har en timad sekvens och
*              avbrott kan medf�ra att vi missar v�r deadline).
*           2. Startar den timade sekvensen s� att vi kan byta prescaler
*              genom att ettst�lla bitar WDCE (WDT Change Enable) samt WDE
*              (WDT System Reset Enable) i kontroll- och statusregistret
*              WDTCSR (WDT Control and Status Register). 
*           3. Inom fyra klockcykler, tilldela prescaler-bitarna via ing�ende
*              argument timeout_ms. Enumerationer �r av datatypen int, s� 
*              typomvandla timeout_ms till uint8 med en cast.
*           4. �teraktivera avbrott efter bytet.

*           - timeout_ms: Timeout m�tt i millisekunder.
********************************************************************************/
static inline void wdt_init(const enum wdt_timeout timeout_ms)
{
   asm("CLI");
   WDTCSR |= (1 << WDCE) | (1 << WDE);
   WDTCSR = (uint8_t)(timeout_ms);
   asm("SEI");
   return;
}

/********************************************************************************
* wdt_clear: Nollst�ller Watchdog-timern, vilket inneb�r att �terinitiering
*            m�ste ske (via anrop av funktionen wdt_init) vid senare anv�ndning.
*
*            1. �terst�ll Watchdog-timern s� att vi inte r�kar f� en timeout.
*            2. Inaktiverar avbrott under den timade sekvensen.
*            3. Startar den timade sekvensen f�r att st�nga av Watchdog-timern.
*            4. St�nger av Watchdog-timern genom att nollst�lla WDTCSR.
*            5. �teraktiverar avbrott efter att Watchdog-timern har inaktiverats.
********************************************************************************/
static inline void wdt_clear(void)
{
   wdt_reset();
   asm("CLI");
   WDTCSR |= (1 << WDCE) | (1 << WDE);
   WDTCSR = 0x00;
   asm("SEI");
   return;
}

/********************************************************************************
* wdt_enable_system_reset: Aktiverar Watchdog-timern i System Reset Mode,
*                          vilket inneb�r att systemet �terst�lls ifall
*                          Watchdog-timern l�per ut.
*
*                          1. �terst�ller Watchdog-timern (valfritt).
*                          2. Inaktiverar avbrott under timad sekvens.
*                          3. Startar timad sekvens s� att vi kan aktivera
*                             Watchdog System Reset inom fyra klockcykler.
*                          4. Vi aktiverar Watchdog System Reset genom
*                             ettst�llning av biten WDE i WDTCSR. Se till
*                             att inte r�ka nollst�lla prescaler-bitarna!
*                          5. �teraktiverar avbrott efter aktiveringen.
********************************************************************************/
static inline void wdt_enable_system_reset(void)
{
   wdt_reset();
   asm("CLI");
   WDTCSR |= (1 << WDCE) | (1 << WDE);
   WDTCSR |= (1 << WDE);
   asm("SEI");
   return;
}

/********************************************************************************
* wdt_disable_system_reset: Inaktiverar Watchdog-timern i System Reset Mode,
*                           vilket inneb�r att systemet inte �terst�lls ifall
*                           Watchdog-timern l�per ut.
* 
*                           1. �terst�ller Watchdog-timern (valfritt).
*                           2. Inaktiverar avbrott under den timade sekvensen.
*                           3. Startar den timade sekvensen.
*                           4. Inaktiverar System Reset genom att nollst�lla
*                              biten WDE (Watchdog System Reset Enable) i
*                              WDTCSR (m�ste g�ras inom fyra klockcykler).
*                           5. �teraktiverar avbrott efter att Watchdog
*                              System Reset har nollst�llts.
********************************************************************************/
static inline void wdt_disable_system_reset(void)
{
   wdt_reset();
   asm("CLI");
   WDTCSR |= (1 << WDCE) | (1 << WDE);
   WDTCSR &= ~(1 << WDE);
   asm("SEI");
   return;
}

/********************************************************************************
* wdt_enable_interrupt: Aktiverar Watchdog-timern i Interrupt Mode, vilket
*                       inneb�r att ett avbrott med avbrottsvektor WDT_vect
*                       �ger rum ifall Watchdog-timern l�per ut. 
*
*                       Notering: Vid WDT-avbrott m�ste denna funktion anropas
*                                 f�r att �teraktivera WDT-avbrott.
*
*                       1. �terst�ll Watchdog-timern (valfritt).
*                       2. Aktiverar Watchdog-avbrott genom att ettst�lla biten 
*                          WDIE (Watchdog Interrupt Enable) i WDTCSR. Ingen
*                          timad sekvens beh�vs, men se till att inte skriva
*                          �ver �vriga bitar i registret.
********************************************************************************/
static inline void wdt_enable_interrupt(void)
{
   wdt_reset();
   WDTCSR |= (1 << WDIE);
   return;
}

/********************************************************************************
* wdt_disable_interrupt: Inaktiverar Watchdog-timern i Interrupt Mode, vilket
*                        inneb�r att avbrott inte �ger rum ifall Watchdog-timern
*                        l�per ut.
*
*                        1. �terst�ll Watchdog-timern (valfritt).
*                        2. Inaktiverar Watchdog-avbrott genom att nollst�lla 
*                           biten WDIE (Watchdog Interrupt Enable) i WDTCSR. 
********************************************************************************/
static inline void wdt_disable_interrupt(void)
{
   wdt_reset();
   WDTCSR &= ~(1 << WDIE);
   return;
}

#endif /* WDT_H_ */