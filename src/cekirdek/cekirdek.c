/* cekirdek.c — alt sistem baslatma ve kooperatif ana dongu.
 * Kullaniciya dogrudan gorunmez; kabuk bir gorev olarak buraya baglanir.
 */
#include "cekirdek.h"
#include "bellek.h"
#include "olay.h"
#include "zamanlayici.h"
#include "../hal/hal.h"
#include "../tds/tds.h"
#include "../kabuk/kabuk.h"

void cekirdek_baslat(void) {
    hal_baslat();
    bellek_baslat();
    olay_baslat();
    zamanlayici_baslat();
    tds_baslat();
    if (!tds_yukle())   /* kalici imaji yukle; ilk acilista yoksa: */
        tds_tohum();    /* gomulu ornek uygulamalari tohumla (ve kalici yaz) */
    kabuk_baslat();
    /* Kabuk REPL'ini tek kooperatif gorev olarak kaydet. */
    gorev_ekle("kabuk", kabuk_gorev, NULL);
}

void cekirdek_dongu(void) {
    for (;;) {
        zamanlayici_tik();
        if (kabuk_cikis_istendi()) break;
    }
}
