# Tamga Dili — Spesifikasyon (Sürüm 0.1)

Tamga, satır-tabanlı, Türkçe anahtar kelimeli, ağaç-yürüyen bir yorumlanan
dildir. Kaynak **UTF-8**'dir. Uygulamalar `.tam` uzantısıyla saklanır.

## 1. Sözlüksel yapı

- **Yorum**: `#` ile başlar, satır sonuna kadar.
- **Satır sonu** anlamlıdır (deyim ayırıcı).
- **Sayı**: `42`, `3.14` (çift duyarlık `double`).
- **Metin**: `"..."`; kaçışlar: `\n \t \\ \"`.
- **Tanımlayıcı**: harf/`_`/Türkçe harf ile başlar; içinde rakam olabilir.
  (Lexer, `>= 0x80` baytları tanımlayıcı karakteri sayar → Türkçe harfler çalışır.)

### Anahtar kelimeler
`yaz` `eğer` `ise` `değilse` `bitir` `iken` `her` `içinde` `işlev` `döndür`
`dur` `devam` `doğru` `yanlış` `boş` `ve` `veya` `değil`

### Operatörler
`+` `-` `*` `/` `%` `=` `==` `!=` `<` `>` `<=` `>=` `(` `)` `,` `.`

## 2. Değer türleri

| Tür      | Örnek            | Notlar |
|----------|------------------|--------|
| sayı     | `42`, `3.14`     | çift duyarlık |
| metin    | `"merhaba"`      | en çok 127 bayt |
| mantık   | `doğru`/`yanlış` | |
| boş      | `boş`            | değer yokluğu |
| işlev    | `işlev ...`      | birinci sınıf değil (yalnız çağrılır) |
| dizi     | `[1, 2, 3]`      | başvuru türü; dinamik büyür |
| sözlük   | `{"a": 1}`       | anahtar-değer; başvuru türü; anahtarlar metin |

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
`dur` döngüyü tamamen kırar; `devam` bu turu atlayıp sonraki tura geçer.
(Her ikisi `iken` ve `her` için geçerlidir; `dur` işlev sınırını aşmaz.)
```tamga
her s içinde [1, 2, 3, 4, 5]
    eğer s == 4 ise
        dur            # 4'e gelince döngüyü bitir
    bitir
    eğer s % 2 == 0 ise
        devam          # çiftleri atla
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
İşlevler **ileri bildirimli**dir: bir işlev tanımından önce çağrılabilir
(yorumlayıcı önce tüm küresel işlevleri kaydeder).

## 4. İfadeler ve öncelik

Yüksekten düşüğe:
```
1. birincil      sayı, metin, doğru/yanlış, boş, (ifade), değişken, çağrı
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

## 4.1 Diziler ve yerleşik fonksiyonlar

Diziler `[...]` ile oluşturulur, `0` tabanlı `d[i]` ile indekslenir ve **başvuru
türüdür** (bir işleve geçince aynı dizi değişir).

```tamga
d = [10, 20, 30]
yaz d[0]              # 10
d[1] = 99            # öğe değiştir
yaz uzunluk(d)        # 3
ekle(d, 40)          # sona ekle -> yeni uzunluk
son = çıkar(d)        # sondan çıkar -> çıkarılan öğe
bos = dizi()         # boş dizi ([] ile aynı)
izgara = [[1, 2], [3, 4]]
yaz izgara[1][0]      # 3
```

Yerleşik (global) fonksiyonlar:

| Fonksiyon       | Açıklama |
|-----------------|----------|
| `uzunluk(x)`    | dizi öğe sayısı veya metin uzunluğu (bayt) |
| `ekle(d, x)`    | diziye sona ekler, yeni uzunluğu döndürür |
| `çıkar(d)`      | sondaki öğeyi çıkarır ve döndürür |
| `dizi()`        | boş dizi oluşturur |

Metinler de `m[i]` ile indekslenip tek karakterlik metin döndürür.

## 4.1.1 Sözlükler

Sözlükler `{ "anahtar": değer, ... }` ile oluşturulur, `s["anahtar"]` ile
erişilir/atanır. Anahtarlar **metin**, değerler her türden olabilir. Sözlükler
**başvuru türüdür**.

```tamga
kisi = {"ad": "Ayşe", "yas": 28}
yaz kisi["ad"]            # Ayşe
kisi["yas"] = 29         # güncelle
kisi["sehir"] = "İzmir"  # yeni anahtar
yaz uzunluk(kisi)         # 3
bos = sözlük()           # boş sözlük

eğer var_mı(kisi, "ad") ise
    yaz kisi["ad"]
bitir

sil_anahtar(kisi, "yas")
k = anahtarlar(kisi)      # anahtarların dizisi
```

