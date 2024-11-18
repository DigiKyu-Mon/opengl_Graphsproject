
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <cmath>
#include <shader.h>
#include <cube.h>
#include <cubeproject.h>
#include <keyframe.h>

//for text
#include <text.h>
// for texture backgroud
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// provide PlaySound function
#include <windows.h>
// To show score
#include <string>


using namespace std;

// Function Prototypes
GLFWwindow* glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void render();
void loadTexture();
void initKeyframes();
void updateAnimData();

// Global variables
GLFWwindow* mainWindow = NULL;
// background shader
Shader* backShader = NULL;

// cube shader
Shader* globalShader1 = NULL;
Shader* globalShader2 = NULL;
Shader* globalShader3 = NULL;
Shader* globalShader4 = NULL;

//button shader
Shader* buttonShader_q = NULL;
Shader* buttonShader_w = NULL;
Shader* buttonShader_e = NULL;
Shader* buttonShader_r = NULL;

// text shader
Shader* textShader1 = NULL;
Text* text1 = NULL;
Shader* textShader2 = NULL;
Text* text2 = NULL;
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
Cube* cube;
Cube* cube1;
Cube* cube2;
Cube* cube3;
Cube* cube4;
Button* cube_q;
Button* cube_w;
Button* cube_e;
Button* cube_r;
glm::mat4 projection, view, model;

