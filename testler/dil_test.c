/* dil_test.c — Tamga ayristirici + yorumlayici entegrasyon testleri.
 *
 * Programlar calistirilir, sonuc bir degiskene atanir ve kuresel kapsamdan
 * okunarak dogrulanir (boylece konsol ciktisi gerekmez).
 *
 * Derleme icin gereken kaynaklar Makefile 'diltest' hedefindedir.
 */
#include "../src/tamga/tamga.h"
#include "../src/tamga/ortam.h"
#include "../src/cekirdek/bellek.h"
#include "../src/tds/tds.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static int gecti = 0, kaldi = 0;

static Deger calistir_var(const char *kaynak, const char *ad) {
    bellek_baslat();   /* her test bagimsiz: arenayi sifirla (AST birikmesini onle) */
    Yorumlayici y;
    yorumlayici_baslat(&y);
    tamga_calistir_ortamda(&y, kaynak, false);
    Deger v;
    if (!ortam_al(&y.kuresel, ad, &v)) v = deger_bos();
    return v;
}

static void sayi_bekle(const char *ad, const char *kaynak, double beklenen) {
    Deger v = calistir_var(kaynak, "x");
    if (v.tur == DEG_SAYI && fabs(v.sayi - beklenen) < 1e-9) {
        printf("  + %s\n", ad); gecti++;
    } else {
        printf("  X %s: tur=%d deger=%.4f (beklenen %.4f)\n", ad, v.tur, v.sayi, beklenen);
        kaldi++;
    }
}

static void metin_bekle(const char *ad, const char *kaynak, const char *beklenen) {
    Deger v = calistir_var(kaynak, "x");
    if (v.tur == DEG_METIN && strcmp(v.metin, beklenen) == 0) {
        printf("  + %s\n", ad); gecti++;
    } else {
        printf("  X %s: tur=%d '%s' (beklenen '%s')\n", ad, v.tur, v.metin, beklenen);
        kaldi++;
    }
}

static void mantik_bekle(const char *ad, const char *kaynak, bool beklenen) {
    Deger v = calistir_var(kaynak, "x");
    if (v.tur == DEG_MANTIK && v.mantik == beklenen) {
        printf("  + %s\n", ad); gecti++;
    } else {
        printf("  X %s: tur=%d (mantik=%d beklenen %d)\n", ad, v.tur, v.mantik, beklenen);
        kaldi++;
    }
}

