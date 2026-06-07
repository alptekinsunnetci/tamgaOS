/* tarayici_test.c — Tamga lexer birim testleri.
 * Derle:  gcc -I src testler/tarayici_test.c src/tamga/tarayici.c -o testler_tarayici
 * Calistir: ./testler_tarayici
 */
#include "../src/tamga/tarayici.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static int gecti = 0, kaldi = 0;

static void bekle(const char *kaynak, const BelirtecTuru *beklenen, int n, const char *ad) {
    Tarayici t;
    tarayici_baslat(&t, kaynak);
    int ok = 1;
    for (int i = 0; i < n; i++) {
        Belirtec b = tarayici_sonraki(&t);
        if (b.tur != beklenen[i]) {
            printf("  X %s: belirtec %d = %s, beklenen %s\n",
                   ad, i, belirtec_adi(b.tur), belirtec_adi(beklenen[i]));
            ok = 0;
            break;
        }
    }
    if (ok) { printf("  + %s\n", ad); gecti++; } else kaldi++;
}

int main(void) {
    printf("Tamga lexer testleri:\n");

    /* yaz "Merhaba" */
    bekle("yaz \"Merhaba\"",
          (BelirtecTuru[]){ B_YAZ, B_METIN, B_DOSYASONU }, 3, "yaz + metin");

    /* atama + aritmetik */
    bekle("sayi = 10 + 5 * 2",
          (BelirtecTuru[]){ B_TANIMLAYICI, B_ATAMA, B_SAYI, B_ARTI, B_SAYI,
                            B_CARPI, B_SAYI, B_DOSYASONU }, 8, "atama + aritmetik");

    /* eger / ise / bitir + karsilastirma (Turkce anahtarlar) */
    bekle("eğer sayi > 5 ise\nyaz \"Buyuk\"\nbitir",
          (BelirtecTuru[]){ B_EGER, B_TANIMLAYICI, B_BUYUK, B_SAYI, B_ISE,
                            B_SATIRSONU, B_YAZ, B_METIN, B_SATIRSONU, B_BITIR,
                            B_DOSYASONU }, 11, "eger/ise/bitir");

    /* iken dongusu */
    bekle("iken i < 5\nbitir",
          (BelirtecTuru[]){ B_IKEN, B_TANIMLAYICI, B_KUCUK, B_SAYI, B_SATIRSONU,
                            B_BITIR, B_DOSYASONU }, 7, "iken");

    /* islev / dondur */
    bekle("işlev topla(a, b)\ndöndür a + b\nbitir",
          (BelirtecTuru[]){ B_ISLEV, B_TANIMLAYICI, B_PARANTEZ_AC, B_TANIMLAYICI,
                            B_VIRGUL, B_TANIMLAYICI, B_PARANTEZ_KAPA, B_SATIRSONU,
                            B_DONDUR, B_TANIMLAYICI, B_ARTI, B_TANIMLAYICI,
                            B_SATIRSONU, B_BITIR, B_DOSYASONU }, 15, "islev/dondur");

    /* modul.metot cagrisi: dosya.yaz(...) — 'yaz' anahtar kelimedir; metin
       korundugundan parser bunu '.'dan sonra uye adi olarak kullanir. */
    bekle("dosya.yaz(\"a\", \"b\")",
          (BelirtecTuru[]){ B_TANIMLAYICI, B_NOKTA, B_YAZ, B_PARANTEZ_AC,
                            B_METIN, B_VIRGUL, B_METIN, B_PARANTEZ_KAPA,
                            B_DOSYASONU }, 9, "modul.metot");

    /* keyword tokeni metnini korur (uye adi cozumu icin) */
    {
        Tarayici t; tarayici_baslat(&t, "yaz");
        Belirtec b = tarayici_sonraki(&t);
        if (b.tur == B_YAZ && strcmp(b.metin, "yaz") == 0) {
            printf("  + anahtar kelime metni korunur\n"); gecti++;
        } else { printf("  X anahtar kelime metni ('%s')\n", b.metin); kaldi++; }
    }

    /* operatorler: == != <= >= ve veya değil */
    bekle("a == b != c <= d >= e ve f veya g değil",
          (BelirtecTuru[]){ B_TANIMLAYICI, B_ESIT, B_TANIMLAYICI, B_ESITDEGIL,
                            B_TANIMLAYICI, B_KUCUKESIT, B_TANIMLAYICI, B_BUYUKESIT,
                            B_TANIMLAYICI, B_VE, B_TANIMLAYICI, B_VEYA,
                            B_TANIMLAYICI, B_DEGIL, B_DOSYASONU }, 15, "operatorler");

    /* doğru / yanlış / boş + yorum atlama */
    bekle("doğru yanlış boş # yorum satiri\n",
          (BelirtecTuru[]){ B_DOGRU, B_YANLIS, B_BOS, B_SATIRSONU,
                            B_DOSYASONU }, 5, "sabitler + yorum");

    /* Sayi degeri ondalik */
    {
        Tarayici t; tarayici_baslat(&t, "3.14");
        Belirtec b = tarayici_sonraki(&t);
        if (b.tur == B_SAYI && fabs(b.sayi - 3.14) < 1e-9) {
            printf("  + ondalik sayi degeri\n"); gecti++;
        } else { printf("  X ondalik sayi degeri (%.4f)\n", b.sayi); kaldi++; }
    }

    /* Metin icerigi dogru cozuluyor mu */
    {
        Tarayici t; tarayici_baslat(&t, "\"satir\\nsonu\"");
        Belirtec b = tarayici_sonraki(&t);
        if (b.tur == B_METIN && strcmp(b.metin, "satir\nsonu") == 0) {
            printf("  + metin kacis cozumu\n"); gecti++;
        } else { printf("  X metin kacis cozumu ('%s')\n", b.metin); kaldi++; }
    }

    printf("\nSonuc: %d gecti, %d kaldi\n", gecti, kaldi);
    return kaldi == 0 ? 0 : 1;
}