// for lighting
bool light_q = false;
bool light_w = false;
bool light_e = false;
bool light_r = false;
glm::vec3 lightSize(0.1f, 0.1f, 0.1f);
glm::vec3 lightPos_nopress(0.0f, -1.0f, -10.0f);
glm::vec3 lightPos(0.0f, 1.0f, 4.0f);
glm::vec3 lightPos_back(0.0f, 0.0f, 0.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

// for object
glm::vec3 BackSize(76.0f, 40.0f, 2.0f);
glm::vec3 objectSize(2.0f, 1.0f, 2.0f);
glm::vec3 objectColor(1.0f, 1.0f, 1.0f);
glm::vec3 object_qColor(1.0f, 0.0f, 0.0f);
glm::vec3 object_wColor(1.0f, 1.0f, 0.0f);
glm::vec3 object_eColor(0.0f, 1.0f, 0.0f);
glm::vec3 object_rColor(0.0f, 0.0f, 1.0f);
glm::vec3 object_backPos(0.0f, 0.0f, -10.0f);
glm::vec3 object_qPos(-3.0f, -9.8f, 0.0f);
glm::vec3 object_wPos(-1.0f, -9.8f, 0.0f);
glm::vec3 object_ePos(1.0f, -9.8f, 0.0f);
glm::vec3 object_rPos(3.0f, -9.8f, 0.0f);
float ambientStrenth = 0.1f;
float specularStrength = 0.5f;
float specularPower = 1.0f;

// for score
string str;
string str2;
int score;
int miss;
bool hit;
bool good;
bool perfect;
bool missshow;

// for texture
static unsigned int texture; // Array of texture ids.
float textr;
float textg;
float textb;

// for camera
glm::vec3 cameraPos(0.0f, 0.0f, 40.0f);
glm::vec3 cameraAt(0.0f, 0.0f, 0.0f);

// for animation
enum RenderMode { INIT, ANIM, STOP };
RenderMode renderMode;                  // current rendering mode
float beginT;                           // animation beginning time (in sec)
float timeT;                            // current time (in sec)
float animEndTime = 150.0f;               // ending time of animation (in sec)
float x1Trans, y1Trans, z1Trans;           // current cube1 translation factors
float x1Angle, y1Angle, z1Angle;           // current cube1 rotation factors
float x2Trans, y2Trans, z2Trans;           // current cube2 translation factors
float x2Angle, y2Angle, z2Angle;           // current cube2 rotation factors
float x3Trans, y3Trans, z3Trans;           // current cube3 translation factors
float x3Angle, y3Angle, z3Angle;           // current cube3 rotation factors
float x4Trans, y4Trans, z4Trans;           // current cube3 translation factors
float x4Angle, y4Angle, z4Angle;           // current cube3 rotation factors
KeyFraming xTKF(2), xRKF(4), y1TKF(283), y2TKF(246), y3TKF(207), y4TKF(200), yRKF(4), zTKF(2), zRKF(4);   // cube4 rotation keyframes



int main()
{
    mainWindow = glAllInit();

    // text shader
    textShader1 = new Shader("text.vs", "text.frag");
    text1 = new Text((char*)"fonts/GAMERIA.ttf", textShader1, SCR_WIDTH, SCR_HEIGHT);
    textShader2 = new Shader("text.vs", "text.frag");
    text2 = new Text((char*)"fonts/GAMERIA.ttf", textShader1, SCR_WIDTH, SCR_HEIGHT);


    // shader loading and compile (by calling the constructor)
    backShader = new Shader("back_maps.vs", "back_maps.fs");
    globalShader1 = new Shader("basic_lighting.vs", "basic_lighting.fs");
    globalShader2 = new Shader("basic_lighting.vs", "basic_lighting.fs");
    globalShader3 = new Shader("basic_lighting.vs", "basic_lighting.fs");
    globalShader4 = new Shader("basic_lighting.vs", "basic_lighting.fs");
    buttonShader_q = new Shader("botton_lighting.vs", "botton_lighting.fs");
    buttonShader_w = new Shader("botton_lighting.vs", "botton_lighting.fs");
    buttonShader_e = new Shader("botton_lighting.vs", "botton_lighting.fs");
    buttonShader_r = new Shader("botton_lighting.vs", "botton_lighting.fs");

    // projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // lighting parameters to fragment shader
    globalShader1->use();
    globalShader1->setMat4("projection", projection);
    globalShader1->setVec3("objectColor", object_wColor);
    globalShader1->setVec3("lightColor", lightColor);
    globalShader1->setVec3("lightPos", lightPos);
    globalShader1->setFloat("ambientStrenth", ambientStrenth);
    globalShader2->use();
    globalShader2->setMat4("projection", projection);
    globalShader2->setVec3("objectColor", object_eColor);
    globalShader2->setVec3("lightColor", lightColor);
    globalShader2->setVec3("lightPos", lightPos);
    globalShader2->setFloat("ambientStrenth", ambientStrenth);
    globalShader3->use();
    globalShader3->setMat4("projection", projection);
    globalShader3->setVec3("objectColor", object_rColor);
    globalShader3->setVec3("lightColor", lightColor);
    globalShader3->setVec3("lightPos", lightPos);
    globalShader3->setFloat("ambientStrenth", ambientStrenth);
    globalShader4->use();
    globalShader4->setMat4("projection", projection);
    globalShader4->setVec3("objectColor", object_qColor);
    globalShader4->setVec3("lightColor", lightColor);
    globalShader4->setVec3("lightPos", lightPos);
    globalShader4->setFloat("ambientStrenth", ambientStrenth);

    // back cube
    backShader->use();
    backShader->setMat4("projection", projection);
    // load texture
    loadTexture();

    // back cube initialization
    cube = new Cube();

    // cube initialization
    cube_q = new Button();
    cube_w = new Button();
    cube_e = new Button();
    cube_r = new Button();

    // cube1 initialization
    cube1 = new Cube();
    // cube2 initialization
    cube2 = new Cube();
    // cube3 initialization
    cube3 = new Cube();
    // cube2 initialization
    cube4 = new Cube();

    // initialize animation data
    initKeyframes();
    timeT = 0.0f;
    int score = 0;
    int miss = 0;
    hit = false;
    good = false;
    perfect = false;
    missshow = false;

    updateAnimData();
    renderMode = INIT;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(mainWindow)) {

        // play sound 
        if (renderMode == ANIM) {
            PlaySound(TEXT("bgm.wav"), NULL, SND_ASYNC | SND_NOSTOP);
        }
        if (renderMode == STOP || renderMode == INIT) {
            PlaySound(TEXT("bgm.wav"), NULL, SND_ASYNC | SND_PURGE);
        }
        render();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void loadTexture() {

    // Create texture ids.
    glGenTextures(1, &texture);

    // All upcomming GL_TEXTURE_2D operations now on "texture" object
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters for wrapping.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);   // vertical flip the texture
    unsigned char* image = stbi_load("source.bmp", &width, &height, &nrChannels, 0);

    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

}

void initKeyframes() {

    // x-translation keyframes
    xTKF.setKey(0, 0, -3.0);
    xTKF.setKey(1, animEndTime, -3.0);

    // Cube1 y-translation keyframes
    y1TKF.setKey(0, 0, 20.0);
    y1TKF.setKey(1, 3.0705, 20.0);
    y1TKF.setKey(2, 4.0705, -10.0);
    y1TKF.setKey(3, 4.0705, 20.0);
    y1TKF.setKey(4, 6.6711, 20.0);
    y1TKF.setKey(5, 7.6711, -10.0);
    y1TKF.setKey(6, 7.6711, 20.0);
    y1TKF.setKey(7, 13.4722, 20.0);
    y1TKF.setKey(8, 14.4722, -10.0);
    y1TKF.setKey(9, 14.4722, 20.0);
    y1TKF.setKey(10, 16.1429, 20.0);
    y1TKF.setKey(11, 17.1429, -10.0);
    y1TKF.setKey(12, 17.1429, 20.0);
    y1TKF.setKey(13, 18.7831, 20.0);
    y1TKF.setKey(14, 19.7831, -10.0);
    y1TKF.setKey(15, 19.7831, 20.0);
    y1TKF.setKey(16, 20.0134, 20.0);
    y1TKF.setKey(17, 21.0134, -10.0);
    y1TKF.setKey(18, 21.0134, 20.0);
    y1TKF.setKey(19, 22.6836, 20.0);
    y1TKF.setKey(20, 23.6836, -10.0);
    y1TKF.setKey(21, 23.6836, 20.0);
    y1TKF.setKey(22, 25.094, 20.0);
    y1TKF.setKey(23, 26.094, -10.0);
    y1TKF.setKey(24, 26.094, 20.0);
    y1TKF.setKey(25, 26.7444, 20.0);
    y1TKF.setKey(26, 27.7444, -10.0);
    y1TKF.setKey(27, 27.7444, 20.0);
    y1TKF.setKey(28, 27.9349, 20.0);
    y1TKF.setKey(29, 28.9349, -10.0);
    y1TKF.setKey(30, 28.9349, 20.0);
    y1TKF.setKey(31, 29.1647, 20.0);
    y1TKF.setKey(32, 30.1647, -10.0);
    y1TKF.setKey(33, 30.1647, 20.0);
    y1TKF.setKey(34, 30.5652, 20.0);
    y1TKF.setKey(35, 31.5652, -10.0);
    y1TKF.setKey(36, 31.5652, 20.0);
    y1TKF.setKey(37, 31.8753, 20.0);
    y1TKF.setKey(38, 32.8753, -10.0);
    y1TKF.setKey(39, 32.8753, 20.0);
    y1TKF.setKey(40, 33.2356, 20.0);
    y1TKF.setKey(41, 34.2356, -10.0);
    y1TKF.setKey(42, 34.2356, 20.0);
    y1TKF.setKey(43, 36.0564, 20.0);
    y1TKF.setKey(44, 37.0564, -10.0);
    y1TKF.setKey(45, 37.0564, 20.0);
    y1TKF.setKey(46, 37.1661, 20.0);
    y1TKF.setKey(47, 38.1661, -10.0);
    y1TKF.setKey(48, 38.1661, 20.0);
    y1TKF.setKey(49, 38.7465, 20.0);
    y1TKF.setKey(50, 39.7465, -10.0);
    y1TKF.setKey(51, 39.7465, 20.0);
    y1TKF.setKey(52, 40.9073, 20.0);
    y1TKF.setKey(53, 41.9073, -10.0);
    y1TKF.setKey(54, 41.9073, 20.0);
    y1TKF.setKey(55, 41.9373, 20.0);
    y1TKF.setKey(56, 42.9373, -10.0);
    y1TKF.setKey(57, 42.9373, 20.0);
    y1TKF.setKey(58, 43.1774, 20.0);
    y1TKF.setKey(59, 44.1774, -10.0);
    y1TKF.setKey(60, 44.1774, 20.0);
    y1TKF.setKey(61, 44.9302, 20.0);
    y1TKF.setKey(62, 45.9302, -10.0);
    y1TKF.setKey(63, 45.9302, 20.0);
    y1TKF.setKey(64, 46.4979, 20.0);
    y1TKF.setKey(65, 47.4979, -10.0);
    y1TKF.setKey(66, 47.4979, 20.0);
    y1TKF.setKey(67, 47.6478, 20.0);
    y1TKF.setKey(68, 48.6478, -10.0);
    y1TKF.setKey(69, 48.6478, 20.0);
    y1TKF.setKey(70, 50.0383, 20.0);
    y1TKF.setKey(71, 51.0383, -10.0);
    y1TKF.setKey(72, 51.0383, 20.0);
    y1TKF.setKey(73, 51.2484, 20.0);
    y1TKF.setKey(74, 52.2484, -10.0);
    y1TKF.setKey(75, 52.2484, 20.0);
    y1TKF.setKey(76, 52.4192, 20.0);
    y1TKF.setKey(77, 53.4192, -10.0);
    y1TKF.setKey(78, 53.4192, 20.0);
    y1TKF.setKey(79, 54.1295, 20.0);
    y1TKF.setKey(80, 55.1295, -10.0);
    y1TKF.setKey(81, 55.1295, 20.0);
    y1TKF.setKey(82, 55.4591, 20.0);
    y1TKF.setKey(83, 56.4591, -10.0);
    y1TKF.setKey(84, 56.4591, 20.0);
    y1TKF.setKey(85, 56.6495, 20.0);
    y1TKF.setKey(86, 57.6495, -10.0);
    y1TKF.setKey(87, 57.6495, 20.0);
    y1TKF.setKey(88, 57.3095, 20.0);
    y1TKF.setKey(89, 58.3095, -10.0);
    y1TKF.setKey(90, 58.3095, 20.0);
    y1TKF.setKey(91, 58.4898, 20.0);
    y1TKF.setKey(92, 59.4898, -10.0);
    y1TKF.setKey(93, 59.4898, 20.0);
    y1TKF.setKey(94, 59.6598, 20.0);
    y1TKF.setKey(95, 60.6598, -10.0);
    y1TKF.setKey(96, 60.6598, 20.0);
    y1TKF.setKey(97, 60.8502, 20.0);
    y1TKF.setKey(98, 61.8502, -10.0);
    y1TKF.setKey(99, 61.8502, 20.0);
    y1TKF.setKey(100, 62.0204, 20.0);
    y1TKF.setKey(101, 63.0204, -10.0);
    y1TKF.setKey(102, 63.0204, 20.0);
    y1TKF.setKey(103, 63.2204, 20.0);
    y1TKF.setKey(104, 64.2204, -10.0);
    y1TKF.setKey(105, 64.2204, 20.0);
    y1TKF.setKey(106, 64.4707, 20.0);
    y1TKF.setKey(107, 65.4707, -10.0);
    y1TKF.setKey(108, 65.4707, 20.0);
    y1TKF.setKey(109, 66.8811, 20.0);
    y1TKF.setKey(110, 67.8811, -10.0);
    y1TKF.setKey(111, 67.8811, 20.0);
    y1TKF.setKey(112, 68.6914, 20.0);
    y1TKF.setKey(113, 69.6914, -10.0);
    y1TKF.setKey(114, 69.6914, 20.0);
    y1TKF.setKey(115, 70.5816, 20.0);
    y1TKF.setKey(116, 71.5816, -10.0);
    y1TKF.setKey(117, 71.5816, 20.0);
    y1TKF.setKey(118, 71.682, 20.0);
    y1TKF.setKey(119, 72.682, -10.0);
    y1TKF.setKey(120, 72.682, 20.0);
    y1TKF.setKey(121, 73.3925, 20.0);
    y1TKF.setKey(122, 74.3925, -10.0);
    y1TKF.setKey(123, 74.3925, 20.0);
    y1TKF.setKey(124, 75.2426, 20.0);
    y1TKF.setKey(125, 76.2426, -10.0);
    y1TKF.setKey(126, 76.2426, 20.0);
    y1TKF.setKey(127, 77.6329, 20.0);
    y1TKF.setKey(128, 78.6329, -10.0);
    y1TKF.setKey(129, 78.6329, 20.0);
    y1TKF.setKey(130, 78.8031, 20.0);
    y1TKF.setKey(131, 79.8031, -10.0);
    y1TKF.setKey(132, 79.8031, 20.0);
    y1TKF.setKey(133, 80.0736, 20.0);
    y1TKF.setKey(134, 81.0736, -10.0);
    y1TKF.setKey(135, 81.0736, 20.0);
    y1TKF.setKey(136, 82.4739, 20.0);
    y1TKF.setKey(137, 83.4739, -10.0);
    y1TKF.setKey(138, 83.4739, 20.0);
    y1TKF.setKey(139, 83.6238, 20.0);
    y1TKF.setKey(140, 84.6238, -10.0);
    y1TKF.setKey(141, 84.6238, 20.0);
    y1TKF.setKey(142, 84.5643, 20.0);
    y1TKF.setKey(143, 85.5643, -10.0);
    y1TKF.setKey(144, 85.5643, 20.0);
    y1TKF.setKey(145, 87.2445, 20.0);
    y1TKF.setKey(146, 88.2445, -10.0);
    y1TKF.setKey(147, 88.2445, 20.0);
    y1TKF.setKey(148, 89.095, 20.0);
    y1TKF.setKey(149, 90.095, -10.0);
    y1TKF.setKey(150, 90.095, 20.0);
    y1TKF.setKey(151, 90.8451, 20.0);
    y1TKF.setKey(152, 91.8451, -10.0);
    y1TKF.setKey(153, 91.8451, 20.0);
    y1TKF.setKey(154, 92.6155, 20.0);
    y1TKF.setKey(155, 93.6155, -10.0);
    y1TKF.setKey(156, 93.6155, 20.0);
    y1TKF.setKey(157, 94.207, 20.0);
    y1TKF.setKey(158, 95.207, -10.0);
    y1TKF.setKey(159, 95.207, 20.0);
    y1TKF.setKey(160, 95.336, 20.0);
    y1TKF.setKey(161, 96.336, -10.0);
    y1TKF.setKey(162, 96.336, 20.0);
    y1TKF.setKey(163, 96.517, 20.0);
    y1TKF.setKey(164, 97.517, -10.0);
    y1TKF.setKey(165, 97.517, 20.0);
    y1TKF.setKey(166, 98.026, 20.0);
    y1TKF.setKey(167, 99.026, -10.0);
    y1TKF.setKey(168, 99.026, 20.0);
    y1TKF.setKey(169, 99.277, 20.0);
    y1TKF.setKey(170, 100.277, -10.0);
    y1TKF.setKey(171, 100.277, 20.0);
    y1TKF.setKey(172, 100.758, 20.0);
    y1TKF.setKey(173, 101.758, -10.0);
    y1TKF.setKey(174, 101.758, 20.0);
    y1TKF.setKey(175, 102.307, 20.0);
    y1TKF.setKey(176, 103.307, -10.0);
    y1TKF.setKey(177, 103.307, 20.0);
    y1TKF.setKey(178, 102.907, 20.0);
    y1TKF.setKey(179, 103.907, -10.0);
    y1TKF.setKey(180, 103.907, 20.0);
    y1TKF.setKey(181, 104.748, 20.0);
    y1TKF.setKey(182, 105.748, -10.0);
    y1TKF.setKey(183, 105.748, 20.0);
    y1TKF.setKey(184, 105.308, 20.0);
    y1TKF.setKey(185, 106.308, -10.0);
    y1TKF.setKey(186, 106.308, 20.0);
    y1TKF.setKey(187, 106.508, 20.0);
    y1TKF.setKey(188, 107.508, -10.0);
    y1TKF.setKey(189, 107.508, 20.0);
    y1TKF.setKey(190, 109.148, 20.0);
    y1TKF.setKey(191, 110.148, -10.0);
    y1TKF.setKey(192, 110.148, 20.0);
    y1TKF.setKey(193, 110.008, 20.0);
    y1TKF.setKey(194, 111.008, -10.0);
    y1TKF.setKey(195, 111.008, 20.0);
    y1TKF.setKey(196, 110.959, 20.0);
    y1TKF.setKey(197, 111.959, -10.0);
    y1TKF.setKey(198, 111.959, 20.0);
    y1TKF.setKey(199, 111.58, 20.0);
    y1TKF.setKey(200, 112.58, -10.0);
    y1TKF.setKey(201, 112.58, 20.0);
    y1TKF.setKey(202, 112.339, 20.0);
    y1TKF.setKey(203, 113.339, -10.0);
    y1TKF.setKey(204, 113.339, 20.0);
    y1TKF.setKey(205, 114.059, 20.0);
    y1TKF.setKey(206, 115.059, -10.0);
    y1TKF.setKey(207, 115.059, 20.0);
    y1TKF.setKey(208, 114.849, 20.0);
    y1TKF.setKey(209, 115.849, -10.0);
    y1TKF.setKey(210, 115.849, 20.0);
    y1TKF.setKey(211, 116.84, 20.0);
    y1TKF.setKey(212, 117.84, -10.0);
    y1TKF.setKey(213, 117.84, 20.0);
    y1TKF.setKey(214, 117.61, 20.0);
    y1TKF.setKey(215, 118.61, -10.0);
    y1TKF.setKey(216, 118.61, 20.0);
    y1TKF.setKey(217, 118.56, 20.0);
    y1TKF.setKey(218, 119.56, -10.0);
    y1TKF.setKey(219, 119.56, 20.0);
    y1TKF.setKey(220, 119.5, 20.0);
    y1TKF.setKey(221, 120.5, -10.0);
    y1TKF.setKey(222, 120.5, 20.0);
    y1TKF.setKey(223, 120.32, 20.0);
    y1TKF.setKey(224, 121.32, -10.0);
    y1TKF.setKey(225, 121.32, 20.0);
    y1TKF.setKey(226, 121.1, 20.0);
    y1TKF.setKey(227, 122.1, -10.0);
    y1TKF.setKey(228, 122.1, 20.0);
    y1TKF.setKey(229, 122.491, 20.0);
    y1TKF.setKey(230, 122.891, -10.0);
    y1TKF.setKey(231, 122.891, 20.0);
    y1TKF.setKey(232, 123.891, 20.0);
    y1TKF.setKey(233, 124.891, -10.0);
    y1TKF.setKey(234, 124.891, 20.0);
    y1TKF.setKey(235, 125.261, 20.0);
    y1TKF.setKey(236, 125.999, -10.0);
    y1TKF.setKey(237, 126.000, 20.0);
    y1TKF.setKey(238, 126.831, 20.0);
    y1TKF.setKey(239, 127.831, -10.0);
    y1TKF.setKey(240, 127.831, 20.0);
    y1TKF.setKey(241, 128.541, 20.0);
    y1TKF.setKey(242, 129.541, -10.0);
    y1TKF.setKey(243, 129.541, 20.0);
    y1TKF.setKey(244, 125.601, 20.0);
    y1TKF.setKey(245, 126.601, -10.0);
    y1TKF.setKey(246, 126.601, 20.0);
    y1TKF.setKey(247, 127.141, 20.0);
    y1TKF.setKey(248, 127.941, -10.0);
    y1TKF.setKey(249, 127.941, 20.0);
    y1TKF.setKey(250, 128.371, 20.0);
    y1TKF.setKey(251, 128.971, -10.0);
    y1TKF.setKey(252, 128.971, 20.0);
    y1TKF.setKey(253, 129.711, 20.0);
    y1TKF.setKey(254, 130.711, -10.0);
    y1TKF.setKey(255, 130.711, 20.0);
    y1TKF.setKey(256, 131.612, 20.0);
    y1TKF.setKey(257, 132.612, -10.0);
    y1TKF.setKey(258, 132.612, 20.0);
    y1TKF.setKey(259, 133.112, 20.0);
    y1TKF.setKey(260, 134.112, -10.0);
    y1TKF.setKey(261, 134.112, 20.0);
    y1TKF.setKey(262, 134.982, 20.0);
    y1TKF.setKey(263, 135.582, -10.0);
    y1TKF.setKey(264, 135.582, 20.0);
    y1TKF.setKey(265, 135.983, 20.0);
    y1TKF.setKey(266, 136.983, -10.0);
    y1TKF.setKey(267, 136.983, 20.0);
    y1TKF.setKey(268, 138.873, 20.0);
    y1TKF.setKey(269, 139.873, -10.0);
    y1TKF.setKey(270, 139.873, 20.0);
    y1TKF.setKey(271, 142.114, 20.0);
    y1TKF.setKey(272, 143.114, -10.0);
    y1TKF.setKey(273, 143.114, 20.0);
    y1TKF.setKey(274, 144.804, 20.0);
    y1TKF.setKey(275, 145.804, -10.0);
    y1TKF.setKey(276, 145.804, 20.0);
    y1TKF.setKey(277, 145.844, 20.0);
    y1TKF.setKey(278, 146.844, -10.0);
    y1TKF.setKey(279, 146.844, 20.0);
    y1TKF.setKey(280, 149.985, 20.0);
    y1TKF.setKey(281, 150.00, -10.0);
    y1TKF.setKey(282, animEndTime, 20.0);


    // Cube2 y-translation keyframes
    y2TKF.setKey(0, 0, 20.0);
    y2TKF.setKey(1, 9.2815, 20.0);
    y2TKF.setKey(2, 10.2815, -10.0);
    y2TKF.setKey(3, 10.2815, 20.0);
    y2TKF.setKey(4, 14.9224, 20.0);
    y2TKF.setKey(5, 15.9224, -10.0);
    y2TKF.setKey(6, 15.9224, 20.0);
    y2TKF.setKey(7, 17.4029, 20.0);
    y2TKF.setKey(8, 18.4029, -10.0);
    y2TKF.setKey(9, 18.4029, 20.0);
    y2TKF.setKey(10, 21.4434, 20.0);
    y2TKF.setKey(11, 22.4434, -10.0);
    y2TKF.setKey(12, 22.4434, 20.0);
    y2TKF.setKey(13, 23.3739, 20.0);
    y2TKF.setKey(14, 24.3739, -10.0);
    y2TKF.setKey(15, 24.3739, 20.0);
    y2TKF.setKey(16, 29.1647, 20.0);
    y2TKF.setKey(17, 30.1647, -10.0);
    y2TKF.setKey(18, 30.1647, 20.0);
    y2TKF.setKey(19, 33.8856, 20.0);
    y2TKF.setKey(20, 34.8856, -10.0);
    y2TKF.setKey(21, 34.8856, 20.0);
    y2TKF.setKey(22, 36.448, 20.0);
    y2TKF.setKey(23, 37.448, -10.0);
    y2TKF.setKey(24, 37.448, 20.0);
    y2TKF.setKey(25, 38.1463, 20.0);
    y2TKF.setKey(26, 39.1463, -10.0);
    y2TKF.setKey(27, 39.1463, 20.0);
    y2TKF.setKey(28, 39.5564, 20.0);
    y2TKF.setKey(29, 40.5564, -10.0);
    y2TKF.setKey(30, 40.5564, 20.0);
    y2TKF.setKey(31, 41.2377, 20.0);
    y2TKF.setKey(32, 42.2377, -10.0);
    y2TKF.setKey(33, 42.2377, 20.0);
    y2TKF.setKey(34, 42.5271, 20.0);
    y2TKF.setKey(35, 43.5271, -10.0);
    y2TKF.setKey(36, 43.5271, 20.0);
    y2TKF.setKey(37, 44.2074, 20.0);
    y2TKF.setKey(38, 45.2074, -10.0);
    y2TKF.setKey(39, 45.2074, 20.0);
    y2TKF.setKey(40, 46.0077, 20.0);
    y2TKF.setKey(41, 47.0077, -10.0);
    y2TKF.setKey(42, 47.0077, 20.0);
    y2TKF.setKey(43, 48.8181, 20.0);
    y2TKF.setKey(44, 49.8181, -10.0);
    y2TKF.setKey(45, 49.8181, 20.0);
    y2TKF.setKey(46, 52.4389, 20.0);
    y2TKF.setKey(47, 53.4389, -10.0);
    y2TKF.setKey(48, 53.4389, 20.0);
    y2TKF.setKey(49, 53.5687, 20.0);
    y2TKF.setKey(50, 54.5687, -10.0);
    y2TKF.setKey(51, 54.5687, 20.0);
    y2TKF.setKey(52, 54.459, 20.0);
    y2TKF.setKey(53, 55.459, -10.0);
    y2TKF.setKey(54, 55.459, 20.0);
    y2TKF.setKey(55, 55.7292, 20.0);
    y2TKF.setKey(56, 56.7292, -10.0);
    y2TKF.setKey(57, 56.7292, 20.0);
    y2TKF.setKey(58, 56.9398, 20.0);
    y2TKF.setKey(59, 57.9398, -10.0);
    y2TKF.setKey(60, 57.9398, 20.0);
    y2TKF.setKey(61, 58.4898, 20.0);
    y2TKF.setKey(62, 59.4898, -10.0);
    y2TKF.setKey(63, 59.4898, 20.0);
    y2TKF.setKey(64, 62.6405, 20.0);
    y2TKF.setKey(65, 63.6405, -10.0);
    y2TKF.setKey(66, 63.6405, 20.0);
    y2TKF.setKey(67, 64.7408, 20.0);
    y2TKF.setKey(68, 65.7408, -10.0);
    y2TKF.setKey(69, 65.7408, 20.0);
    y2TKF.setKey(70, 67.8412, 20.0);
    y2TKF.setKey(71, 68.8412, -10.0);
    y2TKF.setKey(72, 68.8412, 20.0);
    y2TKF.setKey(73, 69.2816, 20.0);
    y2TKF.setKey(74, 70.2816, -10.0);
    y2TKF.setKey(75, 70.2816, 20.0);
    y2TKF.setKey(76, 71.0923, 20.0);
    y2TKF.setKey(77, 72.0923, -10.0);
    y2TKF.setKey(78, 72.0923, 20.0);
    y2TKF.setKey(79, 72.482, 20.0);
    y2TKF.setKey(80, 73.482, -10.0);
    y2TKF.setKey(81, 73.482, 20.0);
    y2TKF.setKey(82, 73.7023, 20.0);
    y2TKF.setKey(83, 74.7023, -10.0);
    y2TKF.setKey(84, 74.7023, 20.0);
    y2TKF.setKey(85, 75.8826, 20.0);
    y2TKF.setKey(86, 76.8826, -10.0);
    y2TKF.setKey(87, 76.8826, 20.0);
    y2TKF.setKey(88, 77.8932, 20.0);
    y2TKF.setKey(89, 78.8932, -10.0);
    y2TKF.setKey(90, 78.8932, 20.0);
    y2TKF.setKey(91, 79.113, 20.0);
    y2TKF.setKey(92, 80.113, -10.0);
    y2TKF.setKey(93, 80.113, 20.0);
    y2TKF.setKey(94, 80.3235, 20.0);
    y2TKF.setKey(95, 81.3235, -10.0);
    y2TKF.setKey(96, 81.3235, 20.0);
    y2TKF.setKey(97, 81.5436, 20.0);
    y2TKF.setKey(98, 82.0436, -10.0);
    y2TKF.setKey(99, 82.0436, 20.0);
    y2TKF.setKey(100, 82.5535, 20.0);
    y2TKF.setKey(101, 83.5535, -10.0);
    y2TKF.setKey(102, 83.5535, 20.0);
    y2TKF.setKey(103, 84.0342, 20.0);
    y2TKF.setKey(104, 85.0342, -10.0);
    y2TKF.setKey(105, 85.0342, 20.0);
    y2TKF.setKey(106, 85.8541, 20.0);
    y2TKF.setKey(107, 86.2541, -10.0);
    y2TKF.setKey(108, 86.2541, 20.0);
    y2TKF.setKey(109, 86.4146, 20.0);
    y2TKF.setKey(110, 87.0146, -10.0);
    y2TKF.setKey(111, 87.0146, 20.0);
    y2TKF.setKey(112, 87.8645, 20.0);
    y2TKF.setKey(113, 88.8645, -10.0);
    y2TKF.setKey(114, 88.8645, 20.0);
    y2TKF.setKey(115, 89.095, 20.0);
    y2TKF.setKey(116, 90.095, -10.0);
    y2TKF.setKey(117, 90.095, 20.0);
    y2TKF.setKey(118, 90.2449, 20.0);
    y2TKF.setKey(119, 91.2449, -10.0);
    y2TKF.setKey(120, 91.2449, 20.0);
    y2TKF.setKey(121, 91.4659, 20.0);
    y2TKF.setKey(122, 92.4659, -10.0);
    y2TKF.setKey(123, 92.4659, 20.0);
    y2TKF.setKey(124, 92.6652, 20.0);
    y2TKF.setKey(125, 93.1652, -10.0);
    y2TKF.setKey(126, 93.1652, 20.0);
    y2TKF.setKey(127, 93.195, 20.0);
    y2TKF.setKey(128, 94.195, -10.0);
    y2TKF.setKey(129, 94.195, 20.0);
    y2TKF.setKey(130, 94.496, 20.0);
    y2TKF.setKey(131, 95.496, -10.0);
    y2TKF.setKey(132, 95.496, 20.0);
    y2TKF.setKey(133, 95.596, 20.0);
    y2TKF.setKey(134, 96.596, -10.0);
    y2TKF.setKey(135, 96.596, 20.0);
    y2TKF.setKey(136, 96.827, 20.0);
    y2TKF.setKey(137, 97.827, -10.0);
    y2TKF.setKey(138, 97.827, 20.0);
    y2TKF.setKey(139, 98.636, 20.0);
    y2TKF.setKey(140, 99.636, -10.0);
    y2TKF.setKey(141, 99.636, 20.0);
    y2TKF.setKey(142, 99.857, 20.0);
    y2TKF.setKey(143, 100.857, -10.0);
    y2TKF.setKey(144, 100.857, 20.0);
    y2TKF.setKey(145, 101.058, 20.0);
    y2TKF.setKey(146, 102.058, -10.0);
    y2TKF.setKey(147, 102.058, 20.0);
    y2TKF.setKey(148, 103.577, 20.0);
    y2TKF.setKey(149, 104.577, -10.0);
    y2TKF.setKey(150, 104.577, 20.0);
    y2TKF.setKey(151, 105.907, 20.0);
    y2TKF.setKey(152, 106.907, -10.0);
    y2TKF.setKey(153, 106.907, 20.0);
    y2TKF.setKey(154, 106.638, 20.0);
    y2TKF.setKey(155, 107.638, -10.0);
    y2TKF.setKey(156, 107.638, 20.0);
    y2TKF.setKey(157, 107.738, 20.0);
    y2TKF.setKey(158, 108.338, -10.0);
    y2TKF.setKey(159, 108.338, 20.0);
    y2TKF.setKey(160, 108.698, 20.0);
    y2TKF.setKey(161, 109.698, -10.0);
    y2TKF.setKey(162, 109.698, 20.0);
    y2TKF.setKey(163, 110.398, 20.0);
    y2TKF.setKey(164, 110.998, -10.0);
    y2TKF.setKey(165, 110.998, 20.0);
    y2TKF.setKey(166, 111.308, 20.0);
    y2TKF.setKey(167, 112.109, -10.0);
    y2TKF.setKey(168, 112.109, 20.0);
    y2TKF.setKey(169, 112.751, 20.0);
    y2TKF.setKey(170, 113.751, -10.0);
    y2TKF.setKey(171, 113.751, 20.0);
    y2TKF.setKey(172, 114.369, 20.0);
    y2TKF.setKey(173, 115.369, -10.0);
    y2TKF.setKey(174, 115.369, 20.0);
    y2TKF.setKey(175, 115.771, 20.0);
    y2TKF.setKey(176, 116.771, -10.0);
    y2TKF.setKey(177, 116.771, 20.0);
    y2TKF.setKey(178, 117.739, 20.0);
    y2TKF.setKey(179, 118.739, -10.0);
    y2TKF.setKey(180, 118.739, 20.0);
    y2TKF.setKey(181, 119.66, 20.0);
    y2TKF.setKey(182, 120.66, -10.0);
    y2TKF.setKey(183, 120.66, 20.0);
    y2TKF.setKey(184, 120.781, 20.0);
    y2TKF.setKey(184, 121.781, -10.0);
    y2TKF.setKey(185, 121.781, 20.0);
    y2TKF.setKey(186, 121.84, 20.0);
    y2TKF.setKey(187, 122.84, -10.0);
    y2TKF.setKey(188, 122.84, 20.0);
    y2TKF.setKey(199, 122.98, 20.0);
    y2TKF.setKey(200, 123.571, -10.0);
    y2TKF.setKey(201, 123.571, 20.0);
    y2TKF.setKey(202, 124.171, 20.0);
    y2TKF.setKey(203, 125.171, -10.0);
    y2TKF.setKey(204, 125.171, 20.0);
    y2TKF.setKey(205, 126.221, 20.0);
    y2TKF.setKey(206, 127.221, -10.0);
    y2TKF.setKey(207, 127.221, 20.0);
    y2TKF.setKey(208, 128.021, 20.0);
    y2TKF.setKey(209, 129.021, -10.0);
    y2TKF.setKey(210, 129.021, 20.0);
    y2TKF.setKey(211, 129.222, 20.0);
    y2TKF.setKey(212, 130.222, -10.0);
    y2TKF.setKey(213, 130.222, 20.0);
    y2TKF.setKey(214, 130.412, 20.0);
    y2TKF.setKey(215, 131.412, -10.0);
    y2TKF.setKey(216, 131.412, 20.0);
    y2TKF.setKey(217, 132.803, 20.0);
    y2TKF.setKey(218, 133.803, -10.0);
    y2TKF.setKey(219, 133.803, 20.0);
    y2TKF.setKey(220, 136.433, 20.0);
    y2TKF.setKey(221, 137.433, -10.0);
    y2TKF.setKey(222, 137.433, 20.0);
    y2TKF.setKey(223, 137.773, 20.0);
    y2TKF.setKey(224, 138.773, -10.0);
    y2TKF.setKey(225, 138.773, 20.0);
    y2TKF.setKey(226, 139.763, 20.0);
    y2TKF.setKey(227, 140.763, -10.0);
    y2TKF.setKey(228, 140.763, 20.0);
    y2TKF.setKey(229, 141.254, 20.0);
    y2TKF.setKey(230, 142.254, -10.0);
    y2TKF.setKey(231, 142.254, 20.0);
    y2TKF.setKey(232, 142.385, 20.0);
    y2TKF.setKey(233, 143.385, -10.0);
    y2TKF.setKey(234, 143.385, 20.0);
    y2TKF.setKey(235, 143.604, 20.0);
    y2TKF.setKey(236, 144.604, -10.0);
    y2TKF.setKey(237, 144.604, 20.0);
    y2TKF.setKey(238, 145.204, 20.0);
    y2TKF.setKey(239, 146.204, -10.0);
    y2TKF.setKey(240, 146.204, 20.0);
    y2TKF.setKey(241, 147.495, 20.0);
    y2TKF.setKey(242, 148.495, -10.0);
    y2TKF.setKey(243, 148.495, 20.0);
    y2TKF.setKey(245, animEndTime, 20.0);


    // Cube3 y-translation keyframes
    y3TKF.setKey(0, 0, 20.0);
    y3TKF.setKey(1, 4.1205, 20.0);
    y3TKF.setKey(2, 5.1205, -10.0);
    y3TKF.setKey(3, 5.1205, 20.0);
    y3TKF.setKey(4, 8.1314, 20.0);
    y3TKF.setKey(5, 9.1314, -10.0);
    y3TKF.setKey(6, 9.1314, 20.0);
    y3TKF.setKey(7, 16.1429, 20.0);
    y3TKF.setKey(8, 17.1429, -10.0);
    y3TKF.setKey(9, 17.1429, 20.0);
    y3TKF.setKey(10, 20.0134, 20.0);
    y3TKF.setKey(11, 21.0134, -10.0);
    y3TKF.setKey(12, 21.0134, 20.0);
    y3TKF.setKey(13, 27.9349, 20.0);
    y3TKF.setKey(14, 28.9349, -10.0);
    y3TKF.setKey(15, 28.9349, 20.0);
    y3TKF.setKey(16, 30.5652, 20.0);
    y3TKF.setKey(17, 31.5652, -10.0);
    y3TKF.setKey(18, 31.5652, 20.0);
    y3TKF.setKey(19, 34.3858, 20.0);
    y3TKF.setKey(20, 35.3858, -10.0);
    y3TKF.setKey(21, 35.3858, 20.0);
    y3TKF.setKey(22, 36.7661, 20.0);
    y3TKF.setKey(23, 37.7661, -10.0);
    y3TKF.setKey(24, 37.7661, 20.0);
    y3TKF.setKey(25, 38.3263, 20.0);
    y3TKF.setKey(26, 39.3263, -10.0);
    y3TKF.setKey(27, 39.3263, 20.0);
    y3TKF.setKey(28, 40.1167, 20.0);
    y3TKF.setKey(29, 41.1167, -10.0);
    y3TKF.setKey(30, 41.1167, 20.0);
    y3TKF.setKey(31, 41.6068, 20.0);
    y3TKF.setKey(32, 42.6068, -10.0);
    y3TKF.setKey(33, 42.6068, 20.0);
    y3TKF.setKey(34, 42.227, 20.0);
    y3TKF.setKey(35, 43.227, -10.0);
    y3TKF.setKey(36, 43.227, 20.0);
    y3TKF.setKey(37, 43.4972, 20.0);
    y3TKF.setKey(38, 44.4972, -10.0);
    y3TKF.setKey(39, 44.4972, 20.0);
    y3TKF.setKey(40, 47.6679, 20.0);
    y3TKF.setKey(41, 48.6679, -10.0);
    y3TKF.setKey(42, 48.6679, 20.0);
    y3TKF.setKey(43, 51.2484, 20.0);
    y3TKF.setKey(44, 52.2484, -10.0);
    y3TKF.setKey(45, 52.2484, 20.0);
    y3TKF.setKey(46, 53.5687, 20.0);
    y3TKF.setKey(47, 54.5687, -10.0);
    y3TKF.setKey(48, 54.5687, 20.0);
    y3TKF.setKey(49, 54.7692, 20.0);
    y3TKF.setKey(50, 55.7692, -10.0);
    y3TKF.setKey(51, 55.7692, 20.0);
    y3TKF.setKey(52, 57.3095, 20.0);
    y3TKF.setKey(53, 58.3095, -10.0);
    y3TKF.setKey(54, 58.3095, 20.0);
    y3TKF.setKey(55, 61.4301, 20.0);
    y3TKF.setKey(56, 62.4301, -10.0);
    y3TKF.setKey(57, 62.4301, 20.0);
    y3TKF.setKey(58, 63.8606, 20.0);
    y3TKF.setKey(59, 64.8606, -10.0);
    y3TKF.setKey(60, 64.8606, 20.0);
    y3TKF.setKey(61, 65.621, 20.0);
    y3TKF.setKey(62, 66.621, -10.0);
    y3TKF.setKey(63, 66.621, 20.0);
    y3TKF.setKey(64, 68.1315, 20.0);
    y3TKF.setKey(65, 69.1315, -10.0);
    y3TKF.setKey(66, 69.1315, 20.0);
    y3TKF.setKey(67, 70.2024, 20.0);
    y3TKF.setKey(68, 71.2024, -10.0);
    y3TKF.setKey(69, 71.2024, 20.0);
    y3TKF.setKey(70, 72.8321, 20.0);
    y3TKF.setKey(71, 73.8321, -10.0);
    y3TKF.setKey(72, 73.8321, 20.0);
    y3TKF.setKey(73, 76.573, 20.0);
    y3TKF.setKey(74, 77.573, -10.0);
    y3TKF.setKey(75, 77.573, 20.0);
    y3TKF.setKey(76, 79.4739, 20.0);
    y3TKF.setKey(77, 80.4739, -10.0);
    y3TKF.setKey(78, 80.4739, 20.0);
    y3TKF.setKey(79, 80.6434, 20.0);
    y3TKF.setKey(80, 81.6434, -10.0);
    y3TKF.setKey(81, 81.6434, 20.0);
    y3TKF.setKey(82, 81.8438, 20.0);
    y3TKF.setKey(83, 82.8438, -10.0);
    y3TKF.setKey(84, 82.8438, 20.0);
    y3TKF.setKey(85, 84.2841, 20.0);
    y3TKF.setKey(86, 85.2841, -10.0);
    y3TKF.setKey(87, 85.2841, 20.0);
    y3TKF.setKey(88, 85.4445, 20.0);
    y3TKF.setKey(89, 86.4445, -10.0);
    y3TKF.setKey(90, 86.4445, 20.0);
    y3TKF.setKey(91, 86.6943, 20.0);
    y3TKF.setKey(92, 87.6943, -10.0);
    y3TKF.setKey(93, 87.6943, 20.0);
    y3TKF.setKey(94, 88.4847, 20.0);
    y3TKF.setKey(95, 89.4847, -10.0);
    y3TKF.setKey(96, 89.4847, 20.0);
    y3TKF.setKey(97, 89.6748, 20.0);
    y3TKF.setKey(98, 90.6748, -10.0);
    y3TKF.setKey(99, 90.6748, 20.0);
    y3TKF.setKey(100, 90.8575, 20.0);
    y3TKF.setKey(101, 91.8575, -10.0);
    y3TKF.setKey(102, 91.8575, 20.0);
    y3TKF.setKey(103, 92.6554, 20.0);
    y3TKF.setKey(104, 93.6554, -10.0);
    y3TKF.setKey(105, 93.6554, 20.0);
    y3TKF.setKey(106, 95.086, 20.0);
    y3TKF.setKey(107, 96.086, -10.0);
    y3TKF.setKey(108, 96.086, 20.0);
    y3TKF.setKey(109, 96.256, 20.0);
    y3TKF.setKey(110, 97.256, -10.0);
    y3TKF.setKey(111, 97.256, 20.0);
    y3TKF.setKey(112, 97.436, 20.0);
    y3TKF.setKey(113, 98.436, -10.0);
    y3TKF.setKey(114, 98.436, 20.0);
    y3TKF.setKey(115, 98.046, 20.0);
    y3TKF.setKey(116, 99.046, -10.0);
    y3TKF.setKey(117, 99.046, 20.0);
    y3TKF.setKey(118, 99.307, 20.0);
    y3TKF.setKey(119, 100.307, -10.0);
    y3TKF.setKey(120, 100.307, 20.0);
    y3TKF.setKey(121, 100.447, 20.0);
    y3TKF.setKey(122, 101.447, -10.0);
    y3TKF.setKey(123, 101.447, 20.0);
    y3TKF.setKey(124, 101.388, 20.0);
    y3TKF.setKey(125, 102.388, -10.0);
    y3TKF.setKey(126, 102.388, 20.0);
    y3TKF.setKey(127, 102.928, 20.0);
    y3TKF.setKey(128, 103.928, -10.0);
    y3TKF.setKey(129, 103.928, 20.0);
    y3TKF.setKey(130, 106.238, 20.0);
    y3TKF.setKey(131, 107.238, -10.0);
    y3TKF.setKey(132, 107.238, 20.0);
    y3TKF.setKey(133, 108.04, 20.0);
    y3TKF.setKey(134, 109.04, -10.0);
    y3TKF.setKey(135, 109.04, 20.0);
    y3TKF.setKey(136, 109.798, 20.0);
    y3TKF.setKey(137, 110.798, -10.0);
    y3TKF.setKey(138, 110.798, 20.0);
    y3TKF.setKey(139, 110.428, 20.0);
    y3TKF.setKey(140, 111.428, -10.0);
    y3TKF.setKey(141, 111.428, 20.0);
    y3TKF.setKey(142, 111.249, 20.0);
    y3TKF.setKey(143, 112.249, -10.0);
    y3TKF.setKey(144, 112.249, 20.0);
    y3TKF.setKey(145, 111.759, 20.0);
    y3TKF.setKey(146, 112.759, -10.0);
    y3TKF.setKey(147, 112.759, 20.0);
    y3TKF.setKey(148, 112.859, 20.0);
    y3TKF.setKey(149, 113.859, -10.0);
    y3TKF.setKey(150, 113.859, 20.0);
    y3TKF.setKey(151, 114.239, 20.0);
    y3TKF.setKey(152, 115.239, -10.0);
    y3TKF.setKey(153, 115.239, 20.0);
    y3TKF.setKey(154, 115.919, 20.0);
    y3TKF.setKey(155, 116.919, -10.0);
    y3TKF.setKey(156, 116.919, 20.0);
    y3TKF.setKey(157, 117.15, 20.0);
    y3TKF.setKey(158, 118.15, -10.0);
    y3TKF.setKey(159, 118.15, 20.0);
    y3TKF.setKey(160, 118.38, 20.0);
    y3TKF.setKey(161, 119.38, -10.0);
    y3TKF.setKey(162, 119.38, 20.0);
    y3TKF.setKey(163, 119.99, 20.0);
    y3TKF.setKey(164, 120.99, -10.0);
    y3TKF.setKey(165, 120.99, 20.0);
    y3TKF.setKey(167, 121.26, 20.0);
    y3TKF.setKey(168, 122.26, -10.0);
    y3TKF.setKey(169, 122.26, 20.0);
    y3TKF.setKey(170, 122.651, 20.0);
    y3TKF.setKey(171, 123.651, -10.0);
    y3TKF.setKey(172, 123.651, 20.0);
    y3TKF.setKey(173, 124.361, 20.0);
    y3TKF.setKey(174, 125.361, -10.0);
    y3TKF.setKey(175, 125.361, 20.0);
    y3TKF.setKey(176, 126.841, 20.0);
    y3TKF.setKey(177, 127.841, -10.0);
    y3TKF.setKey(178, 127.841, 20.0);
    y3TKF.setKey(179, 127.441, 20.0);
    y3TKF.setKey(180, 128.441, -10.0);
    y3TKF.setKey(181, 128.441, 20.0);
    y3TKF.setKey(182, 128.622, 20.0);
    y3TKF.setKey(183, 129.622, -10.0);
    y3TKF.setKey(184, 129.622, 20.0);
    y3TKF.setKey(185, 132.272, 20.0);
    y3TKF.setKey(186, 133.272, -10.0);
    y3TKF.setKey(187, 133.272, 20.0);
    y3TKF.setKey(188, 137.393, 20.0);
    y3TKF.setKey(189, 138.393, -10.0);
    y3TKF.setKey(190, 138.393, 20.0);
    y3TKF.setKey(191, 139.953, 20.0);
    y3TKF.setKey(192, 140.953, -10.0);
    y3TKF.setKey(193, 140.953, 20.0);
    y3TKF.setKey(194, 141.565, 20.0);
    y3TKF.setKey(195, 142.565, -10.0);
    y3TKF.setKey(196, 142.565, 20.0);
    y3TKF.setKey(197, 142.674, 20.0);
    y3TKF.setKey(198, 143.674, -10.0);
    y3TKF.setKey(199, 143.674, 20.0);
    y3TKF.setKey(200, 145.524, 20.0);
    y3TKF.setKey(201, 146.524, -10.0);
    y3TKF.setKey(202, 146.524, 20.0);
    y3TKF.setKey(203, 146.756, 20.0);
    y3TKF.setKey(204, 149.000, -10.0);
    y3TKF.setKey(205, 149.000, 20.0);
    y3TKF.setKey(206, animEndTime, 20.0);


    // Cube4 y-translation keyframes
    y4TKF.setKey(0, 0, 20.0);
    y4TKF.setKey(1, 5.4111, 20.0);
    y4TKF.setKey(2, 6.4111, -10.0);
    y4TKF.setKey(3, 6.4111, 20.0);
    y4TKF.setKey(4, 10.8016, 20.0);
    y4TKF.setKey(5, 11.8016, -10.0);
    y4TKF.setKey(6, 11.8016, 20.0);
    y4TKF.setKey(7, 12.0921, 20.0);
    y4TKF.setKey(8, 13.0921, -10.0);
    y4TKF.setKey(9, 13.0921, 20.0);
    y4TKF.setKey(10, 14.9927, 20.0);
    y4TKF.setKey(11, 15.9927, -10.0);
    y4TKF.setKey(12, 15.9927, 20.0);
    y4TKF.setKey(13, 17.4329, 20.0);
    y4TKF.setKey(14, 18.4329, -10.0);
    y4TKF.setKey(15, 18.4329, 20.0);
    y4TKF.setKey(16, 18.7831, 20.0);
    y4TKF.setKey(17, 19.7831, -10.0);
    y4TKF.setKey(18, 19.7831, 20.0);
    y4TKF.setKey(19, 21.4434, 20.0);
    y4TKF.setKey(20, 22.4434, -10.0);
    y4TKF.setKey(21, 22.4434, 20.0);
    y4TKF.setKey(22, 22.704, 20.0);
    y4TKF.setKey(23, 23.704, -10.0);
    y4TKF.setKey(24, 23.704, 20.0);
    y4TKF.setKey(25, 26.7444, 20.0);
    y4TKF.setKey(26, 27.7444, -10.0);
    y4TKF.setKey(27, 27.7444, 20.0);
    y4TKF.setKey(28, 29.2148, 20.0);
    y4TKF.setKey(29, 30.2148, -10.0);
    y4TKF.setKey(30, 30.2148, 20.0);
    y4TKF.setKey(31, 30.5652, 20.0);
    y4TKF.setKey(32, 31.5652, -10.0);
    y4TKF.setKey(33, 31.5652, 20.0);
    y4TKF.setKey(34, 31.8753, 20.0);
    y4TKF.setKey(35, 32.8753, -10.0);
    y4TKF.setKey(36, 32.8753, 20.0);
    y4TKF.setKey(37, 34.7659, 20.0);
    y4TKF.setKey(38, 35.7659, -10.0);
    y4TKF.setKey(39, 35.7659, 20.0);
    y4TKF.setKey(40, 38.4664, 20.0);
    y4TKF.setKey(41, 39.4664, -10.0);
    y4TKF.setKey(42, 39.4664, 20.0);
    y4TKF.setKey(43, 40.5567, 20.0);
    y4TKF.setKey(44, 41.5567, -10.0);
    y4TKF.setKey(45, 41.5567, 20.0);
    y4TKF.setKey(46, 42.897, 20.0);
    y4TKF.setKey(47, 43.897, -10.0);
    y4TKF.setKey(48, 43.897, 20.0);
    y4TKF.setKey(49, 44.4875, 20.0);
    y4TKF.setKey(50, 45.4875, -10.0);
    y4TKF.setKey(51, 45.4875, 20.0);
    y4TKF.setKey(52, 46.4979, 20.0);
    y4TKF.setKey(53, 47.4979, -10.0);
    y4TKF.setKey(54, 47.4979, 20.0);
    y4TKF.setKey(55, 48.8488, 20.0);
    y4TKF.setKey(56, 49.8488, -10.0);
    y4TKF.setKey(57, 49.8488, 20.0);
    y4TKF.setKey(58, 50.0383, 20.0);
    y4TKF.setKey(59, 51.0383, -10.0);
    y4TKF.setKey(60, 51.0383, 20.0);
    y4TKF.setKey(61, 51.2484, 20.0);
    y4TKF.setKey(62, 52.2484, -10.0);
    y4TKF.setKey(63, 52.2484, 20.0);
    y4TKF.setKey(64, 52.4389, 20.0);
    y4TKF.setKey(65, 53.4389, -10.0);
    y4TKF.setKey(66, 53.4389, 20.0);
    y4TKF.setKey(67, 53.5687, 20.0);
    y4TKF.setKey(68, 54.5687, -10.0);
    y4TKF.setKey(69, 54.5687, 20.0);
    y4TKF.setKey(70, 56.0493, 20.0);
    y4TKF.setKey(71, 57.0493, -10.0);
    y4TKF.setKey(72, 57.0493, 20.0);
    y4TKF.setKey(73, 57.3095, 20.0);
    y4TKF.setKey(74, 58.3095, -10.0);
    y4TKF.setKey(75, 58.3095, 20.0);
    y4TKF.setKey(76, 59.1403, 20.0);
    y4TKF.setKey(77, 60.1403, -10.0);
    y4TKF.setKey(78, 60.1403, 20.0);
    y4TKF.setKey(79, 60.8703, 20.0);
    y4TKF.setKey(80, 61.8703, -10.0);
    y4TKF.setKey(81, 61.8703, 20.0);
    y4TKF.setKey(82, 62.0504, 20.0);
    y4TKF.setKey(83, 63.0504, -10.0);
    y4TKF.setKey(84, 63.0504, 20.0);
    y4TKF.setKey(85, 63.2705, 20.0);
    y4TKF.setKey(86, 64.2705, -10.0);
    y4TKF.setKey(87, 64.2705, 20.0);
    y4TKF.setKey(88, 66.0517, 20.0);
    y4TKF.setKey(89, 67.0517, -10.0);
    y4TKF.setKey(90, 67.0517, 20.0);
    y4TKF.setKey(91, 70.4118, 20.0);
    y4TKF.setKey(92, 71.4118, -10.0);
    y4TKF.setKey(93, 71.4118, 20.0);
    y4TKF.setKey(94, 73.1522, 20.0);
    y4TKF.setKey(95, 74.1522, -10.0);
    y4TKF.setKey(96, 74.1522, 20.0);
    y4TKF.setKey(97, 78.2932, 20.0);
    y4TKF.setKey(98, 79.2932, -10.0);
    y4TKF.setKey(99, 79.2932, 20.0);
    y4TKF.setKey(100, 79.7643, 20.0);
    y4TKF.setKey(101, 80.7643, -10.0);
    y4TKF.setKey(102, 80.7643, 20.0);
    y4TKF.setKey(103, 81.3236, 20.0);
    y4TKF.setKey(104, 82.3236, -10.0);
    y4TKF.setKey(105, 82.3236, 20.0);
    y4TKF.setKey(106, 85.7342, 20.0);
    y4TKF.setKey(107, 86.7342, -10.0);
    y4TKF.setKey(108, 86.7342, 20.0);
    y4TKF.setKey(109, 86.9645, 20.0);
    y4TKF.setKey(110, 87.9645, -10.0);
    y4TKF.setKey(111, 87.9645, 20.0);
    y4TKF.setKey(112, 88.4847, 20.0);
    y4TKF.setKey(113, 89.4847, -10.0);
    y4TKF.setKey(114, 89.4847, 20.0);
    y4TKF.setKey(115, 89.6748, 20.0);
    y4TKF.setKey(116, 90.6748, -10.0);
    y4TKF.setKey(117, 90.6748, 20.0);
    y4TKF.setKey(118, 92.106, 20.0);
    y4TKF.setKey(119, 93.106, -10.0);
    y4TKF.setKey(120, 93.106, 20.0);
    y4TKF.setKey(121, 93.956, 20.0);
    y4TKF.setKey(122, 94.956, -10.0);
    y4TKF.setKey(123, 94.956, 20.0);
    y4TKF.setKey(124, 98.666, 20.0);
    y4TKF.setKey(125, 99.666, -10.0);
    y4TKF.setKey(126, 99.666, 20.0);
    y4TKF.setKey(127, 100.187, 20.0);
    y4TKF.setKey(128, 101.187, -10.0);
    y4TKF.setKey(129, 101.187, 20.0);
    y4TKF.setKey(130, 101.677, 20.0);
    y4TKF.setKey(131, 102.677, -10.0);
    y4TKF.setKey(132, 102.677, 20.0);
    y4TKF.setKey(133, 104.157, 20.0);
    y4TKF.setKey(134, 105.157, -10.0);
    y4TKF.setKey(135, 105.157, 20.0);
    y4TKF.setKey(136, 105.328, 20.0);
    y4TKF.setKey(137, 106.328, -10.0);
    y4TKF.setKey(138, 106.328, 20.0);
    y4TKF.setKey(139, 106.988, 20.0);
    y4TKF.setKey(140, 107.988, -10.0);
    y4TKF.setKey(141, 107.988, 20.0);
    y4TKF.setKey(142, 108.178, 20.0);
    y4TKF.setKey(143, 109.178, -10.0);
    y4TKF.setKey(144, 109.178, 20.0);
    y4TKF.setKey(145, 109.598, 20.0);
    y4TKF.setKey(146, 110.598, -10.0);
    y4TKF.setKey(147, 110.598, 20.0);
    y4TKF.setKey(148, 112.079, 20.0);
    y4TKF.setKey(149, 113.079, -10.0);
    y4TKF.setKey(150, 113.079, 20.0);
    y4TKF.setKey(151, 113.699, 20.0);
    y4TKF.setKey(152, 114.699, -10.0);
    y4TKF.setKey(153, 114.699, 20.0);
    y4TKF.setKey(154, 115.449, 20.0);
    y4TKF.setKey(155, 116.449, -10.0);
    y4TKF.setKey(156, 116.449, 20.0);
    y4TKF.setKey(157, 116.67, 20.0);
    y4TKF.setKey(158, 117.67, -10.0);
    y4TKF.setKey(159, 117.67, 20.0);
    y4TKF.setKey(160, 119.33, 20.0);
    y4TKF.setKey(161, 120.33, -10.0);
    y4TKF.setKey(162, 120.33, 20.0);
    y4TKF.setKey(163, 120.94, 20.0);
    y4TKF.setKey(164, 121.94, -10.0);
    y4TKF.setKey(165, 121.94, 20.0);
    y4TKF.setKey(166, 123.421, 20.0);
    y4TKF.setKey(167, 124.421, -10.0);
    y4TKF.setKey(168, 124.421, 20.0);
    y4TKF.setKey(169, 124.012, 20.0);
    y4TKF.setKey(170, 125.012, -10.0);
    y4TKF.setKey(171, 125.012, 20.0);
    y4TKF.setKey(172, 129.853, 20.0);
    y4TKF.setKey(173, 130.853, -10.0);
    y4TKF.setKey(174, 130.853, 20.0);
    y4TKF.setKey(175, 131.022, 20.0);
    y4TKF.setKey(176, 132.022, -10.0);
    y4TKF.setKey(177, 132.022, 20.0);
    y4TKF.setKey(178, 133.422, 20.0);
    y4TKF.setKey(179, 134.422, -10.0);
    y4TKF.setKey(180, 134.422, 20.0);
    y4TKF.setKey(181, 135.182, 20.0);
    y4TKF.setKey(182, 136.182, -10.0);
    y4TKF.setKey(183, 136.182, 20.0);
    y4TKF.setKey(184, 137.603, 20.0);
    y4TKF.setKey(185, 138.603, -10.0);
    y4TKF.setKey(186, 138.603, 20.0);
    y4TKF.setKey(187, 140.104, 20.0);
    y4TKF.setKey(188, 141.104, -10.0);
    y4TKF.setKey(189, 141.104, 20.0);
    y4TKF.setKey(190, 141.823, 20.0);
    y4TKF.setKey(191, 142.823, -10.0);
    y4TKF.setKey(192, 142.823, 20.0);
    y4TKF.setKey(193, 142.994, 20.0);
    y4TKF.setKey(194, 143.994, -10.0);
    y4TKF.setKey(195, 143.994, 20.0);
    y4TKF.setKey(196, 147.805, 20.0);
    y4TKF.setKey(197, 149.00, -10.0);
    y4TKF.setKey(198, 149.00, 20.0);
    y4TKF.setKey(199, animEndTime, 20.0);

    // z-translation keyframes
    zTKF.setKey(0, 0, 0.0);
    zTKF.setKey(1, animEndTime, 0.0);

    // x-rotation keyframes
    xRKF.setKey(0, 0, 0.0);
    xRKF.setKey(1, 1.5, 0.0);
    xRKF.setKey(2, 3.0, 0.0);
    xRKF.setKey(3, animEndTime, 0.0);

    // y-rotation keyframes
    yRKF.setKey(0, 0, 0.0);
    yRKF.setKey(1, 1.5, 0.0);
    yRKF.setKey(2, 3.0, 0.0);
    yRKF.setKey(3, animEndTime, 0.0);

    // z-rotation keyframes
    zRKF.setKey(0, 0, 0.0);
    zRKF.setKey(1, 1.5, 0.0);
    zRKF.setKey(2, 3.0, 0.0);
    zRKF.setKey(3, animEndTime, 0.0);
}

void updateAnimData() {
    if (timeT > animEndTime) {
        renderMode = STOP;
        timeT = animEndTime;
    }
    // cube1 transfer by timeT
    x1Trans = xTKF.getValLinear(timeT);
    y1Trans = y1TKF.getValLinear(timeT);
    z1Trans = zTKF.getValLinear(timeT);
    x1Angle = xRKF.getValLinear(timeT);
    y1Angle = yRKF.getValLinear(timeT);
    z1Angle = zRKF.getValLinear(timeT);
    // cube2 transfer by timeT
    x2Trans = xTKF.getValLinear(timeT);
    y2Trans = y2TKF.getValLinear(timeT);
    z2Trans = zTKF.getValLinear(timeT);
    x2Angle = xRKF.getValLinear(timeT);
    y2Angle = yRKF.getValLinear(timeT);
    z2Angle = zRKF.getValLinear(timeT);
    // cube3 transfer by timeT
    x3Trans = xTKF.getValLinear(timeT);
    y3Trans = y3TKF.getValLinear(timeT);
    z3Trans = zTKF.getValLinear(timeT);
    x3Angle = xRKF.getValLinear(timeT);
    y3Angle = yRKF.getValLinear(timeT);
    z3Angle = zRKF.getValLinear(timeT);
    // cube4 transfer by timeT
    x4Trans = xTKF.getValLinear(timeT);
    y4Trans = y4TKF.getValLinear(timeT);
    z4Trans = zTKF.getValLinear(timeT);
    x4Angle = xRKF.getValLinear(timeT);
    y4Angle = yRKF.getValLinear(timeT);
    z4Angle = zRKF.getValLinear(timeT);
}

void render() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Drawing texts
    if (renderMode == INIT) {
        text1->RenderText("Press ENTER to play game", 25.0f, 570.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
        text1->RenderText("Red button is Q", 200.0f, 450.0f, 0.7f, glm::vec3(1.0f, 0.0f, 0.5f));
        text1->RenderText("YELLOW button is R", 200.0f, 400.0f, 0.7f, glm::vec3(0.7f, 1.0f, 0.0f));
        text1->RenderText("GREEN button is U", 200.0f, 350.0f, 0.7f, glm::vec3(0.2f, 1.0f, 0.0f));
        text1->RenderText("BLUE button is P", 200.0f, 300.0f, 0.7f, glm::vec3(0.0f, 0.2f, 1.0f));
        text1->RenderText("After start, press enter want to stop", 200.0f, 250.0f, 0.7f, glm::vec3(1.0f, 0.7f, 0.0f));
        text1->RenderText("Yonsei University", 1500.0f, 25.0f, 0.7f, glm::vec3(0.3f, 0.7f, 0.9f));
    }

    // set carmera
    view = glm::lookAt(cameraPos, cameraAt, glm::vec3(0.0f, 10.0f, 0.0f));

    if (renderMode == ANIM) {
        float cTime = (float)glfwGetTime(); // current time
        timeT = cTime - beginT;
        updateAnimData();
        text1->RenderText("SCORE", 300.0f, 800.0f, 2.0f, glm::vec3(1.0, 0.3f, 0.7f));
        str = to_string(score);
        text1->RenderText(str, 450.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));

        // q score
        if (light_q == true) {
            if (y1Trans < -7.0f && y1Trans >= -8.0f && (good == false && perfect == false)) {
                updateAnimData();
                hit = true;
                score = score + 1;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                good = false;
                perfect = false;
                missshow = false;
            }
            else if (y1Trans < -8.0f && y1Trans >= -9.0f && (hit == false && perfect == false)) {
                updateAnimData();
                good = true;
                score = score + 3;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                hit = false;
                perfect = false;
                missshow = false;

            }
            else if (y1Trans < -9.0f && y1Trans > -9.8f && (hit == false && good == false)) {
                updateAnimData();
                score = score + 5;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                perfect = true;
                hit = false;
                good = false;
                missshow = false;
            }
        }
        if (light_q == false && y1Trans <= -9.8f && !(hit ==true || good == true || perfect == true)) {
            miss = miss + 1;
            score = score - 2;
            str = to_string(score);
            text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
            missshow = true;
            hit = false;
            good = false;
            perfect = false;
        }
        // w score
        if (light_w == true) {
            if (y2Trans < -7.0f && y2Trans >= -8.0f && (good == false && perfect == false)) {
                updateAnimData();
                score = score + 1;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                hit = true;
                good = false;
                perfect = false;
                missshow = false;
            }
            else if (y2Trans < -8.0f && y2Trans >= -9.0f && (hit == false && perfect == false)) {
                updateAnimData();
                score = score + 3;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                good = true;
                hit = false;
                perfect = false;
                missshow = false;
            }
            else if (y2Trans < -9.0f && y2Trans > -9.8f && (hit == false && good == false)) {
                updateAnimData();
                score = score + 5;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                perfect = true;
                hit = false;
                good = false;
                missshow = false;
            }
        }
        if (light_w == false && y2Trans <= -9.8f && !(hit == true || good == true || perfect == true)) {
            miss = miss + 1;
            score = score - 2;
            str = to_string(score);
            text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
            missshow = true;
            hit = false;
            good = false;
            perfect = false;
        }

        // e score
        if (light_e == true) {
            if (y3Trans < -7.0f && y3Trans >= -8.0f && (good == false && perfect == false)) {
                updateAnimData();
                score = score + 1;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                hit = true;
                good = false;
                perfect = false;
                missshow = false;
            }
            else if (y3Trans < -8.0f && y3Trans >= -9.0f && (hit == false && perfect == false)) {
                updateAnimData();
                score = score + 3;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                good = true;
                hit = false;
                perfect = false;
                missshow = false;
            }
            else if (y3Trans < -9.0f && y3Trans > -9.8f && (hit == false && good == false)) {
                updateAnimData();
                score = score + 5;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                perfect = true;
                hit = false;
                good = false;
                missshow = false;
            }
        }
        if (light_e == false && y3Trans <= -9.8f && !(hit == true || good == true || perfect == true)) {
            miss = miss + 1;
            score = score - 2;
            str = to_string(score);
            text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
            missshow = true;
            hit = false;
            good = false;
            perfect = false;
        }

        // r score
        if (light_r == true) {
            if (y4Trans < -7.0f && y4Trans >= -8.0f && (good == false && perfect == false)) {
                
                score = score + 1;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                hit = true;
                good = false;
                perfect = false;
                missshow = false;
            }
            else if (y4Trans < -8.0f && y4Trans >= -9.0f && (hit == false && perfect == false)) {
                score = score + 3;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                good = true;
                hit = false;
                perfect = false;
                missshow = false;
            }
            else if (y4Trans < -9.0f && y4Trans > -9.9f && (hit == false && good == false)) {
                
                score = score + 5;
                str = to_string(score);
                text1->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
                perfect = true;
                hit = false;
                good = false;
                missshow = false;
            }
        }
        if (light_r == false && y4Trans <= -9.8f && (hit == true || good == true || perfect == true)) {
            miss = miss + 1;
            score = score - 2;
            str = to_string(score);
            text2->RenderText(str, 300.0f, 600.0f, 2.0f, glm::vec3(0.7, 0.3f, 1.0f));
            missshow = true;
            hit = false;
            good = false;
            perfect = false;
        }
    }
    if (hit) {
        str2 = "HIT";
        hit = false;
    }
    if (good) {
        str2 = "GOOD";
        good = false;
    }
    if (perfect) {
        str2 = "PERFECT";
        perfect = false;
    }
    if (missshow) {
        str2 = "MISS";
        missshow = false;
    }
    text2->RenderText(str2, 1100.0f, 800.0f, 3.0f, glm::vec3(1.0, 0.3f, 0.9f));

    //back cube light
    backShader->use();
    backShader->setMat4("view", view);
    backShader->setVec3("light.position", lightPos_back);
    backShader->setVec3("viewPos", cameraPos);
    backShader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    backShader->setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    backShader->setVec3("light.specular", 0.7f, 0.7f, 0.7f);

    // material properties
    backShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    backShader->setFloat("material.shininess", 1.0f);

    //back cube object
    model = glm::mat4(1.0f);
    model = glm::translate(model, object_backPos);
    model = glm::scale(model, BackSize);
    backShader->setMat4("model", model);
    // texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    cube->draw(backShader);

    // q button shader
    if (light_q == false) {
        buttonShader_q->use();
        buttonShader_q->setMat4("projection", projection);
        buttonShader_q->setVec3("objectColor", object_qColor);
        buttonShader_q->setVec3("lightColor", lightColor);
        buttonShader_q->setVec3("lightPos", lightPos_nopress);
        buttonShader_q->setVec3("viewPos", cameraPos);
        buttonShader_q->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_q->setFloat("specularStrength", specularStrength);
        buttonShader_q->setFloat("specularPower", specularPower);
    }
    else {
        buttonShader_q->use();
        buttonShader_q->setMat4("projection", projection);
        buttonShader_q->setVec3("objectColor", object_qColor);
        buttonShader_q->setVec3("lightColor", lightColor);
        buttonShader_q->setVec3("lightPos", lightPos);
        buttonShader_q->setVec3("viewPos", cameraPos);
        buttonShader_q->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_q->setFloat("specularStrength", specularStrength);
        buttonShader_q->setFloat("specularPower", specularPower);
    }

    // w button shader
    if (light_w == false) {
        buttonShader_w->use();
        buttonShader_w->setMat4("projection", projection);
        buttonShader_w->setVec3("objectColor", object_wColor);
        buttonShader_w->setVec3("lightColor", lightColor);
        buttonShader_w->setVec3("lightPos", lightPos_nopress);
        buttonShader_w->setVec3("viewPos", cameraPos);
        buttonShader_w->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_w->setFloat("specularStrength", specularStrength);
        buttonShader_w->setFloat("specularPower", specularPower);
    }
    else {
        buttonShader_w->use();
        buttonShader_w->setMat4("projection", projection);
        buttonShader_w->setVec3("objectColor", object_wColor);
        buttonShader_w->setVec3("lightColor", lightColor);
        buttonShader_w->setVec3("lightPos", lightPos);
        buttonShader_w->setVec3("viewPos", cameraPos);
        buttonShader_w->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_w->setFloat("specularStrength", specularStrength);
        buttonShader_w->setFloat("specularPower", specularPower);
    }
    // e button shader
    if (light_e == false) {
        buttonShader_e->use();
        buttonShader_e->setMat4("projection", projection);
        buttonShader_e->setVec3("objectColor", object_eColor);
        buttonShader_e->setVec3("lightColor", lightColor);
        buttonShader_e->setVec3("lightPos", lightPos_nopress);
        buttonShader_e->setVec3("viewPos", cameraPos);
        buttonShader_e->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_e->setFloat("specularStrength", specularStrength);
        buttonShader_e->setFloat("specularPower", specularPower);
    }
    else {
        buttonShader_e->use();
        buttonShader_e->setMat4("projection", projection);
        buttonShader_e->setVec3("objectColor", object_eColor);
        buttonShader_e->setVec3("lightColor", lightColor);
        buttonShader_e->setVec3("lightPos", lightPos);
        buttonShader_e->setVec3("viewPos", cameraPos);
        buttonShader_e->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_e->setFloat("specularStrength", specularStrength);
        buttonShader_e->setFloat("specularPower", specularPower);
    }

    // r button shader
    if (light_r == false) {
        buttonShader_r->use();
        buttonShader_r->setMat4("projection", projection);
        buttonShader_r->setVec3("objectColor", object_rColor);
        buttonShader_r->setVec3("lightColor", lightColor);
        buttonShader_r->setVec3("lightPos", lightPos_nopress);
        buttonShader_r->setVec3("viewPos", cameraPos);
        buttonShader_r->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_r->setFloat("specularStrength", specularStrength);
        buttonShader_r->setFloat("specularPower", specularPower);
    }
    else {
        buttonShader_r->use();
        buttonShader_r->setMat4("projection", projection);
        buttonShader_r->setVec3("objectColor", object_rColor);
        buttonShader_r->setVec3("lightColor", lightColor);
        buttonShader_r->setVec3("lightPos", lightPos);
        buttonShader_r->setVec3("viewPos", cameraPos);
        buttonShader_r->setFloat("ambientStrenth", ambientStrenth);
        buttonShader_r->setFloat("specularStrength", specularStrength);
        buttonShader_r->setFloat("specularPower", specularPower);
    }

    // cube_q object
    buttonShader_q->use();
    buttonShader_q->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, object_qPos);
    model = glm::scale(model, objectSize);
    buttonShader_q->setMat4("model", model);
    cube_q->draw(buttonShader_q);

    // cube_w object
    buttonShader_w->use();
    buttonShader_w->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, object_wPos);
    model = glm::scale(model, objectSize);
    buttonShader_w->setMat4("model", model);
    cube_w->draw(buttonShader_w);

    // cube_e object
    buttonShader_e->use();
    buttonShader_e->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, object_ePos);
    model = glm::scale(model, objectSize);
    buttonShader_e->setMat4("model", model);
    cube_e->draw(buttonShader_e);

    // cube_r object
    buttonShader_r->use();
    buttonShader_r->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, object_rPos);
    model = glm::scale(model, objectSize);
    buttonShader_r->setMat4("model", model);
    cube_r->draw(buttonShader_r);

    // view matrix to fragment shader
    globalShader1->use();
    globalShader1->setMat4("view", view);
    globalShader2->use();
    globalShader2->setMat4("view", view);
    globalShader3->use();
    globalShader3->setMat4("view", view);
    globalShader4->use();
    globalShader4->setMat4("view", view);


    // cube1 object
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x1Trans, y1Trans, z1Trans));
    glm::vec3 eulerAngles1(glm::radians(x1Angle), glm::radians(y1Angle), glm::radians(z1Angle));
    glm::quat q1(eulerAngles1);
    glm::mat4 rotMatrix1 = q1.operator glm::mat4x4();
    model = model * rotMatrix1;
    globalShader1->setMat4("model", model);
    cube1->draw(globalShader1);
    // cube2 object
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x2Trans + 2, y2Trans, z2Trans));
    glm::vec3 eulerAngles2(glm::radians(x2Angle), glm::radians(y2Angle), glm::radians(z2Angle));
    glm::quat q2(eulerAngles2);
    glm::mat4 rotMatrix2 = q2.operator glm::mat4x4();
    model = model * rotMatrix2;
    globalShader2->setMat4("model", model);
    cube2->draw(globalShader2);
    // cube3 object
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x3Trans + 4, y3Trans, z3Trans));
    glm::vec3 eulerAngles3(glm::radians(x3Angle), glm::radians(y3Angle), glm::radians(z3Angle));
    glm::quat q3(eulerAngles3);
    glm::mat4 rotMatrix3 = q3.operator glm::mat4x4();
    model = model * rotMatrix3;
    globalShader3->setMat4("model", model);
    cube3->draw(globalShader3);
    // cube4 object
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x4Trans + 6, y4Trans, z4Trans));
    glm::vec3 eulerAngles4(glm::radians(x4Angle), glm::radians(y4Angle), glm::radians(z4Angle));
    glm::quat q4(eulerAngles4);
    glm::mat4 rotMatrix4 = q4.operator glm::mat4x4();
    model = model * rotMatrix4;
    globalShader4->setMat4("model", model);
    cube4->draw(globalShader4);

    // swap buffers
    glfwSwapBuffers(mainWindow);
}


