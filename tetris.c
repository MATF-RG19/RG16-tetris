#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include <string.h>
#include <math.h>

/* Postavljam ID i interval tajmera */
#define TIMER_ID 0
#define TIMER_INTERVAL 400

/* Postavljam dimenzije matrice */
#define MAX_X 12
#define  MAX_Y 22

#define MAX_LENGTH_STRING 50

/* Deklarisanje callback funkcija */
static void on_keyboard(unsigned char key, int x, int y);
static void on_specijalkey(int key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_timer(int value);

/* Deklarisanje pomocnih funkcija */
static void nacrtaj_okvir(void);
static void sakri_figuru(void);
static void iscrtaj_figuru(int x);
static void popuni_matricu(void);
static void resetuj_matricu(void);
static void ispisi_matricu_u_terminalu(void);
static void kolizije(void);
static void rotacije_na_granicama(void);
static void iscrtavanje_matrice_na_ekran(void);
static void brisanje_reda(void);

static void renderStrokeString(int x, int y,int z,void* font, char *string);



/* Deklarisanje globalnih promenljivi */
static int window_width, window_height;
static int animation_ongoing;
static int matrica[MAX_Y][MAX_X];
static int rotiraj_scenu = 0;
static int random_broj;
static int broj_rotacija = 0;
static int jedinstveni_broj;
static int rotiraj = 0;
static int transliraj = 0;
static int x_trenutno = 5, y_trenutno = 0;
static int levo_desno = 0;
static int pala_je_figura = 1;
static int x_stop,y_stop;
static int skor = 0;

static char word[MAX_LENGTH_STRING]; /*Koristimo staticku alokaciju za reci koje ispisujemo, dali smo pretpostavku o njihovoj duzini */


int main(int argc, char **argv){

	/* GLUT inicijalizacija */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	/* Pravljenje prozora */
	glutInitWindowSize(400, 700);
	glutInitWindowPosition(500, 20);
	glutCreateWindow(argv[0]);

	/* Registracija callback funkcija */
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
	glutSpecialFunc(on_specijalkey);

	/* Inicijalizuje se seed za random broj */
	srand(time(NULL));

	random_broj = (rand() % 7) + 1;
	animation_ongoing = 0;
	broj_rotacija = 0;

	resetuj_matricu();
	ispisi_matricu_u_terminalu();

	/* OpenGL inicijalizacija */
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);

	/* Program ulazi u glavnu petlju */
	glutMainLoop();

	return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
			exit(0);
			break;
		case 's':
		case 'S':
			/* Pokretanje igre */
			if(!animation_ongoing){
				glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
				animation_ongoing = 1;
			}
			glutPostRedisplay();
			break;
		case 'p':
		case 'P':
			/* Pauziranje igre */
			animation_ongoing = 0;
			break;
		case 'r':
		case 'R':
			/* Restartovanje igre */
			random_broj = (rand() % 7) + 1;
			levo_desno = 0;
			broj_rotacija = 0;
			rotiraj = 0;
			transliraj = 0;
			x_trenutno = 5;
			y_trenutno = 0;
			skor = 0;
			animation_ongoing = 0;
			resetuj_matricu();
			glutPostRedisplay();
			break;
		/* Rotiranje scene */
		case 'q':
		case 'Q':
			rotiraj_scenu +=5;
			glutPostRedisplay();
			break;
		case 'w':
		case 'W':
			rotiraj_scenu -=5;
			glutPostRedisplay();
			break;
		case 32:			
			break;
	}

}

static void on_specijalkey(int key, int x, int y){
	switch(key){
		case GLUT_KEY_UP:
			if (animation_ongoing)
			{
				rotacije_na_granicama();
				glutPostRedisplay();
			}
			break;
		case GLUT_KEY_DOWN:
			break;
		case GLUT_KEY_LEFT:
			if(animation_ongoing)
				levo_desno = -1;
			break;
		case GLUT_KEY_RIGHT:
			if(animation_ongoing)
				levo_desno = 1;
			break;
	}
	glutPostRedisplay();
}

static void on_reshape(int width, int height){

	/* Postavlja se viewpost */
	glViewport(0, 0, width, height);

	/* Postavljaju se parametri projekcije */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float) width / height, 1, 30);
}

