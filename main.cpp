#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

// Camera and movement variables
float angle = 0.0f, yAngle = 0.0f;
float lx = 0.0f, lz = -1.0f, ly = 0.0f;
float x = 0.0f, z = 5.0f, y = 1.0f;
float deltaMove = 0, deltaStrafe = 0, deltaFly = 0;
bool lightOn = true;
float lightPosition[] = {0.0f, 10.0f, 5.0f, 1.0f};
float radarAngle = 0.0f;
float timeOfDay = 0.0f;  // Day-night cycle

// Fog effect variables
bool fogEnabled = true;

// Tree positions
std::vector<std::pair<float, float>> treePositions;

// Function to set up lighting for day-night cycle
void setupLighting() {
    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    // Simulating day-night cycle by dimming the ambient light
    float brightness = (sin(timeOfDay) + 1) / 2.0f;  // Cycles between 0 and 1
    ambientLight[0] *= brightness;
    ambientLight[1] *= brightness;
    ambientLight[2] *= brightness;

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    if (lightOn) {
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }
}

// Function to set up fog
void setupFog() {
    if (fogEnabled) {
        glEnable(GL_FOG);
        GLfloat fogColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
        glFogfv(GL_FOG_COLOR, fogColor);
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 20.0f);
        glFogf(GL_FOG_END, 60.0f);
    } else {
        glDisable(GL_FOG);
    }
}

// Update camera position based on movement
void updateCameraPosition() {
    x += deltaMove * lx * 0.1f;
    z += deltaMove * lz * 0.1f;
    x += deltaStrafe * cos(angle) * 0.1f;
    z += deltaStrafe * sin(angle) * 0.1f;
    y += deltaFly * 0.1f;
}

