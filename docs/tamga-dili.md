# Tamga Dili — Spesifikasyon

Tamga; satır-tabanlı, Türkçe anahtar kelimeli, ağaç-yürüyen bir yorumlanan
dildir. Kaynak **UTF-8**'dir (lexer baştaki BOM'u atlar). Uygulamalar `.tam`
uzantısıyla saklanır.

## 1. Sözlüksel yapı

- **Yorum:** `#` ile başlar, satır sonuna kadar.
- **Satır sonu** anlamlıdır (deyim ayırıcı).
- **Sayı:** `42`, `3.14` (çift duyarlık `double`).
- **Metin:** `"..."`; kaçışlar: `\n \t \\ \"`.
- **Tanımlayıcı:** harf/`_`/Türkçe harf ile başlar, içinde rakam olabilir.
  Lexer `>= 0x80` baytları tanımlayıcı karakteri sayar → Türkçe harfler çalışır.

### Anahtar kelimeler
`yaz` `eğer` `ise` `değilse` `bitir` `iken` `her` `içinde` `işlev` `döndür`
`dur` `devam` `doğru` `yanlış` `boş` `ve` `veya` `değil`

### Operatörler ve noktalama
`+` `-` `*` `/` `%` `=` `==` `!=` `<` `>` `<=` `>=`
`(` `)` `[` `]` `{` `}` `:` `,` `.`

## 2. Değer türleri

| Tür      | Örnek            | Notlar |
|----------|------------------|--------|
| sayı     | `42`, `3.14`     | çift duyarlık |
| metin    | `"merhaba"`      | host'ta ≤127, gömülüde ≤63 bayt |
| mantık   | `doğru`/`yanlış` | |
| boş      | `boş`            | değer yokluğu |
| dizi     | `[1, 2, 3]`      | başvuru türü; dinamik büyür |
| sözlük   | `{"a": 1}`       | anahtar-değer; başvuru türü; anahtar metin |
| işlev    | `işlev ...`      | yalnız çağrılır (birinci sınıf değil) |

## 3. Deyimler

### Yazdırma
```tamga
yaz "Merhaba Dünya"
yaz 2 + 3 * 4          # 14
```

### Atama
```tamga
sayi = 10
ad = "Tamga"
d[0] = 99             # dizi/sözlük öğesine de atanır
```

### Koşul
```tamga
eğer sayi > 5 ise
    yaz "Büyük"
değilse
    yaz "Küçük"
bitir
```

### Döngü (koşullu)
```tamga
i = 0
iken i < 5
    yaz i
    i = i + 1
bitir
```

### Döngü (her — for-each)
Bir dizinin **elemanlarını** ya da bir sözlüğün **anahtarlarını** gezer:
```tamga
her sayı içinde [10, 20, 30]
    yaz sayı
bitir

renkler = {"k": "kırmızı", "m": "mavi"}
her anahtar içinde renkler
    yaz anahtar + " = " + renkler[anahtar]
bitir
```

### Döngü kontrolü: dur / devam
`dur` döngüyü tamamen kırar; `devam` bu turu atlayıp sonraki tura geçer
(`iken` ve `her` için geçerli; `dur` işlev sınırını aşmaz).
```tamga
her s içinde [1, 2, 3, 4, 5]
    eğer s == 4 ise
        dur
    bitir
    eğer s % 2 == 0 ise
        devam
    bitir
    yaz s              # 1, 3
bitir
```

### İşlev
```tamga
işlev topla(a, b)
    döndür a + b
bitir
yaz topla(10, 20)     # 30
```
İşlevler **ileri bildirimlidir**: tanımdan önce çağrılabilir (yorumlayıcı önce
tüm küresel işlevleri kaydeder). Özyineleme desteklenir.

## 4. İfadeler ve öncelik

Yüksekten düşüğe:
```
1. birincil      sayı, metin, doğru/yanlış, boş, dizi, sözlük, (ifade), değişken, çağrı, indeksleme
2. birli         - değil
3. çarpma        * / %
4. toplama       + -
5. karşılaştırma == != < > <= >=
6. mantık        ve veya   (kısa devre)
```
`+` operatörü, taraflardan biri metinse **metin birleştirme** yapar:
```tamga
yaz "toplam = " + 30      # "toplam = 30"
```

## 5. Diziler

`[...]` ile oluşturulur, `0` tabanlı `d[i]` ile indekslenir, başvuru türüdür.
```tamga
d = [10, 20, 30]
yaz d[0]              # 10
d[1] = 99
ekle(d, 40)          # sona ekle -> yeni uzunluk
son = çıkar(d)        # sondan çıkar
bos = dizi()         # boş dizi ([] ile aynı)
izgara = [[1, 2], [3, 4]]
yaz izgara[1][0]      # 3
```
Metinler de `m[i]` ile indekslenip tek karakterlik metin döndürür.

## 6. Sözlükler

`{ "anahtar": değer, ... }` ile oluşturulur, `s["anahtar"]` ile erişilir/atanır.
Anahtarlar metin, değerler her türden olabilir; başvuru türüdür.
```tamga
kisi = {"ad": "Ayşe", "yas": 28}
kisi["yas"] = 29
kisi["sehir"] = "İzmir"   # yeni anahtar
yaz uzunluk(kisi)          # 3
eğer var_mı(kisi, "ad") ise
    yaz kisi["ad"]
bitir
sil_anahtar(kisi, "yas")
her k içinde kisi
    yaz k + " = " + kisi[k]
bitir
```