static void on_display(void){

	/* Brise se predhodni sadrzaj prozora */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Podeesava se tacka pogleda */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	15, 15, 6,
				15, 15, 0,
				0, 1, 0);
	glTranslatef(15, 15, 0);
	glRotatef(rotiraj_scenu, 0, 1, 0);

	/* Iscrtavam okvir */
	glPushMatrix();
		nacrtaj_okvir();
	glPopMatrix();

	/* Sakrivam figuru na vrhu */
	glPushMatrix();
		sakri_figuru();
	glPopMatrix();

	/* Iscrtavam figuru */
	glPushMatrix();
		iscrtaj_figuru(random_broj);
	glPopMatrix();

	/* Iscrtavam matricu */
	glPushMatrix();
		iscrtavanje_matrice_na_ekran();
	glPopMatrix();

	/* Ispisujemo skor */
	glPushMatrix();
		    sprintf(word,"Score: %d", skor);
		    /* Postavljamo koordinate ispisivanja teksta*/
		    const int x = 100;
		    const int y = -2100;
		    const int z = 0;
		    glPushMatrix();
		        glPushAttrib(GL_LINE_BIT);
		            glLineWidth(4); /*Postavljamo debljinu linije */
		            renderStrokeString(x,y,z,GLUT_STROKE_MONO_ROMAN,word);
		        glPopAttrib();
		    glPopMatrix();

	glPopMatrix();
	glutSwapBuffers();
}

/* Kod za ispisivanje teksta preuzet od kolege :D https://github.com/MATF-RG19/RG42-snake-3d/blob/master/Sources/drawing.c */
static void renderStrokeString(int x, int y,int z,void* font, char *string) 
{
    int len; /*duzina stringa */
    glColor3f(1,0,0); /*Postavljanje boje teksta */
    /*Postavljamo dimenzije slova */
    glScalef(0.001,0.001,1);
    glTranslatef(x,y,z);
    len = strlen(string);
    int i;
    for (i = 0; i < len; i++)
        glutStrokeCharacter(font, string[i]);

}


static void on_timer(int value){
	if(value != TIMER_ID)
		return;

	if (animation_ongoing)
	{
		kolizije();
		popuni_matricu();
		brisanje_reda();
		if(pala_je_figura){
			random_broj = (rand() % 7) + 1;
			levo_desno = 0;
			broj_rotacija = 0;
			rotiraj = 0;
			transliraj = 0;
			x_trenutno = 5;
			y_trenutno = 0;
			pala_je_figura = 0;
			if(!matrica[y_trenutno+1][x_trenutno])
				skor += 7;
		}
	}


	glutPostRedisplay();
	if(animation_ongoing)
		glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);

}
/* Crta se okvir. */
static void nacrtaj_okvir(void){
	glTranslatef(0, 0.1, 0);
	glScalef(2, 4, 0.2);
	glColor3f(1, 1, 1);
	glutWireCube(1);
}

/* Sakrivam figuru na vrhu. */
static void sakri_figuru(void){
	glTranslatef(0, 2.5, 0);
	glScalef(100, 0.8, 0.3);
	glColor3f(0, 0, 0);
	glutSolidCube(1);
}