| Fonksiyon            | Açıklama |
|----------------------|----------|
| `sözlük()`           | boş sözlük oluşturur |
| `uzunluk(s)`         | anahtar sayısı |
| `anahtarlar(s)`      | anahtarların dizisi |
| `var_mı(s, anahtar)` | anahtar var mı (doğru/yanlış) |
| `sil_anahtar(s, k)`  | anahtarı siler |

İç içe kullanım: `veri["liste"][0]`, `veri["adres"]["il"]`.

## 4.2 Giriş ve dönüşüm

| Fonksiyon    | Açıklama |
|--------------|----------|
| `oku()`      | konsoldan bir satır okur (metin döndürür) |
| `sayı(x)`    | metni/değeri sayıya çevirir (`sayi` da geçerli) |
| `metin(x)`   | değeri metne çevirir |

```tamga
yaz "Adınız?"
ad = oku()
yaş = sayı(oku())
yaz "Merhaba " + ad + ", " + metin(yaş) + " yaşındasınız."
```

## 4.2.1 Metin işlevleri

| Fonksiyon                    | Açıklama |
|------------------------------|----------|
| `parça(metin, baş, [uz])`    | alt dizgi (byte indeksli; uz yoksa sona kadar) |
| `bul(metin, alt)`            | alt dizginin konumu, yoksa -1 |
| `içerir(metin, alt)`         | içeriyor mu (doğru/yanlış) |
| `başlar(metin, ön)`          | ön ek ile başlıyor mu |
| `biter(metin, son)`          | son ek ile bitiyor mu |
| `kırp(metin)`                | baştaki/sondaki boşlukları atar |
| `değiştir(metin, eski, yeni)`| tüm `eski`leri `yeni` ile değiştirir |
| `parçala(metin, ayraç)`      | ayraçtan bölüp **dizi** döndürür |
| `birleştir(dizi, ayraç)`     | dizi öğelerini ayraçla **metne** birleştirir |

```tamga
yaz parça("merhaba", 0, 3)              # mer
yaz değiştir("a-b-c", "-", "+")        # a+b+c
parçalar = parçala("ali,veli,deli", ",") # ["ali", "veli", "deli"]
yaz birleştir(parçalar, " | ")          # ali | veli | deli
eğer içerir("merhaba", "rha") ise
    yaz "bulundu"
bitir
```
Metinler ≤ 127 bayttır; sonuçlar bu sınıra kırpılır.

## 5. Sistem modülleri

`modül.metot(argümanlar)` biçiminde çağrılır.

### sistem
```tamga
sistem.bellek()        # kullanılan bayt
sistem.bellek_bos()    # boş bayt
sistem.sure()          # açılıştan beri ms
```

### zaman
```tamga
zaman.simdi()          # ms
zaman.bekle(1000)      # 1 sn bekle
```

### dosya  (yol başında `/` yoksa /belgeler altına yazılır)
```tamga
dosya.yaz("not.txt", "Merhaba")   # yazılan bayt
dosya.oku("not.txt")              # "Merhaba"
dosya.ekle("not.txt", "!")        # sona ekler
dosya.var("not.txt")              # doğru/yanlış
dosya.sil("not.txt")              # doğru/yanlış
dosya.listele("/belgeler")        # girdi sayısı (konsola listeler)
```

### gpio
```tamga
gpio.yon(25, 1)        # pin 25 çıkış
gpio.yaz(25, 1)        # yüksek
gpio.oku(15)           # pin değeri
```

### wifi  (`ağ` takma adı da geçerli)
```tamga
wifi.tara()                       # ağ sayısı (konsola listeler)
wifi.baglan("SSID", "parola")     # doğru/yanlış
```

## 6. Tam örnek

```tamga
işlev faktoriyel(n)
    sonuc = 1
    i = 1
    iken i <= n
        sonuc = sonuc * i
        i = i + 1
    bitir
    döndür sonuc
bitir

yaz "5! = " + faktoriyel(5)       # 5! = 120
```

## 7. Sınırlar (Sürüm 0.1)

- Yerel/küresel kapsam vardır; gerçek closure yoktur.
- Diziler ve sözlükler var (başvuru türü). Sözlük anahtarları en çok 39 bayt.
- Metin en çok 127 bayt; işlev en çok 8 argüman.
- Özyineleme derinliği yığınla sınırlıdır (gömülü ortamda dikkatli kullanın).

## 8. Gramer (EBNF)

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
