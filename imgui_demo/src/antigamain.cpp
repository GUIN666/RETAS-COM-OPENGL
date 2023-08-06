

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GL/glew.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <math.h>
#include <glm/glm.hpp>

#define INF 9999 /*!< Define o "Infinito" */
#define PI 3.14159265358979324 /*! Valor de PI */
/**
 *  Define as operações
 */
#define INACTIVE 0              
#define TWO_POINTS 1            /*!< Reta definida por dois pontos */
#define VECTOR_POINT 2          /*!< Reta definida por ponto e vetor */
#define LINE_SEGMENT 3          /*!< Equação paramétrica de um segmento de reta */
#define ANGLES_LINE 4           /*!< Ângulo entre retas */
#define PARALLEL_STRAIGHT 5     /*!< Retas paralelas */
#define ORTHOGONAL_STRAIGHT 6   /*!< Retas ortogonais */

/**
 * Definições da esfera
 */
#define SLICES 25 
#define STACKS (SLICES-2)

using namespace std;

/**
 * Estrutura que armazena coordenadas:
 *  - seja de um ponto ou vetor
 */
struct S {
  GLdouble x, y, z;

  // Construtor
  S(GLdouble x = 0, GLdouble y = 0, GLdouble z = 0) 
    : x(x), y(y), z(z) {}
};

typedef S Point, Obj, VectorLine;



Obj obj;
Point pointTemp1, pointTemp2, point, pointTemp3;

VectorLine vectorU, vectorV;

/**
 * Variáveis Globais
 */
GLint heightvp, widthvp; /*!< Variáveis que contém as dimenções da janela*/
int keyboard_event_x = 0, keyboard_event_y = 0; /*!< Variáveis para a movimentação do cenário */
double angle_x = 0, angle_y = 0; /*! Ângulo de rotação do cenário */
double objx = 0, objy = 0, objz = 0; /*!< Variáveis usadas para pegar a coordenada do click na tela */
double move_x = 0, move_y = 0, move_z = 0; /*!< Variáveis para movimentação do ponto corrente */
int operation = INACTIVE; /*!< Contêm a escolha da operação */
int countAngleLine = 0; /*!< Contador de retas para a operação Ângulo entre duas retas */
int pointCurrent = 0; /*!< Contador de clicks na tela */
GLfloat colorLines[3] = {0.490, 0.011, 0.486}; /*!< Cor das retas */

/**
 * Classe que representa a reta definida por dois pontos 
 */ 
class TwoPoints {
private:
    int x1, y1, z1, x2, y2, z2; /*!< Pontos para definir a reta */
    float v1, v2, v3; /*!< Coodernadas do vetor da reta */
public:
    /**
     * Construtor.
     * @param x1Val um inteiro contendo a coordenada x do primeiro ponto
     * @param y1Val um inteiro contendo a coordenada y do primeiro ponto
     * @param z1Val um inteiro contendo a coordenada z do primeiro ponto
     * @param x2Val um inteiro contendo a coordenada x do segundo ponto
     * @param y2Val um inteiro contendo a coordenada y do segundo ponto
     * @param z2Val um inteiro contendo a coordenada z do segundo ponto
     */
    TwoPoints(int x1Val, int y1Val, int z1Val, int x2Val, int y2Val, int z2Val) {
        x1 = x1Val; y1 = y1Val;  z1 = z1Val;
        x2 = x2Val; y2 = y2Val;  z2 = z2Val;        
    }
    /**
     * Construtor padrão
     */ 
    TwoPoints() { }
    /**
     * Objetivo: desenhar um reta, com base nos pontos adquiridos pelo Construtor da classe
     */
    void drawTwoPoints(void); 

    /**
     * Objetivo: Desenha a Reta de modo interativo
     * @param x1 um inteiro contendo a coordenada x do primeiro ponto
     * @param y1 um inteiro contendo a coordenada y do primeiro ponto
     * @param z1 um inteiro contendo a coordenada z do primeiro ponto
     * @param x2 um inteiro contendo a coordenada x do segundo ponto
     * @param y2 um inteiro contendo a coordenada y do segundo ponto
     * @param z2 um inteiro contendo a coordenada z do segundo ponto
     */ 
    void drawTwoPointsInterativeClass(int x1, int y1, int z1, int x2, int y2, int z2);

    // Mostra a equação geral da reta
    // void equationOverall();
};
/*
// Parâmetros da esfera
int numMeridians = 100;  // Número de meridianos (linhas verticais)
int numParallels = 50;  // Número de paralelos (linhas horizontais)
float radius = 1.0f;  // Raio da esfera
*/
void DrawSphere(float radius, int numMeridians, int numParallels) 
{
    // Vetor para armazenar os vértices da esfera
    std::vector<float> vertices;

    // Gerar os vértices da esfera
    for (int i = 0; i <= numParallels; ++i)
    {
        float theta = i * M_PI / numParallels; // Ângulo theta (latitude)
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= numMeridians; ++j)
        {
            float phi = j * 2 * M_PI / numMeridians; // Ângulo phi (longitude)
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Coordenadas x, y, e z do vértice
            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // Adicionar o vértice à lista de vértices
            vertices.push_back(radius * x);
            vertices.push_back(radius * y);
            vertices.push_back(radius * z);
        }
    }

    // Desenhar a esfera
    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
}
void TwoPoints::drawTwoPoints() {
    int t;

    // definindo as coordenadas do vetor: V = (v1, v2)
    v1 = x2-x1;
    v2 = y2-y1;
    v3 = z2-z1;

    // Primeiro Ponto
    glColor3f(0.0, 0.0, 100.0);
    glPushMatrix();
      glTranslatef(x1, y1, z1);
      //DrawSphere(1.0, SLICES, STACKS);
      glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
    glPopMatrix();

    // Reta
    glColor3fv(colorLines);
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
        for (t = -INF; t < INF; t++)
        {
            // Equação parametrica da Reta, onde t é um valor que varia assim achamos outros pontos da reta
            glVertex3f( x1 + t*v1 , y1 + t*v2 , z1 + t*v3 );
        }
    glEnd();
    glLineWidth(1.0);


    // Segundo ponto
    glColor3f(0.0, 0.0, 100.0);
    glPushMatrix();
      glTranslatef(x2, y2, z2);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix();
}