/* Iscrtava se figura i dodeljuje joj se jedinstveni broj u zavisnosti od pozicije */
static void iscrtaj_figuru(int x){

	/* Iscrtava se linija */
	if(x == 1){
		if(broj_rotacija%4 == 0)
			jedinstveni_broj = 1;
		if(broj_rotacija%4 == 1)
			jedinstveni_broj = 2;
		if(broj_rotacija%4 == 2)
			jedinstveni_broj = 3;
		if(broj_rotacija%4 == 3)
			jedinstveni_broj = 4;

		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-0.5 + transliraj, 11 - y_trenutno, 0);
		glRotatef(rotiraj, 0, 0, 1);

		glColor3f(0, 0, 1);
		glTranslatef(0, 0, 0);
		glutSolidCube(1);

		glColor3f(0, 1, 0);
		glTranslatef(1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 0, 0);
		glTranslatef(1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 1, 0);
		glTranslatef(-3, 0, 0);
		glutSolidCube(1);

	}

	/* Iscrtava se L */
	if(x == 2){
		if(broj_rotacija%4 == 0)
			jedinstveni_broj = 5;
		if(broj_rotacija%4 == 1)
			jedinstveni_broj = 6;
		if(broj_rotacija%4 == 2)
			jedinstveni_broj = 7;
		if(broj_rotacija%4 == 3)
			jedinstveni_broj = 8;

		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-0.5 + transliraj, 11 - y_trenutno, 0);
		glRotatef(rotiraj, 0, 0, 1);

		glColor3f(0, 0, 1);
		glTranslatef(0, 0, 0);
		glutSolidCube(1);

		glColor3f(0, 1, 0);
		glTranslatef(-1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 0, 0);
		glTranslatef(2, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 1, 0);
		glTranslatef(0, 1, 0);
		glutSolidCube(1);
	}

	/* Iscrtava se obrnuto L */
	if(x == 3){
		if(broj_rotacija%4 == 0)
			jedinstveni_broj = 9;
		if(broj_rotacija%4 == 1)
			jedinstveni_broj = 10;
		if(broj_rotacija%4 == 2)
			jedinstveni_broj = 11;
		if(broj_rotacija%4 == 3)
			jedinstveni_broj = 12;

		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-0.5 + transliraj, 11 - y_trenutno, 0);
		glRotatef(rotiraj, 0, 0, 1);

		glColor3f(0, 0, 1);
		glTranslatef(0, 0, 0);
		glutSolidCube(1);

		glColor3f(0, 1, 0);
		glTranslatef(1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 0, 0);
		glTranslatef(-2, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 1, 0);
		glTranslatef(0, 1, 0);
		glutSolidCube(1);
	}

	/* Iscrtava se T */
	if(x == 4){
		if(broj_rotacija%4 == 0)
			jedinstveni_broj = 13;
		if(broj_rotacija%4 == 1)
			jedinstveni_broj = 14;
		if(broj_rotacija%4 == 2)
			jedinstveni_broj = 15;
		if(broj_rotacija%4 == 3)
			jedinstveni_broj = 16;

		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-0.5 + transliraj, 12 - y_trenutno, 0);
		glRotatef(rotiraj, 0, 0, 1);

		glColor3f(0, 0, 1);
		glTranslatef(0, 0, 0);
		glutSolidCube(1);

		glColor3f(0, 1, 0);
		glTranslatef(1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 0, 0);
		glTranslatef(-2, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 1, 0);
		glTranslatef(1, -1, 0);
		glutSolidCube(1);

	}

	/* Iscrtava se Z */
	if(x == 5){
		if(broj_rotacija%4 == 0)
			jedinstveni_broj = 17;
		if(broj_rotacija%4 == 1)
			jedinstveni_broj = 18;
		if(broj_rotacija%4 == 2)
			jedinstveni_broj = 19;
		if(broj_rotacija%4 == 3)
			jedinstveni_broj = 20;

		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-0.5 + transliraj, 11 - y_trenutno, 0);
		glRotatef(rotiraj, 0, 0, 1);

		glColor3f(0, 0, 1);
		glTranslatef(0, 0, 0);
		glutSolidCube(1);

		glColor3f(0, 1, 0);
		glTranslatef(1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 0, 0);
		glTranslatef(-1, 1, 0);
		glutSolidCube(1);

		glColor3f(1, 1, 0);
		glTranslatef(-1, 0, 0);
		glutSolidCube(1);
	}

	/* Iscrtava se obrnuto Z */
	if(x == 6){
		if(broj_rotacija%4 == 0)
			jedinstveni_broj = 21;
		if(broj_rotacija%4 == 1)
			jedinstveni_broj = 22;
		if(broj_rotacija%4 == 2)
			jedinstveni_broj = 23;
		if(broj_rotacija%4 == 3)
			jedinstveni_broj = 24;

		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-0.5 + transliraj, 11 - y_trenutno, 0);
		glRotatef(rotiraj, 0, 0, 1);

		glColor3f(0, 0, 1);
		glTranslatef(0, 0, 0);
		glutSolidCube(1);

		glColor3f(0, 1, 0);
		glTranslatef(-1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 0, 0);
		glTranslatef(1, 1, 0);
		glutSolidCube(1);

		glColor3f(1, 1, 0);
		glTranslatef(1, 0, 0);
		glutSolidCube(1);
	}

	/* Iscrtava se kvadrat */
	if(x == 7){
		jedinstveni_broj = 0;

		glScalef(0.2, 0.2, 0.2);
		/* glRotatef(rotiraj, 0, 0, 0); */
		glTranslatef(-0.5 + transliraj, 11 - y_trenutno, 0);

		glColor3f(0, 0, 1);
		glTranslatef(0, 0, 0);
		glutSolidCube(1);

		glColor3f(0, 1, 0);
		glTranslatef(1, 0, 0);
		glutSolidCube(1);

		glColor3f(1, 0, 0);
		glTranslatef(0, 1, 0);
		glutSolidCube(1);

		glColor3f(1, 1, 0);
		glTranslatef(-1, 0, 0);
		glutSolidCube(1);
	}

}

