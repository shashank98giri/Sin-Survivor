
map<char,bool> isPressed;
void handleSpecialFunc(int key,int x,int y){
    switch(key){
        
        case GLUT_KEY_UP :
            isPressed['w']=true;
            break;
        case GLUT_KEY_DOWN:
            isPressed['w']=true;
            break;
        case GLUT_KEY_LEFT:
            isPressed['w'] = true;
            break;
        case GLUT_KEY_RIGHT:
            isPressed['w'] = true;
            break;
    }
}
void handleKeyboardFunc(unsigned char key,int x,int y){

}




void initPerspectiveAndCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(isPressed['1'])eyePos[2]=min(eyePos[2]+0.1f,0.5f);
    if(isPressed['3'])eyePos[2]=max(eyePos[2]-0.1f,-0.7f);
    gluPerspective(45.0, windowWidth / windowHeight, 0.01, 1000);
    gluLookAt(eyePos[0], eyePos[1], eyePos[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}