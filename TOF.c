#include <stdio.h>
#include <stdlib.h>
#include "LOV7C.h"
#include "TOF.h"
#define enregistrements 25
#define max 500

/*****************************************************MANIPULATION DE L'ENTETE ***********************************************************************/
void AFF_ENTETE_TOF(TOF *fichier, int i, int valeur)
{
	if (i == 1)(fichier->entete).dernier_bloc = valeur;	// modifier le numéro représentant le dernier bloc
}

int ENTETE_TOF(TOF *fichier, int i)
{
	if (i == 1) return (fichier->entete).dernier_bloc;
	else return -1;	// Si elle n'existe pas on retourne -1
}

void AFFICHER_ENTETE_TOF(TOF *fichier)
{
	printf("-\n");
	printf("Le numero de dernier bloc est (%d) \n", ENTETE_TOF(fichier, 1));
	printf("-\n");
}

/***********************************************************LIREDIR/ECRIREDIR **************************************************************************/
void LIREDIR_TOF(TOF *fichier, int i, Buffer_TOF *buf)
{
	fseek(fichier->file, (sizeof(Entete_TOF) + sizeof(Tbloc_TOF) *(i - 1)), SEEK_SET);
	fread(buf, sizeof(Buffer_TOF), 1, fichier->file);
	rewind(fichier->file);
}

void ECRIREDIR_TOF(TOF *fichier, int i, Buffer_TOF*buf)
{
	fseek(fichier->file, sizeof(Entete_TOF) + sizeof(Tbloc_TOF) *(i - 1), SEEK_SET);
	fwrite(buf, sizeof(Buffer_TOF), 1, fichier->file);
	rewind(fichier->file);
}

/***********************************************************OUVRIR/FERMER ****************************************************************************/
TOF* OUVRIR_TOF(char nom_physique[], char mode)
{
	TOF *fichier = (TOF*) malloc(sizeof(TOF));
	Buffer_TOF buf;
if ((mode == 'A') || (mode == 'a'))
	{
		fichier->file = fopen(nom_physique, "rb+");
		fread(&(fichier->entete), sizeof(Entete_TOF), 1, fichier->file);
	}

else
	{
		if ((mode == 'N') || (mode == 'n'))
		{
			fichier->file = fopen(nom_physique, "wb+");
			AFF_ENTETE_TOF(fichier, 1, 1);
			fwrite(&(fichier->entete), sizeof(Entete_TOF), 1, fichier->file);
			sprintf(buf.tab, "%s", "");
			ECRIREDIR_TOF (fichier, 1, &buf);
		}
		else
		{
			printf("Erreur lors de l'ouverture du fichier veuillez reessayer !\n");
		}
	}

	return (fichier);
}

void FERMER_TOF(TOF *fichier)
{
	rewind(fichier->file);
	fwrite(&(fichier->entete), sizeof(Entete_TOF), 1, fichier->file);
	rewind(fichier->file);
	fclose(fichier->file);
}

/**********************************************************ALLOCATION D'UN BLOC *********************************************************************/

void ALLOCBLOC_TOF(TOF *fichier)
{
	Buffer_TOF *buf = (Buffer_TOF*) malloc(sizeof(Buffer_TOF));
	printf("\n");
	AFF_ENTETE_TOF(fichier, 1, ENTETE_TOF(fichier, 1) + 1);
	buf->nb_car_insr = 0;
	sprintf(buf->tab, "%s", "");
	ECRIREDIR_TOF(fichier, ENTETE_TOF(fichier, 1), buf);
}

void AFFICHER_BLOC_TOF(TOF *fichier, int i)
{
	Buffer_TOF *buf = (Buffer_TOF*) malloc(sizeof(Buffer_TOF));
	LIREDIR_TOF(fichier, i, buf);
	int j;
	printf("Le bloc numero (%d) et nb est %d \n", i, buf->nb_car_insr);
	for (j = 0; j < buf->nb_car_insr; j++)
	{
		printf("%c", buf->tab[j]);
	}

	printf("\n");
	printf("Le nombre de caracteres dans ce bloc est (%d) .\n", buf->nb_car_insr);
	printf("\n");
}

void AFFICHER_FICHIER_TOF(char nom[30])
{
	TOF * fichier0;
	int i = 1;
	fichier0 = OUVRIR_TOF(nom, 'A');
	if (fichier0 != NULL)
	{
		while (i <= ENTETE_TOF(fichier0, 1))
		{
			AFFICHER_BLOC_TOF(fichier0, i);
			i++;
		}
	}
	else
	{
		printf("Erreur lors de l'ouverture du fichier veuillez reessayer !\n");
	}
}

/******************************************************************NEEDS ****************************************************************************/
void CREATION_TOF()
{
    LOV7C *FICHIER = OUVRIR_LOV7C("OUVRAGES.bin", 'A');
	TOF *FICHIER0 = OUVRIR_TOF("Periodiques.bin", 'N');
	Buffer_LOV7C *BUF = (Buffer_LOV7C*) malloc(sizeof(Buffer_LOV7C));
	Buffer_TOF *BUF0 = (Buffer_TOF*) malloc(sizeof(Buffer_TOF));
	int i = 1, i0 = 1, j = 0, m = 0, x=0, l=0,taille;
	LIREDIR_LOV7C(FICHIER, i, BUF);	// on va lire le premier bloc
	while (i <= ENTETE_LOV7C(FICHIER, 2))
	{
		taille = RECUPERER(FICHIER, 2, &i, &j, BUF);
		for (x = l+3; x < l+30; x++)
		{
			if (m < max)
			{
				BUF0->tab[m] = BUF->tab[x];
				m++;
			}
        else
			{
				BUF0->nb_car_insr = m;
				ECRIREDIR_TOF(FICHIER0, i0, BUF0);
				ALLOCBLOC_TOF(FICHIER0);
				x--;
				BUF0->tab[0] = BUF->tab[x];
				m = 0;
                i0++;
			}

	    }
	    BUF0->tab[m] = '#';
	    m++;
		l+=taille;
		j += taille - 2;
		if (l == (BUF->nb_car_insr))
		{
			i++;
			LIREDIR_LOV7C(FICHIER, i, BUF);
			j = 0;
			l = 0;
		}
	}
	BUF0->nb_car_insr = m;
	ECRIREDIR_TOF(FICHIER0, i0, BUF0);
	FERMER_TOF(FICHIER0);
	printf("PERIODIQUES.bin: \n");
	AFFICHER_ENTETE_TOF(FICHIER0);

}
