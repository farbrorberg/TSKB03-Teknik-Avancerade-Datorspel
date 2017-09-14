// New version by Ingemar 2010
// Removed all dependencies of the Wild Magic (wml) library.
// Replaced it with VectorUtils2 (in source)
// Replaced old shader module with the simpler "ShaderUtils" unit.

// 2013: Adapted to VectorUtils3 and MicroGlut.

// gcc skinning.c ../common/*.c -lGL -o skinning -I../common

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#ifdef __APPLE__
// Mac
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
//uses framework Cocoa
#else
	#ifdef WIN32
// MS
		#include <stdio.h>
		#include <GL/glew.h>
		#include <GL/glut.h>
	#else
// Linux
		#include <GL/gl.h>
		#include "MicroGlut.h" // #include <GL/glut.h>
	#endif
#endif

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"

// Ref till shader
GLuint g_shader;

typedef struct Triangle
{
	GLuint				v1;
	GLuint				v2;
	GLuint				v3;
} Triangle;

#define kMaxRow 10
#define kMaxCorners 8
#define kMaxg_poly ((kMaxRow-1) * kMaxCorners * 2)
#ifndef Pi
#define Pi 3.1416
#endif
#ifndef true
#define true 1
#endif


Triangle g_poly[kMaxg_poly];

// vertices
Point3D g_vertsOrg[kMaxRow][kMaxCorners];
Point3D g_normalsOrg[kMaxRow][kMaxCorners];

// vertices sent to OpenGL
Point3D g_vertsRes[kMaxRow][kMaxCorners];
Point3D g_normalsRes[kMaxRow][kMaxCorners];

// vertex attributes sent to OpenGL
Point3D g_boneWeights[kMaxRow][kMaxCorners];

//float weight[kMaxRow] = {0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0};
float weight[kMaxRow] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

Model *cylinderModel; // Collects all the above for drawing with glDrawElements

mat4 modelViewMatrix, projectionMatrix;

///////////////////////////////////////////////////
//		B U I L D	C Y L I N D E R
// Desc:	bygger upp cylindern 
//
void BuildCylinder()
{
	long	row, corner, cornerIndex;
	float g_vertstex[kMaxRow][kMaxCorners][2];

	// sätter värden till alla vertexar i meshen
	for (row = 0; row < kMaxRow; row++)
		for (corner = 0; corner < kMaxCorners; corner++)
		{
			g_vertsOrg[row][corner].x = row;
			g_vertsOrg[row][corner].y = cos(corner * 2*Pi / kMaxCorners);
			g_vertsOrg[row][corner].z = sin(corner * 2*Pi / kMaxCorners);

			g_normalsOrg[row][corner].x = 0;
			g_normalsOrg[row][corner].y = cos(corner * 2*Pi / kMaxCorners);
			g_normalsOrg[row][corner].z = sin(corner * 2*Pi / kMaxCorners);

			g_boneWeights[row][corner].x = (1-weight[row]);
			g_boneWeights[row][corner].y = weight[row];
			g_boneWeights[row][corner].z = 0.0;
		};

	// g_poly definerar mellan vilka vertexar som 
	// trianglarna ska ritas
	for (row = 0; row < kMaxRow-1; row++)
		for (corner = 0; corner < kMaxCorners; corner++)
		{
	// Quads built from two triangles

			if (corner < kMaxCorners-1) 
			{
				cornerIndex = row * kMaxCorners + corner;
				g_poly[cornerIndex * 2].v1 = cornerIndex;
				g_poly[cornerIndex * 2].v2 = cornerIndex + 1;
				g_poly[cornerIndex * 2].v3 = cornerIndex + kMaxCorners + 1;
	
				g_poly[cornerIndex * 2 + 1].v1 = cornerIndex;
				g_poly[cornerIndex * 2 + 1].v2 = cornerIndex + kMaxCorners + 1;
				g_poly[cornerIndex * 2 + 1].v3 = cornerIndex + kMaxCorners;
			}
			else
			{ // Specialfall: sista i varvet, gå runt hörnet korrekt
				cornerIndex = row * kMaxCorners + corner;
				g_poly[cornerIndex * 2].v1 = cornerIndex;
				g_poly[cornerIndex * 2].v2 = cornerIndex + 1 - kMaxCorners;
				g_poly[cornerIndex * 2].v3 = cornerIndex + kMaxCorners + 1 - kMaxCorners;
	
				g_poly[cornerIndex * 2 + 1].v1 = cornerIndex;
				g_poly[cornerIndex * 2 + 1].v2 = cornerIndex + kMaxCorners + 1 - kMaxCorners;
				g_poly[cornerIndex * 2 + 1].v3 = cornerIndex + kMaxCorners;
			}
		}
	
	// lägger en kopia av originalmodellen i g_vertsRes & g_normalsRes

	for (row = 0; row < kMaxRow; row++)
		for (corner = 0; corner < kMaxCorners; corner++)
		{
			g_vertsRes[row][corner] = g_vertsOrg[row][corner];
			g_normalsRes[row][corner] = g_normalsOrg[row][corner];
			g_vertstex[row][corner][0]=(1-weight[row]);
			g_vertstex[row][corner][1]=weight[row];
		}
	
	// Build Model from cylinder data
	cylinderModel = LoadDataToModel(
			(GLfloat*) g_vertsRes,
			(GLfloat*) g_normalsRes,
			(GLfloat*) g_vertstex, // texCoords
			(GLfloat*) g_vertstex, // colors
			(GLuint*) g_poly, // indices
			kMaxRow*kMaxCorners,
			kMaxg_poly * 3);
}


