#include "ecs/ecs.h"
#include "Game/game.h"

//---------------------------------------------------------------------------------------
void timer(int value)
{
    // Post re-paint request to activate display()
    glutPostRedisplay();

    // next timer call milliseconds later
    glutTimerFunc(15, timer, 0);
}

//---------------------------------------------------------------------------------------
//  MAIN
//---------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // Initialize xCore , param is window name
    xcore::Init("ECS");

    // Initialize the game
    InitializeGame();

    // Create the graphics and main loop
    glutInitWindowSize(s_AsteroidGame.m_Width, s_AsteroidGame.m_Height);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutCreateWindow(xcore::get().m_pAppName);
    glutDisplayFunc([]
        {
            s_AsteroidGame.m_GameMgr->Run();
        });
    glutReshapeFunc([](int w, int h)
        {
            s_AsteroidGame.m_Width = w;
            s_AsteroidGame.m_Height = h;
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, w, 0, h, -1, 1);
            glScalef(1, -1, 1);
            glTranslatef(0, -h, 0);
        });
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    xcore::Kill();
    return 0;
}