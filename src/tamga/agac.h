/* agac.h — Tamga soyut sozdizim agaci (AST).
 *
 * Tek bir "sisman" Dugum yapisi kullanilir (gomulu icin basit, az tip).
 * Deyimler (statement) bir blok icinde 'sonraki' ile tek yonlu zincir olur.
 */
#ifndef TAMGA_AGAC_H
#define TAMGA_AGAC_H

#include "belirtec.h"

typedef enum {
    D_SAYI,       /* sayi sabiti        -> sayi */
    D_METIN,      /* metin sabiti       -> metin */
    D_MANTIK,     /* doğru/yanlış       -> mantik */
    D_BOS,        /* boş */
    D_DEGISKEN,   /* degisken erisimi   -> metin (ad) */
    D_DIZI,       /* dizi literali [..] -> argumanlar (oge ifadeleri) */
    D_SOZLUK,     /* sözlük literali {..} -> argumanlar (anahtar,deger sirayla) */
    D_INDEKS,     /* nesne[indeks]      -> sol (nesne), sag (indeks/anahtar) */
    D_IKILI,      /* ikili islem        -> sol, op, sag */
    D_BIRLI,      /* birli islem        -> op, sag */
    D_ATAMA,      /* hedef = ifade      -> sol (lvalue: D_DEGISKEN/D_INDEKS), sag (deger) */
    D_YAZ,        /* yaz ifade          -> sol (ifade) */
    D_EGER,       /* eğer..ise..bitir   -> sol (kosul), govde, govde2 (degilse) */
    D_IKEN,       /* iken..bitir        -> sol (kosul), govde */
    D_HER,        /* her <ad> içinde..  -> metin (ad), sol (koleksiyon), govde */
    D_ISLEV,      /* işlev tanimi       -> metin (ad), parametreler, govde */
    D_DONDUR,     /* döndür ifade       -> sol (ifade, NULL olabilir) */
    D_DUR,        /* dur (break) */
    D_DEVAM,      /* devam (continue) */
    D_CAGRI       /* cagri              -> metin (ad), modul, argumanlar */
} DugumTuru;

typedef struct Dugum {
    DugumTuru tur;

    double         sayi;                          /* D_SAYI */
    int            mantik;                         /* D_MANTIK */
    char           metin[BELIRTEC_AZAMI_UZUNLUK]; /* metin sabiti / ad / uye */
    char           modul[BELIRTEC_AZAMI_UZUNLUK]; /* D_CAGRI modul adi ("" = yok) */
    BelirtecTuru   op;                            /* D_IKILI / D_BIRLI */

    struct Dugum  *sol;          /* genel sol/operand/ifade/kosul */
    struct Dugum  *sag;          /* ikili sag */
    struct Dugum  *govde;        /* eger-then / iken / islev govde (deyim zinciri) */
    struct Dugum  *govde2;       /* eger-else deyim zinciri */
    struct Dugum  *parametreler; /* islev parametreleri (D_DEGISKEN zinciri) */
    struct Dugum  *argumanlar;   /* cagri argumanlari (ifade zinciri) */

    struct Dugum  *sonraki;      /* blok icinde sonraki deyim */
    int            satir;
} Dugum;

Dugum *agac_dugum(DugumTuru tur);  /* sifirlanmis dugum ayir (cekirdek bellek) */
void   agac_serbest(Dugum *d);     /* dugumu ve cocuklarini ozyineli serbest birak */

#endif /* TAMGA_AGAC_H */
