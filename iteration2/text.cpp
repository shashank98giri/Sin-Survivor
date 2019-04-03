//header files
#include <GL/glut.h>
#include <bits/stdc++.h>
using namespace std;

//global variables used in all other files
float   framerate,
        piby180 = 3.14/180,
        sceneX,
        sceneY,
        cnt=0,
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
        tunnelAngle = 0.0,
        globalZ = 0,
        zLength = 1.0,
        tunnelSpeed=0.1;

int     windowWidth = 600,
        windowHeight = 600,
        windowX = 0,
        windowY = 0,
        showMenu = 1,
        tunnelSides = 10,
        tunnelUnitsCount = 51,
        level;
        

GLint   tunnelTexture,
        startTexture,
        obstacleTexture;


float tunnelCurve(float, float (*)(float));
void drawGame();
void drawMenu();
void initGameAndControls();
void initDeque();
void handleTimer(int);

// Data structures for positioning tunnel and obstacles
struct tunnelUnitInfo {
    float   centerX,
            centerY,
            centerZ;
    pair<int,int> obstaclePos;
    tunnelUnitInfo(float cX, float cY, float cZ, pair<int,int> oP) {
        centerX = cX;
        centerY = cY;
        centerZ = cZ;
        obstaclePos = oP;
    }
};
map<char, bool> isPressed;
deque<tunnelUnitInfo> dq;

// Loads Texture in Windows 3.0, 24bit .bmp format
GLuint LoadBMP(const char *fileName)
{
    FILE *file;
    unsigned char header[54],*data;
    unsigned int dataPos,size,width, height;
    file = fopen(fileName, "rb");
    fread(header, 1, 54, file);
    dataPos = *(int*)&(header[0x0A]);
    size = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);
    
    if(size==0) size = width * height * 3;
    if (dataPos == 0) dataPos = 54;
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
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    //glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    delete [] data;
    return texture;
}


void makeMenu() {
    glDisable(GL_LIGHTING);
    //glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,startTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(0.2,0);glVertex3f(-1,-1,0);
        glTexCoord2f(0.8,0);glVertex3f(1,-1,0);
        glTexCoord2f(1,1);glVertex3f(1,1,0);
        glTexCoord2f(0.4,1);glVertex3f(-1,1,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //glColor3f(0.1, 0.1, 0.1);
    glEnable(GL_LIGHTING);
}




void handleResize(int newWidth, int newHeight) {
    windowWidth = newWidth; windowHeight = newHeight;
    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective 
    glLoadIdentity(); //Reset the camera
    gluPerspective(45.0, (float)newWidth/(float)newHeight, 0.01, 1000);
}

void handleMouse(int button, int state, int x, int y) {
    if(showMenu == 1 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        showMenu = 0;
        framerate=1000;
        handleResize(windowWidth, windowHeight);
        initGameAndControls();
    }
    

    glutPostRedisplay();
}




// function for tunnel color
void initMaterialProperties() {
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glShadeModel(GL_SMOOTH);
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
void initWindow() {
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowX, windowY);
    glutCreateWindow("Tunnel Trouble");
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.1,0.1,0.1,0.0);
}

// begin game and set all keyboard controls
void initGameAndControls() {
    glutDisplayFunc(drawGame);
    // system("killall paplay");
    // system("amixer -D pulse sset Master 100%");
    // system("paplay music.wav &");
    initDeque();
    glutTimerFunc(0, handleTimer, 0);
    //initPerspectiveAndCamera();
    
}

// initializing Resize Menu and Mouse functions
void initBasicCallbacks() {
    glutReshapeFunc(handleResize);
    glutDisplayFunc(drawMenu);
    glutMouseFunc(handleMouse);
}

// loads Textures
void loadTextures() {
    tunnelTexture = LoadBMP("textures/white.bmp");
    startTexture = LoadBMP("textures/start.bmp");
    obstacleTexture=LoadBMP("textures/obstacle.bmp");
}



