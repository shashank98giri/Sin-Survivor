
map<char,bool> isPressed;
void handleSpecialFunc(int key,int x,int y){
    switch(key){
        
        case GLUT_KEY_UP :
            isPressed['w']=true;
            break;
        case GLUT_KEY_DOWN:
            isPressed['s']=true;
            break;
        case GLUT_KEY_LEFT:
            isPressed['a'] = true;
            break;
        case GLUT_KEY_RIGHT:
            isPressed['d'] = true;
            break;
    }
}
void handleKeyboardFunc(unsigned char key,int x,int y){
    switch(key){
        case 'w':isPressed['w']=true;
                break;
        case 'a':isPressed['a']=true;
                break;
        case 's':
            isPressed['s'] = true;
            break;
        case 'd':
            isPressed['d'] = true;
            break;
        case '1':
            isPressed['1'] = true;
            break;
        case '3':
            isPressed['3'] = true;
            break;
    }
}




void initPerspectiveAndCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(isPressed['1'])eyePos[1]=min(eyePos[1]+0.1f,-0.2f);
    if(isPressed['3'])eyePos[1]=max(eyePos[1]-0.1f,-0.9f);
    //if(isPressed['2'])eyePos[2]=min(eyePos[2])
    if(isPressed['a'])center[0]=max(center[0]-0.5f,-1.0f);
    if(isPressed['d'])center[0]=min(center[0]+0.5f,1.0f);
    
    gluPerspective(45.0, windowWidth / windowHeight, 0.01, -1000);
    gluLookAt(eyePos[0], eyePos[1], eyePos[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}