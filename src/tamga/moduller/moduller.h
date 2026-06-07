/* moduller.h — Tamga yerlesik sistem modulleri.
 * Her modul, 'modul.metot(arg...)' cagrilarini isler. Taninan cagrida 'cikti'yi
 * doldurur ve true doner; modul/metot taninmazsa false (yorumlayici hata verir).
 */
#ifndef TAMGA_MODULLER_H
#define TAMGA_MODULLER_H

#include "../deger.h"

bool modul_sistem(const char *ad, Deger *arg, int n, Deger *cikti);
bool modul_dosya (const char *ad, Deger *arg, int n, Deger *cikti);
bool modul_zaman (const char *ad, Deger *arg, int n, Deger *cikti);
bool modul_gpio  (const char *ad, Deger *arg, int n, Deger *cikti);
bool modul_wifi  (const char *ad, Deger *arg, int n, Deger *cikti);

/* Modul adina gore dogru isleyiciye yonlendirir. */
bool modul_cagir(const char *modul, const char *ad, Deger *arg, int n, Deger *cikti);

#endif /* TAMGA_MODULLER_H */
