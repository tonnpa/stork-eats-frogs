//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2013-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fA1ggvA�nyt kivA�ve, a lefoglalt adat korrekt felszabadA�tA!sa nA�lkA1l 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Nguyen Phan Anh
// Neptun : BQZUZ3
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)                                                                                                                                                                                                            
#include <OpenGL/gl.h>                                                                                                                                                                                                            
#include <OpenGL/glu.h>                                                                                                                                                                                                           
#include <GLUT/glut.h>                                                                                                                                                                                                            
#else                                                                                                                                                                                                                             
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)                                                                                                                                                                       
#include <windows.h>                                                                                                                                                                                                              
#endif                                                                                                                                                                                                                            
#include <GL/gl.h>                                                                                                                                                                                                                
#include <GL/glu.h>                                                                                                                                                                                                               
#include <GL/glut.h>                                                                                                                                                                                                              
#endif          

#include <iostream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

#define OBJ_NUM 10
#define CTR_TMAX 100
#define PI 3.14

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z;
	Vector() {
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0) {
		x = x0; y = y0; z = z0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator/(float a) {
		return Vector(x / a, y / a, z / a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) {         // dot product
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) {         // cross product
		return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	float Length() { return sqrt(x * x + y * y + z * z); }
};

typedef Vector Point;
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
	float r, g, b;

	Color() {
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0) {
		r = r0; g = g0; b = b0;
	}
	Color operator*(float a) {
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c) {
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c) {
		return Color(r + c.r, g + c.g, b + c.b);
	}
};

class ParamSurface{
public:
	virtual void draw() = 0;
};

void glPoint3f(Point p){
	glVertex3f(p.x, p.y, p.z);
}

void draw2DCircle(Point point){
	glColor3f(1, 0, 0);
	float radius = 0.1;
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(point.x, point.y, 0);
	for (int i = 0; i <= 360; ++i){
		glVertex3f(point.x + radius*cos(i / 360.0 * 2 * PI), point.y + radius*sin(i / 360.0 * 2 * PI), 0);
	}
	glEnd();
}

class CTRSpline : public ParamSurface{
	Point p[4];
	Vector v[4];
	float t[4];
	Vector a2[3], a3[3];

	int pointCount;
	int functionCount;
	
public:
	CTRSpline():pointCount(4), functionCount(3){}

	void setPoints(Point& p0, Point& p1, Point& p2, Point& p3){
		p[0] = p0;
		p[1] = p1;
		p[2] = p2;
		p[3] = p3;
	}
	void setWeights(float t0, float t1, float t2, float t3){
		t[0] = t0;
		t[1] = t1;
		t[2] = t2;
		t[3] = t3;
	}
	void setup(){
		v[0] = v[3] = Vector (0,0,0);
		for (int i = 1; i < pointCount-1; ++i){
			v[i] = ((p[i + 1] - p[i]) / (t[i + 1] - t[i]) + (p[i] - p[i - 1]) / (t[i] - t[i - 1]))*0.5;
		}

		for (int i = 0; i < functionCount; ++i){
			a2[i] = (p[i + 1] - p[i]) * 3 / powf(t[i + 1] - t[i], 2) - (v[i+1]+v[i]*2)/(t[i+1]-t[i]);
			a3[i] = (p[i] - p[i + 1]) * 2 / powf(t[i + 1] - t[i], 3) + (v[i + 1] + v[i]) / powf(t[i + 1] - t[i], 2);
		}
	}
	Point curvePoint(float u){
		int i = 0;
		for (int j = 0; j < 4; ++j){
			if (u > t[j]){
				i = j;
			}
		}
		std::cout << i;
		return a3[i] * powf(u - t[i], 3) + a2[i] * powf(u - t[i], 2) + v[i] * (u - t[i]) + p[i];
	}
	void draw(){
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < CTR_TMAX; ++i){
			glPoint3f(curvePoint(i));
		}
		glEnd();
		for (int i = 0; i < pointCount; ++i){
			draw2DCircle(p[i]);
		}
	}

	void drawFunction(int f){
		glColor3f(0, 0, 1);
		glBegin(GL_LINE_STRIP);
		for (int i = t[f]; i < t[f+1]; ++i){
			glPoint3f(curvePoint(i));
		}

		for (int i = 0; i < pointCount; ++i){
			draw2DCircle(p[i]);
		}
	}
};

class Object{
	ParamSurface* surfaces[OBJ_NUM];
	int surfaceCount;
	
public:
	Object() :surfaceCount(0){}
	void addSurface(ParamSurface* newSurface){
		if (surfaceCount < OBJ_NUM){
			surfaces[surfaceCount] = newSurface;
			surfaceCount++;
		}
	}
};

const int screenWidth = 600;	// alkalmazA!s ablak felbontA!sa
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazA!s ablaknyi kA�p

CTRSpline* midline;
// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glViewport(0, 0, screenWidth, screenHeight);
	gluOrtho2D(-8, 10, -8, 10);

	midline = new CTRSpline();
	midline->setPoints(Point(-5., 5.25, 0), Point(-4, 1.25, 0), Point(-1, -0.25, 0), Point(2.65, -2.5, 0));
	midline->setWeights(0, 33, 66, 100);
	midline->setup();
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	midline->draw();

	glutSwapBuffers();     				// Buffercsere: rajzolas vege
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {}
// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {}
// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {}
// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y){}
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {}
// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
	glutInit(&argc, argv); 				// GLUT inicializalasa
	glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
	glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();					// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}