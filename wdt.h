/********************************************************************************
* wdt.h: Innehåller drivrutiner för mikrokontrollerns Watchdog-timer, både i 
*        System Reset Mode, vilket innebär att mikrodatorn återställs vid 
*        timeout, samt i Interrupt Mode, vilket innebär avbrott vid timeout.
*        Det är också möjligt att kombinera tidigare nämnda modes för att
*        avbrott sker, följt av systemåterställning.
*
*        Avbrottsvektorn för timeout-avbrott är WDT_vect. Timeout-avbrott
*        måste återaktiveras i motsvarande avbrottsrutin, vilket genomförs
*        via anrop av funktionen wdt_enable_interrupt.
********************************************************************************/
#ifndef WDT_H_
#define WDT_H_

/* Inkluderingsdirektiv: */
#include "misc.h"

/********************************************************************************
* wdt_timeout: Enumeration för val av timeout för mikrodatorns Watchdog-timer.
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
* wdt_reset: Återställer Watchdog-timern, vilket måste ske kontinuerligt innan
*            timern löper ut för att undvika systemåterställning eller avbrott.
*
*            1. Inaktiverar avbrott via assemblerinstruktionen CLI.
*            2. Återställer Watchdog-timern via assemblerinstruktionen WDR.
*            3. Nollställer Watchdog reset-flagga WDRF i MCUSR.
*            4. Återaktiverar avbrott efter återställningen är slutförd. 
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
* wdt_init: Initierar Watchdog-timern med angiven timeout mätt i millisekunder.
*
*           1. Inaktiverar avbrott under bytet (vi har en timad sekvens och
*              avbrott kan medföra att vi missar vår deadline).
*           2. Startar den timade sekvensen så att vi kan byta prescaler
*              genom att ettställa bitar WDCE (WDT Change Enable) samt WDE
*              (WDT System Reset Enable) i kontroll- och statusregistret
*              WDTCSR (WDT Control and Status Register). 
*           3. Inom fyra klockcykler, tilldela prescaler-bitarna via ingående
*              argument timeout_ms. Enumerationer är av datatypen int, så 
*              typomvandla timeout_ms till uint8 med en cast.
*           4. Återaktivera avbrott efter bytet.

*           - timeout_ms: Timeout mätt i millisekunder.
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
* wdt_clear: Nollställer Watchdog-timern, vilket innebär att återinitiering
*            måste ske (via anrop av funktionen wdt_init) vid senare användning.
*
*            1. Återställ Watchdog-timern så att vi inte råkar få en timeout.
*            2. Inaktiverar avbrott under den timade sekvensen.
*            3. Startar den timade sekvensen för att stänga av Watchdog-timern.
*            4. Stänger av Watchdog-timern genom att nollställa WDTCSR.
*            5. Återaktiverar avbrott efter att Watchdog-timern har inaktiverats.
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
*                          vilket innebär att systemet återställs ifall
*                          Watchdog-timern löper ut.
*
*                          1. Återställer Watchdog-timern (valfritt).
*                          2. Inaktiverar avbrott under timad sekvens.
*                          3. Startar timad sekvens så att vi kan aktivera
*                             Watchdog System Reset inom fyra klockcykler.
*                          4. Vi aktiverar Watchdog System Reset genom
*                             ettställning av biten WDE i WDTCSR. Se till
*                             att inte råka nollställa prescaler-bitarna!
*                          5. Återaktiverar avbrott efter aktiveringen.
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
*                           vilket innebär att systemet inte återställs ifall
*                           Watchdog-timern löper ut.
* 
*                           1. Återställer Watchdog-timern (valfritt).
*                           2. Inaktiverar avbrott under den timade sekvensen.
*                           3. Startar den timade sekvensen.
*                           4. Inaktiverar System Reset genom att nollställa
*                              biten WDE (Watchdog System Reset Enable) i
*                              WDTCSR (måste göras inom fyra klockcykler).
*                           5. Återaktiverar avbrott efter att Watchdog
*                              System Reset har nollställts.
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
*                       innebär att ett avbrott med avbrottsvektor WDT_vect
*                       äger rum ifall Watchdog-timern löper ut. 
*
*                       Notering: Vid WDT-avbrott måste denna funktion anropas
*                                 för att återaktivera WDT-avbrott.
*
*                       1. Återställ Watchdog-timern (valfritt).
*                       2. Aktiverar Watchdog-avbrott genom att ettställa biten 
*                          WDIE (Watchdog Interrupt Enable) i WDTCSR. Ingen
*                          timad sekvens behövs, men se till att inte skriva
*                          över övriga bitar i registret.
********************************************************************************/
static inline void wdt_enable_interrupt(void)
{
   wdt_reset();
   WDTCSR |= (1 << WDIE);
   return;
}

/********************************************************************************
* wdt_disable_interrupt: Inaktiverar Watchdog-timern i Interrupt Mode, vilket
*                        innebär att avbrott inte äger rum ifall Watchdog-timern
*                        löper ut.
*
*                        1. Återställ Watchdog-timern (valfritt).
*                        2. Inaktiverar Watchdog-avbrott genom att nollställa 
*                           biten WDIE (Watchdog Interrupt Enable) i WDTCSR. 
********************************************************************************/
static inline void wdt_disable_interrupt(void)
{
   wdt_reset();
   WDTCSR &= ~(1 << WDIE);
   return;
}

#endif /* WDT_H_ */