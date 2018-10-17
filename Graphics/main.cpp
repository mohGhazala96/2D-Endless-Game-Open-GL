#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>    // Needed to perform IO operations
#include <vector>
#include <algorithm>
using namespace std;
#define SPACEBAR 32
#include <string.h>
#include <math.h>
bool isGameover;
bool isPowerUpDestroyed;
bool isPowerUp1;
bool powerUpMovingUp;

double powerUpX;
double powerUpY;

int rotatingPlayer=0;
int currentScore=0;
double moveX;
double currentPlayerPos;
bool canShoot;
double enemyX;
double enemyY;
double enemySpeed;
bool isReverse;
double enemyHealth;
double enemyHealthMain;

struct Vector3 { int x; int y; int z; };
vector<Vector3> Bullets;
vector<Vector3> EnemyBullets;

//method signuters
void KeyRight(unsigned char key, int x, int y);
void KeyLeft(unsigned char key, int x, int y);
void Timer(int value);
void createPowerUp();

void moveEnemy();
void shoot (vector<Vector3> &positions);
int p0[2];
int p1[2];
int p2[2];
int p3[2];
int tar=4;

int* bezier(float t, int* p0,int* p1,int* p2,int* p3)
{
    int res[2];
    res[0]=pow((1-t),3)*p0[0]+3*t*pow((1-t),2)*p1[0]+3*pow(t,2)*(1-t)*p2[0]+pow(t,3)*p3[0];
    res[1]=pow((1-t),3)*p0[1]+3*t*pow((1-t),2)*p1[1]+3*pow(t,2)*(1-t)*p2[1]+pow(t,3)*p3[1];
    return res;
}

float RandomFloat(float min, float max)
{
    float r = (float)std::rand() / (float)RAND_MAX;
    return min + r * (max - min);
}

void motionFunction(int x, int y)
{
    y=600-y;
    if(x<0)
        x=0;
    if(x>700)
        x=700;
    if(y<0)
        y=0;
    if(y>600)
        y=600;
    if(tar==0)
    {
        p0[0]=x;
        p0[1]=y;
    }
    else if(tar==1)
    {
        p1[0]=x;
        p1[1]=y;
    }
    else if(tar==2)
    {
        p2[0]=x;
        p2[1]=y;
    }
    if(tar==3)
    {
        p3[0]=x;
        p3[1]=y;
    }
    glutPostRedisplay();
}




void print(int x, int y, const char *string)
{
    int len, i;

    //set the position of the text in the window using the x and y coordinates
    glRasterPos2f(x, y);

    //get the length of the string to display
    len = (int) strlen(string);

    //loop to display character by character
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,string[i]);
    }
}




// draws rectangles using the (x,y) of the bottom left vertex, width (w) and height (h)
void drawRect(int x, int y, int w, int h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}
// draws triangles using the (x,y) of the bottom left vertex, width (w) and height (h)
void drawScaleneTriangle(int x, int y, int w, int h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + (w/2.0f), y + h);
    glEnd();
}

void drawRightTriangle(int x, int y, int w, int h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glEnd();
}

void drawLeftTriangle(int x, int y, int w, int h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x , y+h);
    glVertex2f(x + w, y);
    glEnd();
}


// draws a circle using OpenGL's gluDisk, given (x,y) of its center and tis radius
void drawCircle(int x, int y, float r) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    GLUquadric *quadObj = gluNewQuadric();
    gluDisk(quadObj, 0, r, 50, 50);
    glPopMatrix();
}
void SpecialInput(int key, int x, int y)
{
    if(!isGameover)
    switch(key)
    {

        case SPACEBAR:
            //            cout << "hello, world" << endl;  // Say Hello
            //shoot
            shoot(Bullets);
            currentPlayerPos=currentPlayerPos;
            break;
        case GLUT_KEY_LEFT:
            currentPlayerPos-=50;
            rotatingPlayer=-3;
            break;
        case GLUT_KEY_RIGHT:
            currentPlayerPos+=50;
            rotatingPlayer=3;
            break;
    }
    glutPostRedisplay();
}

void shoot (vector<Vector3> &positions)
{


    Vector3  newPos ;
    newPos.x=currentPlayerPos;
    newPos.y=120;
    newPos.z=0;
    positions.push_back(newPos);


}
void EnemyShoot (vector<Vector3> &positions)
{


    Vector3  newPos ;
    newPos.x=enemyX;
    newPos.y=enemyY-10;
    newPos.z=0;
    positions.push_back(newPos);


}

void drawHealth(float health) {

    float incrementation= 0;
    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);
    for(float i = 0; i < health; i +=0.25,incrementation+=18) {
        glVertex2f(20, 700+incrementation);
        glVertex2f(70, 700+incrementation);
        glVertex2f(70,700 + incrementation+10);
        glVertex2f(20, 700 +incrementation+10);
    }
    glEnd();
}

