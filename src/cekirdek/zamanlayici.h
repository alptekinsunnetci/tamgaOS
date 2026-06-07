/* zamanlayici.h — kooperatif gorev zamanlayici.
 * Sabit gorev tablosu; her tik'te hazir gorevler sirayla cagrilir.
 * Milestone 1'de kabuk tek gorev olarak kosar; cerceve ileri milestone'lar icin.
 */
#ifndef TAMGA_ZAMANLAYICI_H
#define TAMGA_ZAMANLAYICI_H

#include <stdbool.h>

typedef void (*GorevFn)(void *baglam);

int  gorev_ekle(const char *ad, GorevFn fn, void *baglam); /* indeks veya -1 */
void gorev_durdur(int indeks);
void zamanlayici_baslat(void);
void zamanlayici_tik(void);     /* tum hazir gorevleri bir kez calistir */
int  gorev_sayisi(void);

#endif /* TAMGA_ZAMANLAYICI_H */
