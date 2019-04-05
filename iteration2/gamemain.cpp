//header files
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <bits/stdc++.h>
#include "stb_image.h"
using namespace std;

//global variables used in all other files
float framerate,
    piby180 = 3.14 / 180,
    sceneX,
    sceneY,
    cnt = 0,
    eyePos[3] = {0.0, -0.7, 0.0},
    center[3] = {0.0, -0.0, -10.0},
    up[3] = {0.0, 1.0, 0.0},
    sceneDeltaX,
    sceneDeltaY,
    sceneAngleX,
    sceneAngleY,
    sceneDeltaAngleX,
    sceneDeltaAngleY,
    light0_X = 0.0,
    light0_Y = 0.0,
    light0_Z = 0.0,
    division = 0.0,
    sceneRotate = 0.0,
    subwayAngle = 0.0,
    globalZ = 0,
    zLength = 1.0,
    subwaySpeed = 0.1;

int windowWidth = 1500,
    windowHeight = 1500,
    windowX = 0,
    windowY = 0,
    showMenu = 1,
    subwaySides = 10,
    subwayUnitsCount = 51,
    level;

GLint subwayTexture,
    startTexture,
    obstacleTexture,
    rewardTexture;

float wallWidth = 1,
      wallAngle = 360.0 / subwaySides,
      wallDistFromCenter = (wallWidth / 2) / tan((wallAngle / 2) * (3.14159 / 180));

#include "lighting.h"
#include "keyboard.h"
float subwayCurve(float, float (*)(float));
void drawGame();
void drawMenu();
void initGameAndControls();
void initDeque();
void handleTimer(int);

// Data structures for positioning subway and obstacles
struct subwayUnitInfo
{
    float centerX,
        centerY,
        centerZ;
    pair<int, int> obstaclePos;
    subwayUnitInfo(float cX, float cY, float cZ, pair<int, int> oP)
    {
        centerX = cX;
        centerY = cY;
        centerZ = cZ;
        obstaclePos = oP;
    }
};
deque<subwayUnitInfo> dq;

GLuint LoadAny(const char *fileName)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(fileName, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}
// Loads Texture in Windows 3.0, 24bit .bmp format
GLuint LoadBMP(const char *fileName)
{
    FILE *file;
    unsigned char header[54], *data;
    unsigned int dataPos, size, width, height;
    file = fopen(fileName, "rb");
    fread(header, 1, 54, file);
    dataPos = *(int *)&(header[0x0A]);
    size = *(int *)&(header[0x22]);
    width = *(int *)&(header[0x12]);
    height = *(int *)&(header[0x16]);

    if (size == 0)
        size = width * height * 3;
    if (dataPos == 0)
        dataPos = 54;
    data = new unsigned char[size];
    fread(data, 1, size, file);
    fclose(file);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    //glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    delete[] data;
    return texture;
}

void makeMenu()
{
    glDisable(GL_LIGHTING);
    //glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, startTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.2, 0);
    glVertex3f(-1, -1, 0);
    glTexCoord2f(0.8, 0);
    glVertex3f(1, -1, 0);
    glTexCoord2f(1, 1);
    glVertex3f(1, 1, 0);
    glTexCoord2f(0.4, 1);
    glVertex3f(-1, 1, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //glColor3f(0.1, 0.1, 0.1);
    glEnable(GL_LIGHTING);
}

void handleResize(int newWidth, int newHeight)
{
    windowWidth = newWidth;
    windowHeight = newHeight;
    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
    glLoadIdentity();            //Reset the camera
    gluPerspective(45.0, (float)newWidth / (float)newHeight, 0.01, 1000);
}

void handleMouse(int button, int state, int x, int y)
{
    if (showMenu == 1 && button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        showMenu = 0;
        framerate = 50;
        handleResize(windowWidth, windowHeight);
        initGameAndControls();
    }

    glutPostRedisplay();
}


// // function for obstacle color
// void initObstacleProperties() {
//     GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
//     GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
//     GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//     glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
// }

// generate Window
void initWindow()
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("subway Simulator");
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.1, 0.1, 0.1, 0.0);
}

// begin game and set all keyboard controls
void initGameAndControls()
{
    glutDisplayFunc(drawGame);
    initDeque();
    glutTimerFunc(0, handleTimer, 0);
    glutSpecialFunc(handleSpecialFunc);
    glutKeyboardFunc(handleKeyboardFunc);
    initPerspectiveAndCamera();
}