/* Kada padne figura zabelezi je u matrici. */
static void popuni_matricu(void){
	if(pala_je_figura){
		switch(jedinstveni_broj){
			case 0:
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno - 1][x_trenutno + 1] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				break;
			case 1:
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno][x_trenutno + 2] = 1;
				break;
			case 2:
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno - 2][x_trenutno] = 1;
				break;
			case 3:
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno][x_trenutno - 2] = 1;
				break;
			case 4:
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno + 2][x_trenutno] = 1;
				break;
			case 5:
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno - 1][x_trenutno + 1] = 1;
				break;
			case 6:
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno - 1] = 1;
				break;
			case 7:
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno + 1][x_trenutno - 1] = 1;
				break;
			case 8:
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno + 1] = 1;
				break;
			case 9:
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno - 1][x_trenutno - 1] = 1;
				break;
			case 10:
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno - 1] = 1;
				break;
			case 11:
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno + 1][x_trenutno + 1] = 1;
				break;
			case 12:
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno + 1] = 1;
				break;
			case 13:
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno + 1][x_trenutno] = 1;
				break;
			case 14:
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				break;
			case 15:
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				break;
			case 16:
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno - 1] = 1;
				break;
			case 17:
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno - 1] = 1;
				break;
			case 18:
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno + 1][x_trenutno - 1] = 1;
				break;
			case 19:
				matrica[y_trenutno][x_trenutno -1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno + 1] = 1;
				break;
			case 20:
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno - 1][x_trenutno + 1] = 1;
				break;
			case 21:
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno - 1][x_trenutno + 1] = 1;
				break;
			case 22:
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno - 1] = 1;
				matrica[y_trenutno - 1][x_trenutno - 1] = 1;
				break;
			case 23:
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno] = 1;
				matrica[y_trenutno + 1][x_trenutno - 1] = 1;
				break;
			case 24:
				matrica[y_trenutno - 1][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno] = 1;
				matrica[y_trenutno][x_trenutno + 1] = 1;
				matrica[y_trenutno + 1][x_trenutno + 1] = 1;
				break;
		}
	}
}

static void resetuj_matricu(void){
	int i, j;

	for(i=0; i<MAX_X; i++)
		for (j=0; j<MAX_Y; j++)
			if(i == 0 || i== 11 || j == 21)
				matrica[j][i] = 1;
			else
				matrica[j][i] = 0;
		
}
static void ispisi_matricu_u_terminalu(void){
	int i, j;

	for (j=0; j<MAX_Y; j++)	{
		for(i=0; i<MAX_X; i++)
				if(matrica[j][i] == 1)
					printf("1 ");
				else if(matrica[j][i] == 0)
					printf("0 ");
		printf("\n");
			
	}

}

