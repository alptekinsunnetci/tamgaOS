/* tarayici.c — Tamga lexer.
 *
 * Kaynak UTF-8'dir. Turkce harfler (ğ ş ı ö ç ü İ) cok baytlidir; tam Unicode
 * tablosu tutmak yerine 0x80 ve uzeri her bayti "tanimlayici karakteri" sayariz.
 * Anahtar kelimeler bu sayede UTF-8 byte dizisi olarak strcmp ile eslesir.
 * Satir-tabanli dil: yeni satir B_SATIRSONU belirteci uretir.
 */
#include "tarayici.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void tarayici_baslat(Tarayici *t, const char *kaynak) {
    t->kaynak = kaynak;
    t->konum = 0;
    t->satir = 1;
    /* UTF-8 BOM (EF BB BF) varsa atla — Windows editorleri ekleyebilir. */
    const unsigned char *k = (const unsigned char *)kaynak;
    if (k[0] == 0xEF && k[1] == 0xBB && k[2] == 0xBF) t->konum = 3;
}

static char bak(Tarayici *t)      { return t->kaynak[t->konum]; }
static char bak2(Tarayici *t)     { return t->kaynak[t->konum] ? t->kaynak[t->konum + 1] : '\0'; }
static char ilerle(Tarayici *t)   { return t->kaynak[t->konum++]; }

/* Bir tanimlayici/anahtar karakterinin parcasi mi? (harf, _, rakam(devam), >=0x80) */
static int tanim_basi(unsigned char c)  { return isalpha(c) || c == '_' || c >= 0x80; }
static int tanim_govde(unsigned char c) { return isalnum(c) || c == '_' || c >= 0x80; }

static Belirtec uret(BelirtecTuru tur, int satir) {
    Belirtec b;
    b.tur = tur;
    b.metin[0] = '\0';
    b.sayi = 0;
    b.satir = satir;
    return b;
}

/* Toplanan tanimlayici metnini anahtar kelimeyle esle. */
static BelirtecTuru anahtar_mi(const char *s) {
    if (strcmp(s, "yaz") == 0)     return B_YAZ;
    if (strcmp(s, "eğer") == 0)    return B_EGER;
    if (strcmp(s, "ise") == 0)     return B_ISE;
    if (strcmp(s, "değilse") == 0) return B_DEGILSE;
    if (strcmp(s, "bitir") == 0)   return B_BITIR;
    if (strcmp(s, "iken") == 0)    return B_IKEN;
    if (strcmp(s, "her") == 0)     return B_HER;
    if (strcmp(s, "içinde") == 0)  return B_ICINDE;
    if (strcmp(s, "işlev") == 0)   return B_ISLEV;
    if (strcmp(s, "döndür") == 0)  return B_DONDUR;
    if (strcmp(s, "doğru") == 0)   return B_DOGRU;
    if (strcmp(s, "yanlış") == 0)  return B_YANLIS;
    if (strcmp(s, "boş") == 0)     return B_BOS;
    if (strcmp(s, "ve") == 0)      return B_VE;
    if (strcmp(s, "veya") == 0)    return B_VEYA;
    if (strcmp(s, "değil") == 0)   return B_DEGIL;
    if (strcmp(s, "dur") == 0)     return B_DUR;
    if (strcmp(s, "devam") == 0)   return B_DEVAM;
    return B_TANIMLAYICI;
}

