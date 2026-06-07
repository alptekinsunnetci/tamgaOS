/* mod_zaman.c — 'zaman' modulu.
 * zaman.simdi()    -> aciliktan beri ms
 * zaman.bekle(ms)  -> ms kadar bekle
 */
#include "moduller.h"
#include "../../cekirdek/syscall.h"
#include <string.h>

bool modul_zaman(const char *ad, Deger *arg, int n, Deger *cikti) {
    if (strcmp(ad, "simdi") == 0) {
        *cikti = deger_sayi((double)sys_zaman_ms());
        return true;
    }
    if (strcmp(ad, "bekle") == 0) {
        uint32_t ms = (n > 0 && arg[0].tur == DEG_SAYI) ? (uint32_t)arg[0].sayi : 0;
        sys_bekle_ms(ms);
        *cikti = deger_bos();
        return true;
    }
    return false;
}
