/* tamga.h — Tamga dili dis cephesi (kaynak metni -> calistir). */
#ifndef TAMGA_TAMGA_H
#define TAMGA_TAMGA_H

#include "yorumlayici.h"
#include <stdbool.h>

/* Kaynagi var olan bir yorumlayici ortaminda calistir (REPL: degiskenler kalir).
 * repl_yaz true ise tek-ifade satirlarinin sonucu otomatik yazdirilir.
 * Hata olursa false ve y->mesaj doldurulur. */
bool tamga_calistir_ortamda(Yorumlayici *y, const char *kaynak, bool repl_yaz);

/* Kaynagi temiz bir yorumlayici ile calistir (uygulama calistirma).
 * Hata mesaji 'hata' tamponuna yazilir (NULL olabilir). */
bool tamga_calistir(const char *kaynak, char *hata, int boyut);

#endif /* TAMGA_TAMGA_H */
