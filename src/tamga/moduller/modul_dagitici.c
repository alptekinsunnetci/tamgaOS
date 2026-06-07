/* modul_dagitici.c — modul adina gore dogru isleyiciye yonlendirme. */
#include "moduller.h"
#include <string.h>

bool modul_cagir(const char *modul, const char *ad, Deger *arg, int n, Deger *cikti) {
    if (strcmp(modul, "sistem") == 0) return modul_sistem(ad, arg, n, cikti);
    if (strcmp(modul, "dosya")  == 0) return modul_dosya (ad, arg, n, cikti);
    if (strcmp(modul, "zaman")  == 0) return modul_zaman (ad, arg, n, cikti);
    if (strcmp(modul, "gpio")   == 0) return modul_gpio  (ad, arg, n, cikti);
    if (strcmp(modul, "wifi")   == 0) return modul_wifi  (ad, arg, n, cikti);
    /* 'ağ' takma adi -> wifi */
    if (strcmp(modul, "ağ")     == 0) return modul_wifi  (ad, arg, n, cikti);
    return false;
}
