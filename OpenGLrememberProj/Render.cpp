#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	double AQ[] = { 0, 3, 0 };
	double BQ[] = {6, 5, 0 };
	double CQ[] = { 7, 1, 0 };
	double DQ[] = { 12, 3, 0 };
	double E[] = { 7, 6, 0 };
	double F[] = { 10, 13, 0 };
	double N[] = { 2, 12, 0 };
	double G[] = { 4, 6, 0 };

	double AA[] = { 0, 3, 4 };
	double BB[] = { 6, 5, 4 };
	double CC[] = { 7, 1, 4 };
	double DD[] = { 12, 3, 4 };
	double EE[] = { 7, 6, 4 };
	double FF[] = { 10, 13, 4 };
	double NN[] = { 2, 12, 4 };
	double GG[] = { 4, 6, 4 };
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glTexCoord2f(0, 0);
	glVertex3dv(AQ);
	glTexCoord2f(1, 0);
	glVertex3dv(BQ);
	glTexCoord2f(1, 1);
	glVertex3dv(E);
	glTexCoord2f(0, 1);
	glVertex3dv(G);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glTexCoord2f(0, 0);
	glVertex3dv(BQ);
	glTexCoord2f(1, 0);
	glVertex3dv(CQ);
	glTexCoord2f(1, 1);
	glVertex3dv(DQ);
	glTexCoord2f(0, 1);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glTexCoord2f(0, 0);
	glVertex3dv(E);
	glTexCoord2f(1, 0);
	glVertex3dv(F);
	glTexCoord2f(1, 1);
	glVertex3dv(N);
	glTexCoord2f(0, 1);
	glVertex3dv(G);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.2);
	glTexCoord2f(0, 0);
	glVertex3dv(AA);
	glTexCoord2f(1, 0);
	glVertex3dv(BB);
	glTexCoord2f(1, 1);
	glVertex3dv(EE);
	glTexCoord2f(0, 1);
	glVertex3dv(GG);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.2);
	glTexCoord2f(0, 0);
	glVertex3dv(BB);
	glTexCoord2f(1, 0);
	glVertex3dv(CC);
	glTexCoord2f(1, 1);
	glVertex3dv(DD);
	glTexCoord2f(0, 1);
	glVertex3dv(EE);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.2);
	glTexCoord2f(0, 0);
	glVertex3dv(EE);
	glTexCoord2f(1, 0);
	glVertex3dv(FF);
	glTexCoord2f(1, 1);
	glVertex3dv(NN);
	glTexCoord2f(0, 1);
	glVertex3dv(GG);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.2, 0.2);
	glTexCoord2f(0, 0);
	glVertex3dv(AQ);
	glTexCoord2f(1, 0);
	glVertex3dv(BQ);
	glTexCoord2f(1, 1);
	glVertex3dv(BB);
	glTexCoord2f(0, 1);
	glVertex3dv(AA);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.6, 0.2, 0.2);
	glTexCoord2f(0, 0);
	glVertex3dv(BQ);
	glTexCoord2f(1, 0);
	glVertex3dv(CQ);
	glTexCoord2f(1, 1);
	glVertex3dv(CC);
	glTexCoord2f(0, 1);
	glVertex3dv(BB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.2, 0.6);
	glTexCoord2f(0, 0);
	glVertex3dv(CQ);
	glTexCoord2f(1, 0);
	glVertex3dv(DQ);
	glTexCoord2f(1, 1);
	glVertex3dv(DD);
	glTexCoord2f(0, 1);
	glVertex3dv(CC);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.2, 0.6, 0.6);
	glTexCoord2f(0, 0);
	glVertex3dv(DQ);
	glTexCoord2f(1, 0);
	glVertex3dv(E);
	glTexCoord2f(1, 1);
	glVertex3dv(EE);
	glTexCoord2f(0, 1);
	glVertex3dv(DD);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.6, 0.6, 0.6);
	glTexCoord2f(0, 0);
	glVertex3dv(E);
	glTexCoord2f(1, 0);
	glVertex3dv(F);
	glTexCoord2f(1, 1);
	glVertex3dv(FF);
	glTexCoord2f(0, 1);
	glVertex3dv(EE);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0.4, 0.8);
	glTexCoord2f(0, 0);
	glVertex3dv(N);
	glTexCoord2f(1, 0);
	glVertex3dv(G);
	glTexCoord2f(1, 1);
	glVertex3dv(GG);
	glTexCoord2f(0, 1);
	glVertex3dv(NN);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0.8, 0.4);
	glTexCoord2f(0, 0);
	glVertex3dv(G);
	glTexCoord2f(1, 0);
	glVertex3dv(AQ);
	glTexCoord2f(1, 1);
	glVertex3dv(AA);
	glTexCoord2f(0, 1);
	glVertex3dv(GG);
	glEnd();
	float altpoints1x[110];
	float altpoints1y[110];
	float altpoints2x[110];
	float altpoints2y[110];

	double FN[] = { (N[0] + F[0]) / 2, (N[1] + F[1]) / 2, 0 };
	double d = sqrt((F[0] - N[0]) * (F[0] - N[0]) - (F[1] - N[1]) * (F[1] - N[1]));
	double ds = d / 2;
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.9, 0.2, 0.4);
	for (int i = 3; i < 105; i++) {
		float angle = i * 3.14159 / 100;
		float x = FN[0] + ds * cos(angle);
		float y = FN[1] + ds * sin(angle);
		altpoints1x[i] = { x };
		altpoints1y[i] = { y };
		glVertex3d(x, y, 0);
	};
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.6, 0.1, 0.1);
	for (int i = 3; i < 105; i++) {
		float angle = i * 3.14159 / 100;
		glVertex3d(FN[0] + ds * cos(angle), FN[1] + ds * sin(angle), 4);
	};
	glEnd();
	glBegin(GL_QUAD_STRIP);
	int b = 104;
	glColor3d(0.1, 0.8, 0.8);
	for (int b = 104; b > 3; b--) {
		glVertex3f(altpoints1x[b], altpoints1y[b], 0);
		glVertex3f(altpoints1x[b], altpoints1y[b], 4);
		glVertex3f(altpoints1x[b - 1], altpoints1y[b - 1], 0);
		glVertex3f(altpoints1x[b - 1], altpoints1y[b - 1], 4);
	};
	glEnd();

	//������ ��������� ���������� ��������
	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };


	/*glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}