/* Za svaku figuru u svakoj poziciji, preko jedinstvenog broja,
proverava gde moze da se krece i kada dolazi do poziva nove figure. */
static void kolizije(void){
	switch(jedinstveni_broj){
		/* Jedinstveni broj za kocku. */
		case 0:
			/* Proverava da li kvadrat moze da pada(da se krece ka dole). */
			if(!matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Proverava da li figura moze da se krece levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno - 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Proverava da li figura moze da se krece desno */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno - 1][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		/* Jedinstveni brojevi za liniju */
		case 1:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 2]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 3]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 2:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1] && !matrica[y_trenutno + 2][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 3:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 3]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 4:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 3][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1] && !matrica[y_trenutno + 2][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		/* Jedinstveni brojevi za L */
		case 5:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno+ 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno - 1][x_trenutno]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno + 1][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 6:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 2] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 7:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno - 1] && !matrica[y_trenutno +1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 2]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 8:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno] && !matrica[y_trenutno][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 2] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		/* Jedinstveni brojevi za obrnuto L */
		case 9:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno - 1][x_trenutno - 2]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 10:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno + 1][x_trenutno - 2] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno - 1][x_trenutno]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 11:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno-1] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 2][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno + 1][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 12:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno] && !matrica[y_trenutno][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno +1][x_trenutno]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 2] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		/* Jedinstveni brojevi za T */
		case 13:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 14:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 15:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 16:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		/* Jedinstveni brojevi za Z */
		case 17:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 2] && !matrica[y_trenutno][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 18:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 2]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 19:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno] && !matrica[y_trenutno + 2][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 20:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 2] && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno + 1][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		/* Jedinstveni brojevi za obrnuto Z */
		case 21:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno][x_trenutno - 2]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 2] && !matrica[y_trenutno][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 22:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 0]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 23:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 2][x_trenutno - 1] && !matrica[y_trenutno + 2][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 2]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno +1][x_trenutno + 1]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
		case 24:
			/* Proverava da li moze da pada. */
			if(!matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 2][x_trenutno + 1]){
				pala_je_figura = 0;
				y_trenutno++;
			}
			else{
				x_stop = x_trenutno;
				y_stop = y_trenutno;
				pala_je_figura = 1;
			}
			/* Kretanje levo. */
			if(levo_desno == -1 && !matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno][x_trenutno] && !matrica[y_trenutno +1][x_trenutno - 1]){
				x_trenutno -= 1;
				transliraj -= 1;
				levo_desno = 0;
			}
			else{
			}
			/* Kretanje desno. */
			if(levo_desno == 1 && !matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno + 1][x_trenutno + 2]){
				x_trenutno += 1;
				transliraj += 1;
				levo_desno = 0;
			}
			else{
			}
			break;
	}

}