// Draw jet engines for airplanes
void drawJetEngines(float x, float y, float z) {
    glPushMatrix();
    glColor3f(0.2f, 0.2f, 0.2f);
    glTranslatef(x, y, z);
    glScalef(0.6f, 0.6f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Draw airplane landing gear
void drawLandingGear(float x, float y, float z) {
    glPushMatrix();
    glColor3f(0.2f, 0.2f, 0.2f);
    glTranslatef(x, y - 0.8f, z);
    glScalef(0.2f, 0.2f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Draw airplanes with jet engines, cockpit, and landing gear
void drawAirplane(float x, float y, float z, float scale = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    // Fuselage
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glScalef(2.0f, 0.6f, 6.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Cockpit (front part)
    glColor3f(0.2f, 0.2f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 2.8f);
    glScalef(1.0f, 1.0f, 1.5f);
    glutSolidCube(0.8f);
    glPopMatrix();

    // Wings
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.5f);
    glScalef(6.0f, 0.1f, 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Tail fin
    glPushMatrix();
    glTranslatef(0.0f, 0.8f, -2.8f);
    glScalef(0.5f, 1.5f, 0.5f);
    glutSolidCube(0.5f);
    glPopMatrix();

    // Jet engines
    drawJetEngines(-1.2f, -0.1f, -1.5f);
    drawJetEngines(1.2f, -0.1f, -1.5f);

    // Landing gear
    drawLandingGear(-0.8f, 0.0f, 1.8f);
    drawLandingGear(0.8f, 0.0f, 1.8f);

    glPopMatrix();
}

// Draw road markings on the runway
void drawRoadMarkings() {
    glColor3f(1.0f, 1.0f, 1.0f); // White markings
    for (float i = -50.0f; i < 50.0f; i += 10.0f) {
        glBegin(GL_QUADS);
        glVertex3f(-1.0f, 0.01f, i);
        glVertex3f(1.0f, 0.01f, i);
        glVertex3f(1.0f, 0.01f, i + 5.0f);
        glVertex3f(-1.0f, 0.01f, i + 5.0f);
        glEnd();
    }
}

// Draw the runway with lights and road markings
void drawRunway(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray for the runway
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, -50.0f);
    glVertex3f(10.0f, 0.0f, -50.0f);
    glVertex3f(10.0f, 0.0f, 50.0f);
    glVertex3f(-10.0f, 0.0f, 50.0f);
    glEnd();

    // Draw runway lights
    glColor3f(0.8f, 0.8f, 0.0f); // Yellow runway lights
    for (float i = -50.0f; i < 50.0f; i += 5.0f) {
        glPushMatrix();
        glTranslatef(-9.5f, 0.05f, i);
        glutSolidSphere(0.1f, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(9.5f, 0.05f, i);
        glutSolidSphere(0.1f, 10, 10);
        glPopMatrix();
    }

    // Add road markings to the runway
    drawRoadMarkings();

    glPopMatrix();
}

// Draw a tree
void drawTree(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Tree trunk
    glColor3f(0.6f, 0.4f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(0.3f, 2.0f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Tree leaves
    glColor3f(0.0f, 0.5f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 2.5f, 0.0f);
    glutSolidSphere(1.5f, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

// Draw the ground surrounding the runway with grass and trees
void drawGround(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.0f, 0.8f, 0.0f); // Green for grass
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -0.1f, -100.0f);
    glVertex3f(100.0f, -0.1f, -100.0f);
    glVertex3f(100.0f, -0.1f, 100.0f);
    glVertex3f(-100.0f, -0.1f, 100.0f);
    glEnd();

    // Draw trees
    for (const auto& pos : treePositions) {
        drawTree(pos.first, 0.0f, pos.second);
    }

    glPopMatrix();
}

// Draw a simple taxiway leading off the runway
void drawTaxiway(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.3f, 0.3f, 0.3f); // Dark gray for taxiway
    glBegin(GL_QUADS);
    glVertex3f(10.0f, 0.0f, 30.0f);
    glVertex3f(20.0f, 0.0f, 30.0f);
    glVertex3f(20.0f, 0.0f, 50.0f);
    glVertex3f(10.0f, 0.0f, 50.0f);
    glEnd();

    glPopMatrix();
}

// Draw the control tower with a rotating radar
void drawControlTower(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.5f, 0.5f, 0.5f); // Gray for the tower
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    glScalef(2.0f, 10.0f, 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Control room on top
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 6.0f, 0.0f);
    glScalef(3.0f, 2.0f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Radar on top of control tower
    glPushMatrix();
    glTranslatef(0.0f, 7.0f, 0.0f);
    glRotatef(radarAngle, 0.0f, 1.0f, 0.0f);
    glColor3f(0.3f, 0.3f, 0.3f);
    glutSolidCone(0.5f, 1.0f, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

// Render the airport scene
void renderScene() {
    updateCameraPosition();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(x, y, z,
              x + lx, y + ly, z + lz,
              0.0f, 1.0f, 0.0f);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Sky blue

    // Enable lighting and draw the airport elements
    setupLighting();
    setupFog();

    drawGround(0.0f, 0.0f, 0.0f);
    drawRunway(0.0f, 0.0f, 0.0f);
    drawTaxiway(0.0f, 0.0f, 0.0f);
    drawControlTower(12.0f, 0.0f, -30.0f);
    
    // Airplanes on both sides
    drawAirplane(-7.0f, 0.5f, -20.0f);
    drawAirplane(7.0f, 0.5f, -20.0f);
    drawAirplane(0.0f, 0.5f, 0.0f, 2.0f); // Main airplane in the middle

    // Update radar rotation
    radarAngle += 0.5f;
    if (radarAngle > 360.0f) radarAngle = 0.0f;

    // Simulate day-night cycle
    timeOfDay += 0.0001f;
    if (timeOfDay > 2 * 3.14159f) timeOfDay = 0.0f;

    glutSwapBuffers();
}

// Handle keyboard input for movement and toggling light/fog
void processNormalKeys(unsigned char key, int xx, int yy) {
    float speed = 0.1f;

    switch (key) {
        case 'w':
            deltaMove = speed;
            break;
        case 's':
            deltaMove = -speed;
            break;
        case 'a':
            deltaStrafe = -speed;
            break;
        case 'd':
            deltaStrafe = speed;
            break;
        case ' ':
            deltaFly = speed;
            break;
        case 'x':
            deltaFly = -speed;
            break;
        case 't': // Toggle lights
            lightOn = !lightOn;
            break;
        case 'f': // Toggle fog
            fogEnabled = !fogEnabled;
            break;
        case 27: // Escape key
            exit(0);
            break;
    }
}

// Stop movement when key is released
void releaseNormalKeys(unsigned char key, int xx, int yy) {
    switch (key) {
        case 'w':
        case 's':
            deltaMove = 0;
            break;
        case 'a':
        case 'd':
            deltaStrafe = 0;
            break;
        case ' ':
        case 'x':
            deltaFly = 0;
            break;
    }
}

// Handle mouse movement for looking around
void mouseMove(int x, int y) {
    static bool warpPointer = false;
    if (!warpPointer) {
        int dx = x - glutGet(GLUT_WINDOW_WIDTH) / 2;
        int dy = y - glutGet(GLUT_WINDOW_HEIGHT) / 2;

        angle += dx * 0.001f;
        yAngle += dy * 0.001f;

        lx = sin(angle);
        lz = -cos(angle);
        ly = -sin(yAngle);

        warpPointer = true;
        glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
    } else {
        warpPointer = false;
    }
}

// Initialize tree positions
void initializeTrees() {
    srand(time(0)); // Seed for random number generation
    for (int i = 0; i < 50; ++i) {
        float treeX, treeZ;
        do {
            treeX = (rand() % 200) - 100; // Random x position between -100 and 100
            treeZ = (rand() % 200) - 100; // Random z position between -100 and 100
        } while (treeX > -10.0f && treeX < 10.0f && treeZ > -50.0f && treeZ < 50.0f); // Ensure trees are not on the runway
        treePositions.push_back({treeX, treeZ});
    }
}

// Main function
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Enhanced 3D Airport Scene with Day-Night Cycle");

    glEnable(GL_DEPTH_TEST);

    initializeTrees(); // Initialize tree positions

    glutDisplayFunc(renderScene);
    glutReshapeFunc([](int w, int h) {
        if (h == 0) h = 1;
        float ratio = w * 1.0f / h;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, w, h);
        gluPerspective(45.0f, ratio, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
    });
    glutIdleFunc(renderScene);
    glutKeyboardFunc(processNormalKeys);
    glutKeyboardUpFunc(releaseNormalKeys);
    glutPassiveMotionFunc(mouseMove);

    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();

    return 0;
}