void singleSide(float xWidth) {
    float miniSquares = 1;
    for(float z = -zLength/2; z < zLength/2; z += zLength/miniSquares) {
        for(float x = -xWidth/2; x < xWidth/2; x += xWidth/miniSquares) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tunnelTexture);
            glBegin(GL_QUADS);
                glNormal3f(0.0, 1.0, 0.0);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(x, 0, z);
                glTexCoord2f(1.0f, 0.0f); glVertex3f(x + xWidth/miniSquares, 0, z);
                glTexCoord2f(1.0f, 1.0f); glVertex3f(x + xWidth/miniSquares, 0, z + zLength/miniSquares);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(x, 0, z + zLength/miniSquares);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
    }
}
// Designing a Single Obstacle
void singleObstacle(float xWidth, float yHeight, float zThickness) {
    glPushMatrix();
    glScalef(xWidth, yHeight, zThickness);
    glTranslatef(0.0, 0.5, 0.5);
    glEnable(GL_TEXTURE_2D);
    float angle, radian, x, y;       // values needed by drawCircleOutline
    
    glBindTexture(GL_TEXTURE_2D, obstacleTexture);

    glBegin(GL_POLYGON);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5, 0.0,0);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(.5, .5,0);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0, .5,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void singleReward(float xWidth,float yHeight,float zThickness){
    glPushMatrix();
    glScalef(xWidth,yHeight,zThickness);
    glTranslatef(0,0.5,0.5);
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
    
        for (int angle=0.0; angle<360.0; angle+=2.0)
        {
            float radian = angle * (piby180);
            float x = cos(radian) * 0.25  + 0.25;
            float y = sin(radian) * 0.25  + 0.25;        
            glVertex3f(x, y,0);
        }
    glEnd();
    glColor3f(0.1,0.1,0.1);
    glPopMatrix();

}

void genrandom(){
    cnt=rand()%5;
}
pair<int,int> rarity(){
    static int i=0;
    if(i==cnt){
        genrandom();
        i=0;
        return make_pair(rand()%tunnelSides,rand()%10<9);
        
    }
    i++;
    return make_pair(-1,1);      

    
}
// Designing single Tunnel Ring
void tunnelUnit(pair<int,int> oP) {
    float   wallWidth = 0.5,
            wallAngle = 360.0/tunnelSides,
            wallDistFromCenter = (wallWidth/2)/tan((wallAngle/2)*(3.14159/180));

    for(int i = 0; i < tunnelSides; i++) {
        glPushMatrix();
        glRotatef(-wallAngle*i, 0.0, 0.0, 1.0);
        glTranslatef(0.0, -wallDistFromCenter, 0);
        if( i == oP.first ) {
            if(oP.second){
                //initObstacleProperties();
                singleObstacle(wallWidth, 0.5, 0.5);  
            }
            else
                singleReward(wallWidth,0.5,0.5);
            
        }
        initMaterialProperties();
        singleSide(wallWidth);
        glPopMatrix();
    }
} 


// Function to calculate tunnel position
float tunnelCurve(float zPos, float (*curveFunc)(float)) {
    float stretch = 1000;
    float radToDeg = zPos/piby180;
    float amplitude = 1;
    return amplitude * curveFunc( radToDeg / stretch );
}

// Function to calculate derivative of tunnel curve
float tunnelCurveDerivative(float zPos, float (*curveFunc)(float)) {
    return (tunnelCurve(zPos + 0.01, curveFunc) - tunnelCurve(zPos, curveFunc)) / 0.01;
}



// function to initialize starting deque()
void initDeque() {
    for(int i = 0; i < tunnelUnitsCount; i++) {
        pair<int,int> oP=rarity();        
        tunnelUnitInfo tui(tunnelCurve(globalZ, sin), tunnelCurve(globalZ, cos), globalZ,oP);
        dq.push_back(tui);
        globalZ -= zLength;
    }
}


void octagonRoomPlacer(tunnelUnitInfo tui) {
    
}

