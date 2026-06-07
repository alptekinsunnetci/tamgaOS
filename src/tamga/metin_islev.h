/* metin_islev.h — Tamga metin (string) yerlesik fonksiyonlari.
 * yerlesik_cagir bunu cagirir; taninan cagrida cikti'yi doldurup true doner.
 * Aramalar/bolmeler byte-tabanlidir (UTF-8 alt dizgilerle uyumlu); parça()
 * byte indeksleri kullanir.
 */
#ifndef TAMGA_METIN_ISLEV_H
#define TAMGA_METIN_ISLEV_H

#include "deger.h"

bool metin_yerlesik(const char *ad, Deger *arg, int n, Deger *cikti);

#endif /* TAMGA_METIN_ISLEV_H */