void TwoPoints::drawTwoPointsInterativeClass(int x1Val, int y1Val, int z1Val, int x2Val, int y2Val, int z2Val) {
  int t;

    // definindo as coordenadas do vetor: V = (v1, v2)
  v1 = x2Val-x1Val;
  v2 = y2Val-y1Val;
  v3 = z2Val-z1Val;

  //Primeiro Ponto
  if (pointCurrent == 2) {
    glColor3f(0.0, 0.0, 100.0);
    glPushMatrix();
      glTranslatef(x1Val, y1Val, z1Val);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix();
  }

  // Reta
  glColor3fv(colorLines);
  glLineWidth(2.0);
  glBegin(GL_LINE_STRIP);
      for (t = -INF; t < INF; t++)
      {
          // Equação parametrica da Reta, onde t é um valor que varia assim achamos outros pontos da reta
          glVertex3f( x1Val + t*v1 , y1Val + t*v2 , z1Val + t*v3 );
      }
  glEnd();
  glLineWidth(1.0);

  // Segundo ponto
  if (pointCurrent == 1) {
    glColor3f(100.0, 100.0, 0.0);
    glPushMatrix();
      glTranslatef(x2Val, y2Val, z2Val);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix(); 
  }
}

// Vetor da Classe TwoPoints
vector<TwoPoints> twoPoints;

// Iterator para manipular o Vetor.
vector<TwoPoints>::iterator twoPointsIterator; 

/**
 * Classe que representa a reta definida por ponto e vetor
 */
class VectorPoint {

private:
    // Coordenadas do Ponto
    int ptx, pty, ptz;
    // Coodernadas do vetor da reta
    float v1, v2, v3;
    int x1, x2, y1, y2, z1, z2;

    
public: 
    // Construtor
    VectorPoint(
      int x, int y, int z, 
      int x1Val, int y1Val, int z1Val,
      int x2Val, int y2Val, int z2Val 
    ) {        
      ptx = x; pty = y; ptz = z;
      x1 = x1Val; y1 = y1Val; z1 = z1Val;
      x2 = x2Val; y2 = y2Val; z2 = z2Val;
    }

    VectorPoint() { }

    // Desenha o vetor, o ponto e por fim a Reta gerada
    void drawVectorPoint(void);

    // Desenha o Ponto e o Vetor - enquanto estiver no modo interativo
    void drawVectorPointInterativeClass(int, int, int, int, int, int, int, int, int);
    
    // desenha a seta que indica direção do vetor
    // void drawArrow(int, int, int, int);
};