static void rotacije_na_granicama(void){
	switch(jedinstveni_broj){
		case 1:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 2][x_trenutno]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			break;
		case 2:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			/* Pita da li je rotacija na levoj ivici */
			else if(x_trenutno == 1){			
				if(!matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno][x_trenutno + 3]){
					transliraj += 2;
					x_trenutno = 3;
					broj_rotacija++;
					rotiraj += 90;
					levo_desno = 0;
				}
			}
			/* Rotacija na levoj ivici */
			else if(x_trenutno == 2){
				if(!matrica[y_trenutno][x_trenutno = 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2]){
					transliraj += 1;
					x_trenutno = 3;
					broj_rotacija++;
					rotiraj += 90;
					levo_desno = 0;
				}
			}
			/* Rotacija na desnoj ivici */
			else if(x_trenutno == MAX_X - 2){
				if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno][x_trenutno - 3]){
					transliraj -= 1;
					x_trenutno -= 1;
					broj_rotacija++;
					rotiraj += 90;
					levo_desno = 0;
				}
			}
			break;
		case 3:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 2][x_trenutno]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;	
			}
			break;
		case 4:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			/* Rotacija na levoj ivici */
			else if(x_trenutno == 1){
				if(!matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno][x_trenutno + 3]){
					transliraj += 1;
					x_trenutno += 1;
					broj_rotacija++;
					rotiraj += 90;
					levo_desno = 0;
				}
			}
			/* Rotacija na desnoj ivici */
			else if(x_trenutno == MAX_X - 2){
				if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno][x_trenutno - 3]){
					transliraj -= 2;
					x_trenutno -= 2;
					broj_rotacija++;
					rotiraj += 90;
					levo_desno = 0;
				}
			}
			/* Rotacija na desnoj ivici */
			else if(x_trenutno == MAX_X - 3){
				if(!matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj -= 1;
					x_trenutno -= 1;
					levo_desno = 0;
				}
			}
			break;
		case 5:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno]){
				broj_rotacija++;
				rotiraj += 90;
			}			
			break;
		case 6:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			else if(x_trenutno == MAX_X - 2){
				if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj -= 1;
					x_trenutno = MAX_X;
				}
			}
			break;
		case 7:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			break;
		case 8:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno - 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			/* Rotacija na levoj ivici */
			if(x_trenutno == 1){
				if(!matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2] && !matrica[y_trenutno - 1][x_trenutno + 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj += 1;
					x_trenutno = 2;
				}
			}
			break;
		case 9:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			break;
		case 10:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			/* Rotacija na desnoj ivici */
			if(x_trenutno == MAX_X - 2){
				if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno][x_trenutno - 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj -= 1;
					x_trenutno -= 1;
				}
			}
			break;
		case 11:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno - 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			break;
		case 12:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno - 1][x_trenutno - 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			/* Rotacija na levoj ivici */
			if(x_trenutno == 1){
				if(!matrica[y_trenutno][x_trenutno] && !matrica[y_trenutno][x_trenutno] && !matrica[y_trenutno][x_trenutno]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj += 1;
					x_trenutno += 1;
				}
			}
			break;
		case 13:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			break;
		case 14:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1]){
				broj_rotacija++;
				rotiraj += 90;
			}
			/* Rotacija na levoj ivici */
			if(x_trenutno == 1){
				if(!matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno][x_trenutno + 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj += 1;
					x_trenutno += 1;
				}
			}
			break;
		case 15:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno + 1][x_trenutno]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			break;
		case 16:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			/* Rotacija na desnoj ivici */
			if(x_trenutno == MAX_X - 2){
				if(!matrica[y_trenutno][x_trenutno - 2] && !matrica[y_trenutno + 1][x_trenutno - 1]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj -= 1;
					x_trenutno -= 1;
				}
			}
			break;
		case 17:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			break;
		case 18:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			/* Rotacija na desnoj ivici */
			if(x_trenutno == MAX_X - 2){
				if(!matrica[y_trenutno + 1][x_trenutno] && !matrica[y_trenutno][x_trenutno - 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj -= 1;
					x_trenutno -= 1;
				}
			}
			break;
		case 19:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno - 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			break;
		case 20:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno - 1][x_trenutno - 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			/* Rotacija na levoj ivici */
			if(x_trenutno == 1){
				if(!matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno][x_trenutno + 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj += 1;
					x_trenutno += 1;
				}
			}
			break;
		case 21:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno - 1] && !matrica[y_trenutno + 1][x_trenutno]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			break;
		case 22:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno + 1] && !matrica[y_trenutno + 1][x_trenutno - 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			/* Rotacija na desnoj ivici */
			if(x_trenutno == MAX_X - 2){
				if(!matrica[y_trenutno + 1][x_trenutno - 1] && !matrica[y_trenutno +  1][x_trenutno - 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj -= 1;
					x_trenutno -= 1;
				}
			}
			break;
		case 23:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno - 1][x_trenutno] && !matrica[y_trenutno + 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			break;
		case 24:
			/* Pita samo da li moze da se rotira */
			if(!matrica[y_trenutno][x_trenutno - 1] && !matrica[y_trenutno - 1][x_trenutno + 1]){
				broj_rotacija++;
				rotiraj += 90;
				levo_desno = 0;
			}
			/* Rotacija na levoj ivici */
			if(x_trenutno == 1){
				if(!matrica[y_trenutno - 1][x_trenutno + 1] && !matrica[y_trenutno - 1][x_trenutno + 2]){
					broj_rotacija++;
					rotiraj += 90;
					transliraj += 1;
					x_trenutno += 1;
				}
			}
			break;
	}
	
}

/* Konstantno treba da iscrtava matricu na ekran */
static void iscrtavanje_matrice_na_ekran(void){
	int i, j;
	for(j = 1; j < MAX_Y-1; j++){
		for(i = 1; i < MAX_X-1; i++){
			if(matrica[j][i]){
				glPushMatrix();
					glColor3f(0.75, 0.75, 0.75);
					glScalef(0.2, 0.2, 0.2);
					glTranslatef(-5.5 + i, 11 - j, 0);
					glutSolidCube(1);
				glPopMatrix();
			}
		}
	}

}
/* Kada se popuni red u matrici brise se i azurira se matrica */
static void brisanje_reda(void){
	int i, j, k, m;

	for(j = 1; j < MAX_Y-1; j++){
		for(i = 1; i < MAX_X-1; i++){
			if(matrica[j][i] == 0)
				break;\
			/* red je popunjen, obrisi ga */
			if(i == MAX_X-2){ 
				for(k = j; k > 0; k--){
					for(m = 0; m < MAX_X-1; m++){
						matrica[k][m] = matrica[k-1][m];
					}

				}
			}

		}
	}
}