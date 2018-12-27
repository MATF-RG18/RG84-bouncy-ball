#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define TIMER_ID 1
#define TIMER_INTERVAL 20

float move_parameter = 0.035, jump_parameter = 0.02, game_parameter = 0.08;
int game_ongoing, animation_ongoing_left, animation_ongoing_right, stop_point, animation_ongoing_jump;
int random, map_drawn = 0;
float move_x, move_y, move_z;

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_timer(int id);


int main(int argc, char **argv){

    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 0);
    glutCreateWindow(argv[0]);

    /* Registruju se callback funkcije. */
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    srand(time(NULL));
	int i, j;
    /* Inicijalizujemo promenljive */
    animation_ongoing_left = 0;
    animation_ongoing_right = 0;
    animation_ongoing_jump = 0;
    stop_point = 0;
    game_ongoing = 0;
    move_x = 0;
    move_y = 0;
    move_z = 0;    

    /* generisemo promenljive za mapu */
    FILE* f = fopen("map_generator", "w");
    if(f == NULL){
    	printf("Neuspelo otvaranje fajla f\n");
    	exit(0);
    }
    
    if(map_drawn == 0){

    	for(i = 0;i < 1000;i++)
    		for(j = 0;j < 5;j++){
    			random = rand() % 5;
    			fprintf(f, "%d ", random);
    		}

    }
    fclose(f);
    glutMainLoop();

    return 0;
}

static void on_keyboard(unsigned char key, int x, int y){

    switch (key) {
    case 27:
        /* Zavrsava se program. */
        exit(0);
        break;

    case 'a':
    case 'A':
    	/* Strelica za pomeranje u levo */
    	if(game_ongoing == 0)
    		break;
    	else{
    		if(animation_ongoing_left == 0){
    			if(move_x > 0.6){
    				printf("GAME OVER\n");
					printf("Your score is: %d\n", (int)(move_z * 1000));
    				game_ongoing = 0;
    				map_drawn = 1;
    			}
    			if(animation_ongoing_jump == 1)
    				break;
    			animation_ongoing_left = 1;
    			glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    		}
    	};
    	break;

    case 'd':
    case 'D':
    	/* Strelica za pomeranje u desno */
    	if(game_ongoing == 0)
    		break;
    	else{
    		if(animation_ongoing_right == 0){
    			if(move_x < -0.6){
    				printf("GAME OVER\n");
    				printf("Your score is: %d\n", (int)(move_z * 1000));
    				game_ongoing = 0;
    				map_drawn = 1;
    			}
    			if(animation_ongoing_jump == 1)
    				break;
    			animation_ongoing_right = 1;
    			glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    		}
    	};
    	break;

    case 32:
    	/* Space dugme za skok */
    	if(game_ongoing == 0)
    		break;
    	else{
    		if(animation_ongoing_jump == 0){
    			animation_ongoing_jump = 1;
    			glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    		}
    	};
    	break;

    case 's':
    case 'S':
        /* Pokrecemo igricu */
        if (!game_ongoing) {
            game_ongoing = 1;
            map_drawn = 1;
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
        }
        else game_ongoing = 0;
        break;
    }
}

void checkForHoles(){

	FILE *f = fopen("coordinates", "r");
	if(f == NULL){
		printf("Neuspelo otvaranje fajla koordinate za citanje\n");
		exit(0);
	}

	float coor_x, coor_z;
	int hole_x, hole_z, ball_x, ball_z;

	while(!feof(f)){

		fscanf(f, "%f", &coor_x);
		fscanf(f, "%f", &coor_z);
		hole_x = (int)(coor_x * 1000);
		hole_z = (int)(coor_z * 1000);
		ball_x = (int)(move_x * 1000);
		ball_z = (int)(move_z * 1000);

		if(abs(hole_z - ball_z) < 150){
			if(ball_x == hole_x){
				if(animation_ongoing_jump == 0){
					printf("Your score is: %d\n", ball_z);
                    move_y = -0.4;
                    game_ongoing = 0;
				}
			}
		}

	}
	fclose(f);
}

static void on_timer(int id){

    if (TIMER_ID != id)
        return;
    /* animacija za pomeranje u levo */
    if(animation_ongoing_left){

    	move_x += move_parameter;
    	stop_point++;
 		if(stop_point > 10){
			animation_ongoing_left = 0;
			stop_point = 0;
		}
	}
	/* animacija za pomeranje u desno */
    if(animation_ongoing_right){
    	
    	move_x -= move_parameter;
    	stop_point++;
 		if(stop_point > 10){
 			stop_point = 0;
   			animation_ongoing_right = 0;
 		}

    }
    /* animacija za skok */
    if(animation_ongoing_jump){

		if(stop_point < 41){
			move_y += jump_parameter;
    		stop_point++;
    	}
    	else{
    		stop_point++;
    		move_y -= jump_parameter;
    		if(stop_point > 81){
    			animation_ongoing_jump = 0;
    			stop_point = 0;
    		}
    	}

	}
	if(game_ongoing){
		game_parameter += 0.02;
	}

	checkForHoles();
	
	glutPostRedisplay();

    if (game_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_reshape(int width, int height){

    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (float) width / height, 1, 10);
}

static void init_lights()
{
    /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    GLfloat light_position[] = { 1, 15, 0, 0 };

    /* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };

    /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };

    /* Spekularna boja svetla. */
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

static void set_material(int id){

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.3, 0.3, 0.3, 1 };

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0.4, 0.4, 0.4, 1 };

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 30;

    diffuse_coeffs[1] = 1.0;

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}

