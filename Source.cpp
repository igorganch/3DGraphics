#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderinit.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);
void drawTextCube();

void init();

void drawLampObject(Shader& lightCubeShader, glm::mat4& projection, glm::mat4& view, glm::mat4& model);

void drawCube();

// variables
const unsigned int screen_width = 800;
const unsigned int screen_height = 600;
unsigned int VBO, cubeVAO, planeVAO, texCubeVAO;
unsigned int lightCubeVAO;
unsigned int texture;

// camera variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;

// timers
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// light position
glm::vec3 lightPos(1.5f, 3.0f, 1.5f);

bool TPressed;
bool YPressed;
bool QPressed;
bool OPressed;
bool PPressed;
int Rrandom = 100;
int rubyrandom;
int texrandom;
int main()
{
    // initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create window with glfw
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "IharGancharenka", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // mouse input capture with glfw
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // enable the depth buffer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // setup the shader programs
    Shader cube("material.vs", "material.fs");
    Shader lightCube("lightsource.vs", "lightsource.fs");
    Shader planeCube("material.vs", "material.fs");
    Shader texCube("lightingmap.vs", "lightingmap.fs");
    init();
    unsigned int diffuseMap = loadTexture("assets/container2.png");
    unsigned int specularMap = loadTexture("assets/container2_specular.png");

    texCube.use();
    texCube.setInt("material.diffuse", 0);
    texCube.setInt("material.specular", 1);


    // Shader rubyCube("material.vs", "material.fs");



   
    float object[][3] = { {-0.8f, -0.8f, 0.0f }, {0.8f, -0.8f, 0.0f}, {-0.8f, 0.8f, 0.0f}, {0.8f, 0.8f, 0.0f} };
    while (!glfwWindowShouldClose(window))
    {
      
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        processInput(window);

        // bg rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

       

        //////////////////////////////////////////////////////P L A N E  C U B E   ///////////////////////////////////////////////////////////////////////////////////////////////
        planeCube.use();

        planeCube.setVec3("light.position", lightPos);
        planeCube.setVec3("viewPos", camera.Position);
        glm::vec3 lightColor;
        lightColor.x = 1.0f;
        lightColor.y = 1.0f;
        lightColor.z = 1.0f;
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        planeCube.setVec3("light.ambient", ambientColor);
        planeCube.setVec3("light.diffuse", diffuseColor);
        planeCube.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        planeCube.setVec3("material.ambient", 6.7f, 6.7f, 6.7f);
        planeCube.setVec3("material.diffuse", 0.55f, 0.55f, 0.55f);
        planeCube.setVec3("material.specular", 0.774597f, 0.774597f, 0.774597f);
        planeCube.setFloat("material.shininess", 60.6f);


        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        planeCube.setMat4("projection", projection);
        planeCube.setMat4("view", view);



        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3((14.0f) * 8, 0.25f, (14.0f) * 8));
        model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
        planeCube.setMat4("model", model);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //////////////////////////////////////////////////////      E N D    ///////////////////////////////////////////////////////////////////////////////////////////////


          ////////////////////////////////////////////////////// T E X T U R E     C U B E   ///////////////////////////////////////////////////////////////////////////////////////////////






         //////////////////////////////////////////////////////      E N D    ///////////////////////////////////////////////////////////////////////////////////////////////









        ////////////////////////////////////////////////////// R E G U L A R       C U B E   ///////////////////////////////////////////////////////////////////////////////////////////////

        cube.use();
        cube.setVec3("light.position", lightPos);
        cube.setVec3("viewPos", camera.Position);
        cube.setMat4("projection", projection);
        cube.setMat4("view", view);

        cube.setVec3("light.ambient", ambientColor);
        cube.setVec3("light.diffuse", diffuseColor);
        cube.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        /*
        cube.setVec3("objectColor", 0.0f, 1.0f, 0.0f);
        cube.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        cube.setVec3("lightPos", lightPos);
        cube.setVec3("viewPos", camera.Position);

        cube.setMat4("projection", projection);
        cube.setMat4("view", view);
        */
        // world transformation
        model = glm::mat4(1.0f);

        for (unsigned int i = 0; i < 4; i++) {
            glm::mat4 model = glm::mat4(1.0f);
           
            if (OPressed == true && i == rubyrandom) {
                cube.setVec3("material.ambient", 0.1745f, 0.01175f, 0.01175f);
                cube.setVec3("material.diffuse", 0.61424f, 0.04136f, 0.04136f);
                cube.setVec3("material.specular", 0.727811f, 0.626959f, 0.626959f);
                cube.setFloat("material.shininess", 0.6f);



            }
            else {
                cube.setVec3("material.ambient", 0.0215f, 0.1745f, 0.0215f);
                cube.setVec3("material.diffuse", 0.07568f, 0.61424f, 0.07568f);
                cube.setVec3("material.specular", 0.774597f, 0.774597f, 0.774597f);
                cube.setFloat("material.shininess", 20.0f);
            }
            if (PPressed == true && i == texrandom) {

                // shader configuration
                // --------------------
                texCube.use();
                texCube.setVec3("light.position", lightPos);
                texCube.setVec3("viewPos", camera.Position);

                // light properties
                texCube.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
                texCube.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
                texCube.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

                // material properties
                texCube.setFloat("material.shininess", 64.0f);
                texCube.setMat4("projection", projection);
                texCube.setMat4("view", view);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap);
                // bind specular map
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, specularMap);

                if (i == Rrandom) {
                    if (YPressed != true) {
                        float angle = glfwGetTime() * 45.0f;
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(object[i][0], object[i][1], object[i][2]));
                        model = glm::translate(model, glm::vec3(object[i][0], object[i][1], object[i][2]));
                    }
                }
                else {
                    model = glm::translate(model, glm::vec3(object[i][0], object[i][1], object[i][2]));
                }
                if (TPressed == true && i == Rrandom) {
                    model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
                }
                if (YPressed == true && i == Rrandom) {
                    float angle = glfwGetTime() * 45.0f;
                    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
                }

              
                texCube.setMat4("model", model);
                drawTextCube();
                cube.use();
            }
            else {
                if (i == Rrandom) {
                    if (YPressed != true) {
                        float angle = glfwGetTime() * 45.0f;
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(object[i][0], object[i][1], object[i][2]));
                        model = glm::translate(model, glm::vec3(object[i][0], object[i][1], object[i][2]));
                    }
                }
                else {
                    model = glm::translate(model, glm::vec3(object[i][0], object[i][1], object[i][2]));
                }
                if (TPressed == true && i == Rrandom) {
                    model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
                }
                if (YPressed == true && i == Rrandom) {
                    float angle = glfwGetTime() * 45.0f;
                    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
                }


              
                cube.setMat4("model", model);
                drawCube();
            }
        }




        //////////////////////////////////////////////////////      E N D    ///////////////////////////////////////////////////////////////////////////////////////////////
        // draw the cube object


        //draw the lamp object


        drawLampObject(lightCube, projection, view, model);







        // swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}