## 7. Yerleşik fonksiyonlar

### Giriş ve dönüşüm
| Fonksiyon  | Açıklama |
|------------|----------|
| `oku()`    | konsoldan bir satır okur (metin) |
| `sayı(x)`  | metni/değeri sayıya çevirir (`sayi` da geçerli) |
| `metin(x)` | değeri metne çevirir |

### Dizi
`dizi()` · `uzunluk(d)` · `ekle(d, x)` · `çıkar(d)`

### Sözlük
`sözlük()` · `uzunluk(s)` · `anahtarlar(s)` · `var_mı(s, anahtar)` · `sil_anahtar(s, anahtar)`

### Metin
| Fonksiyon                    | Açıklama |
|------------------------------|----------|
| `parça(metin, baş, [uz])`    | alt dizgi (byte indeksli) |
| `bul(metin, alt)`            | konum, yoksa -1 |
| `içerir(metin, alt)`         | içeriyor mu |
| `başlar(metin, ön)`          | ön ek ile başlıyor mu |
| `biter(metin, son)`          | son ek ile bitiyor mu |
| `kırp(metin)`                | baş/son boşlukları atar |
| `değiştir(metin, eski, yeni)`| tümünü değiştirir |
| `parçala(metin, ayraç)`      | ayraçtan bölüp **dizi** döndürür |
| `birleştir(dizi, ayraç)`     | dizi öğelerini ayraçla **metne** birleştirir |

```tamga
satir = dosya.oku("/belgeler/veri.csv")
alanlar = parçala(satir, ",")
her a içinde alanlar
    yaz kırp(a)
bitir
```

## 8. Sistem modülleri

`modül.metot(argümanlar)` biçiminde çağrılır.

```tamga
sistem.bellek()        # kullanılan bayt   (sistem.bellek_bos / bellek_toplam)
sistem.sure()          # açılıştan beri ms

zaman.simdi()          # ms
zaman.bekle(1000)      # 1 sn bekle

dosya.yaz("not.txt", "Merhaba")   # yazılan bayt (yol /'siz ise /belgeler altına)
dosya.oku("not.txt")              # "Merhaba"
dosya.ekle("not.txt", "!")        # sona ekler
dosya.var("not.txt")              # doğru/yanlış
dosya.sil("not.txt")              # doğru/yanlış
dosya.listele("/belgeler")        # girdi sayısı (konsola listeler)

gpio.yon(2, 1)         # pin 2 çıkış
gpio.yaz(2, 1)         # yüksek
gpio.oku(15)           # pin değeri

wifi.tara()                       # ağ sayısı (konsola listeler)
wifi.baglan("SSID", "parola")     # doğru/yanlış      (ağ.* takma adı geçerli)
```

## 9. Sınırlar

- Yerel/küresel kapsam vardır; gerçek closure yoktur.
- Metin: host ≤127, gömülü ≤63 bayt. Sözlük anahtarı ≤39 bayt.
- İşlev en çok 8 argüman; kapsam başına değişken sayısı `ORTAM_AZAMI` ile sınırlı.
- Özyineleme derinliği bellek arenasıyla sınırlıdır (her çağrı bir `Ortam` ayırır).
- REPL satırları bağımsız ayrıştırılır: çok satırlı bloklar (`eğer/iken/her/işlev`)
  satır satır yazılamaz — `.tam` dosyasına yazılıp `çalıştır` ile koşulur.

## 10. Gramer (EBNF)

```
program       := (deyim SATIRSONU*)*
deyim         := atama | yazdir | eger | iken | her | islev | dondur
               | 'dur' | 'devam' | ifade
atama         := lvalue '=' ifade
lvalue        := TANIMLAYICI | birincil '[' ifade ']'
yazdir        := 'yaz' ifade
eger          := 'eğer' ifade 'ise' program ['değilse' program] 'bitir'
iken          := 'iken' ifade program 'bitir'
her           := 'her' TANIMLAYICI 'içinde' ifade program 'bitir'
islev         := 'işlev' TANIMLAYICI '(' [params] ')' program 'bitir'
dondur        := 'döndür' [ifade]
ifade         := mantik
mantik        := karsilastirma (('ve'|'veya') karsilastirma)*
karsilastirma := toplama (('=='|'!='|'<'|'>'|'<='|'>=') toplama)*
toplama       := carpma (('+'|'-') carpma)*
carpma        := birli (('*'|'/'|'%') birli)*
birli         := ('-'|'değil') birli | birincil
birincil      := atom ('[' ifade ']')*
atom          := SAYI | METIN | 'doğru' | 'yanlış' | 'boş'
               | dizi | sozluk | TANIMLAYICI | cagri | '(' ifade ')'
dizi          := '[' [ifade (',' ifade)*] ']'
sozluk        := '{' [ifade ':' ifade (',' ifade ':' ifade)*] '}'
cagri         := TANIMLAYICI ['.' uye] '(' [ifade (',' ifade)*] ')'
```

## 11. Tam örnek

```tamga
işlev fak(n)
    eğer n <= 1 ise
        döndür 1
    bitir
    döndür n * fak(n - 1)
bitir

her i içinde [1, 2, 3, 4, 5]
    yaz i + "! = " + fak(i)
bitir
```