//////////////////////////////////////
//		B O N E
// Desc:	en enkel ben-struct med en 
//			pos-vektor och en rot-vektor 
typedef struct Bone
{
	vec3 pos;
	mat4 rot;
} Bone;

///////////////////////////////////////
//		G _ B O N E S
// vårt skelett; just nu innehåller det 2 ben ...
Bone g_bones[2];


///////////////////////////////////////////////////////
//		S E T U P	B O N E S
//
// Desc:	sätter ut ben 0 i origo och
//			ben 1 pp pos (4.5, 0, 0)
void setupBones(void)
{
	g_bones[0].pos = SetVector(0.0f, 0.0f, 0.0f);
	g_bones[1].pos = SetVector(4.5f, 0.0f, 0.0f);
	g_bones[0].rot = IdentityMatrix();
	g_bones[1].rot = IdentityMatrix();
}


///////////////////////////////////////////////////////
//		D E F O R M	C Y L I N D E R 
//
// Desc:	deformera cylindermeshen enligt skelettet
void DeformCylinder()
{
	// Point3D v1, v2;
	int row, corner;

	//T_ben1
	//T_ben1
	mat4 T_ben1 = T(g_bones[0].pos.x, g_bones[0].pos.y, g_bones[0].pos.z);
	mat4 T_ben2 = T(g_bones[1].pos.x, g_bones[1].pos.y, g_bones[1].pos.z);

	//M_ben1 = T_ben1 * R_ben1
	//M_ben2 = T_ben2 * R_ben2
	mat4 M_ben1 = Mult(T_ben1, g_bones[0].rot);
	mat4 M_ben2 = Mult(T_ben2, g_bones[1].rot);

	//M^(-1)ben1
	//M^(-1)ben2
	mat4 MI_ben1 = InvertMat4(M_ben1);
	mat4 MI_ben2 = InvertMat4(M_ben2);

	//M'_ben1 = T_vila * R_vila * R_anim
	//M'_ben2 = T_vila * R_vila * R_anim
	mat4 MP_ben1 = Mult(M_ben1, g_bones[0].rot);
	mat4 MP_ben2 = Mult(M_ben2, g_bones[1].rot);


	// för samtliga vertexar
	for (row = 0; row < kMaxRow; row++)
	{
		for (corner = 0; corner < kMaxCorners; corner++)
		{
			g_vertsRes[row][corner] = g_vertsOrg[row][corner];
			
			// ----=========	Uppgift 1: Hard skinning (stitching) i CPU ===========-----
			// Deformera cylindern enligt det skelett som finns
			// i g_bones.
			//
			// Gör hard skinning.
			//
			// g_bones innehåller benen.
			// g_vertsOrg innehåller ursprunglig vertexdata.
			// g_vertsRes innehåller den vertexdata som skickas till OpenGL.
			//
			// row traverserar i cylinderns längdriktning,
			// corner traverserar "runt" cylindern

			//Transformation av vertex från benkoordinater till modellkoordinater

			//Från modell- till ben-koordinater
			//v_ben2 = M^(-1)_ben1 * M^(-1)_ben2 * v_m
			//vec3 v_ben2 = MultVec3(Mult(MI_ben1, MI_ben2), g_vertsRes[row][corner]);

			//Från ben- till modell-koordinater
			//v_m = M_ben1 * M_ben2 * v_ben2
			//vec3 v_m = MultVec3(Mult(M_ben1, M_ben2), v_ben2);

			//v'_m = M'_ben1 * M'_ben2 * M^(-1)_ben2 * M^(-1)_ben1 * v_m
//			vec3 vPrim = MultVec3(Mult(Mult(MP_ben1, MP_ben2), Mult(MI_ben1, MI_ben2)), g_vertsOrg[row][corner]);
//
//			if( weight[row]  > 0 ){
//				g_vertsRes[row][corner].x = vPrim.x * weight[row];
//				g_vertsRes[row][corner].y = vPrim.y * weight[row];
//				g_vertsRes[row][corner].z = vPrim.z * weight[row];
//			}

			// ---=========	Uppgift 2: Soft skinning i CPU ===========------
			// Deformera cylindern enligt det skelett som finns
			// i g_bones.
			//
			// Gör soft skinning.
			//
			// g_bones innehåller benen.
			// g_boneWeights innehåller blendvikter för benen.
			// g_vertsOrg innehåller ursprunglig vertexdata.
			// g_vertsRes innehåller den vertexdata som skickas till OpenGL.


			// v'_m = (w_1 * M_ben1 * v_1) + (w_2 * M_ben2 * v_2)
			// (1 - weight[row] <=> The first 4 slots belong to bone1, the other 4 to bone2
			vec3 vPrim = VectorAdd(
					ScalarMult(MultVec3(M_ben1, g_vertsOrg[row][corner]),(1 - weight[row])),
					ScalarMult(MultVec3(M_ben2, g_vertsOrg[row][corner]),weight[row])
			);

				g_vertsRes[row][corner].x = vPrim.x;
				g_vertsRes[row][corner].y = vPrim.y;
				g_vertsRes[row][corner].z = vPrim.z;
		}
	}
}


