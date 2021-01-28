#include <stdio.h>
#include <unistd.h> //isletim sistemi API'sina erisim saglayan k�t�phane
#include <stdlib.h>
#include <pthread.h> //thread yapisinin �alisabilmesi i�in k�t�phane
#include <semaphore.h> /* Birden fazla process'in es zamanli �alismasi halinde birbirleri i�in risk
                          teskil ettigi zamanlarda senkronize olmasini sunan k�t�phanedir.*/
#define CHAIRS 5
void *musteri(void *numara);
void beklemeSure(int saniye);
void *berber(void *);
//Semaphorelarin tanimi
sem_t berberKoltugu; //Berber koltuguna �zel izin saglar
sem_t beklemeOdasi; //Bekleme odasina girmesine izin verilen m�steri sayisini sinirlar
sem_t berberYatagi; //M�sterinin gelene kadar berberin uyumasina izin vermek i�in kullanilir.
sem_t keserkenBekleme; //Berberin sa�larini kesene kadar m�sterinin beklemesini sa�lamak i�in kullanilir.

int son =0;//T�m kesme islemi bittiginde threadi durdurmak i�in isaretlenicek

void *musteri(void *numara){
int num = *(int *)numara;
beklemeSure(5);
printf("%d Numarali Musteri berber dukkanina geldi.", num+1);
 if (sem_trywait(&beklemeOdasi) == -1) {
        printf("Bekleme odasi full.%d.Musteri gidiyor.\n",num+1); //Bekleme odasinda yer yoksa musteri gidiyor.
        return 0;
    }
printf("%d Numarali Musteri Bekleme odasina giriyor.\n", num+1); //Berber koltu�unun bo�almas�n� bekliyor
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
     beklemeSure(1);//M�sterinin sa�ini kesmek i�in 1 mslik zaman atadik ama bu zaten �nemsiz ��nk� bekleme fonkisyonunda direk 1'e esitledik
     printf("Berber musterinin sacini kesti.\n");
     sem_post(&keserkenBekleme);//Kesim bitince m�steriyi yolladik
    }
    else {
         printf("Berber Dukkani kapatiyor.\n");
    }
   }

}

int main(int argc, char *argv[])
{
    pthread_t berbertid;
    pthread_t mustid[15];//Max M�steri sayisini ayarladigimiz yer
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
        pthread_create(&mustid[i], NULL, musteri, (void *)&maxMusteriSayi[i]); //M�sterileri yarattik.
    }
    for (i = 0; i < numMusteri; i++) { //bitmesini beklemesi i�in threadleri joinliyoruz
        pthread_join(mustid[i],NULL);
    }
    son=1;
     sem_post(&berberYatagi); // Berber d�kkani kapatabilsin diye uyandiriyoruz.
    pthread_join(berbertid,NULL);
    system("PAUSE");




return 0;
}


