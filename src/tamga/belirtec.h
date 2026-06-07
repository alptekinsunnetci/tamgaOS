/* belirtec.h — Tamga dili token (belirtec) turleri.
 * Anahtar kelimeler ve tasarim icin: docs/tamga-dili.md
 */
#ifndef TAMGA_BELIRTEC_H
#define TAMGA_BELIRTEC_H

typedef enum {
    /* Sabitler / literaller */
    B_SAYI,          /* 42, 3.14 */
    B_METIN,         /* "merhaba" */
    B_DOGRU,         /* doğru */
    B_YANLIS,        /* yanlış */
    B_BOS,           /* boş */
    B_TANIMLAYICI,   /* degisken / islev adi */

    /* Anahtar kelimeler */
    B_YAZ,           /* yaz */
    B_EGER,          /* eğer */
    B_ISE,           /* ise */
    B_DEGILSE,       /* değilse */
    B_BITIR,         /* bitir */
    B_IKEN,          /* iken */
    B_HER,           /* her */
    B_ICINDE,        /* içinde */
    B_ISLEV,         /* işlev */
    B_DONDUR,        /* döndür */
    B_VE,            /* ve */
    B_VEYA,          /* veya */
    B_DEGIL,         /* değil */
    B_DUR,           /* dur (break) */
    B_DEVAM,         /* devam (continue) */

    /* Operatorler */
    B_ARTI,          /* + */
    B_EKSI,          /* - */
    B_CARPI,         /* * */
    B_BOLU,          /* / */
    B_MOD,           /* % */
    B_ATAMA,         /* = */
    B_ESIT,          /* == */
    B_ESITDEGIL,     /* != */
    B_KUCUK,         /* < */
    B_BUYUK,         /* > */
    B_KUCUKESIT,     /* <= */
    B_BUYUKESIT,     /* >= */

    /* Noktalama */
    B_PARANTEZ_AC,   /* ( */
    B_PARANTEZ_KAPA, /* ) */
    B_KOSE_AC,       /* [ */
    B_KOSE_KAPA,     /* ] */
    B_SUSLU_AC,      /* { */
    B_SUSLU_KAPA,    /* } */
    B_IKINOKTA,      /* : */
    B_VIRGUL,        /* , */
    B_NOKTA,         /* . */

    /* Yapisal */
    B_SATIRSONU,     /* \n */
    B_DOSYASONU,     /* girdi sonu */
    B_HATA           /* taninmayan belirtec */
} BelirtecTuru;

#define BELIRTEC_AZAMI_UZUNLUK 64

typedef struct {
    BelirtecTuru tur;
    char         metin[BELIRTEC_AZAMI_UZUNLUK]; /* ham metin (tanimlayici/metin/sayi) */
    double       sayi;     /* tur == B_SAYI ise deger */
    int          satir;    /* hata raporlama icin */
} Belirtec;

const char *belirtec_adi(BelirtecTuru tur); /* hata ayiklama icin okunabilir ad */

#endif /* TAMGA_BELIRTEC_H */