/////////////////////////////////////////////
//		A N I M A T E	B O N E S
// Desc:	en väldigt enkel amination av skelettet
//			vrider ben 1 i en sin(counter) 
void animateBones(void)
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	// Hur mycket skall vi vrida?
	float angle = sin(time * 3.f) / 2.0f * 3.0f;

	// rotera på ben 1
	g_bones[1].rot = Rz(angle);
//	printf("%f %f\n", angle, time);
}


///////////////////////////////////////////////
//		S E T	B O N E	R O T A T I O N
// Desc:	s�tter bone rotationen i vertex shadern
void setBoneRotation(void)
{
	// Uppgift 3 TODO: Här behöver du skicka över benens rotation
	// till vertexshadern
}


///////////////////////////////////////////////
//		 S E T	B O N E	L O C A T I O N
// Desc:	s�tter bone positionen i vertex shadern
void setBoneLocation(void)
{
	// Uppgift 3 TODO: H�r beh�ver du skicka �ver benens position
	// till vertexshadern
}


///////////////////////////////////////////////
//		 D R A W	C Y L I N D E R
// Desc:	s�tter bone positionen i vertex shadern
void DrawCylinder()
{
	animateBones();

	// ---------=========	UPG 2 ===========---------
	// Ers�tt DeformCylinder med en vertex shader som g�r vad DeformCylinder g�r.
	// Begynnelsen till shaderkoden ligger i filen "shader.vert" ...
	
	DeformCylinder();
	
	setBoneLocation();
	setBoneRotation();
	
// update cylinder vertices:
	glBindVertexArray(cylinderModel->vao);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderModel->vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point3D)*kMaxRow*kMaxCorners, g_vertsRes, GL_DYNAMIC_DRAW);
	
	DrawModel(cylinderModel, g_shader, "in_Position", "in_Normal", "in_TexCoord");
}


void DisplayWindow()
{
	mat4 m;
	
	glClearColor(0.4, 0.4, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);

    m = Mult(projectionMatrix, modelViewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "matrix"), 1, GL_TRUE, m.m);
	
	DrawCylinder();

	glutSwapBuffers();
}


void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

void keyboardFunc( unsigned char key, int x, int y)
{
// Add any keyboard control you want here
	if(key == 27)	//Esc
		exit(-1);
}

void reshape(GLsizei w, GLsizei h)
{
	Point3D cam = {5,0,8};
	Point3D look = {5,0,0};

    glViewport(0, 0, w, h);
    GLfloat ratio = (GLfloat) w / (GLfloat) h;
    projectionMatrix = perspective(90, ratio, 0.1, 1000);
 //   glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_TRUE, projectionMatrix);
	modelViewMatrix = lookAt(cam.x, cam.y, cam.z,
							look.x, look.y, look.z, 
							0,1,0);
}

/////////////////////////////////////////
//		M A I N
//
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2); // Might not be needed in Linux
	glutCreateWindow("Them bones");

	glutDisplayFunc(DisplayWindow);
	glutTimerFunc(20, &OnTimer, 0);
	glutKeyboardFunc( keyboardFunc ); 
	glutReshapeFunc(reshape);

	// Set up depth buffer
	glEnable(GL_DEPTH_TEST);

	// initiering
#ifdef WIN32
	glewInit();
#endif
	BuildCylinder();
	setupBones();
	g_shader = loadShaders("shader.vert" , "shader.frag");

	glutMainLoop();
	exit(0);
}
