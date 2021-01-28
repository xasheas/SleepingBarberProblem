#include <stdio.h>
#include <unistd.h> //isletim sistemi API'sina erisim saglayan kütüphane
#include <stdlib.h>
#include <pthread.h> //thread yapisinin çalisabilmesi için kütüphane
#include <semaphore.h> /* Birden fazla process'in es zamanli çalismasi halinde birbirleri için risk
                          teskil ettigi zamanlarda senkronize olmasini sunan kütüphanedir.*/
#define CHAIRS 5
void *musteri(void *numara);
void beklemeSure(int saniye);
void *berber(void *);
//Semaphorelarin tanimi
sem_t berberKoltugu; //Berber koltuguna özel izin saglar
sem_t beklemeOdasi; //Bekleme odasina girmesine izin verilen müsteri sayisini sinirlar
sem_t berberYatagi; //Müsterinin gelene kadar berberin uyumasina izin vermek için kullanilir.
sem_t keserkenBekleme; //Berberin saçlarini kesene kadar müsterinin beklemesini saðlamak için kullanilir.

int son =0;//Tüm kesme islemi bittiginde threadi durdurmak için isaretlenicek

void *musteri(void *numara){
int num = *(int *)numara;
beklemeSure(5);
printf("%d Numarali Musteri berber dukkanina geldi.", num+1);
 if (sem_trywait(&beklemeOdasi) == -1) {
        printf("Bekleme odasi full.%d.Musteri gidiyor.\n",num+1); //Bekleme odasinda yer yoksa musteri gidiyor.
        return 0;
    }
printf("%d Numarali Musteri Bekleme odasina giriyor.\n", num+1); //Berber koltuðunun boþalmasýný bekliyor
sem_wait(&berberKoltugu); //Berber koltugu bosaldigi icin bekleme odasindaki yerini birakiyor.
sem_post(&beklemeOdasi); // Berberi uyandiriyoruz.
printf("%d Numarali Musteri berberi uyandirdi.\n", num+1);
sem_post(&berberYatagi);
sem_wait(&keserkenBekleme);// Berberin sacini kesmesini bekliyor
sem_post(&berberKoltugu);// Koltuktan kalkiyor
printf("%d Numarali musteri berber dukkanindan ayriliyor.\n", num+1);

}
void beklemeSure(int saniye){
    int bekleme = 1; //Bekleme Suresini ayarladik
     sleep(bekleme);
}
void *berber(void *onemsiz){
    while (!son) { //Biri berberi uyandirana kadar uyuyor.
    printf("Berber uyuyor.\n");
    sem_wait(&berberYatagi);
    if (!son)
    {
     printf("Berber Sac kesiyor\n");
     beklemeSure(1);//Müsterinin saçini kesmek için 1 mslik zaman atadik ama bu zaten önemsiz çünkü bekleme fonkisyonunda direk 1'e esitledik
     printf("Berber musterinin sacini kesti.\n");
     sem_post(&keserkenBekleme);//Kesim bitince müsteriyi yolladik
    }
    else {
         printf("Berber Dukkani kapatiyor.\n");
    }
   }

}

int main(int argc, char *argv[])
{
    pthread_t berbertid;
    pthread_t mustid[15];//Max Müsteri sayisini ayarladigimiz yer
    int i,temp,numMusteri,numKoltuk; int maxMusteriSayi[15];
    printf("Musteri sayisini giriniz:");
    scanf("%d",&temp);
    numMusteri = temp;
    numKoltuk = CHAIRS;
    for(i=0; i<15; i++){
        maxMusteriSayi[i]=i;
    }
    sem_init(&beklemeOdasi, 0, numKoltuk);
    sem_init(&berberKoltugu, 0, 1);
    sem_init(&berberYatagi, 0, 0);
    sem_init(&keserkenBekleme, 0, 0);

    pthread_create(&berbertid, NULL, berber, NULL); //Berberi yarattik.

    for (i = 0; i < numMusteri; i++) {
        pthread_create(&mustid[i], NULL, musteri, (void *)&maxMusteriSayi[i]); //Müsterileri yarattik.
    }
    for (i = 0; i < numMusteri; i++) { //bitmesini beklemesi için threadleri joinliyoruz
        pthread_join(mustid[i],NULL);
    }
    son=1;
     sem_post(&berberYatagi); // Berber dükkani kapatabilsin diye uyandiriyoruz.
    pthread_join(berbertid,NULL);
    system("PAUSE");




return 0;
}


