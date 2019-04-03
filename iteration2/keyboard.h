void initPerspectiveAndCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, windowWidth / windowHeight, 0.01, 1000);
    gluLookAt(eyePos[0], eyePos[1], eyePos[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}