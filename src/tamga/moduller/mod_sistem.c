/* mod_sistem.c — 'sistem', 'gpio' ve 'wifi' modulleri.
 * sistem.bellek()  -> kullanilan bayt
 * sistem.bellek_bos() -> bos bayt
 * sistem.sure()    -> aciliktan beri ms
 * gpio.yon(pin, cikis)/gpio.yaz(pin, seviye)/gpio.oku(pin)
 * wifi.tara()/wifi.baglan(ssid, parola)
 */
#include "moduller.h"
#include "../../cekirdek/syscall.h"
#include <string.h>
#include <stdio.h>

static int sayi_arg(Deger *arg, int n, int i) {
    if (i >= n) return 0;
    return (arg[i].tur == DEG_SAYI) ? (int)arg[i].sayi : 0;
}

bool modul_sistem(const char *ad, Deger *arg, int n, Deger *cikti) {
    (void)arg; (void)n;
    if (strcmp(ad, "bellek") == 0)      { *cikti = deger_sayi((double)sys_bellek_kullanilan()); return true; }
    if (strcmp(ad, "bellek_bos") == 0)  { *cikti = deger_sayi((double)(sys_bellek_toplam() - sys_bellek_kullanilan())); return true; }
    if (strcmp(ad, "bellek_toplam")==0) { *cikti = deger_sayi((double)sys_bellek_toplam()); return true; }
    if (strcmp(ad, "sure") == 0)        { *cikti = deger_sayi((double)sys_zaman_ms()); return true; }
    return false;
}

bool modul_gpio(const char *ad, Deger *arg, int n, Deger *cikti) {
    if (strcmp(ad, "yon") == 0) { sys_gpio_yon((uint8_t)sayi_arg(arg,n,0), sayi_arg(arg,n,1)); *cikti = deger_bos(); return true; }
    if (strcmp(ad, "yaz") == 0) { sys_gpio_yaz((uint8_t)sayi_arg(arg,n,0), sayi_arg(arg,n,1)); *cikti = deger_bos(); return true; }
    if (strcmp(ad, "oku") == 0) { *cikti = deger_sayi(sys_gpio_oku((uint8_t)sayi_arg(arg,n,0))); return true; }
    return false;
}

bool modul_wifi(const char *ad, Deger *arg, int n, Deger *cikti) {
    if (strcmp(ad, "tara") == 0) {
        char tampon[256];
        int sayi = sys_wifi_tara(tampon, sizeof tampon);
        sys_yaz("Bulunan aglar:\n");
        sys_yaz(tampon);
        *cikti = deger_sayi(sayi);
        return true;
    }
    if (strcmp(ad, "baglan") == 0) {
        const char *ssid = (n > 0 && arg[0].tur == DEG_METIN) ? arg[0].metin : "";
        const char *parola = (n > 1 && arg[1].tur == DEG_METIN) ? arg[1].metin : "";
        int r = sys_wifi_baglan(ssid, parola);
        *cikti = deger_mantik(r == 0);
        return true;
    }
    return false;
}
