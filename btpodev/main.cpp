#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

// Ogrenci bilgilerini tutan yapi
struct Ogrenci {
    string no;
    string ad;
    string soyad;
    vector<int> notlar;
    
    // Yoklama verileri (14 hafta)
    vector<int> katildigiSaatler;
    vector<int> katilmadigiSaatler;
    
    // Hesaplananlar
    int toplamVar = 0;
    int toplamYok = 0;
    double devamsizlikOrani = 0.0;
    bool kaldi = false;
};

// Yuzdeler icin global degiskenler
struct Yuzdeler {
    int ara1 = 0, ara2 = 0;
    int kisa1 = 0, kisa2 = 0;
    int odev1 = 0, odev2 = 0;
    int proje1 = 0, proje2 = 0;
    int donemSonu = 0;
} ayarlar;

// Dosya isimleri
const string DOSYA_YUZDELER = "yuzdeler.dat";
const string DOSYA_ARA = "as.dat";
const string DOSYA_KISA = "ks.dat";
const string DOSYA_ODEV = "odv.dat";
const string DOSYA_PROJE = "prj.dat";
const string DOSYA_SONU = "ds.dat";

// Iki ayri yoklama dosyasi
const string DOSYA_YOKLAMA_VAR = "yoklama_var.dat"; // EK 1
const string DOSYA_YOKLAMA_YOK = "yoklama_yok.dat"; // EK 2

const string DOSYA_BASARI = "basarinot.dat";
const string DOSYA_HARF = "harfnot.dat";

// Dosyadan veri okuma fonksiyonu (Notlar icin)
vector<Ogrenci> dosyaOku(string dosyaAdi, int notSayisi) {
    vector<Ogrenci> liste;
    ifstream dosya(dosyaAdi);
    if (!dosya.is_open()) return liste;

    string satir;
    while (getline(dosya, satir)) {
        if (satir.empty()) continue;
        stringstream ss(satir);
        Ogrenci o;
        ss >> o.no >> o.ad >> o.soyad;
        for (int i = 0; i < notSayisi; ++i) {
            int notDegeri;
            if (ss >> notDegeri) o.notlar.push_back(notDegeri);
            else o.notlar.push_back(0); 
        }
        liste.push_back(o);
    }
    dosya.close();
    return liste;
}

// 14 sayili yoklama dosyasi okuma (generic)
vector<Ogrenci> yoklamaDosyasiOku(string dosyaAdi) {
    vector<Ogrenci> liste;
    ifstream dosya(dosyaAdi);
    if (!dosya.is_open()) return liste;

    string satir;
    while (getline(dosya, satir)) {
        if (satir.empty()) continue;
        stringstream ss(satir);
        Ogrenci o;
        ss >> o.no >> o.ad >> o.soyad;

        for (int i = 0; i < 14; ++i) {
            int saat;
            if (ss >> saat) o.notlar.push_back(saat); // Gecici olarak notlar vektorune atiyoruz
            else o.notlar.push_back(0);
        }
        liste.push_back(o);
    }
    dosya.close();
    return liste;
}

// Iki yoklama dosyasini birlestirip analiz etme
vector<Ogrenci> yoklamaBirlestir() {
    // Once dosyalari ham olarak oku (notlar vektorunu tasiyici olarak kullaniyoruz)
    vector<Ogrenci> l_var = yoklamaDosyasiOku(DOSYA_YOKLAMA_VAR);
    vector<Ogrenci> l_yok = yoklamaDosyasiOku(DOSYA_YOKLAMA_YOK);
    
    vector<Ogrenci> sonuc;
    
    // Eslesme yapalim
    for (size_t i = 0; i < l_var.size(); ++i) {
        Ogrenci o = l_var[i];
        // 'notlar' vektorundeki veriyi asil yerine alalim
        for(int x : o.notlar) {
            o.katildigiSaatler.push_back(x);
            o.toplamVar += x;
        }
        o.notlar.clear(); // Temizle
        
        // Simdi diger dosyadan 'yok' verisini bulalim
        bool bulundu = false;
        for (size_t j = 0; j < l_yok.size(); ++j) {
            if (l_yok[j].no == o.no) {
                bulundu = true;
                for(int y : l_yok[j].notlar) {
                    o.katilmadigiSaatler.push_back(y);
                    o.toplamYok += y;
                }
                break;
            }
        }
        
        if (!bulundu) {
            // Eger yok dosyasi yoksa 0 kabul et
            for(int k=0; k<14; k++) o.katilmadigiSaatler.push_back(0);
        }
        
        // HESAPLAMA: Kullanicinin yeni kurali:
        // Eger YoklamaYok > YoklamaVar ise KALDI.
        // Aksi takdirde (Var >= Yok) GECER.
        
        if (o.toplamYok > o.toplamVar) {
            o.kaldi = true;
        } else {
            o.kaldi = false;
        }
        
        sonuc.push_back(o);
    }
    return sonuc;
}

