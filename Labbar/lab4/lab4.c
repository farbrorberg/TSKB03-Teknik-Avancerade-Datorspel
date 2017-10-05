// Demo of heavily simplified sprite engine
// by Ingemar Ragnemalm 2009
// used as base for lab 4 in TSBK03.
// OpenGL 3 conversion 2013.

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#include <GL/gl.h>
	#include "MicroGlut.h"
#endif

#include <stdlib.h>
#include <math.h>
#include "LoadTGA.h"
#include "SpriteLight.h"
#include "GL_utilities.h"

// L�gg till egna globaler h�r efter behov.

FPoint calcAvoidance(SpritePtr curr, SpritePtr next)
{
    float h = curr->position.h - next->position.h;
    float v = curr->position.v - next->position.v;

    float dist = sqrt(pow(h,2) + pow(v,2));

    // Nearby boids are affected more
    h *= (1.0f - dist/100.0f) / dist;
    v *= (1.0f - dist/100.0f) / dist;

    // Alternative dist
//    h *= pow(1.0f / dist, 2);
//    v *= pow(1.0f / dist, 2);

    FPoint avoidance = (FPoint) {h,v};
    return avoidance;
}


SpriteRec *oddSheep, *dog;

void SpriteBehavior() // Din kod!
{
// L�gg till din labbkod h�r. Det g�r bra att �ndra var som helst i
// koden i �vrigt, men mycket kan samlas h�r. Du kan utg� fr�n den
// globala listroten, gSpriteRoot, f�r att kontrollera alla sprites
// hastigheter och positioner, eller arbeta fr�n egna globaler.

    float count;
    float minRadius     = 100.0;
    float oddAvoidance  = 1.0;
    float distance      = 0.0;

    float cohWeight     = 0.01;
    float alignWeight   = 0.03;
    float avoidWeight   = 1.3;

    FPoint avoidance = {0, 0};

	SpritePtr curr = gSpriteRoot;
	SpritePtr next = curr->next;


    while(curr != NULL)
    {
        count = 0.0;
        curr->avgPosition.h = 0.0;
        curr->avgPosition.v = 0.0;
        curr->speedDiff.h = 0.0;
        curr->speedDiff.v = 0.0;
        curr->vAvoidance.h = 0.0;
        curr->vAvoidance.v = 0.0;

        next = gSpriteRoot;
        while(next != NULL)
        {

            distance = sqrt(
                pow(curr->position.h - next->position.h,2) +
                pow(curr->position.v - next->position.v,2)
            );

            if( curr != next && curr != dog && curr != oddSheep &&
                next != dog && next != oddSheep) {

                if (distance < minRadius) {

                    // Cohesion
                    curr->avgPosition.h += next->position.h;
                    curr->avgPosition.v += next->position.v;

                    // Separation
                    avoidance = calcAvoidance(curr, next);
                    curr->vAvoidance.h += avoidance.h;
                    curr->vAvoidance.v += avoidance.v;

                    // Alignment
                    curr->speedDiff.h += next->speed.h - curr->speed.h;
                    curr->speedDiff.v += next->speed.v - curr->speed.v;

                    count += 1;
                }
            }

            // Avoid the dog
            if(next == dog && distance < minRadius ){
                curr->speed.h += 1/( distance + 0.01) * oddAvoidance*(curr->position.h - next->position.h);
                curr->speed.v += 1/( distance + 0.01) * oddAvoidance*(curr->position.v - next->position.v);

            }

            next = next->next;
        }


        if( count != 0 && curr != dog)
        {
                // Cohesion
                curr->avgPosition.h /= count;
                curr->avgPosition.v /= count;

                // Separation
                curr->vAvoidance.h /= count;
                curr->vAvoidance.v /= count;

                // Alignment
                curr->speedDiff.h /= count;
                curr->speedDiff.v /= count;
        }

        curr = curr->next;
    }


    //Accumulated speed
    curr = gSpriteRoot;
    while( curr != NULL )
    {
        if( curr == dog)
        {
          curr = curr->next;
        }

        if (curr != oddSheep)
        {
            // Cohesion
            curr->speed.h += (curr->avgPosition.h - curr->position.h) * cohWeight;
            curr->speed.v += (curr->avgPosition.v - curr->position.v) * cohWeight;

            // Separation
            curr->speed.h += curr->vAvoidance.h * avoidWeight;
            curr->speed.v += curr->vAvoidance.v * avoidWeight;

            // Alignment
            curr->speed.h += curr->speedDiff.h * alignWeight;
            curr->speed.v += curr->speedDiff.v * alignWeight;

            // Update boid position
            curr->position.h += curr->speed.h;
            curr->position.v += curr->speed.v;
        }

        //Random movement...
        if(curr == oddSheep)
        {

          //Clamping speed
          if(curr->speed.h > 5)
          {
            curr->speed.h = 2.5;
          }
          if(curr->speed.v > 5)
          {
            curr->speed.v = 2.5;
          }

          curr->speed.h += 2.0 * random() / (float) RAND_MAX - 1.0;
          curr->speed.v += 2.0 * random() / (float) RAND_MAX - 1.0;

        }

        curr = curr->next;
    }
}

// Drawing routine
void Display()
{
	SpritePtr sp;
	
	glClearColor(0, 0, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	DrawBackground();
	
	SpriteBehavior(); // Din kod!
	
// Loop though all sprites. (Several loops in real engine.)
	sp = gSpriteRoot;
	do
	{
		HandleSprite(sp); // Callback in a real engine
		DrawSprite(sp);
		sp = sp->next;
	} while (sp != NULL);
	
	glutSwapBuffers();
}

void Reshape(int h, int v)
{
	glViewport(0, 0, h, v);
	gWidth = h;
	gHeight = v;
}

void Timer(int value)
{
	glutTimerFunc(20, Timer, 0);
	glutPostRedisplay();
}

// Example of user controllable parameter
float someValue = 0.0;

void Key(unsigned char key,
         __attribute__((unused)) int x,
         __attribute__((unused)) int y)
{
  switch (key)
  {
    case '+':
    	someValue += 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case '-':
    	someValue -= 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case 0x1b:
      exit(0);
  }
}

void Init()
{
	TextureData *sheepFace, *blackFace, *dogFace, *foodFace;
	
	LoadTGATextureSimple("bilder/leaves.tga", &backgroundTexID); // Bakgrund
	
	sheepFace = GetFace("bilder/sheep.tga"); // Ett f�r
	blackFace = GetFace("bilder/blackie.tga"); // Ett svart f�r
	dogFace = GetFace("bilder/dog.tga"); // En hund
	foodFace = GetFace("bilder/mat.tga"); // Mat
	
	NewSprite(sheepFace, 100, 200, 1, 1);
	NewSprite(sheepFace, 200, 100, 1.5, -1);
	NewSprite(sheepFace, 250, 200, -1, 1.5);
	NewSprite(sheepFace, 150, 200, -1, 1.5);
	NewSprite(sheepFace, 50, 200, -1, 1.5);
	NewSprite(sheepFace, 150, 100, -1, 1.5);

    oddSheep = NewSprite(blackFace, 0, 100, -1, 1.5);
    dog = NewSprite(dogFace , 0, 100, -1, 4.5);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 2);
	glutCreateWindow("SpriteLight demo / Flocking");
	
	glutDisplayFunc(Display);
	glutTimerFunc(20, Timer, 0); // Should match the screen synch
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	
	InitSpriteLight();
	Init();
	
	glutMainLoop();
	return 0;
}