void VectorPoint::drawVectorPoint() {
   int t;
    
    // definindo o vetor diretor v = (v1, v2, v3)
    v1 = x2 - x1;
    v2 = y2 - y1;
    v3 = z2 - z1;

     // Desenha a Reta
    glColor3fv(colorLines);
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
      for (t = -INF; t < INF; t++)
      {
          // Equação parametrica da Reta, onde t pertence aos Reais
          glVertex3f( ptx + t*v1 , pty + t*v2 , ptz + t*v3);
      }
    glEnd();

    // Desenha o Ponto
    glColor3f(0.0, 0.0, 100.0);
    glPushMatrix();
      glTranslatef(ptx, pty, ptz);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix();

    // Desenha o Vetor
    glColor3f(0.0, 100.0, 100.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();

    // drawArrow(x1, y1, z1, x2, y2, z2);
    
    glLineWidth(1.0);
}

void VectorPoint::drawVectorPointInterativeClass(int ptx, int pty, int ptz, int x1, int y1, int z1, int x2, int y2, int z2) {
    v1 = x2 - x1;
    v2 = y2 - y1;
    v3 = z2 - z1;

    //Desenha o Ponto
    if (pointCurrent != 1) {
      glColor3f(0.0, 0.0, 100.0);
      glPushMatrix();
        glTranslatef(ptx, pty, ptz);
        DrawSphere(1.0, SLICES, STACKS);
      glPopMatrix();
    }

    if (pointCurrent == 2 || pointCurrent == 3) {
        //Desenha o Vetor
        glColor3f(100.0, 0.0, 0.0);
        glLineWidth(2.0);
        glBegin(GL_LINES);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
        glEnd();

        //desenha a setinha
        // drawArrow(x1, y1, x2, y2);

        glLineWidth(1.0);
    }
}

// Vetor da Classe VectorPoint
vector<VectorPoint> vectorPoint;

// Iterator para manipular o Vetor
vector<VectorPoint>::iterator vectorPointIterator;


//====================================================================================//
// Classe que representa a equação paramétrica de um segmento de reta
//====================================================================================//
class LineSegment {
private:
  // Pontos para definir a reta
  int x1, y1, z1, x2, y2, z2;
  // Coodernadas do vetor da reta
  float v1, v2, v3;
public:
  // Construtor 
  LineSegment(int x1Val, int y1Val, int z1Val, int x2Val, int y2Val, int z2Val) { 
    x1 = x1Val; y1 = y1Val; z1 = z1Val;  
    x2 = x2Val; y2 = y2Val; z2 = z2Val;   
  }

  LineSegment() { }

  // Desenha a Reta e as paralelas a ela
  void drawLineSegment(void);

  // Desenha a Reta Principal de modo interativo
  void drawLineSegmentInterativeClass(int, int, int, int, int, int);

  // mostra a equação paramétrica do segmento de reta
  // void equationParametric();
};

//====================================================================================//
// Objetivo da função: desenhar o segmento de reta, utilizando os dois pontos selecionados
//====================================================================================//
void LineSegment::drawLineSegment() {
  int t;

  // definindo o vetor: v=(v1,v2,v3)
  v1 = x2 - x1;
  v2 = y2 - y1;
  v3 = z2 - z1;

  // Segmento de reta
  glColor3fv(colorLines);
  glLineWidth(2.0);
  glBegin(GL_LINE_STRIP);
  // Como é um segmento de reta o intervalo é 0 <= t <= 1 
  for (t = 0; t <= 1; t++) {   
    // Equação parametrica do Segmento de reta
    glVertex3f( x1 + t*v1 , y1 + t*v2 , z1 + t*v3);   
  }
  glEnd();
  glLineWidth(1.0);

  // Primeiro e Segundo ponto
  glColor3f(0.0, 0.0, 100.0);
  glPushMatrix();
    glTranslatef(x1, y1, z1);
    DrawSphere(1.0, SLICES, STACKS);
  glPopMatrix();

    glColor3f(0.0, 0.0, 100.0);
  glPushMatrix();
    glTranslatef(x2, y2, z2);
    DrawSphere(1.0, SLICES, STACKS);
  glPopMatrix();
}

//====================================================================================//
// Objetivo da função: desenhar o segmento de reta (de forma interativa com o mouse).
//====================================================================================//
void LineSegment::drawLineSegmentInterativeClass(int x1, int y1, int z1, int x2, int y2, int z2) {
  int t;

  // Primeiro ponto
  if (pointCurrent != 1) {
    glColor3f(0.0, 0.0, 100.0);
    glPushMatrix();
      glTranslatef(x1, y1, z1);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix();
  }

  // Calculo das coordenadas do vetor
  v1 = x2 - x1;
  v2 = y2 - y1;
  v3 = z2 - z1;

  // Segmento de reta
  glColor3f(0.0, 0.0, 0.0);
  glLineWidth(2.0);
  glBegin(GL_LINE_STRIP);
  // Como é um segmento de reta o intervalo é 0 <= t <= 1 
  for (t = 0; t <= 1; t++) {   
    // Equação parametrica do Segmento de reta
    glVertex3f( x1 + t*v1 , y1 + t*v2 , z1 + t*v3);   
  }
  glEnd();
  glLineWidth(1.0);

  // Segundo ponto
  if (pointCurrent == 1) {
    glColor3f(100.0, 100.0, 0.0);
    glPushMatrix();
      glTranslatef(x2, y2, z2);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix();
  }
}

// Vetor da Classe LineSegment
vector<LineSegment> lineSegment;

// Iterator para manipular o vetor
vector<LineSegment>::iterator lineSegmentIterator;

//====================================================================================//
// Classe que representa o Ãngulo entre retas
//====================================================================================// 
class AnglesLine {
private:
  // Pontos para definir a reta
  int x1, y1, z1, x2, y2, z2;
  // Coodernadas do vetor da reta
  float v1, v2, v3;
public:
  // Construtor
  AnglesLine(int x1Val, int y1Val, int z1Val, int x2Val, int y2Val, int z2Val) {
    x1 = x1Val; y1 = y1Val; z1 = z1Val;
    x2 = x2Val; y2 = y2Val; z2 = z2Val;
  }
  // Construtor para desenhar a reta de modo interativo
  AnglesLine() { }
  // Desenha as duas Retas
  void drawAnglesLine(void); 

  // Desenha a Reta de modo interativo
  void drawAnglesLineInterativeClass(int, int, int, int, int, int);
};

//====================================================================================//
// Objetivo da função: desenhar as duas retas para depois calcular o ângulo ente elas
//====================================================================================//
void AnglesLine::drawAnglesLine() {
    int t;

    // Calculo das coordenadas do vetor
    v1 = x2 - x1;
    v2 = y2 - y1;
    v3 = z2 - z1;

    // Reta
    glLineWidth(2.0);
    glColor3fv(colorLines);
    glBegin(GL_LINE_STRIP);
        for (t = -INF; t < INF; t++) {
            // Equação parametrica da Reta, onde t pertence aos Reais
            glVertex3f( x1 + t*v1 , y1 + t*v2 , z1 + t*v3);
        }
    glEnd();
    glLineWidth(1.0);
    glPopMatrix();
}

//====================================================================================//
// Objetivo da função: desenhar as retas de forma interativa, usando a 
// class TwoPoints (reta definida por dois pontos)
//====================================================================================//
void AnglesLine::drawAnglesLineInterativeClass(int x1, int y1, int z1, int x2, int y2, int z2) {
    TwoPoints obj;
    obj.drawTwoPointsInterativeClass(x1, y1, z1, x2, y2, z2);
}

void calculeAngle() {
  float angle, param1, param2;
  if (countAngleLine == 2) {
    param1 = fabs(vectorU.x*vectorV.x + vectorU.y*vectorV.y + vectorU.z*vectorV.z);
    param2 = sqrt(vectorU.x*vectorU.x + vectorU.y*vectorU.y + vectorU.z*vectorU.z) * sqrt(vectorV.x*vectorV.x + vectorV.y*vectorV.y + vectorV.z*vectorV.z);
    
    angle = acos(param1/param2) * 180.0 / PI;

    stringstream stream;
    string s;
    stream << fixed << setprecision(2) << (angle);
    s = stream.str();

    glColor3f(1.0, 0.0, 1.0);
    glRasterPos3f(vectorU.x, vectorU.y, vectorU.z);
  }
}

// Vetor da Classe AnglesLine
vector<AnglesLine> anglesLine;      

// Iterator para manipular o vetor
vector<AnglesLine>::iterator anglesLineIterator;

//====================================================================================//
// Classe que representa as Retas Paralelas
//====================================================================================//
class ParallelStraight {

private:
  // Pontos para definir a reta
  int x1, y1, z1, x2, y2, z2;
  // Coodernadas do vetor da reta
  float v1, v2, v3;
public:
  // Construtor 
  ParallelStraight(int x1Val, int y1Val, int z1Val, int x2Val, int y2Val, int z2Val) {    
    x1 = x1Val; y1 = y1Val; z1 = z1Val; 
    x2 = x2Val; y2 = y2Val; z2 = z2Val;        
  }

  ParallelStraight() { }

  // Desenha a Reta e as paralelas a ela
  void drawParallelStraight(void);

  // Desenha a Reta Principal de modo interativo
  void drawParallelStraightInterativeClass(int, int, int, int, int, int);
};

//====================================================================================//
// Objetivo da função: desenhar a reta principal e as paralelas a ela
//====================================================================================//
void ParallelStraight::drawParallelStraight() {
    int t;
    int cont = 10, cont2 = 10;
    float m; // coeficiente angular da reta
    float x, y, z;
    // Calculo das coordenadas do vetor
    v1 = x2 - x1;
    v2 = y2 - y1;
    v3 = z2 - z1;

    // Retas Paralelas
    glLineWidth(2.0);
    for (int i = 0; i < 5; i++) {
      glColor3f(0.0, 100.0, 100.0);
      if (i == 0) {
        glColor3fv(colorLines); 
        x = x1; y = y1; z = z1;
      }else if (i % 2 == 0) {
        x = x1+cont; y = y1+cont; z = z1+cont; 
        cont += 10;
      } else {
        x = x1-cont2; y = y1-cont2; z = z1-cont2;
        cont2 += 10;
      }
      glBegin(GL_LINE_STRIP);
        for (t = -INF; t < INF; t++) {   
          // Equação parametrica da Reta, onde t pertence aos Reais
          glVertex3f( x + t*v1 , y + t*v2 , z + t*v3);
        }
      glEnd();
    }
    glLineWidth(1.0);

    // Primeiro e Segundo ponto
    glColor3f(0.0, 0.0, 100.0);
    glPushMatrix();
      glTranslatef(x1, y1, z1);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix();

      glColor3f(0.0, 0.0, 100.0);
    glPushMatrix();
      glTranslatef(x2, y2, z2);
      DrawSphere(1.0, SLICES, STACKS);
    glPopMatrix();
}

//====================================================================================//
// Objetivo da função: desenhar a reta principal de forma interativa, utilizando a classe
// TwoPoints (reta definida por dois pontos)
//====================================================================================//
void ParallelStraight::drawParallelStraightInterativeClass(int x1, int y1, int z1, int x2, int y2, int z2) {
  TwoPoints obj;
  obj.drawTwoPointsInterativeClass(x1, y1, z1, x2, y2, z2);
}
// Vetor da Classe ParallelStraight
vector<ParallelStraight> parallelStraight;

// Iterator para manipular o vetor
vector<ParallelStraight>::iterator parallelStraightIterator;


//====================================================================================//
// Classe que representa as Retas Paralelas 
//====================================================================================//
class OrthogonalStraight {
private:
  //Pontos para definir a reta
  int x1, y1, z1, x2, y2, z2;
  // Coodernadas do vetor da reta
  float v1, v2, v3;
public:
  //Construtor 
  OrthogonalStraight(int x1Val, int y1Val, int z1Val, int x2Val, int y2Val, int z2Val) {
    x1 = x1Val; y1 = y1Val; z1 = z1Val; 
    x2 = x2Val; y2 = y2Val; z2 = z2Val;   
  }

  OrthogonalStraight() { }

  //Desenha a Reta e sua Ortogonal
  void drawOrthogonalStraight(void);

  //Desenha a Reta Principal de modo interativo
  void drawOrthogonalStraightInterativeClass(int, int, int, int, int, int);
};

//====================================================================================//
// Objetivo da função: desenhar uma reta a partir de dois pontos, depois desenhar uma 
// reta ortogonal a primeira.
//====================================================================================//
void OrthogonalStraight::drawOrthogonalStraight() {
    int t;
    float w1, w2, w3;
    int q1, q2, q3; 

    //Calculo das coordenadas do vetor
    v1 = x2 - x1;
    v2 = y2 - y1;
    v3 = z2 - z1;

    //Vetor ortogonal
    w1 = v2*(-1);
    w2 = v1;
    w3 = 0.0;

    //Ponto para a reta ortogonal
    q1 = (x2+x1)/2;
    q2 = (y2+y1)/2;
    q3 = (z2+z1)/2 + 20;


    glLineWidth(2.0);
    //Primeira Reta
    glColor3fv(colorLines);
    glBegin(GL_LINE_STRIP);
        for (t = -INF; t < INF; t++) {
            // Equação parametrica da Reta, onde t pertence aos Reais
            glVertex3f( x1 + t*v1 , y1 + t*v2 , z1 + t*v3);
        }
    glEnd();

    //Reta Ortogonal a primeira Reta
    glColor3f(0.0, 100.0, 100.0);
    glBegin(GL_LINE_STRIP);
        for (t = -INF; t < INF; t++) {
            // Equação parametrica da Reta, onde t pertence aos Reais
            glVertex3f( q1 + t*w1 , q2 + t*w2 , q3 + t*w3);
        }
    glEnd();

    glLineWidth(1.0);
}

//====================================================================================//
// Objetivo da função: desenhar uma reta a partir de dois pontos, de forma interativa,
// usando a classe TwoPoints.
//====================================================================================//
void OrthogonalStraight::drawOrthogonalStraightInterativeClass(int x1, int y1, int z1, int x2, int y2, int z2) {
    TwoPoints obj;
    obj.drawTwoPointsInterativeClass(x1, y1, z1, x2, y2, z2);
}

// Vetor da Classe OrthogonalStraight
vector<OrthogonalStraight> orthogonalStraight;

// Iterator para manipular o vetor
vector<OrthogonalStraight>::iterator orthogonalStraightIterator;

void clearVectors() {
  twoPoints.clear();
  vectorPoint.clear();
  lineSegment.clear();
  anglesLine.clear();
  parallelStraight.clear();
  orthogonalStraight.clear();
}

void drawGrid() {
  glColor3f(0.8, 0.8, 0.8);
  for (int i = 0; i < 170; i += 10)
  {
    glBegin(GL_LINES);
    glVertex3f(-80 + i, 0, 100);
    glVertex3f(-80 + i, 0, -100);
    glEnd();
  }
  for (int i = 0; i < 220; i += 20)
  {
    glBegin(GL_LINES);
    glVertex3f(-80, 0, 100 - i);
    glVertex3f(80, 0, 100 - i);
    glEnd();
  }
}

void drawAxis() {
  glLineWidth(2.2);
    glColor3f(0, 0, 1);
    glRasterPos3f(0, 0, 102);
    //writeBitmapString((char*)"Z");   
    glBegin( GL_LINES );
        glVertex3f( 0, 0, 100);
        glVertex3f( 0, 0, 0);
    glEnd();
    glColor3f(1, 0, 0);
    glRasterPos3f(82, 0, 0);
    //writeBitmapString((char*)"X");
    glBegin( GL_LINES );
        glVertex3f( 0, 0, 0);
        glVertex3f( 80, 0, 0);
    glEnd();
    glColor3f(0, 1, 0);
    glRasterPos3f(0, 100, 0);
    //writeBitmapString((char*)"Y");
    glBegin( GL_LINES );
        glVertex3f( 0, 100, 0);
        glVertex3f( 0, 0, 0);
    glEnd();
  glLineWidth(1.0);
}

void drawIterative() {
  float x1, x2, y1, y2, z1, z2;
  x1 = pointTemp1.x;
  x2 = pointTemp2.x;
  y1 = pointTemp1.y;
  y2 = pointTemp2.y;
  z1 = pointTemp1.z;
  z2 = pointTemp2.z;

  if (operation == TWO_POINTS) {
    TwoPoints obj;
    obj.drawTwoPointsInterativeClass(x1, y1, z1, x2, y2, z2);
  }

  if (operation == VECTOR_POINT) {
    VectorPoint obj;
    obj.drawVectorPointInterativeClass(x1, y1, z1, x2, y2, z2, pointTemp3.x, pointTemp3.y, pointTemp3.z);
  }

  if (operation == LINE_SEGMENT) {
    LineSegment obj;
    obj.drawLineSegmentInterativeClass(x1, y1, z1, x2, y2, z2);
  }

  if (operation == ANGLES_LINE) {
    AnglesLine obj;
    obj.drawAnglesLineInterativeClass(x1, y1, z1, x2, y2, z2);
  }

  if (operation == PARALLEL_STRAIGHT) {
    ParallelStraight obj;
    obj.drawParallelStraightInterativeClass(x1, y1, z1, x2, y2, z2);
  }

  if (operation == ORTHOGONAL_STRAIGHT) {
    OrthogonalStraight obj;
    obj.drawOrthogonalStraightInterativeClass(x1, y1, z1, x2, y2, z2);
  }
}

void drawCurrentPoint(Point obj, int whatPoint) {
  glPushMatrix();
    glColor3f(100.0, 0.0, 0.0);
    glTranslatef(move_x, move_y, move_z);
    glTranslatef(obj.x, obj.y, obj.z);
    DrawSphere(1.0, SLICES, STACKS);

    if (whatPoint == 1) {
      pointTemp1.x = obj.x + move_x;
      pointTemp1.y = obj.y + move_y;
      pointTemp1.z = obj.z + move_z;
    } else if (whatPoint == 2) {
      pointTemp2.x = obj.x + move_x;
      pointTemp2.y = obj.y + move_y;
      pointTemp2.z = obj.z + move_z;
    } else if (whatPoint == 3) {
      pointTemp3.x = obj.x + move_x;
      pointTemp3.y = obj.y + move_y;
      pointTemp3.z = obj.z + move_z;
    }

  glPopMatrix();
  stringstream stream, stream2, stream3;
  string s = ("Ponto( ");
  stream << fixed << setprecision(2) << (obj.x+move_x);
  s += stream.str();
  s += ", ";
  
  stream2 << fixed << setprecision(2) << (obj.y+move_y);
  s += stream2.str();
  s += ", ";
  stream3 << fixed << setprecision(2) << (obj.z+move_z);
  s += stream3.str();
  s += " )";
  glColor3f(1.0, 0.0, 1.0);
  glRasterPos3f(obj.x, obj.y, 0);
}

void drawInformative() {
  switch (operation) {
  case INACTIVE:
    glColor3f(0.8, 0.0, 0.0);
    glRasterPos3f(-100, -30, 100);
    break;
  
  default:
    break;
  }
}
//====================================================================================//
// Objetivo da função: desenhar a reta e sua equação que estam no vector
//====================================================================================//
void drawTwoPointsIterator(void) {
    
    twoPointsIterator = twoPoints.begin();
    while (twoPointsIterator != twoPoints.end() ) {
      twoPointsIterator->drawTwoPoints();
      // twoPointsIterator->equationOverall();
	    twoPointsIterator++;
    }
}

//====================================================================================//
// Objetivo da função: desenhar a reta e o vetor que estam no vector
//====================================================================================//
void drawVectorPointIterator(void) {
  vectorPointIterator = vectorPoint.begin();
  while (vectorPointIterator != vectorPoint.end()) {
    vectorPointIterator->drawVectorPoint();
    vectorPointIterator++;
  } 
}

//====================================================================================//
// Objetivo da função: desenhar o segmento de reta e sua equação paramétrica que estam no
// vector.
//====================================================================================//
void drawLineSegmentIterator(void) {
  lineSegmentIterator = lineSegment.begin();
  while (lineSegmentIterator != lineSegment.end()) {
    lineSegmentIterator->drawLineSegment();
    // lineSegmentIterator->equationParametric();
    lineSegmentIterator++;
  } 
}

//====================================================================================//
// Objetivo da função: desenhar as retas que estam no vetor e o ângulo entre elas
//====================================================================================//
void drawAnglesLineIterator(void) {
  anglesLineIterator = anglesLine.begin();
  while (anglesLineIterator != anglesLine.end()) {
    anglesLineIterator->drawAnglesLine();
    anglesLineIterator++;
  }   

  if (operation == ANGLES_LINE) {
    calculeAngle();
  }//projetada para operações com formas geométricas, como pontos, linhas e v
}

//====================================================================================//
// Objetivo da função: desenhar as retas paralelas que estam no vetor
//====================================================================================//
void drawParallelStraightIterator(void) {
  parallelStraightIterator = parallelStraight.begin();
  while (parallelStraightIterator != parallelStraight.end()) {
      parallelStraightIterator->drawParallelStraight();
      parallelStraightIterator++;
  } 
}

//====================================================================================//
// Objetivo da função: desenhar a reta e sua ortogonal que estam no vetor
//====================================================================================//
void drawOrthogonalStraightIterator(void) {   
  orthogonalStraightIterator = orthogonalStraight.begin();
  while (orthogonalStraightIterator != orthogonalStraight.end()) {
    orthogonalStraightIterator->drawOrthogonalStraight();
    orthogonalStraightIterator++;
  }
}

/*
void display() {
  int offset = 10;
  glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
  
  drawInformative();
  
  if (keyboard_event_x == 1) {
    glRotatef(angle_x, 0.0, 1.0, 0.0);
  } else if (keyboard_event_y == 1) {
    glRotatef(angle_y, 1.0, 0.0, 0.0);
  }
  
  drawAxis(); /*desenha os eixos -> x, y, z
  drawGrid();
  
  if (pointCurrent == 1) {
    drawCurrentPoint(obj, 1);
    drawIterative();
  } 
  if (pointCurrent == 2) {
    drawCurrentPoint(obj, 2);
    drawIterative();
  }
  if (pointCurrent == 3) {
    drawCurrentPoint(obj, 3);
    drawIterative();
  }

  drawTwoPointsIterator();
  drawVectorPointIterator();
  drawLineSegmentIterator();
  drawAnglesLineIterator();
  drawParallelStraightIterator();
  drawOrthogonalStraightIterator();

  glFlush();
  glutSwapBuffers();
}
*/
/*
float anguloGraus = 90.0f;
float anguloRadiano = anguloGraus * M_PI / 180.0f;
float rotacaoMatrix[4][4] = {
    {cos(anguloRadiano), 0, sin(anguloRadiano), 0},
    {0, 1, 0, 0},
    {-sin(anguloRadiano), 0, cos(anguloRadiano), 0},
    {0, 0, 0, 1}
};
*/
void display(GLFWwindow* window) {
    int offset = 10;
    Point p1(1.0,1.0,1.0);
    Point p2(1.0,1.0,1.0);
    Point p3(1.0,1.0,1.0);
    // Limpar os buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawInformative();

    // Aplicar transformações de rotação.
    if (keyboard_event_x == 1) {
        glRotatef(angle_x, 0.0, 1.0, 0.0);
    } else if (keyboard_event_y == 1) {
        glRotatef(angle_y, 1.0, 0.0, 0.0);
    }

    drawAxis();
    drawGrid();

    if (pointCurrent == 1) {
        drawCurrentPoint(p1, 1);
        drawIterative();
    }
    if (pointCurrent == 2) {
        drawCurrentPoint(p2, 2);
        drawIterative();
    }
    if (pointCurrent == 3) {
        drawCurrentPoint(p3, 3);
        drawIterative();
    }

    drawTwoPointsIterator();
    drawVectorPointIterator();
    drawLineSegmentIterator();
    drawAnglesLineIterator();
    drawParallelStraightIterator();
    drawOrthogonalStraightIterator();

    // Trocar buffers
    glfwSwapBuffers(window);
}

void mouse(GLFWwindow* window, int button, double x, double y) {
  GLint viewport[4];
  GLdouble matModelView[16], matProjection[16];
  GLdouble winX, winY;  
  float z = 1 - 0.0001;
    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, matModelView);
    glGetDoublev (GL_PROJECTION_MATRIX, matProjection); 
    winX =  x;
    winY = viewport[3] -  y;

    gluUnProject ( winX, winY, z,
                  matModelView, matProjection, viewport,
                  &obj.x, &obj.y, &obj.z
                );
    
    glfwGetCursorPos(window, &obj.x, &obj.y);
    move_x = 0;
    move_y = 0;
    move_z = 0;
  
    if (pointCurrent == 0) {
      if (operation != ANGLES_LINE) {
        clearVectors();
      } else {
        if (countAngleLine == 2) {
          countAngleLine = 0;
          clearVectors();
        }
      }
      
      pointCurrent++;
    } else if (pointCurrent == 1) {
      pointCurrent++;
    } else if (pointCurrent == 2) {
      pointCurrent++;
      if (operation == TWO_POINTS) {
        twoPoints.push_back(
          TwoPoints(
            pointTemp1.x, pointTemp1.y, pointTemp1.z,
            pointTemp2.x, pointTemp2.y, pointTemp2.z
          )
        );
        pointCurrent = 0;
      }
      if (operation == LINE_SEGMENT) {
        lineSegment.push_back(
          LineSegment(
            pointTemp1.x, pointTemp1.y, pointTemp1.z,
            pointTemp2.x, pointTemp2.y, pointTemp2.z
          )
        );
        pointCurrent = 0;
      }
      if (operation == ANGLES_LINE) {
        anglesLine.push_back(
          AnglesLine(
            pointTemp1.x, pointTemp1.y, pointTemp1.z,
            pointTemp2.x, pointTemp2.y, pointTemp2.z
          )
        );
        if (countAngleLine == 0) {
          vectorU.x = pointTemp1.x; 
          vectorU.y = pointTemp1.y;
          vectorU.z = pointTemp1.z;
        } else {
          vectorV.x = pointTemp2.x; 
          vectorV.y = pointTemp2.y;
          vectorV.z = pointTemp2.z;
        }
        countAngleLine++;
        pointCurrent = 0;
      }
      if (operation == PARALLEL_STRAIGHT) {
        parallelStraight.push_back(
          ParallelStraight(
            pointTemp1.x, pointTemp1.y, pointTemp1.z,
            pointTemp2.x, pointTemp2.y, pointTemp2.z
          )
        );
        pointCurrent = 0;
      }
      if (operation == ORTHOGONAL_STRAIGHT) {
        orthogonalStraight.push_back(
          OrthogonalStraight(
            pointTemp1.x, pointTemp1.y, pointTemp1.z,
            pointTemp2.x, pointTemp2.y, pointTemp2.z
          )
        );
        pointCurrent = 0;
      }
          
    } else if (pointCurrent == 3) {
      if (operation == VECTOR_POINT) {
        vectorPoint.push_back(
          VectorPoint(
            pointTemp1.x, pointTemp1.y, pointTemp1.z,
            pointTemp2.x, pointTemp2.y, pointTemp2.z,
            pointTemp3.x, pointTemp3.y, pointTemp3.z
          )
        );
        pointCurrent = 0;
      }
    }

    keyboard_event_y = 0;
    keyboard_event_x = 0;

}