void Anim()
{
    
    vector<Vector3> BulletsTemp;
    for (auto bullet : Bullets){
        bool isHit =false;
        Vector3 currentBulletPos;
        currentBulletPos.x=bullet.x;
        currentBulletPos.y=bullet.y+5;
        currentBulletPos.z=0;
        vector<Vector3> EnemyBulletsTemp;

        if((enemyX+140 >=currentBulletPos.x && enemyX-80 <=currentBulletPos.x ) &&
           (enemyY+40 >=currentBulletPos.y && enemyY-40 <=currentBulletPos.y ) ){
            
            isHit=true;
            enemyHealth-=0.25;
            currentScore+=1;
            if(enemyHealth==0)
            {
                enemySpeed=0;
                isReverse=false;
                p1[0]=RandomFloat(50,100);
                p1[1]=RandomFloat(500,700);
                
                p2[0]=RandomFloat(850,900);
                p2[1]=RandomFloat(500,700);
                enemyHealthMain+=0.25;

                enemyHealth=enemyHealthMain;
            }
            continue;
        }
        if((powerUpX+35 >=currentBulletPos.x && powerUpX-25  <=currentBulletPos.x ) &&
           (powerUpY+15 >=currentBulletPos.y && powerUpY-15 <=currentBulletPos.y ) ){
            isPowerUpDestroyed=true;
            if(isPowerUp1){
                currentScore+=10;
            }else{
                enemyHealth-=0.25;
            }
            isHit=true;
            
            continue;
        }
        
        
        for (auto bullet : EnemyBullets){
           bool EnemyIsHit =false;
   
            if((bullet.x+30 >=currentBulletPos.x && bullet.x-30 <=currentBulletPos.x ) &&
               (bullet.y+15 >=currentBulletPos.y && bullet.y-15 <=currentBulletPos.y ) ){
                EnemyIsHit=true;
                isHit=true;
            }

       
            if(!EnemyIsHit)
                EnemyBulletsTemp.push_back(bullet);


        }
        EnemyBullets=EnemyBulletsTemp;
        if(!isHit)
            BulletsTemp.push_back(currentBulletPos);

    }
    Bullets=BulletsTemp;

    vector<Vector3> EnemyBulletsTemp;
    for (auto bullet : EnemyBullets){
        //140 is death
        int playerY=125;
        Vector3 newPos;
        newPos.x=bullet.x;
        newPos.y=bullet.y-5;
        newPos.z=0;
        
        if((currentPlayerPos+50 >=newPos.x && currentPlayerPos-50 <=newPos.x ) &&
           (playerY+20 >=newPos.y && playerY-20 <=newPos.y ) ){

     
                isGameover=true;
            continue;
        }

        EnemyBulletsTemp.push_back(newPos);

    }
    EnemyBullets=EnemyBulletsTemp;
    moveEnemy();
    if(powerUpY<500&&powerUpMovingUp){
        powerUpY+=1.2f;

    }else{
        powerUpMovingUp=false;

    }
    
     if(powerUpY>200&&!powerUpMovingUp){
        powerUpY-=1.2f;

     }else{
         powerUpMovingUp=true;

     }
    glutPostRedisplay();
}
void moveEnemy() {

        int* p =bezier(enemySpeed,p0,p1,p2,p3);
        enemyX= p[0];
        enemyY=p[1];

    if(isReverse==false){
        if(enemySpeed>1){
            enemySpeed=1;

            isReverse=true;
        }
        else
            enemySpeed+=0.005;

        if(enemySpeed==0 || (enemySpeed>=0.245f&&enemySpeed<=0.255f )|| (enemySpeed>=0.495f&&enemySpeed<=0.505f )|| (enemySpeed>=0.745f&&enemySpeed<=0.705f )||enemySpeed==1 ){
            EnemyShoot(EnemyBullets);

        }
    }else{

        if(enemySpeed<0){
            enemySpeed=0;
            isReverse=false;
            p1[0]=RandomFloat(50,100);
            p1[1]=RandomFloat(500,700);

            p2[0]=RandomFloat(850,900);
            p2[1]=RandomFloat(500,700);
        }
        else
            enemySpeed-=0.005;
        if(enemySpeed==0 || (enemySpeed>=0.245f&&enemySpeed<=0.25f )|| (enemySpeed>=0.495f&&enemySpeed<=0.5f )|| (enemySpeed>=0.745f&&enemySpeed<=0.75f )||enemySpeed==1 ){
            EnemyShoot(EnemyBullets);

        }
    }



}

void createPowerUp(){
    double randomRange =RandomFloat(1, 6);
    if(isPowerUpDestroyed){
        isPowerUpDestroyed=false;
        powerUpX = RandomFloat(100, 800);
        powerUpY = RandomFloat(200, 500);
    if(randomRange>3){
        isPowerUp1=true;


    }else{
        isPowerUp1=false;

    }
    }
}
void Timer(int value){
    createPowerUp();
    glutTimerFunc(RandomFloat(3000, 10000), Timer, 0);
}

