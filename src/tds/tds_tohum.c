/* tds_tohum.c — gomulu ornek uygulamalari TDS'e yukler.
 *
 * Host'ta '.tam' dosyalari uygulamalar/ klasorunden de okunabilir; ancak Pico
 * W'de host dosya sistemi YOKTUR. Bu dosya, ornek uygulamalari calisma aninda
 * /uygulamalar altina yazar ki 'listele /uygulamalar' ve 'çalıştır' her iki
 * platformda da calissin. (Kalici littlefs gelince bu tohumlama opsiyonel olur.)
 */
#include "tds.h"
#include <string.h>

static const char MERHABA[] =
    "yaz \"Merhaba Dünya\"\n"
    "sayi = 10\n"
    "eğer sayi > 5 ise\n"
    "    yaz \"Büyük\"\n"
    "bitir\n"
    "i = 0\n"
    "iken i < 5\n"
    "    yaz i\n"
    "    i = i + 1\n"
    "bitir\n"
    "işlev topla(a, b)\n"
    "    döndür a + b\n"
    "bitir\n"
    "toplam = topla(10, 20)\n"
    "yaz \"toplam = \" + toplam\n";

static const char BILGI[] =
    "yaz \"=== Sistem Bilgisi ===\"\n"
    "yaz \"Bellek: \" + sistem.bellek()\n"
    "yaz \"Süre  : \" + sistem.sure() + \" ms\"\n";

static const char SELAM[] =
    "yaz \"Tamga OS'a hoş geldiniz!\"\n";

void tds_tohum(void) {
    tds_yaz("/uygulamalar/merhaba.tam", MERHABA, sizeof(MERHABA) - 1);
    tds_yaz("/uygulamalar/bilgi.tam",   BILGI,   sizeof(BILGI)   - 1);
    tds_yaz("/uygulamalar/selam.tam",   SELAM,   sizeof(SELAM)   - 1);
}