static void set_material2(int id){

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0, 0, 1, 1 };

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0, 0, 1, 1 };

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 0, 0, 1, 1 };

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 30;

    diffuse_coeffs[1] = 1.0;
    if (id == 2){
        diffuse_coeffs[0] = 1.0;
        diffuse_coeffs[1] = 0.0;
        diffuse_coeffs[2] = 0.0;
    }

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}

void generate_map(){

	glPushMatrix();
    	/* pocetna pozicija */
		glColor3f(1, 0, 0);
		set_material(1);
    	glTranslatef(0, -0.4, 0);
    	glutSolidCube(0.4);
    	glTranslatef(0.4, 0, 0);
    	glutSolidCube(0.4);
    	glTranslatef(-0.8, 0, 0);
    	glutSolidCube(0.4);
    	glTranslatef(1.2, 0, 0);
    	glutSolidCube(0.4);
    	glTranslatef(-1.6, 0, 0);
    	glutSolidCube(0.4);
    	int k, i;
    	for(k = 0;k < 3;k++){
	    	glTranslatef(0, 0, .4);
	    	glutSolidCube(0.4);
	    	glTranslatef(0.4, 0, 0);
	    	glutSolidCube(0.4);
	    	glTranslatef(0.4, 0, 0);
	    	glutSolidCube(0.4);
	    	glTranslatef(0.4, 0, 0);
	    	glutSolidCube(0.4);
	    	glTranslatef(0.4, 0, 0);
	    	glutSolidCube(0.4);
	    	glTranslatef(-1.6, 0, 0);
		}
		FILE* f = fopen("map_generator", "r");
		if(f == NULL){
			printf("Neuspelo otvaranje fajla za citanje\n");
			return;
		}
		/* otvaram falj coordinates za upisivanje koordinata rupa */
		FILE* g = fopen("coordinates", "w");
	    if(f == NULL){
	    	printf("Neuspelo otvaranje fajla g\n");
	    	return;
	    }
		/* pocetak mape se ovde zavrsava, sada sledi random generisana mapa */
		for(k = 0;k < 1000;k++){

			glTranslatef(0, 0, .4);

			for(i = 0;i < 5;i++){
				fscanf(f, "%d", &random);
				if(random == 0){
					if(i == 0){
						fprintf(g, "-0.77 ");
						fprintf(g, "%f ", 1.6 + k * 0.4);
					}
					if(i == 1){
						fprintf(g, "-0.385 ");
						fprintf(g, "%f ", 1.6 + k * 0.4);
					}	
					if(i == 2){
						fprintf(g, "0.0 ");
						fprintf(g, "%f ", 1.6 + k * 0.4);
					}
					if(i == 3){
						fprintf(g, "0.385 ");
						fprintf(g, "%f ", 1.6 + k * 0.4);
					}
					if(i == 4){
						fprintf(g, "0.77 ");
						fprintf(g, "%f ", 1.6 + k * 0.4);
					}

				}
                if(random == 4){
                    if(i == 0){
                        fprintf(g, "-0.77 ");
                        fprintf(g, "%f ", 1.6 + k * 0.4);
                    }
                    if(i == 1){
                        fprintf(g, "-0.385 ");
                        fprintf(g, "%f ", 1.6 + k * 0.4);
                    }   
                    if(i == 2){
                        fprintf(g, "0.0 ");
                        fprintf(g, "%f ", 1.6 + k * 0.4);
                    }
                    if(i == 3){
                        fprintf(g, "0.385 ");
                        fprintf(g, "%f ", 1.6 + k * 0.4);
                    }
                    if(i == 4){
                        fprintf(g, "0.77 ");
                        fprintf(g, "%f ", 1.6 + k * 0.4);
                    }
                    glPushMatrix();
                        glTranslatef(0, 0.2, 0);
                        set_material2(2);
                        glutSolidCube(0.4);
                    glPopMatrix();

                }
                set_material(1);
				if(random == 1){
					glutSolidCube(0.4);
				}
				if(random == 2){
					glutSolidCube(0.4);
				}
				if(random == 3){
					glutSolidCube(0.4);
				}
				glTranslatef(0.4, 0, 0);
			}
			glTranslatef(-2, 0, 0);
		}
    	
    glPopMatrix();
    fclose(g);
}

static void on_display(void){

    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	gluLookAt(0, 3, game_parameter - 5 , 0, 0, game_parameter - 0.02, 0, 1, 0);

    init_lights();
    /* Kreiraja se loptica */
    glPushMatrix();
    	move_z = game_parameter - 0.02;
    	glTranslatef(move_x, move_y, move_z);
        set_material2(1);
        glutSolidSphere(0.2, 64, 64);
    glPopMatrix();
    /* generisu se promenljive po kojima ce se praviti polje za igru */
    generate_map();
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}