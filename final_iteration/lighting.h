// function for lights
float fogcontr = 0, check = 0;
void initLights()
{
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    GLfloat intensity=1;
    //global ambient light
    GLfloat lmodel_ambient[] = {0.5f * intensity, 0.5f * intensity, 0.5f * intensity, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    //individual lights
   
    
    //individual lights
    glEnable(GL_LIGHT0);
    GLfloat light_ambient[] = {1.0f * intensity, 1.0f * intensity, 1.0f * intensity, 1.0f};
    GLfloat light_diffuse[] = {1.0f * intensity, 1.0f * intensity, 1.0f * intensity, 1.0f};
    GLfloat light_specular[] = {1.0f * intensity, 1.0f * intensity, 1.0f * intensity ,1.0f};
    GLfloat light_position[] = {light0_X, light0_Y, light0_Z, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);

    // glEnable(GL_LIGHT1);
    // GLfloat light_position2[] = {light0_X, light0_Y, light0_Z - rand()%20, 1.0};
    // glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    // glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    // glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    // glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    // glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 1.0);
   
}
// function for tunnel color
void initMaterialProperties()
{
    GLfloat mat_ambient[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glShadeModel(GL_SMOOTH);
}

void initFog(){
    // GLfloat fogColor[]={0,0,0,1};

    // glEnable(GL_FOG);
    // glFogi(GL_FOG_MODE,GL_LINEAR);
    // glFogfv(GL_FOG_COLOR, fogColor);
    // glFogf(GL_FOG_DENSITY, 0.35);
    // glHint(GL_FOG_HINT, GL_DONT_CARE);
    // glFogf(GL_FOG_START, 1.0);
    // glFogf(GL_FOG_END, 5.0);
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glPushMatrix();
    float density=0.5;
    glEnable(GL_FOG);
    {
        GLfloat fogColor[4] = {0.5, 0.5, 0.5, 0.5};
        /* if(framecnt%250<125)
        glFogf(GL_FOG_MODE,GL_EXP);
        else
        glFogf(GL_FOG_MODE, GL_LINEAR); */
        if (framecnt % 300 < 100)
            glFogf(GL_FOG_MODE, GL_EXP);
        else if(framecnt %300 <200)
            glFogf(GL_FOG_MODE, GL_LINEAR);
        else {
            glDisable(GL_FOG);
        }
        glFogfv(GL_FOG_COLOR, fogColor);
        //if(framecnt%10==0)density=(rand()%10)*0.1;
        glFogf(GL_FOG_DENSITY, 0.5);
       //glFogf(GL_FOG_COORD_SRC,*fogcoord);
        glHint(GL_FOG_HINT, GL_NICEST);
        glFogf(GL_FOG_START, 5);
        glFogf(GL_FOG_END,40-fogcontr);        
    }
    glPopMatrix();
    fogcontr=min(fogcontr+1.0f,30.f);


}
