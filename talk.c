/*
(30 puncte) Talk bazat doar pe semnale: un acelasi program este lansat de
la 2 terminale diferite de acelasi utilizator,obtinand 2 procese diferite.
Fiecare proces citeste de la tastatura PID-ul celuilalt. Fiecare proces
are un tabel care asociaza cate un semnal != SIGKILL, SIGCONT, SIGSTOP
celor 26 litere, blank-ului si capului de linie. Fiecare proces citeste
intr-un ciclu cate o linie de la tastatura, apoi o parcurge si trimite
celuilalt proces semnalul asociat fiecarui caracter din ea (inclusiv
blank-urile si capul de linie). De asemenea, fiecare proces, la primirea
unui asemenea semnal, va afla caracterul corespunzator si-l va scrie pe
ecran. Se va asigura protectia la pierderea unor semnale si se va
asigura ca semnalele trimise de un proces sa fie primite de celalalt in
aceeasi ordine (de exemplu un proces nu va emite semnalul corespunzator
unui caracter decat daca a primit confirmarea ca celalalt proces a
tratat semnalul pentru caracterul precedent).*/
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define CONFIRMCONEX 20
#define CANCEL 21
#define CONFIRMAREMESAJ 63
#define SPACE 61
#define ENTER 62

char buf[256];
pid_t pa;
int poz;
int conexiune;

void cancel(int n){

        signal(n,cancel);
        poz = 0;
        buf[0] = '\0';
        kill(pa,CONFIRMAREMESAJ);
}

void conex(int n){

        signal(n, conex);
        conexiune = 1;
}

void interrupt(int n) {

        if(kill(pa, SIGINT) == -1)
		perror("Error");
        printf("Conexiune incheiata!\n");
        exit(1);
}

void h1(int n) {

       signal(n,h1);
}

void h2(int n) {

        signal(n,h2);
        puts(buf);
	if(kill(pa,CONFIRMAREMESAJ) == -1){
		perror("Error");
                exit(1);
        }
}

void confirm(int n){

        signal(n,confirm);
}

void hl(int n){

	signal(n,hl);
        if( n == SPACE)
                buf[poz++] = ' ';

        else if( n == ENTER){
                buf[poz] = '\0';
                poz = 0;

        } else
                buf[poz++] = n + 62;

        if(kill(pa,SIGUSR1) == -1){
		perror("Error");
                exit(1);
        }
	//sleep(2);
}
int init(sigset_t* ms, sigset_t* ms1, sigset_t* ms2, sigset_t* ms3, sigset_t* ms4){
//--------
        int i;
        if(sigemptyset(ms) == -1){
                perror("Error");
                return -1;
        }
//--------
        if(sigemptyset(ms1) == -1){
                perror("Error");
                return -1;
        }
        if(sigaddset(ms1, SIGUSR2) == -1){
                perror("Error");
                return -1;
        }

        if(sigaddset(ms1, CONFIRMAREMESAJ) == -1){
                perror("Error");
                return -1;
        }


        for (i = 35; i < 63; i++ )
                if(sigaddset(ms1, i) == -1){
                        perror("Error");
                        return -1;

                }

//--------
        if(sigemptyset(ms2) == -1){
                perror("Error");
                return -1;
        }
        if(sigaddset(ms2, SIGUSR2) == -1){
                perror("Error");
                return -1;
        }
        if(sigaddset(ms2, SIGUSR1) == -1){
                perror("Error");
                return -1;
        }

        if(sigaddset(ms2, CONFIRMAREMESAJ) == -1){
                perror("Error");
                return -1;
        }

        for (i = 35; i < 63; i++ )
                if(sigaddset(ms2, i) == -1){
                        perror("Error");
                        return -1;
                }

//--------
        if(sigemptyset(ms3) == -1){
                perror("Error");
                return -1;
        }

        if(sigaddset(ms3, SIGUSR2) == -1){
                perror("Error");
                return -1;
        }

        if(sigaddset(ms3, SIGUSR1) == -1){
                perror("Error");
                return -1;
        }

        for (i = 35; i < 63; i++ )
                if(sigaddset(ms3, i) == -1){
                        perror("Error");
                        return -1;
                        }

        if(sigaddset(ms3, CONFIRMAREMESAJ) == -1){
                        perror("Error");
                        return -1;
        }

//--------
        if(sigemptyset(ms4) == -1){
                perror("Error");
                return -1;
        }

        if(sigaddset(ms4, SIGUSR2) == -1){
                perror("Error");
                return -1;
        }

        if(sigaddset(ms4, SIGUSR1) == -1){
                perror("Error");
                return -1;
        }

        for (i = 35; i < 63; i++ )
                if(sigaddset(ms4, i) == -1){
                        perror("Error");
                        return -1;
                }
        return 0;
}

int main(int argv, char** argc){

        sigset_t ms, ms1, ms2, ms3, ms4;
        int i, j, OK = 1;
        char c;
        signal(CANCEL,cancel);
        signal(SIGUSR1,h1);
        signal(SIGUSR2,h2);
        signal(SIGINT,interrupt);
        signal(CONFIRMAREMESAJ, confirm);
        signal(CONFIRMCONEX,conex);
        for (i = 35; i < 63; i++ )
                signal(i, hl);

        if(init(&ms, &ms1, &ms2, &ms3, &ms4) == -1)
                return -1;

        printf("PID propriu: %d \n",getpid());

        printf("PID advers: ");scanf("%d%c",&pa,&c);

        if(kill(pa,0) == -1){
                fprintf(stderr,"PID advers gresit!\n");
                return -1;
        }

        if(kill(pa,CONFIRMCONEX) == -1){
                perror("Error");
                return -1;
        }

        while(!conexiune){
                sleep(1);
        }

        printf("S-a stabilit conexiunea!\n");

        while(1){

                if(sigprocmask(SIG_SETMASK, &ms, NULL) == -1){
                        perror("Error");
                        return -1;
                }

                do fgets(buf,256,stdin);
                while(!strlen(buf));

                for( j = 0; j < strlen(buf); j++){

                        if(sigprocmask(SIG_SETMASK, &ms2, NULL) == -1){
                                perror("Error");
                                return -1;
                        }

                        if(buf[j] == ' '){
                                if(kill(pa, SPACE) == -1){
                                        perror("Error");
                                        return -1;

                                }

        		}

                        else if(buf[j] == '\n'){
                                if(kill(pa, ENTER) == -1){
                                        perror("Error");
                                        return -1;

                                }

        		}

                        else if(buf[j] >= 'a' && buf[j] <= 'z'){
                                if(kill(pa, buf[j] - 62) == -1){
                                        perror("Error");
                                        return -1;

                                }

        		}

                        else {
                                buf[strlen(buf) - 1] = '\0';
                                fprintf(stderr,"Mesajul \"%s\" nu a putut fi trimis.\n", buf);
                                OK = 0;
                                break;
                                }
//			sleep(2);
                        sigsuspend(&ms1);

                        }



                        if(sigprocmask(SIG_SETMASK, &ms3, NULL) == -1){
                                perror("Error");
                                return -1;
                        }

                        if(OK){
                                if(kill(pa,SIGUSR2) == -1){
                                        perror("Error");
                                        return -1;
                                }
                        }
                        else
                                if(kill(pa,CANCEL) == -1){
                                        perror("Error");
                                        return -1;
                                }

//                        sleep(2);
                        sigsuspend(&ms4);
                        OK = 1;
                }

        return 0;

}