Belirtec tarayici_sonraki(Tarayici *t) {
    /* Bosluklari ve yorumlari atla (satirsonu HARIC) */
    for (;;) {
        char c = bak(t);
        if (c == ' ' || c == '\t' || c == '\r') { ilerle(t); continue; }
        /* '#' satir sonuna kadar yorum */
        if (c == '#') { while (bak(t) && bak(t) != '\n') ilerle(t); continue; }
        break;
    }

    int satir = t->satir;
    char c = bak(t);

    /* Girdi sonu */
    if (c == '\0') return uret(B_DOSYASONU, satir);

    /* Satir sonu */
    if (c == '\n') { ilerle(t); t->satir++; return uret(B_SATIRSONU, satir); }

    /* Sayi (tam veya ondalik) */
    if (isdigit((unsigned char)c)) {
        Belirtec b = uret(B_SAYI, satir);
        int i = 0;
        while (isdigit((unsigned char)bak(t)) && i < BELIRTEC_AZAMI_UZUNLUK - 1)
            b.metin[i++] = ilerle(t);
        if (bak(t) == '.' && isdigit((unsigned char)bak2(t))) {
            b.metin[i++] = ilerle(t); /* '.' */
            while (isdigit((unsigned char)bak(t)) && i < BELIRTEC_AZAMI_UZUNLUK - 1)
                b.metin[i++] = ilerle(t);
        }
        b.metin[i] = '\0';
        b.sayi = atof(b.metin);
        return b;
    }

    /* Metin dizgisi "..." (basit kacis: \n \t \\ \") */
    if (c == '"') {
        ilerle(t); /* acan " */
        Belirtec b = uret(B_METIN, satir);
        int i = 0;
        while (bak(t) && bak(t) != '"' && i < BELIRTEC_AZAMI_UZUNLUK - 1) {
            char k = ilerle(t);
            if (k == '\\') {
                char e = ilerle(t);
                switch (e) {
                    case 'n': k = '\n'; break;
                    case 't': k = '\t'; break;
                    case '\\': k = '\\'; break;
                    case '"': k = '"'; break;
                    default: k = e; break;
                }
            }
            b.metin[i++] = k;
        }
        b.metin[i] = '\0';
        if (bak(t) == '"') ilerle(t); /* kapatan " */
        return b;
    }

    /* Tanimlayici / anahtar kelime (UTF-8 Turkce dahil) */
    if (tanim_basi((unsigned char)c)) {
        Belirtec b = uret(B_TANIMLAYICI, satir);
        int i = 0;
        while (tanim_govde((unsigned char)bak(t)) && i < BELIRTEC_AZAMI_UZUNLUK - 1)
            b.metin[i++] = ilerle(t);
        b.metin[i] = '\0';
        b.tur = anahtar_mi(b.metin);
        return b;
    }

    /* Operatorler ve noktalama */
    ilerle(t); /* c'yi tuket */
    switch (c) {
        case '+': return uret(B_ARTI, satir);
        case '-': return uret(B_EKSI, satir);
        case '*': return uret(B_CARPI, satir);
        case '/': return uret(B_BOLU, satir);
        case '%': return uret(B_MOD, satir);
        case '(': return uret(B_PARANTEZ_AC, satir);
        case ')': return uret(B_PARANTEZ_KAPA, satir);
        case '[': return uret(B_KOSE_AC, satir);
        case ']': return uret(B_KOSE_KAPA, satir);
        case '{': return uret(B_SUSLU_AC, satir);
        case '}': return uret(B_SUSLU_KAPA, satir);
        case ':': return uret(B_IKINOKTA, satir);
        case ',': return uret(B_VIRGUL, satir);
        case '.': return uret(B_NOKTA, satir);
        case '=':
            if (bak(t) == '=') { ilerle(t); return uret(B_ESIT, satir); }
            return uret(B_ATAMA, satir);
        case '!':
            if (bak(t) == '=') { ilerle(t); return uret(B_ESITDEGIL, satir); }
            break;
        case '<':
            if (bak(t) == '=') { ilerle(t); return uret(B_KUCUKESIT, satir); }
            return uret(B_KUCUK, satir);
        case '>':
            if (bak(t) == '=') { ilerle(t); return uret(B_BUYUKESIT, satir); }
            return uret(B_BUYUK, satir);
    }

    /* Taninmayan */
    Belirtec h = uret(B_HATA, satir);
    h.metin[0] = c; h.metin[1] = '\0';
    return h;
}

const char *belirtec_adi(BelirtecTuru tur) {
    switch (tur) {
        case B_SAYI: return "SAYI";
        case B_METIN: return "METIN";
        case B_DOGRU: return "DOGRU";
        case B_YANLIS: return "YANLIS";
        case B_BOS: return "BOS";
        case B_TANIMLAYICI: return "TANIMLAYICI";
        case B_YAZ: return "YAZ";
        case B_EGER: return "EGER";
        case B_ISE: return "ISE";
        case B_DEGILSE: return "DEGILSE";
        case B_BITIR: return "BITIR";
        case B_IKEN: return "IKEN";
        case B_HER: return "HER";
        case B_ICINDE: return "ICINDE";
        case B_ISLEV: return "ISLEV";
        case B_DONDUR: return "DONDUR";
        case B_VE: return "VE";
        case B_VEYA: return "VEYA";
        case B_DEGIL: return "DEGIL";
        case B_DUR: return "DUR";
        case B_DEVAM: return "DEVAM";
        case B_ARTI: return "ARTI";
        case B_EKSI: return "EKSI";
        case B_CARPI: return "CARPI";
        case B_BOLU: return "BOLU";
        case B_MOD: return "MOD";
        case B_ATAMA: return "ATAMA";
        case B_ESIT: return "ESIT";
        case B_ESITDEGIL: return "ESITDEGIL";
        case B_KUCUK: return "KUCUK";
        case B_BUYUK: return "BUYUK";
        case B_KUCUKESIT: return "KUCUKESIT";
        case B_BUYUKESIT: return "BUYUKESIT";
        case B_PARANTEZ_AC: return "PARANTEZ_AC";
        case B_PARANTEZ_KAPA: return "PARANTEZ_KAPA";
        case B_KOSE_AC: return "KOSE_AC";
        case B_KOSE_KAPA: return "KOSE_KAPA";
        case B_SUSLU_AC: return "SUSLU_AC";
        case B_SUSLU_KAPA: return "SUSLU_KAPA";
        case B_IKINOKTA: return "IKINOKTA";
        case B_VIRGUL: return "VIRGUL";
        case B_NOKTA: return "NOKTA";
        case B_SATIRSONU: return "SATIRSONU";
        case B_DOSYASONU: return "DOSYASONU";
        default: return "HATA";
    }
}