void drawTextCube() {
    glBindVertexArray(texCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);



}
void drawCube()
{
    
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawLampObject(Shader& lightCubeShader, glm::mat4& projection, glm::mat4& view, glm::mat4& model)
{
   
    lightCubeShader.use();
    lightCubeShader.setMat4("projection", projection);
    lightCubeShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); 
    lightCubeShader.setMat4("model", model);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void init()
{

    // vertex data and normals
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    //VAO and VBO for the cube


     ////////////////////////////////////////////////////// R E G U L A R       C U B E   ///////////////////////////////////////////////////////////////////////////////////////////////


    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture
    /*
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    */
    //////////////////////////////////////////////////////      E N D    ///////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////P L A N E  C U B E   ///////////////////////////////////////////////////////////////////////////////////////////////

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(planeVAO);

    // position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //////////////////////////////////////////////////////      E N D    ///////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////// T E X T U R E     C U B E   ///////////////////////////////////////////////////////////////////////////////////////////////






    glGenVertexArrays(1, &texCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(texCubeVAO);

    // position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);






    //////////////////////////////////////////////////////      E N D    ///////////////////////////////////////////////////////////////////////////////////////////////



    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// keyboard input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {  //Rotation

        Rrandom = 0 + (std::rand() % (3 - 0 + 1));
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {// Scale 
        TPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {// Move to center of the wndow 
        YPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {// Reset cubes position to original coordinates 
        TPressed = false;
        YPressed = false;
        QPressed = true;
        Rrandom = 100;
    }
    else {
        int i = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {// Ruby
        OPressed = true;
        rubyrandom = 0 + (std::rand() % (3 - 0 + 1));
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {  //Texture
        PPressed = true;
        texrandom = 0 + (std::rand() % (3 - 0 + 1));


    }





    /*  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {// Move to center of the wndow
          WPressed = true;
      }
       if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {// Reset cubes position to original coordinates
          SPressed = false;
          WPressed = false;
          QPressed = true;
          random = 100;


      }
      else {
          int i = 1;
      }
      */


}

// resizing to match the viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
/*
void applyTexture(unsigned int& texture) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_MIN_FILTER, GL_LINEAR);

}
*/

// mouse axes input
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// mouse scroll input
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
