/* defter.h — Tamga Defteri: satir tabanli metin editoru (ed/nano benzeri).
 * Seri konsol ve pipe uzerinde calisir (tam-ekran TUI degil). TDS'e kaydeder.
 */
#ifndef TAMGA_DEFTER_H
#define TAMGA_DEFTER_H

/* Verilen TDS yolundaki dosyayi duzenle (yoksa yeni olusturulur). */
void defter_calistir(const char *yol);

#endif /* TAMGA_DEFTER_H */