void yuzdelerGiris() {
    cout << "\n--- SINAV YUZDELERI GIRIS ---\n";
    cout << "1. Ara Sinav (Min 10 Max 50): "; cin >> ayarlar.ara1;
    cout << "2. Ara Sinav: "; cin >> ayarlar.ara2;
    cout << "1. Kisa Sinav: "; cin >> ayarlar.kisa1;
    cout << "2. Kisa Sinav: "; cin >> ayarlar.kisa2;
    cout << "1. Odev: "; cin >> ayarlar.odev1;
    cout << "2. Odev: "; cin >> ayarlar.odev2;
    cout << "1. Proje: "; cin >> ayarlar.proje1;
    cout << "2. Proje: "; cin >> ayarlar.proje2;
    cout << "Donem Sonu Sinavi: "; cin >> ayarlar.donemSonu;

    int toplam = ayarlar.ara1 + ayarlar.ara2 + ayarlar.kisa1 + ayarlar.kisa2 + 
                 ayarlar.odev1 + ayarlar.odev2 + ayarlar.proje1 + ayarlar.proje2 + ayarlar.donemSonu;
    
    if (toplam != 100) {
        cout << "HATA: Toplam 100 olmali! (" << toplam << ")\n";
    } else {
        ofstream f(DOSYA_YUZDELER);
        f << ayarlar.ara1 << " " << ayarlar.ara2 << endl;
        f << ayarlar.kisa1 << " " << ayarlar.kisa2 << endl;
        f << ayarlar.odev1 << " " << ayarlar.odev2 << endl;
        f << ayarlar.proje1 << " " << ayarlar.proje2 << endl;
        f << ayarlar.donemSonu << endl;
        f.close();
        cout << "Kaydedildi.\n";
    }
}

void yuzdeleriYukle() {
    ifstream f(DOSYA_YUZDELER);
    if (f.is_open()) {
        f >> ayarlar.ara1 >> ayarlar.ara2;
        f >> ayarlar.kisa1 >> ayarlar.kisa2;
        f >> ayarlar.odev1 >> ayarlar.odev2;
        f >> ayarlar.proje1 >> ayarlar.proje2;
        f >> ayarlar.donemSonu;
        f.close();
    }
}

void listeYazdir(string baslik, vector<Ogrenci>& liste) {
    cout << "\n--- " << baslik << " ---\n";
    cout << left << setw(12) << "No" << setw(10) << "Ad" << setw(15) << "Soyad" << "Notlar" << endl;
    cout << "-----------------------------------------------" << endl;
    for (size_t i = 0; i < liste.size(); ++i) {
        cout << left << setw(12) << liste[i].no << setw(10) << liste[i].ad << setw(15) << liste[i].soyad;
        for (size_t k = 0; k < liste[i].notlar.size(); ++k) {
            if (liste[i].notlar[k] == -1) cout << "G ";
            else cout << liste[i].notlar[k] << " ";
        }
        cout << endl;
    }
}

double katsayiBul(string harf) {
    if (harf == "AA") return 4.00;
    if (harf == "AB") return 3.70;
    if (harf == "BA") return 3.30;
    if (harf == "BB") return 3.00;
    if (harf == "BC") return 2.70;
    if (harf == "CB") return 2.30;
    if (harf == "CC") return 2.00;
    if (harf == "CD") return 1.70;
    if (harf == "DC") return 1.30;
    if (harf == "DD") return 1.00;
    return 0.00;
}

string harfBul(double puan) {
    if (puan >= 84) return "AA";
    if (puan >= 77) return "AB";
    if (puan >= 71) return "BA";
    if (puan >= 66) return "BB";
    if (puan >= 61) return "BC";
    if (puan >= 56) return "CB";
    if (puan >= 50) return "CC";
    if (puan >= 46) return "CD";
    if (puan >= 40) return "DC";
    if (puan >= 33) return "DD";
    return "FF";
}

void notuBul(vector<Ogrenci>& liste, string ogrenciNo, int& not1, int& not2) {
    not1 = 0; not2 = 0;
    for (size_t i = 0; i < liste.size(); ++i) {
        if (liste[i].no == ogrenciNo) {
            if (liste[i].notlar.size() > 0) not1 = liste[i].notlar[0];
            if (liste[i].notlar.size() > 1) not2 = liste[i].notlar[1];
            return;
        }
    }
}