void Display() {



    glClear(GL_COLOR_BUFFER_BIT);
    std::string s = std::to_string(currentScore);
    char const *pchar = s.c_str();  //use char const* as target type
    print(500,760,pchar);

    glPointSize(1);
    glColor3f(0,0,0);
    glBegin(GL_POINTS);
    for(float t=0;t<1;t+=0.001)
    {
        int* p =bezier(t,p0,p1,p2,p3);
        glVertex3f(p[0],p[1],0);

    }
    glEnd();
    glPointSize(9);
    glBegin(GL_POINTS);
    glColor3f(0,0,0);
    glVertex3f(p0[0],p0[1],0);
    glColor3f(0,0,0);
    glVertex3f(p1[0],p1[1],0);
    glColor3f(0,0,0);
    glVertex3f(p2[0],p2[1],0);
    glColor3f(0,0,0);
    glVertex3f(p3[0],p3[1],0);
    glEnd();





    glPushMatrix();

    if(!isPowerUpDestroyed){
        if(isPowerUp1)
            glColor3f(1, 0, 1);
        else
            glColor3f(0, 1, 0.0f);
        drawRect(powerUpX, powerUpY, 30, 30);
    }

    // player bullets
    for (auto bullet : Bullets){
        glColor3f(0.65f, 0.4f, 0.0f);
        drawCircle( bullet.x, bullet.y,10);
        glColor3f(0.65f, 0.4f, 0.0f);

        drawCircle( bullet.x+5, bullet.y-10,10);
        glColor3f(0.65f, 0.4f, 0.0f);
        drawCircle( bullet.x-5, bullet.y-10,10);

    }

    // enemey bullets
    for (auto bullet : EnemyBullets){
        glColor3f(0.1f, 0.3f, 0.8f);
        drawScaleneTriangle(bullet.x, bullet.y,25, 25);
        drawRightTriangle(bullet.x-5, bullet.y+10,25, 25);
        drawLeftTriangle(bullet.x+5, bullet.y+10,25, 25);

    }
    // enemy
    glColor3f(1,1,1);
    drawRect(enemyX,enemyY,50,50);//body
    drawRightTriangle(enemyX-50,enemyY,30, 30);
    drawLeftTriangle(enemyX+70,enemyY,30, 30);


    drawRect(enemyX-65, enemyY-25, 10,60);//shield
    drawRect(enemyX-63, enemyY-60, 3,40);

    drawCircle( enemyX-65, enemyY+40,2);
    drawCircle( enemyX-55, enemyY+40,2);

    drawCircle( enemyX+108, enemyY+40,2);
    drawCircle( enemyX+118, enemyY+40,2);


    drawRect(enemyX+108, enemyY-25, 10,60);//point
    drawRect(enemyX+112, enemyY-60, 3,40);
    //


    //player
    if(!isGameover){
        glPushMatrix();
        drawHealth(enemyHealth);

      glRotated(rotatingPlayer,0,0,1);

        glColor3f(1.0f, 0.0f, 0.0f);
        drawRect(currentPlayerPos-35, 33, 10,40);

        glColor3f(1.0f, 0.0f, 0.0f);
        drawRect(currentPlayerPos+25, 33, 10,40);


        glColor3f(1.0f, 1.0f, 0.0f);
        drawCircle(currentPlayerPos, 100,43);

        glColor3f(1.0f, 0.6f, 0.0f);
        drawCircle(currentPlayerPos, 100,30);

        glColor3f(1.0f, 0.0f, 0.0f);
        drawScaleneTriangle(currentPlayerPos-10, 125,20, 20);

        glColor3f(1.0f, 0.6f, 0.0f);
        drawCircle(currentPlayerPos-30, 25,5);

        glColor3f(1.0f, 0.6f, 0.0f);
        drawCircle(currentPlayerPos+30, 25,5);
        
        rotatingPlayer=0;
        glPopMatrix();

        glPopMatrix();
    }else{
        print(400,400,"GAME OVER");

        
    }

    glFlush();
}

int main(int argc, char** argr) {  //change main from void to int, and put return 0.

    currentScore=0;
    enemyHealth=0.5;
    enemyHealthMain=0.5;
    isGameover=false;
    isPowerUpDestroyed=true;
    powerUpMovingUp=true;
    p0[0]=75;
    p0[1]=700;

    p1[0]=RandomFloat(75,100);
    p1[1]=RandomFloat(500,700);

    p2[0]=RandomFloat(850,900);
    p2[1]=RandomFloat(500,700);

    p3[0]=900;
    p3[1]=700;
    enemyX=0;
    enemyY=0;
    enemySpeed=0;
    currentPlayerPos= 300;
    isReverse=false;
    srand( time(NULL) ); //seeding for the first time only!

    glutInit(&argc, argr);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(150, 150);

    glutCreateWindow("Game");
    glutDisplayFunc(Display);
    glutTimerFunc(10000, Timer, 0);


    glutMotionFunc(motionFunction);

    glutIdleFunc(Anim);

    glutSpecialFunc(SpecialInput);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(0.0, 1000, 0.0, 800);

    glutMainLoop();

    return 0;  //must write return 0 in macOS.
}