// initializing Resize Menu and Mouse functions
void initBasicCallbacks()
{
    glutReshapeFunc(handleResize);
    glutDisplayFunc(drawMenu);
    glutMouseFunc(handleMouse);
}

// loads Textures
void loadTextures()
{
    subwayTexture = LoadBMP("textures/white.bmp");
    startTexture = LoadBMP("textures/start.bmp");
    obstacleTexture = LoadBMP("textures/obstacle.bmp");
    rewardTexture = LoadAny("textures/gold");
}

void singleSide(float xWidth)
{
    float miniSquares = 1;
    for (float z = -zLength / 2; z < zLength / 2; z += zLength / miniSquares)
    {
        for (float x = -xWidth / 2; x < xWidth / 2; x += xWidth / miniSquares)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, subwayTexture);
            glBegin(GL_QUADS);
            glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(x, 0, z);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(x + xWidth / miniSquares, 0, z);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(x + xWidth / miniSquares, 0, z + zLength / miniSquares);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(x, 0, z + zLength / miniSquares);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
    }
}
// Designing a Single Obstacle
void singleObstacle(float xWidth, float yHeight, float zThickness)
{
    glPushMatrix();
    glScalef(xWidth, yHeight, zThickness);
    glTranslatef(0.0, 0.5, 0.5);
    glEnable(GL_TEXTURE_2D);
    float angle, radian, x, y; // values needed by drawCircleOutline

    glBindTexture(GL_TEXTURE_2D, obstacleTexture);

    glBegin(GL_POLYGON);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5, 0.0, 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(.5, .5, 0);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0, .5, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void singleReward(float xWidth, float yHeight, float zThickness)
{
    glPushMatrix();
    glScalef(xWidth, yHeight, zThickness);
    glTranslatef(0, 0.5, 0.5);
    //glColor3f(1, 0, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,rewardTexture);
    glBegin(GL_POLYGON);    
    for (int angle = 0.0; angle < 360.0; angle += 2.0)
    {
        float radian = angle * (piby180);
        float x = cos(radian) * 0.25 + 0.25;
        float y = sin(radian) * 0.25 + 0.25;
        glTexCoord2f(x, y);
        glVertex3f(x, y, 0);
        
    }
    glEnd();
    //glColor3f(0.1, 0.1, 0.1);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void genrandom()
{
    cnt = rand() % 5;
}
pair<int, int> rarity()
{
    static int i = 0;
    if (i == cnt)
    {
        genrandom();
        i = 0;
        int arr[]={0,1,2,9,8};
        return make_pair(arr[rand()%5], rand() % 2);
    }
    i++;
    return make_pair(-1, 1);
}


// Function to calculate subway position
float subwayCurve(float zPos, float (*curveFunc)(float))
{
    float stretch = 1000;
    float radToDeg = zPos / piby180;
    float amplitude = 1;
    return 1;
}

// Function to calculate derivative of subway curve
float subwayCurveDerivative(float zPos, float (*curveFunc)(float))
{
    return (subwayCurve(zPos + 0.01, curveFunc) - subwayCurve(zPos, curveFunc)) / 0.01;
}

// function to initialize starting deque()
void initDeque()
{
    for (int i = 0; i < subwayUnitsCount; i++)
    {
        pair<int, int> oP = rarity();
        subwayUnitInfo tui(subwayCurve(globalZ, sin), subwayCurve(globalZ, cos), globalZ, oP);
        dq.push_back(tui);
        globalZ -= zLength;
    }
}


// draws full subway
void drawFullsubway()
{
    for (int i = 0; i < dq.size(); i++)
    {
        glPushMatrix();
        glTranslatef(dq[i].centerX, dq[i].centerY, dq[i].centerZ);
        float reqAngleX = atan(subwayCurveDerivative(globalZ + subwayUnitsCount * zLength, sin)) * 180 / 3.14;
        float reqAngleY = atan(subwayCurveDerivative(globalZ + subwayUnitsCount * zLength, cos)) * 180 / 3.14;
        glRotatef(reqAngleX, 0.0, 1.0, 0.0);
        glRotatef(reqAngleY, -1.0, 0.0, 0.0);
        
        pair<int, int> oP = dq[i].obstaclePos;
        for (int i = 0; i < subwaySides; i++)
        {
            glPushMatrix();
            glRotatef(-wallAngle * i, 0.0, 0.0, 1.0);
            glTranslatef(0.0, -wallDistFromCenter, 0);
        
            if (i == oP.first)
            {
                if (oP.second)
                {
                    //initObstacleProperties();
                    singleObstacle(2*wallWidth/3, 2*wallWidth/3, 2*wallWidth/3);
                }
                else
                    singleReward(2*wallWidth/3, 2*wallWidth/3, 2*wallWidth/3);
            }
            initMaterialProperties();
            singleSide(wallWidth);
            glPopMatrix();
        }
        glPopMatrix();
    }
    glDisable(GL_LIGHTING);
    glColor3f(2.0,0,0);
    glPushMatrix();
    glRotatef(sceneRotate,0,0,-1);
    glTranslatef(dq[2].centerX,dq[2].centerY,dq[2].centerZ);
    glTranslatef(0, -1, 0);
    glScalef(0.25,0.25,0.25);
    
    glutWireSphere(0.5,20,20);
    glPopMatrix();
    glEnable(GL_LIGHTING);


}

// precalculations for the scene
void sceneMovementCalculations()
{

    sceneX = -subwayCurve(globalZ + subwayUnitsCount * zLength, sin);
    float nextSceneX = -subwayCurve((globalZ - zLength) + subwayUnitsCount * zLength, sin);
    sceneDeltaX = (nextSceneX - sceneX) * division;

    sceneY = -subwayCurve(globalZ + subwayUnitsCount * zLength, cos);
    float nextsceneY = -subwayCurve((globalZ - zLength) + subwayUnitsCount * zLength, cos);
    sceneDeltaY = (nextsceneY - sceneY) * division;

    sceneAngleX = -atan(subwayCurveDerivative(globalZ + subwayUnitsCount * zLength, sin)) * 180 / 3.14;
    float nextsceneAngleX = -atan(subwayCurveDerivative((globalZ - zLength) + subwayUnitsCount * zLength, sin)) * 180 / 3.14;
    sceneDeltaAngleX = ((nextsceneAngleX) - (sceneAngleX)) * division;

    sceneAngleY = -atan(subwayCurveDerivative(globalZ + subwayUnitsCount * zLength, cos)) * 180 / 3.14;
    float nextsceneAngleY = -atan(subwayCurveDerivative((globalZ - zLength) + subwayUnitsCount * zLength, cos)) * 180 / 3.14;
    sceneDeltaAngleY = ((nextsceneAngleY) - (sceneAngleY)) * division;
}

void drawMenu()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    makeMenu();
    glFlush();
}

// draws main game
void drawGame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    sceneMovementCalculations();
    glPushMatrix();
    initPerspectiveAndCamera();
    initLights();
    glTranslatef(sceneX + sceneDeltaX, 0, 0);
    glTranslatef(0, sceneY + sceneDeltaY, 0);
    glRotatef(sceneAngleX + sceneDeltaAngleX, 0.0, 1.0, 0.0);
    glRotatef(sceneAngleY + sceneDeltaAngleY, -1.0, 0.0, 0.0);
    drawFullsubway();
    glPopMatrix();
    glFlush();
    isPressed.clear();
    
}

// generates and acts upon timer event, which is triggered in accordance to framerate
void handleTimer(int)
{
    if (showMenu)
        return;
    division += 0.1;
    for (int i = 0; i < dq.size(); i++)
        dq[i].centerZ += subwaySpeed;

    if (dq[0].centerZ >= zLength)
    {
        // if(detectCollision(dq[1].obstaclePos)) {
        //     glutDisplayFunc(drawGameOver);
        //     resetAll();
        //     //exit(0);
        // }
        division = 0.0;
        int lastIndex = dq.size() - 1;
        float zPos = dq[lastIndex].centerZ - zLength;

        pair<int, int> oP = rarity();
        subwayUnitInfo tui(subwayCurve(globalZ, sin), subwayCurve(globalZ, cos), zPos, oP);
        dq.push_back(tui);
        dq.pop_front();
        globalZ -= zLength;
    }
    glutPostRedisplay();
    glutTimerFunc(1000.0 / framerate, handleTimer, 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    initWindow();
    initBasicCallbacks();
    loadTextures();
    glutMainLoop();
}