void basariAnalizi() {
    yuzdeleriYukle(); 
    if (ayarlar.donemSonu == 0 && ayarlar.ara1 == 0) { cout << "Once yuzdeleri girin.\n"; return; }

    int araSayisi = (ayarlar.ara1 > 0) + (ayarlar.ara2 > 0);
    int kisaSayisi = (ayarlar.kisa1 > 0) + (ayarlar.kisa2 > 0);
    int odevSayisi = (ayarlar.odev1 > 0) + (ayarlar.odev2 > 0);
    int projeSayisi = (ayarlar.proje1 > 0) + (ayarlar.proje2 > 0);
    int finalSayisi = (ayarlar.donemSonu > 0);
    // if (projeSayisi == 0) projeSayisi = 1; // Gereksiz okuma yapmayalim 

    vector<Ogrenci> l_ara = dosyaOku(DOSYA_ARA, araSayisi);
    vector<Ogrenci> l_kisa = dosyaOku(DOSYA_KISA, kisaSayisi);
    vector<Ogrenci> l_odev = dosyaOku(DOSYA_ODEV, odevSayisi);
    vector<Ogrenci> l_proje = dosyaOku(DOSYA_PROJE, projeSayisi);
    vector<Ogrenci> l_sonu = dosyaOku(DOSYA_SONU, finalSayisi);
    vector<Ogrenci> l_yoklama = yoklamaBirlestir(); // Birlestirilmis liste

    ofstream fBasari(DOSYA_BASARI);
    ofstream fHarf(DOSYA_HARF);

    cout << "\n--- BASARI ANALIZI ---\n";
    cout << left << setw(11) << "No" << setw(10) << "Ad" << setw(15) << "Soyad";
    
    // Genisletilmis Sutunlar
    if (ayarlar.ara1 > 0) cout << setw(6) << "Ara1";
    if (ayarlar.ara2 > 0) cout << setw(6) << "Ara2";
    if (ayarlar.kisa1 > 0) cout << setw(6) << "Kis1";
    if (ayarlar.kisa2 > 0) cout << setw(6) << "Kis2";
    if (ayarlar.odev1 > 0) cout << setw(6) << "Odv1";
    if (ayarlar.odev2 > 0) cout << setw(6) << "Odv2";
    if (ayarlar.proje1 > 0) cout << setw(6) << "Prj1";
    if (ayarlar.proje2 > 0) cout << setw(6) << "Prj2";
    if (ayarlar.donemSonu > 0) cout << setw(6) << "Fin";
    
    // Basliklari guncelledik: Devamsiz yerine "Var/Yok" yazalim
    cout << setw(8) << "Puan" << setw(5) << "Harf" << setw(5) << "GPA" << setw(13) << "Var/Yok" << "Durum" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------" << endl;

    // double sinir = 14 * 4 * 0.70; // 39.2 (Eski kural)

    for (size_t i = 0; i < l_ara.size(); ++i) {
        Ogrenci o = l_ara[i];
        double toplamPuan = 0;
        int n_ara1=-1, n_ara2=-1, n_kis1=-1, n_kis2=-1, n_odv1=-1, n_odv2=-1, n_prj1=-1, n_prj2=-1, n_fin=-1;

        if (o.notlar.size() > 0) n_ara1 = o.notlar[0];
        if (o.notlar.size() > 1) n_ara2 = o.notlar[1];
        if (n_ara1 != -1) toplamPuan += n_ara1 * ayarlar.ara1 / 100.0;
        if (n_ara2 != -1) toplamPuan += n_ara2 * ayarlar.ara2 / 100.0;

        notuBul(l_kisa, o.no, n_kis1, n_kis2);
        notuBul(l_odev, o.no, n_odv1, n_odv2);
        notuBul(l_proje, o.no, n_prj1, n_prj2);

        if (n_kis1 != -1) toplamPuan += n_kis1 * ayarlar.kisa1 / 100.0;
        if (n_kis2 != -1) toplamPuan += n_kis2 * ayarlar.kisa2 / 100.0;
        if (n_odv1 != -1) toplamPuan += n_odv1 * ayarlar.odev1 / 100.0;
        if (n_odv2 != -1) toplamPuan += n_odv2 * ayarlar.odev2 / 100.0;
        if (n_prj1 != -1) toplamPuan += n_prj1 * ayarlar.proje1 / 100.0;
        if (n_prj2 != -1) toplamPuan += n_prj2 * ayarlar.proje2 / 100.0;

        for (size_t j = 0; j < l_sonu.size(); ++j) {
            if (l_sonu[j].no == o.no) {
                if (l_sonu[j].notlar.size() > 0) n_fin = l_sonu[j].notlar[0];
                break;
            }
        }

        bool devamsiz = false;
        int katildigi = 0;
        int katilmadigi = 0;
        
        for (const auto& yok : l_yoklama) {
            if (yok.no == o.no) {
                devamsiz = yok.kaldi;
                katildigi = yok.toplamVar;
                katilmadigi = yok.toplamYok;
                break;
            }
        }

        // Devamsiz olsa bile puani hesaplayalim ve Final notunu gosterelim
        if (n_fin != -1) { 
            toplamPuan += n_fin * ayarlar.donemSonu / 100.0; 
        }

        // Harf ve GPA'yi her durumda hesapla
        string harf = harfBul(toplamPuan);
        double gpa = katsayiBul(harf);

        // Durum Belirleme
        string durumStr = "GECER";
        if (devamsiz) {
            durumStr = "KALDI (Devam)";
        } else if (harf == "FF") {
            durumStr = "KALDI (Not)";
        }

        // Yazdirma
        fBasari << o.no << " " << o.ad << " " << o.soyad << " " << toplamPuan << endl;
        fHarf << o.no << " " << o.ad << " " << o.soyad << " " << harf << endl;

        auto yazdirNot = [](int n) {
            if (n == -1) cout << setw(6) << "G";
            else cout << setw(6) << n;
        };

        cout << left << setw(11) << o.no << setw(10) << o.ad << setw(15) << o.soyad;
        if (ayarlar.ara1 > 0) yazdirNot(n_ara1);
        if (ayarlar.ara2 > 0) yazdirNot(n_ara2);
        if (ayarlar.kisa1 > 0) yazdirNot(n_kis1);
        if (ayarlar.kisa2 > 0) yazdirNot(n_kis2);
        if (ayarlar.odev1 > 0) yazdirNot(n_odv1);
        if (ayarlar.odev2 > 0) yazdirNot(n_odv2);
        if (ayarlar.proje1 > 0) yazdirNot(n_prj1);
        if (ayarlar.proje2 > 0) yazdirNot(n_prj2);
        if (ayarlar.donemSonu > 0) yazdirNot(n_fin);

        // Burada oran yerine "Var/Yok" yaziyoruz artik kural degisti
        stringstream ssDevam;
        ssDevam << katildigi << "/" << katilmadigi;

        cout << setw(8) << setprecision(3) << toplamPuan 
             << setw(5) << harf 
             << setw(5) << fixed << setprecision(2) << gpa 
             << setw(13) << ssDevam.str() << " " << durumStr << endl;
    }
    fBasari.close();
    fHarf.close();
    cout << "\nKaydedildi.\n";
}

