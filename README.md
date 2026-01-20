# BTP101 Öğrenci Not ve Devamsızlık Yönetim Sistemi dökümantasyonu

Bu çalışma, bir eğitim kurumundaki öğrencilerin dönem içi performanslarını, sınav notlarını ve 14 haftalık derse katılım verilerini dijital ortamda takip etmek, analiz etmek ve sonuçları raporlamak amacıyla geliştirilmiş bir C++ otomasyonudur. Program, dosya tabanlı bir veri yönetim sistemi kullanarak bilgilerin kalıcı olarak saklanmasını sağlar.

## Sistemin Çalışma Mantığı

Sistem, modüler bir yapıda tasarlanmış olup temel olarak "Veri Girişi", "Veri İşleme" ve "Raporlama" aşamalarından oluşur:

1.  **Yapılandırma:** Program ilk açıldığında `yuzdeler.dat` dosyasını kontrol ederek sınav ve ödevlerin başarı notuna etki oranlarını yükler. Eğer bu dosya yoksa kullanıcıdan yeni yüzdeler girmesini bekler.
2.  **Veri Entegrasyonu:** Farklı dosyalarda (`as.dat`, `ks.dat`, `yoklama_var.dat` vb.) tutulan veriler, öğrenci numaraları (ID) üzerinden eşleştirilerek tek bir yapı altında birleştirilir.
3.  **Mantıksal Değerlendirme:** Sistem, hem akademik başarıyı (not ortalaması) hem de devam zorunluluğunu eş zamanlı olarak kontrol eder. Özellikle devamsızlık hesaplamasında geleneksel yöntemlerin dışında, öğrencinin derste bulunduğu ve bulunmadığı saatlerin doğrudan kıyaslandığı bir algoritma çalıştırılır.
4.  **Çıktı Üretimi:** Analiz sonuçları hem terminal ekranında tablolar halinde sunulur hem de `basarinot.dat` ve `harfnot.dat` dosyalarına kaydedilerek dışa aktarılır.

## Teknik Detaylar ve Fonksiyon Yapıları

Programda verimliliği artırmak ve kod tekrarını önlemek amacıyla çeşitli fonksiyon kategorileri kullanılmıştır:

### Veri Yapıları (Structures)
* **Ogrenci Yapısı:** Öğrencinin kimlik bilgilerini, dinamik not listelerini (`vector<int>`), haftalık yoklama saatlerini ve hesaplanmış başarı durumlarını içeren kapsamlı bir veri modelidir.
* **Yuzdeler Yapısı:** Sınavların ağırlıklarını merkezi bir noktadan yönetmek için kullanılan konfigürasyon yapısıdır.

### Dosya Yönetim Fonksiyonları
* **dosyaOku:** Şablon benzeri bir mantıkla çalışır; dosya adını ve okunacak not sayısını parametre alarak verileri `stringstream` aracılığıyla ayrıştırır ve bir öğrenci listesi döndürür.
* **yoklamaDosyasiOku:** 14 haftalık özel veri formatını işlemek üzere özelleştirilmiş bir okuma fonksiyonudur.

### Analiz ve Hesaplama Fonksiyonları
* **yoklamaBirlestir:** İki farklı yoklama dosyasını (`yoklama_var.dat` ve `yoklama_yok.dat`) okuyarak verileri öğrenci bazında birleştirir. Toplam "Var" ve "Yok" saatlerini hesaplayarak devam durumunu belirler.
* **basariAnalizi:** Sistemin en karmaşık fonksiyonudur. Tüm not dosyalarını okur, `notuBul` fonksiyonu ile her öğrencinin farklı dosyalardaki notlarını bir araya getirir, ağırlıklı ortalamayı hesaplar ve harf notu ataması yapar.
* **harfBul ve katsayiBul:** Sayısal not ortalamasını üniversite standartlarındaki harf sistemine (AA, BA, ..., FF) ve 4.00'lük not katsayısına dönüştürür.

### Kullanıcı Arayüzü Fonksiyonları
* **listeYazdir:** `iomanip` kütüphanesini (`setw`, `left`) kullanarak verilerin terminal ekranında hizalı ve okunaklı bir tablo formunda gösterilmesini sağlar.

## Devamsızlık ve Başarı Kuralları

Sistemde uygulanan karar mekanizmaları şu şekildedir:

* **Devamsızlık Kuralı:** Bir öğrencinin derse katılmadığı toplam saat sayısı (Yok), katıldığı toplam saat sayısından (Var) daha büyükse, notu ne olursa olsun öğrenci devamsızlıktan kalmış sayılır (`KALDI (Devam)`).
* **Not Kuralı:** Devamsızlıktan kalmayan öğrenciler için 100 üzerinden hesaplanan puanlar harf skalasına tabi tutulur. 33 puanın altında kalan öğrenciler doğrudan başarısız sayılır (`KALDI (Not)`).

## Dosya Listesi ve İçerikleri

Projenin sağlıklı çalışması için `btpodev` klasörü içinde bulunması gereken dosyalar:

* **main.cpp:** Uygulamanın kaynak kodu.
* **yuzdeler.dat:** Sınav ağırlık verileri.
* **as.dat:** Ara sınav (Vize) notları.
* **ks.dat:** Kısa sınav (Quiz) notları.
* **odv.dat:** Ödev puanları.
* **prj.dat:** Proje değerlendirmeleri.
* **ds.dat:** Dönem sonu (Final) sınav notları.
* **yoklama_var.dat:** Derse katılım sağlanan saatlerin listesi.
* **yoklama_yok.dat:** Derse katılım sağlanmayan saatlerin listesi.

## Derleme Talimatı

Sistem standart C++ kütüphanelerini kullandığı için güncel herhangi bir C++ derleyicisi ile derlenebilir:

```bash
g++ main.cpp -o btp_sistemi
./btp_sistemi
