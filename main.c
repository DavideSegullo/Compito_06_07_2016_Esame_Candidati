#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 21

typedef enum{false, true} bool;

typedef struct {
    int n_prova;
    int voto;
} dati_prova;

typedef struct prova{
    dati_prova dati;
    struct prova *next;
} prova;

typedef struct {
    bool esito;
    float media;
    int n_prove;
    prova *prove;
} esame;

typedef struct {
    char nome[N];
    char cognome[N];
    int matricola;
    esame esame_sostenuto;
} dati_candidato;

typedef struct candidato{
    dati_candidato dati;
    struct candidato *next;
} candidato;

bool crea_lista_prove(prova **prove, dati_prova dati){

    prova *p = (prova *)malloc(sizeof(prova));

    if (p == NULL)
        return false;

    p->next = NULL;
    p->dati = dati;

    *prove = p;

    return true;
}

bool inserimento_prova(prova **prove, dati_prova dati){

    if(*prove == NULL)
        return crea_lista_prove(prove, dati);

    prova *current = NULL;
    prova *successive = *prove;

    while(successive != NULL && successive->dati.n_prova < dati.n_prova){
        current = successive;
        successive = successive->next;
    }

    prova *p = (prova *)malloc(sizeof(prova));

    if (p==NULL)
        return false;

    p->next = NULL;
    p->dati = dati;

    if (current == NULL){
        p->next = *prove;
        *prove = p;
        return true;
    }

    if (successive == NULL){
        current->next = p;
        return true;
    }

    p->next = successive;
    current->next = p;
    return true;
}

bool crea_lista_candidati(candidato **candidati, dati_candidato dati, dati_prova d_prova){

    candidato *p = (candidato *)malloc(sizeof(candidato));

    if (p == NULL)
        return false;

    p->next = NULL;
    p->dati = dati;

    inserimento_prova(&(p->dati.esame_sostenuto.prove), d_prova);

    *candidati = p;

    return true;
}

bool inserimento_candidato(candidato **candidati, dati_candidato dati, dati_prova d_prova){

    if(*candidati == NULL)
        return crea_lista_candidati(candidati, dati, d_prova);

    candidato *current = NULL;
    candidato *successive = NULL;

    successive = *candidati;

    while(successive != NULL){

        if (successive->dati.matricola == dati.matricola){
            inserimento_prova(&(successive->dati.esame_sostenuto.prove),d_prova);
            return true;
        }

        if (successive->dati.matricola < dati.matricola)
            break;

        current = successive;
        successive = successive->next;
    }

    candidato *p = (candidato *)malloc(sizeof(candidato));

    if (p==NULL)
        return false;

    p->dati = dati;
    p->dati.esame_sostenuto.prove = NULL;
    p->next = NULL;

    inserimento_prova(&(p->dati.esame_sostenuto.prove),d_prova);

    if (current == NULL){
        p->next = *candidati;
        *candidati = p;
        return true;
    }

    if (successive == NULL){
        current->next = p;
        return true;
    }

    p->next = successive;
    current->next = p;
    return true;
}

bool lettura_file(candidato **candidati){

    FILE *file;

    file = fopen("esame.txt","r");

    if (file==NULL)
        return false;

    dati_candidato d_cand;
    d_cand.esame_sostenuto.prove = NULL;
    dati_prova d_prova;

    while (!feof(file)){
        fscanf(file, "%s\t%s\t%d\t%d\t%d\n", d_cand.nome, d_cand.cognome, &(d_cand.matricola), &(d_prova.n_prova), &(d_prova.voto));
        printf("Lettura: %s %s Voto: %d\n",d_cand.nome, d_cand.cognome, d_prova.voto);
        inserimento_candidato(candidati, d_cand, d_prova);
    }

    fclose(file);
    return true;
}

bool stampa_esame_sup(candidato **candidati){

    if (*candidati == NULL){
        printf("Lista vuota\n");
        return false;
    }

    printf("Elenco dei candidati che hanno superato l'esame:\n");
    printf("Nome\tCognome\tMedia\n");

    for(candidato *p = *candidati; p!=NULL; p=p->next){
        float media = 0.0;
        int supSeven = 0;
        int n_prove = 0;

        for(prova *q = p->dati.esame_sostenuto.prove; q!=NULL; q=q->next){

            if(q->dati.voto>=7)
                supSeven++;

            media+=q->dati.voto;
            n_prove++;
        }

        media = media/n_prove;

        p->dati.esame_sostenuto.media = media;
        p->dati.esame_sostenuto.n_prove = n_prove;

        p->dati.esame_sostenuto.esito = (media >= 5.0 && supSeven>=2 && n_prove==4) ? true : false;

        if (p->dati.esame_sostenuto.esito)
            printf("%s\t%s\t%f\n",p->dati.nome, p->dati.cognome, p->dati.esame_sostenuto.media);
    }

    return true;
}

bool risultati_esame_candidato(candidato *candidati){

    int n_matricola;

    printf("Inserire il numero matricola del candidato:\n");
    scanf("%d",&n_matricola);

    candidato *p = NULL;

    for (p=candidati; p!=NULL && p->dati.matricola!=n_matricola; p=p->next);

    if (p==NULL){
        printf("Nessun candidato trovato con il numero di matricola inserito\n");
        return false;
    }

    printf("Candidato trovato..\n");
    printf("Nome: %s\nCognome: %s\n",p->dati.nome,p->dati.cognome);

    prova *q = NULL;
    for (q=p->dati.esame_sostenuto.prove; q!=NULL; q=q->next)
        printf("Prova n.%d\tVoto: %d\n",q->dati.n_prova, q->dati.voto);

    printf("Media: %f\n",p->dati.esame_sostenuto.media);

    return true;
}

bool salvataggio_risultato(candidato *candidati){

    int n_matricola;

    printf("Inserire il numero matricola del candidato:\n");
    scanf("%d",&n_matricola);

    candidato *p = NULL;

    for (p=candidati; p!=NULL && p->dati.matricola!=n_matricola; p=p->next);

    if (p==NULL){
        printf("Nessun candidato trovato con il numero di matricola inserito\n");
        return false;
    }

    FILE *file = NULL;

    file = fopen("esame_risultati.txt","w");

    if (file==NULL)
        return false;

    fprintf(file,"%s\t%s\t%d\n",p->dati.nome,p->dati.cognome,p->dati.matricola);

    for(prova *q=p->dati.esame_sostenuto.prove; q!=NULL; q=q->next)
        fprintf(file,"\t- %d\t%d\n",q->dati.n_prova,q->dati.voto);

    fclose(file);
    return true;
}

void stampa(candidato *candidati){

    for(candidato *p=candidati; p!=NULL; p=p->next)
        printf("%s %s %d\n",p->dati.nome, p->dati.cognome, p->dati.matricola);

}

int main()
{
    candidato *candidati = NULL;

    lettura_file(&candidati);
    stampa_esame_sup(&candidati);
    risultati_esame_candidato(candidati);
    salvataggio_risultato(candidati);

    return 0;
}