void mouseMotion(int x, int y) {
  GLint viewport[4];
  GLdouble matModelView[16], matProjection[16];
  GLdouble winX, winY;  
  float z = 1 - 0.0001;
  
  if (pointCurrent == 1 || pointCurrent == 2) {
    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, matModelView);
    glGetDoublev (GL_PROJECTION_MATRIX, matProjection); 
    winX = (GLdouble) x;
    winY = viewport[3] - (GLdouble) y;

    gluUnProject ( winX, winY, z,
                   matModelView, matProjection, viewport,
                   &objx, &objy, &objz
                 );
    
    if (operation == VECTOR_POINT) {
      pointTemp3.x = objx;
      pointTemp3.y = objy;
      pointTemp3.z = objz;
      
    } else {
      pointTemp2.x = objx;
      pointTemp2.y = objy;
      pointTemp2.z = objz;
    }
  } 
  
  keyboard_event_y = 0;
  keyboard_event_x = 0;

}

void keyboard(GLFWwindow* window, int x, int y) {

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    keyboard_event_x = 1;
    keyboard_event_y = 0;
    angle_x = 2.0;
  }
  else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  {
    keyboard_event_x = 1;
    keyboard_event_y = 0;
    angle_x = -2.0;
  }
  else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)  {
    keyboard_event_y = 1;
    keyboard_event_x = 0;
    angle_y = 2.0;
  }

  else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  {
    keyboard_event_y = 1;
    keyboard_event_x = 0;
    angle_y = -2.0;
  }
}