GLFWwindow* glAllInit()
{
    GLFWwindow* window;

    // glfw: initialize and configure
    if (!glfwInit()) {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project_2020147601", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Allow modern extension features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }
    // Define the viewport dimensions
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // OpenGL states
    glClearColor(0, 0, 0, 1.0f);
    glEnable(GL_DEPTH_TEST);
    return window;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        if (renderMode == INIT) {
            int score = 0;
            int miss = 0;
            renderMode = ANIM;
            beginT = glfwGetTime();
        }
        else if (renderMode == STOP) {
            if (timeT == animEndTime) {
                renderMode = INIT;
            }
            else {
                renderMode = INIT;
            }
        }
        else if (renderMode == ANIM) renderMode = INIT;

        if (renderMode == INIT) {
            int score = 0;
            int miss = 0;
            timeT = 0.0;
            updateAnimData();
        }
    }

    else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        light_q = !(light_q);
    }
    else if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
        light_q = !(light_q);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        light_w = !(light_w);
    }
    else if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
        light_w = !(light_w);
    }
    else if (key == GLFW_KEY_U && action == GLFW_PRESS) {
        light_e = !(light_e);
    }
    else if (key == GLFW_KEY_U && action == GLFW_RELEASE) {
        light_e = !(light_e);
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        light_r = !(light_r);
    }
    else if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
        light_r = !(light_r);
    }
}
