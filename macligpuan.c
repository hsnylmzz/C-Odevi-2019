#include <stdio.h>
#define MAC_SAYISI 100   //en fazla oynanabilecek mac sayisi

struct takim{
	char harf;
	char uzunisim[12];
	unsigned int oyn_mac, galibiyet, beraberlik, maglubiyet, atilangol, yenilengol;
	int averaj, puan;
};
struct mac{
	char ev, karsi;
	unsigned int ev_gol, karsi_gol;
	int gecersiz;
};
struct ayarlar{
	int takim_sayisi;
	int winpoint;
	int tiedpoint;
	int losepoint;
};

void ayarlari_oku(struct ayarlar* cfg)
{
	FILE* f_ayarlar = fopen("ayarlar.txt", "r"); //dosyayi okuyor
    fscanf(f_ayarlar, "%d", &cfg->takim_sayisi); //ilk satirin takim sayisini oldugunu gosteren pointer
    fscanf(f_ayarlar, "%d", &cfg->winpoint);
    fscanf(f_ayarlar, "%d", &cfg->losepoint);
    fscanf(f_ayarlar, "%d", &cfg->tiedpoint);

    fclose(f_ayarlar); // dosyayi kapatiyor

    printf("Takim Sayisi: %d\n", cfg->takim_sayisi);
    printf("Galibiyet Puani: %d\n", cfg->winpoint);
    printf("Maglubiyet Puani: %d\n", cfg->losepoint);
    printf("Beraberlik Puani: %d\n", cfg->tiedpoint);
	printf("\n");
}
void takimlari_oku(struct takim t[], int takim_sayisi)
{
	int i;
	FILE* f_takimlar = fopen("takimlar.txt", "r");

    printf("Takim Listesi:\n");
	for(i=0; i<takim_sayisi; i++)
	{
		fscanf(f_takimlar, "%s %s", &t[i].harf, &t[i].uzunisim);
		printf("(%c) %s\n", t[i].harf, t[i].uzunisim);
		t[i].oyn_mac = 0; //oynanan mac sayısını sıfırlıyoruz
		t[i].galibiyet = 0;
		t[i].maglubiyet = 0;
		t[i].beraberlik = 0;
		t[i].atilangol = 0;
		t[i].yenilengol = 0;
		t[i].averaj = 0;
		t[i].puan = 0;
	}

	printf("\n");
	fclose(f_takimlar);
}
int maclari_oku(struct mac maclar[MAC_SAYISI])// takimi mac structi olan maclar dizisi
{
    FILE* f_maclar = fopen("maclar.txt", "r");
    int i = 0;

    printf("Maclar:\n");
    while (!feof(f_maclar)) // Dosya sonuna kadar oku (feof = file end of file)
    {
        fscanf(f_maclar, "%s %u %s %u", &maclar[i].ev, &maclar[i].ev_gol, &maclar[i].karsi, &maclar[i].karsi_gol);
        printf("%c\t%u\t%c\t%u\n", maclar[i].ev, maclar[i].ev_gol, maclar[i].karsi, maclar[i].karsi_gol);
        maclar[i].gecersiz = 0;
        i ++;
    }

	printf("\n");
	fclose(f_maclar);

	return i;
}
void maclari_kontrolet(struct mac maclar[MAC_SAYISI], int okunan_mac)
{
    int i, j, hata_var = 0;

    printf("Hata kontrolu:\n");
    for (i=0; i<okunan_mac; i++)
    {
        if (maclar[i].gecersiz == 1) { // Zaten isaretlenmisse kontrol etmeye gerek yok
            continue;
        }

        if(maclar[i].ev == maclar[i].karsi) {
            printf("HATA(%d): %c\t%u - %c\t%u\n", i+1, maclar[i].ev, maclar[i].ev_gol, maclar[i].karsi, maclar[i].karsi_gol);
            printf("Ayni takim kendi ile mac yapamaz! ^_^\n");

            maclar[i].gecersiz = 1; //flag ( ayni takim kendi ile mac yapmazsa onu puan tablosuna eklemiyor )
            hata_var = 1;   // hata varsa hata_var a 1 atiyor
            }


        for(j=0; j<i; j++)
        {
            if(maclar[i].ev == maclar[j].ev && maclar[i].karsi == maclar[j].karsi)
            {
                printf("HATA(%d-%d): %c\t%u - %c\t%u\n", i+1, j+1, maclar[i].ev, maclar[i].ev_gol, maclar[i].karsi, maclar[i].karsi_gol);
                printf("Bu mac zaten oynandi!\n");

                maclar[i].gecersiz = 1; //flag ( daha once oynan mac varsa mac puan tablosuna eklemiyor )
                hata_var = 1;   // hata varsa hata_var a 1 atiyor
            }
        }
    }

    if (!hata_var) {  //hata yoksa

        printf("Hata yok!\n");
    }

    printf("\n");
}
void maclari_yazdir(struct mac maclar[MAC_SAYISI], int okunan_mac)
{
    int i;

    printf("Gecerli Maclar Listesi:\n");

    for (i=0; i<okunan_mac; i++)
    {
        if (!(maclar[i].gecersiz)) {
            printf("%c\t%u - %c\t%u\n", maclar[i].ev, maclar[i].ev_gol, maclar[i].karsi, maclar[i].karsi_gol);
        }
    }

    printf("\n");
}
int takim_bul(char harf, struct takim t[], int takim_sayisi)    // takimlarin karsilasinca sirada oldugunu buluyor
{
    int i;

	for(i=0; i<takim_sayisi; i++)
	{
	    if (t[i].harf == harf) {

            return i;
	    }
	}
	return -1;
}
void puan_tablosu_hesapla(struct takim t[], int takim_sayisi, struct mac m[MAC_SAYISI], int okunan_mac, struct ayarlar cfg)
{
    int i, i_ev, i_karsi;

    for (i=0; i<okunan_mac; i++)
    {
        if (!(m[i].gecersiz)) {

            i_ev = takim_bul(m[i].ev, t, takim_sayisi);
            i_karsi = takim_bul(m[i].karsi, t, takim_sayisi);

            if (i_ev != -1 && i_karsi != -1) {

                t[i_ev].oyn_mac ++;
                t[i_karsi].oyn_mac ++;
                t[i_ev].atilangol += m[i].ev_gol;
                t[i_karsi].atilangol += m[i].karsi_gol;
                t[i_ev].yenilengol += m[i].karsi_gol;
                t[i_karsi].yenilengol += m[i].ev_gol;

                if (m[i].ev_gol > m[i].karsi_gol) {

                    t[i_ev].galibiyet ++;
                    t[i_karsi].maglubiyet ++;

                }else if (m[i].karsi_gol > m[i].ev_gol) {

                    t[i_karsi].galibiyet ++;
                    t[i_ev].maglubiyet ++;

                } else if (m[i].karsi_gol == m[i].ev_gol) {

                    t[i_karsi].beraberlik ++;
                    t[i_ev].beraberlik ++;
                }
            }
        }
    }

	for(i=0; i<takim_sayisi; i++)
	{
	 
	  t[i].averaj = t[i].atilangol - t[i].yenilengol;
      t[i].puan = t[i].galibiyet*cfg.winpoint + t[i].maglubiyet*cfg.losepoint + t[i].beraberlik*cfg.tiedpoint;
    }
}
void puan_tablosu_yazdir(struct takim t[], int takim_sayisi, int idx[])
{
    int i;

    printf("#         Isim OM GM MM BM ATG YDG AVG PO\n");

	for(i=0; i<takim_sayisi; i++)

	{
	    printf("%c %12s %2d %2d %2d %2d %3d %3d %3d %2d\n", t[idx[i]].harf, t[idx[i]].uzunisim,
            t[idx[i]].oyn_mac, t[idx[i]].galibiyet, t[idx[i]].maglubiyet, t[idx[i]].beraberlik,
            t[idx[i]].atilangol, t[idx[i]].yenilengol, t[idx[i]].averaj, t[idx[i]].puan);
	}

    printf("\n");
}
int isim_karsilastir(char a[12], char b[12])
{
    int i;

    for (i=0; i<12; i++) {

        if (tolower(a[i]) == tolower(b[i])) {
            continue;

        } else if (tolower(a[i]) > tolower(b[i])) {
            return 1;

        } else if (tolower(a[i]) < tolower(b[i])) {
            return 0;
        }
    }

    return 0;
}
void puan_tablosu_yazdir_harf(struct takim t[], int takim_sayisi)   //takimlari harf sirasina gore yazdirma
{
    int idx[takim_sayisi];

    int i, j, tmp;

	for(i=0; i<takim_sayisi; i++)
	{
	    idx[i] = i;
	}
	for(i=0; i<takim_sayisi - 1; i++) // Bubble sort
	{
        for(j=0; j<takim_sayisi - i - 1; j++)
        {
            if (tolower(t[idx[j]].harf) > tolower(t[idx[j+1]].harf)) // Buyuk-kucuk harfe duyarliligi kaldir
            {
                tmp = idx[j];
                idx[j] = idx[j+1];
                idx[j+1] = tmp;
            }
        }
	}

    printf("Takma isime gore Puan Tablosu:\n");
	puan_tablosu_yazdir(t, takim_sayisi, idx);
}
void puan_tablosu_yazdir_isim(struct takim t[], int takim_sayisi)
{
    int idx[takim_sayisi];
    int i, j, tmp;
	for(i=0; i<takim_sayisi; i++)
	{
	    idx[i] = i;
	}
	for(i=0; i<takim_sayisi - 1; i++) // Bubble sort
	{
        for(j=0; j<takim_sayisi - i - 1; j++)
        {
            if (isim_karsilastir(t[idx[j]].uzunisim, t[idx[j+1]].uzunisim)) // Buyuk-kucuk harfe duyarliligi kaldir
            {
                tmp = idx[j];
                idx[j] = idx[j+1];
                idx[j+1] = tmp;
            }
        }
	}

    printf("Uzun isime gore Puan Tablosu:\n");
	puan_tablosu_yazdir(t, takim_sayisi, idx);
}
void puan_tablosu_yazdir_puan(struct takim t[], int takim_sayisi)   //puana gore siralama
{
    int idx[takim_sayisi];
    int i, j, tmp;
	for(i=0; i<takim_sayisi; i++)
	{
	    idx[i] = i;
	}
	for(i=0; i<takim_sayisi - 1; i++) // Bubble sort
	{
        for(j=0; j<takim_sayisi - i - 1; j++)
        {
            if (t[idx[j]].puan < t[idx[j+1]].puan)
            {
                tmp = idx[j];
                idx[j] = idx[j+1];
                idx[j+1] = tmp;
            }
        }
	}

    printf("Puana gore Puan Tablosu:\n");
	puan_tablosu_yazdir(t, takim_sayisi, idx);
}
void menu(struct mac maclar[], int okunan_mac, struct takim takimlar[], int takim_sayisi){
    int secenek;

	while (1) {
        printf("Menu:\n");
        printf("1) Takma isime gore puan tablosu\n");
        printf("2) Uzun isime gore puan tablosu\n");
        printf("3) Puana gore puan tablosu\n");
        printf("4) Mac ekle\n");
        printf("5) Bitir\n");
        scanf("%d", &secenek);

        switch(secenek) {
        case 1:
            puan_tablosu_yazdir_harf(takimlar, takim_sayisi);
            break;
        case 2:
            puan_tablosu_yazdir_isim(takimlar, takim_sayisi);
            break;
        case 3:
            puan_tablosu_yazdir_puan(takimlar, takim_sayisi);
            break;
        case 4:
            printf("[EV] [EV_GOL] [KARSI] [KARSI_GOL]: ");
            scanf("%s %u %s %u", &maclar[okunan_mac].ev, &maclar[okunan_mac].ev_gol, &maclar[okunan_mac].karsi, &maclar[okunan_mac].karsi_gol);
            maclar[okunan_mac].gecersiz = 0;
            okunan_mac ++;
            maclari_kontrolet(&maclar, okunan_mac);
            break;
        case 5:
            return;
        }
	}
}

int main()
{

	struct ayarlar cfg; // ayarlar structinda cfg degiskeni

	ayarlari_oku(&cfg);

	struct takim takimlar[cfg.takim_sayisi];   //takim structinda takimlar dizisi
	struct mac maclar[MAC_SAYISI];
	int okunan_mac; //okunan maclari tutan degisken

	takimlari_oku(takimlar, cfg.takim_sayisi);  //takimlara okuma fonksiyonu
	okunan_mac = maclari_oku(&maclar);   //maclari_oku fonksiyonu okunan mac sayisini dosyaya okuyor
	maclari_kontrolet(&maclar, okunan_mac);  //hata kontrolu yapan fonksiyon(hatalı mac uyarma)
	maclari_yazdir(maclar, okunan_mac);   //maclari filtreleme
	puan_tablosu_hesapla(&takimlar, cfg.takim_sayisi, maclar, okunan_mac, cfg);
    menu(maclar, okunan_mac, takimlar, cfg.takim_sayisi);


	return 0;
}