int main() {
    setlocale(LC_ALL, "Turkish");
    yuzdeleriYukle();
    while (true) {
        cout << "\n=== BTP101 SİSTEM (YENİ YOKLAMA) ===\n";
        cout << "1. Sinav Yuzdeleri\n";
        cout << "2. Not Listeleri\n";
        cout << "3. Detayli Yoklama Listesi\n";
        cout << "4. Basari Analizi\n";
        cout << "5. Cikis\nSecim: ";
        int secim; cin >> secim;
        if (secim == 1) yuzdelerGiris();
        else if (secim == 2) {
             cout << "1.Ara 2.Kisa 3.Odev 4.Prj 5.Final: "; int n; cin >> n;
             int sayi = 1;
             string dosya = DOSYA_ARA;
             if(n==2) dosya = DOSYA_KISA;
             if(n==3) dosya = DOSYA_ODEV;
             if(n==4) dosya = DOSYA_PROJE;
             if(n==5) dosya = DOSYA_SONU;
             vector<Ogrenci> l = dosyaOku(dosya, sayi);
             listeYazdir("NOT LISTESI", l);
        } else if (secim == 3) {
            vector<Ogrenci> l = yoklamaBirlestir();
            cout << "\n--- YOKLAMA DETAYI (VAR / YOK / ORAN) ---\n";
             cout << left << setw(12) << "No" << setw(10) << "Ad" << setw(15) << "Soyad" 
                  << setw(8) << "T.Var" << setw(8) << "T.Yok" << setw(10) << "Oran" << "Durum" << endl;
             cout << "--------------------------------------------------------------------------\n";
             for(auto& o : l) {
                 cout << left << setw(12) << o.no << setw(10) << o.ad << setw(15) << o.soyad
                      << setw(8) << o.toplamVar << setw(8) << o.toplamYok 
                      << "%" << setw(9) << fixed << setprecision(2) << o.devamsizlikOrani 
                      << (o.kaldi ? "DZ" : "DEVAM") << endl;
             }
        } else if (secim == 4) basariAnalizi();
        else if (secim == 5) return 0;
    }
}