// draws full tunnel
void drawFullTunnel() {
    for(int i = 0; i < dq.size(); i++) {
        glPushMatrix();
        glTranslatef(dq[i].centerX, dq[i].centerY, dq[i].centerZ);
        float reqAngleX = atan(tunnelCurveDerivative(globalZ + tunnelUnitsCount * zLength, sin)) * 180 / 3.14;
        float reqAngleY = atan(tunnelCurveDerivative(globalZ + tunnelUnitsCount * zLength, cos)) * 180 / 3.14;
        glRotatef(reqAngleX, 0.0, 1.0, 0.0);
        glRotatef(reqAngleY, -1.0, 0.0, 0.0);
        float wallWidth = 0.5,
              wallAngle = 360.0 / tunnelSides,
              wallDistFromCenter = (wallWidth / 2) / tan((wallAngle / 2) * (3.14159 / 180));
        pair<int, int> oP = dq[i].obstaclePos;
        for (int i = 0; i < tunnelSides; i++)
        {
            glPushMatrix();
            glRotatef(-wallAngle * i, 0.0, 0.0, 1.0);
            glTranslatef(0.0, -wallDistFromCenter, 0);
            
            if (i == oP.first)
            {
                if (oP.second)
                {
                    //initObstacleProperties();
                    singleObstacle(wallWidth, 0.5, 0.5);
                }
                else
                    singleReward(wallWidth, 0.5, 0.5);
            }
            initMaterialProperties();
            singleSide(wallWidth);
            glPopMatrix();
        }
        glPopMatrix();
    }
}

// precalculations for the scene
void sceneMovementCalculations() {
    
    sceneX = -tunnelCurve(globalZ + tunnelUnitsCount*zLength, sin); 
    float nextSceneX = -tunnelCurve((globalZ - zLength) + tunnelUnitsCount*zLength, sin);
    sceneDeltaX = (nextSceneX - sceneX)*division;

    sceneY = -tunnelCurve(globalZ + tunnelUnitsCount*zLength, cos); 
    float nextsceneY = -tunnelCurve((globalZ - zLength) + tunnelUnitsCount*zLength, cos);
    sceneDeltaY = (nextsceneY - sceneY)*division;

    sceneAngleX = -atan(tunnelCurveDerivative(globalZ + tunnelUnitsCount*zLength, sin)) * 180/3.14;
    float nextsceneAngleX = -atan(tunnelCurveDerivative((globalZ - zLength) + tunnelUnitsCount*zLength, sin)) * 180/3.14;
    sceneDeltaAngleX = ((nextsceneAngleX) - (sceneAngleX))*division;

    sceneAngleY = -atan(tunnelCurveDerivative(globalZ + tunnelUnitsCount*zLength, cos)) * 180/3.14;
    float nextsceneAngleY = -atan(tunnelCurveDerivative((globalZ - zLength) + tunnelUnitsCount*zLength, cos)) * 180/3.14;
    sceneDeltaAngleY = ((nextsceneAngleY) - (sceneAngleY))*division;
}


void drawMenu() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    makeMenu();
    glFlush();
}


// draws main game
void drawGame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    sceneMovementCalculations();
    glPushMatrix();
    
    glTranslatef(sceneX + sceneDeltaX, 0, 0);
    glTranslatef(0, sceneY + sceneDeltaY, 0);
    glRotatef(sceneAngleX + sceneDeltaAngleX, 0.0, 1.0, 0.0);
    glRotatef(sceneAngleY + sceneDeltaAngleY, -1.0, 0.0, 0.0);
    drawFullTunnel();
    glPopMatrix();
    glFlush();
}

// generates and acts upon timer event, which is triggered in accordance to framerate
void handleTimer(int)
{
    if(showMenu) return;
    division += 0.1;
    for(int i = 0; i < dq.size(); i++) dq[i].centerZ += tunnelSpeed;

    if(dq[0].centerZ >= zLength) {
        // if(detectCollision(dq[1].obstaclePos)) {
        //     glutDisplayFunc(drawGameOver);
        //     resetAll();
        //     //exit(0);
        // }
        division = 0.0;
        int lastIndex = dq.size() - 1;
        float zPos = dq[lastIndex].centerZ - zLength;
        
        pair<int,int> oP=rarity();
        tunnelUnitInfo tui(tunnelCurve(globalZ, sin), tunnelCurve(globalZ, cos), zPos, oP);
        dq.push_back(tui);
        dq.pop_front();
        globalZ -= zLength;
    }
    glutPostRedisplay();
    glutTimerFunc(1000.0/framerate, handleTimer, 0);
}

int main(int argc,char **argv) {
    glutInit(&argc,argv);
    initWindow();
    initBasicCallbacks();
    loadTextures();
    glutMainLoop();
}