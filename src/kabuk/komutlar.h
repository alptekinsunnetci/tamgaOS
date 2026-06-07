/* komutlar.h — kabuk yerlesik komutlari. */
#ifndef TAMGA_KOMUTLAR_H
#define TAMGA_KOMUTLAR_H

#include <stdbool.h>

/* argv[0] komut adi. Taninan komut ise isler ve 1 doner; aksi halde 0
 * (kabuk satiri Tamga yorumlayicisina gonderir). cwd kabugun calisma
 * dizini (komutlar degistirebilir). cikis true yapilirsa kabuk sonlanir. */
int komut_calistir(int argc, char **argv, char *cwd, bool *cikis);

#endif /* TAMGA_KOMUTLAR_H */