void keyInput(unsigned char key, int x, int y) {
  keyboard_event_y = keyboard_event_x = 0;
  switch (key) {
  case 'x':
    move_x += 1;
    break;
  case 'X':
    move_x -= 1;
    break;
  case 'y':
    move_y += 1;
    break;
  case 'Y':
    move_y -= 1;
    break;
  case 'z':
    move_z += 1;
    break;
  case 'Z':
    move_z -= 1;
    break;
  case 27:
    exit(0);
    break;
  }
}

void init(int width, int height) {
  glClearColor(0.1, 0.1, 0.1, 1);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1.33, 0.1, 400);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  heightvp = height;
  widthvp = width;
}


  int largura = 800;
  int altura = 600;
  double lastX = largura / 2.0;
  double lastY = altura / 2.0;
Point cameraPos = { -50, 100, 250 };
Point cameraTarget = { 0, 0, 0 };
Point cameraUp = { 0, 2, 0 };
void drawMenu()
{
    int opcao;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)largura / 3, (float)altura));
    // Adicione alguns controles ao menu
    static int counter = 0;
      ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    // opções
    if (ImGui::Button("O que é um reta?"))
    {
      opcao = 9;
    }
    if (ImGui::Button("O que é um ponto?"))
    {
      opcao = 10;
    }
    if (ImGui::Button("Reta definida por dois pontos"))
    {
      opcao = 1;
    }
    if (ImGui::Button("Reta definida por ponto e vetor"))
    {
      opcao = 2;
    }
    if (ImGui::Button("Equação paramétrica da reta"))
    {
      opcao = 3;
    }
    if (ImGui::Button("Ângulo entre duas retas"))
    {
      opcao = 4;
    }
    if (ImGui::Button("Retas Paralelas"))
    {
      opcao = 5;
    }
    if (ImGui::Button("Retas Ortogonais"))
    {
      opcao = 6;
    }

    //ImGui::SameLine(); //mesma linha
    ImGui::Text("Selecione uma opção");  // Mostra o valor do contador

    // Finalize a janela ImGui
    ImGui::End();
}