static void hata_bekle(const char *ad, const char *kaynak) {
    bellek_baslat();
    Yorumlayici y;
    yorumlayici_baslat(&y);
    bool ok = tamga_calistir_ortamda(&y, kaynak, false);
    if (!ok) { printf("  + %s\n", ad); gecti++; }
    else     { printf("  X %s: hata bekleniyordu ama basarili oldu\n", ad); kaldi++; }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);   /* cokme durumunda son satiri gor */
    bellek_baslat();
    tds_baslat();

    printf("Tamga dil (ayristirici + yorumlayici) testleri:\n");

    /* aritmetik ve oncelik */
    sayi_bekle("oncelik: 2+3*4",        "x = 2 + 3 * 4\n", 14);
    sayi_bekle("parantez: (2+3)*4",     "x = (2 + 3) * 4\n", 20);
    sayi_bekle("cikarma/bolme",         "x = 20 / 4 - 1\n", 4);
    sayi_bekle("mod",                   "x = 17 % 5\n", 2);
    sayi_bekle("birli eksi",            "x = -5 + 8\n", 3);
    sayi_bekle("ondalik",               "x = 1.5 * 2\n", 3.0);

    /* metin */
    metin_bekle("metin birlestirme",    "x = \"a\" + \"b\"\n", "ab");
    metin_bekle("metin + sayi",         "x = \"deger: \" + 42\n", "deger: 42");

    /* mantik / karsilastirma */
    mantik_bekle("karsilastirma <",     "x = 3 < 5\n", true);
    mantik_bekle("esitlik ==",          "x = 5 == 5\n", true);
    mantik_bekle("ve kisa devre",       "x = 3 < 5 ve 5 == 5\n", true);
    mantik_bekle("veya",                "x = 1 > 2 veya 2 > 1\n", true);
    mantik_bekle("degil",               "x = değil yanlış\n", true);

    /* kontrol akisi */
    sayi_bekle("eğer-ise (dogru dal)",
        "eğer 5 > 3 ise\n  x = 1\ndeğilse\n  x = 2\nbitir\n", 1);
    sayi_bekle("eğer-değilse (yanlis dal)",
        "eğer 1 > 3 ise\n  x = 1\ndeğilse\n  x = 2\nbitir\n", 2);
    sayi_bekle("iken dongusu (toplam 0..4)",
        "x = 0\ni = 0\niken i < 5\n  x = x + i\n  i = i + 1\nbitir\n", 10);

    /* islev */
    sayi_bekle("islev cagrisi",
        "işlev topla(a, b)\n  döndür a + b\nbitir\nx = topla(10, 20)\n", 30);
    sayi_bekle("islev ileri bildirim",
        "x = kare(6)\nişlev kare(n)\n  döndür n * n\nbitir\n", 36);
    sayi_bekle("ozyineli faktoriyel",
        "işlev fak(n)\n  eğer n <= 1 ise\n    döndür 1\n  bitir\n"
        "  döndür n * fak(n - 1)\nbitir\nx = fak(5)\n", 120);

    /* moduller */
    sayi_bekle("sistem.bellek_toplam",  "x = sistem.bellek_toplam()\n", 98304);
    metin_bekle("dosya yaz/oku",
        "dosya.yaz(\"deneme.txt\", \"selam\")\nx = dosya.oku(\"deneme.txt\")\n", "selam");

    /* diziler */
    sayi_bekle("dizi indeksleme",       "d = [10, 20, 30]\nx = d[1]\n", 20);
    sayi_bekle("dizi uzunluk",          "d = [1, 2, 3, 4]\nx = uzunluk(d)\n", 4);
    sayi_bekle("dizi oge atama",        "d = [1, 2, 3]\nd[0] = 99\nx = d[0]\n", 99);
    sayi_bekle("dizi ekle",             "d = [1]\nekle(d, 2)\nekle(d, 3)\nx = uzunluk(d)\n", 3);
    sayi_bekle("dizi cikar",            "d = [5, 6, 7]\nx = çıkar(d)\n", 7);
    sayi_bekle("bos dizi + ekle",       "d = dizi()\nekle(d, 42)\nx = d[0]\n", 42);
    sayi_bekle("dizi ile dongu toplami",
        "d = [3, 5, 7]\nx = 0\ni = 0\niken i < uzunluk(d)\n  x = x + d[i]\n  i = i + 1\nbitir\n", 15);
    sayi_bekle("ic ice dizi",           "d = [[1, 2], [3, 4]]\nx = d[1][0]\n", 3);
    sayi_bekle("dizi referans semantigi",
        "işlev doldur(z)\n  ekle(z, 9)\nbitir\nd = [1]\ndoldur(d)\nx = uzunluk(d)\n", 2);
    metin_bekle("dizi metin gosterimi", "d = [1, 2, 3]\nx = \"\" + d\n", "[1, 2, 3]");

    /* sozlukler */
    metin_bekle("sözlük erişim",        "s = {\"ad\": \"Ali\"}\nx = s[\"ad\"]\n", "Ali");
    sayi_bekle("sözlük sayı değer",     "s = {\"yas\": 30}\nx = s[\"yas\"]\n", 30);
    sayi_bekle("sözlük atama",          "s = {\"a\": 1}\ns[\"a\"] = 9\nx = s[\"a\"]\n", 9);
    sayi_bekle("sözlük yeni anahtar",   "s = sözlük()\ns[\"k\"] = 5\nx = s[\"k\"]\n", 5);
    sayi_bekle("sözlük uzunluk",        "s = {\"a\": 1, \"b\": 2}\nx = uzunluk(s)\n", 2);
    mantik_bekle("var_mı doğru",        "s = {\"a\": 1}\nx = var_mı(s, \"a\")\n", true);
    mantik_bekle("var_mı yanlış",       "s = {\"a\": 1}\nx = var_mı(s, \"yok\")\n", false);
    sayi_bekle("sil_anahtar",           "s = {\"a\": 1, \"b\": 2}\nsil_anahtar(s, \"a\")\nx = uzunluk(s)\n", 1);
    sayi_bekle("anahtarlar dizi",       "s = {\"a\": 1, \"b\": 2}\nk = anahtarlar(s)\nx = uzunluk(k)\n", 2);
    metin_bekle("anahtarlar ilk",       "s = {\"x\": 1}\nk = anahtarlar(s)\nx = k[0]\n", "x");
    metin_bekle("eksik anahtar boş",    "s = {\"a\": 1}\nd = s[\"yok\"]\nx = \"\" + d\n", "boş");
    sayi_bekle("iç içe sözlük+dizi",    "v = {\"liste\": [10, 20]}\nx = v[\"liste\"][1]\n", 20);

    /* her (for-each) dongusu */
    sayi_bekle("her dizi toplam",
        "x = 0\nher e içinde [10, 20, 30]\n  x = x + e\nbitir\n", 60);
    sayi_bekle("her sözlük anahtar sayısı",
        "s = {\"a\": 1, \"b\": 2, \"c\": 3}\nx = 0\nher k içinde s\n  x = x + 1\nbitir\n", 3);
    metin_bekle("her sözlük değer toplama",
        "s = {\"x\": \"A\", \"y\": \"B\"}\nx = \"\"\nher k içinde s\n  x = x + s[k]\nbitir\n", "AB");
    sayi_bekle("her boş dizi",
        "x = 5\nher e içinde []\n  x = x + 1\nbitir\n", 5);

    /* donusum yerlesikleri */
    sayi_bekle("sayı(metin)",           "x = sayı(\"42\") + 1\n", 43);
    sayi_bekle("sayı(ondalik metin)",   "x = sayı(\"3.5\") * 2\n", 7);
    metin_bekle("metin(sayı)",          "x = metin(7) + \"!\"\n", "7!");

    /* dur / devam (dongu kontrolu) */
    sayi_bekle("iken dur",
        "x = 0\ni = 0\niken i < 100\n  eğer i == 5 ise\n    dur\n  bitir\n  x = i\n  i = i + 1\nbitir\n", 4);
    sayi_bekle("iken devam (tek toplami)",
        "x = 0\ni = 0\niken i < 6\n  i = i + 1\n  eğer i % 2 == 0 ise\n    devam\n  bitir\n  x = x + i\nbitir\n", 9);
    sayi_bekle("her dur",
        "x = 0\nher e içinde [1, 2, 3, 4, 5]\n  eğer e == 3 ise\n    dur\n  bitir\n  x = x + e\nbitir\n", 3);
    sayi_bekle("her devam",
        "x = 0\nher e içinde [1, 2, 3, 4]\n  eğer e == 2 ise\n    devam\n  bitir\n  x = x + e\nbitir\n", 8);
    sayi_bekle("dur islev sinirini asmaz",
        "işlev f()\n  döndür 7\nbitir\nx = 0\nher e içinde [1, 2]\n  x = x + f()\nbitir\n", 14);

    /* metin yerlesikleri */
    metin_bekle("parça",            "x = parça(\"merhaba\", 0, 3)\n", "mer");
    metin_bekle("parça uzunluksuz",  "x = parça(\"merhaba\", 3)\n", "haba");
    sayi_bekle("bul bulunan",        "x = bul(\"merhaba\", \"hab\")\n", 3);
    sayi_bekle("bul yok",            "x = bul(\"merhaba\", \"xyz\")\n", -1);
    mantik_bekle("içerir",           "x = içerir(\"merhaba\", \"rha\")\n", true);
    mantik_bekle("başlar",           "x = başlar(\"merhaba\", \"mer\")\n", true);
    mantik_bekle("biter",            "x = biter(\"merhaba\", \"aba\")\n", true);
    metin_bekle("kırp",              "x = kırp(\"  selam  \")\n", "selam");
    metin_bekle("değiştir",          "x = değiştir(\"a-b-c\", \"-\", \"+\")\n", "a+b+c");
    sayi_bekle("parçala sayısı",     "p = parçala(\"a,b,c\", \",\")\nx = uzunluk(p)\n", 3);
    metin_bekle("parçala öğe",       "p = parçala(\"a,b,c\", \",\")\nx = p[1]\n", "b");
    metin_bekle("birleştir",         "x = birleştir([\"a\", \"b\", \"c\"], \"-\")\n", "a-b-c");
    metin_bekle("parçala+birleştir", "x = birleştir(parçala(\"x.y.z\", \".\"), \"/\")\n", "x/y/z");

    /* hatalar */
    hata_bekle("tanimsiz degisken",     "x = bilinmeyen + 1\n");
    hata_bekle("tanimsiz islev",        "x = yokislev(1)\n");
    hata_bekle("sozdizimi hatasi",      "eğer 5 > 3\n  x = 1\nbitir\n");  /* 'ise' eksik */
    hata_bekle("sifira bolme",          "x = 5 / 0\n");

    printf("\nSonuc: %d gecti, %d kaldi\n", gecti, kaldi);
    return kaldi == 0 ? 0 : 1;
}