int main(int argc, char **argv)
{


  int opcao = 0;
// Variáveis para armazenar a posição do mouse anterior

  // Initialize GLFW
  glfwInit();

  // Create a window
  GLFWwindow *window = glfwCreateWindow(largura, altura, "Retas", NULL, NULL);
  glfwMakeContextCurrent(window);


  // Initialize GLEW
  glewInit();

  // Enable the position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Initialize ImGui
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  // Set up a rotation variable
  float rotation = 0.0f;
  // Configurar a matriz de projeção
  glMatrixMode(GL_PROJECTION);
  init(altura, largura);

  glLoadIdentity();
  gluPerspective(65.0, (GLfloat)largura / (GLfloat)altura, 1.0, 20.0);
  // Configurar a matriz de visualização/modelo
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glOrtho(0, altura, 0, largura, -1., 1.);




  // Callback para movimento do mouse
glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // Y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    float sensitivity = 0.1f; // Change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    angle_x += xoffset;
    angle_y += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (angle_y > 89.0f)
        angle_y = 89.0f;
    if (angle_y < -89.0f)
        angle_y = -89.0f;
});
// Callback para pressionamento do botão do mouse para desenhar um ponto
glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
       double x, y;
        // Obter as coordenadas do mouse
        glfwGetCursorPos(window, &x, &y);
        glEnable(GL_DEPTH_TEST);
        // Obter as matrizes de projeção e modelo e o viewport
        GLdouble modelMatrix[16], projectionMatrix[16];
        GLint viewport[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        // Obter a posição z do pixel sob o cursor
        GLfloat z;
        glReadPixels(x, viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
        
        // Transformar as coordenadas do mouse para o espaço do mundo
        GLdouble objX, objY, objZ;
        gluUnProject(x, viewport[3] - y, z, modelMatrix, projectionMatrix, viewport, &objX, &objY, &objZ);
        
        // Desenhar um ponto no local do clique do mouse
        glPointSize(1.0f);
        glBegin(GL_POINTS);
        glColor3f(0.1f, 0.5f, 0.0f);
        glVertex3f(objX, objY, objZ);
        glEnd();
        
    }
});
// Callback para pressionamento de teclas
glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    float Speed = 1.0f;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // Atualize a rotação ou a posição da câmera com base na tecla pressionada
        if (key == GLFW_KEY_W)
          cameraPos.z += Speed;
        if (key == GLFW_KEY_S)
          cameraPos.z -= Speed;
        if (key == GLFW_KEY_A)
          cameraPos.x -= Speed;
        if (key == GLFW_KEY_D)
          cameraPos.x += Speed;
         
    }
    
});
  while (!glfwWindowShouldClose(window))
  {

    //clear tela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // menu esquerdo
    drawMenu();
  glClearColor(0.1, 0.1, 0.1, 1);
  glViewport(0, 0, altura, largura);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, 
          cameraTarget.x, cameraTarget.y, cameraTarget.z, 
          cameraUp.x, cameraUp.y, cameraUp.z);
    // Set up the model, view, and projection matrices here
    display(window);
    cameraTarget.x = angle_x;
    cameraTarget.y = angle_y;
  
    // enable z buffer
    glEnable(GL_DEPTH_TEST);
    // set the value in z-buffer as 1.0
    glClearDepth(1.0);

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    

    // Swap buffers
    glfwSwapBuffers(window);

    // Poll events
    glfwPollEvents();
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}
