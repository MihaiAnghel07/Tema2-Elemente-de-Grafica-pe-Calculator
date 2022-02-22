#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;
using namespace m1;


Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
    GenerateMaze();
    GeneratePlayerStartPosition();
    GenerateEnemiesPosition();

    camera = new implemented::Camera_Tema2();
    camera->Set(glm::vec3(startX + 0.2f, 0.5f, -startZ), glm::vec3(startX + 0.2f, 0.5f, startZ), glm::vec3(0, 0.5f, 0));
   

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    
    {
        Mesh* healthFrame = Tema2_object2D::CreateHealthBarFrame("healthFrame", glm::vec3(0, 0, 0), 0.5f, glm::vec3(1, 0, 0), true);
        meshes[healthFrame->GetMeshID()] = healthFrame;
        Mesh* health = Tema2_object2D::CreateSquare("health", glm::vec3(0, 0, 0), 0.5f, glm::vec3(1, 0, 0), true);
        meshes[health->GetMeshID()] = health;
    }

    {
        Shader* shader = new Shader("MyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        FoV = 60;
        zFar = 200.0;
        zNear = 0.01f;
        left = -10;
        bottom = -10;
        right = 10;
        top = 10;
        projectionMatrix = glm::perspective(RADIANS(FoV), window->props.aspectRatio, zNear, zFar);
    }

    {
        anglePlayer = 0;
        angleBullet = 0;
        thirdCamera = 1;
        flag = 0;
        flag2 = 0;
        fire = 0;
        flagBullet = 0;
        bulletSpeed = 1.5f;

        timer = 90; // secunde
        timeDecreaseFactor = 0;
        healthDecreaseFactor = 0;
        factor = 1.2f / (2 * timer);
    }

    {
        body_arms_Color = glm::vec3(0, 0.6f, 0.298f);
        head_hands_Color = glm::vec3(1, 0.898f, 0.8f);
        legs_Color = glm::vec3(0.8, 0, 0);
    }
    
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
    
}

void Tema2::GenerateMaze()
{   
    // se deschide fisierul din care citesc labirintul
    std::ifstream file("src/lab_m1/Tema2/maze.txt", ios::in);
    if (!file.is_open()) {
        cerr << "Error: " << strerror(errno) << endl;
    }

    // citesc numarul de labirinte si dimensiunea acestora
    std::string line = "";
    getline(file, line);
    numberOfMazes = line[0] - '0';
    mazeSize = 11;
    
    maze = (int***)calloc(numberOfMazes, sizeof(int**));
    for (int i = 0; i < numberOfMazes; i++) {
        maze[i] = (int**)calloc(mazeSize, sizeof(int*));
        for (int j = 0; j < mazeSize; j++) {
            maze[i][j] = (int*)calloc(mazeSize, sizeof(int));
        }
    }

    // adaug in matricea tridimensionala labirintele din fisier
    int i = 0;
    int j = -1;
    while (getline(file, line)) {
        j++;
        if (line.length() != 0) {
            for (int k = 0; k < mazeSize; k++) {
                maze[i][j][k] = line[k] - '0';
            }
        }
        else {
            i++;
            j = -1;
        }
    }

    file.close();

    // generez un numar random (numarul maze-ului) si ma asigur ca la
    // fiecare rulare se va alege un maze diferit din totalitatea de labirinte
    srand((unsigned)time(NULL));
    ifstream file2("src/lab_m1/Tema2/randomMaze.txt");
    line = "";
    getline(file2, line);
    file2.close();

    while ((randomMaze = rand() % numberOfMazes) == (line[0] - '0'));

    ofstream file3("src/lab_m1/Tema2/randomMaze.txt");
    file3 << randomMaze;
    file3.close();
}

void Tema2::GeneratePlayerStartPosition()
{
    startX = rand() % mazeSize;
    startZ = rand() % mazeSize;
    while (maze[randomMaze][startX][startZ] != 0) {
        startX = rand() % mazeSize;
        startZ = rand() % mazeSize;
    }
}

void Tema2::GenerateEnemiesPosition()
{
    nrEnemies = mazeSize / 2 + 2;
    int count = 0;
    while (count != nrEnemies) {
        enemyX = rand() % mazeSize;
        enemyY = rand() % mazeSize;
        while (maze[randomMaze][enemyX][enemyY] != 0) {
            enemyX = rand() % mazeSize;
            enemyY = rand() % mazeSize;
        }
        maze[randomMaze][enemyX][enemyY] = 2;
        enemyStruct[count].x = enemyX + 0.5f;
        enemyStruct[count].y = -enemyY - 0.5f;
        enemyStruct[count].available = 1;
        enemyStruct[count].offset = 0;
        enemyStruct[count].dir = 1;
        count++;
    }
}

void Tema2::DrawMaze() 
{ 
    // se genereaza peretii labirintului
    for (int i = 0; i < mazeSize; i++) {
        for (int j = 0; j < mazeSize; j++) {
            if (maze[randomMaze][i][j] == 1) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5f, 0.5f, -j - 0.5f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
                RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
            }
        }
    }

    // se genereaza podeaua
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(5, 0, -5));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.5f, 0.3f));
        RenderSimpleMesh(meshes["plane"], shaders["MyShader"], modelMatrix, glm::vec3(0.6f, 1, 0.6f));
    }

}

void Tema2::DrawPlayer() {
 

    if (thirdCamera == 1) {
        playerX = camera->GetTargetPosition().x;
        playerY = camera->GetTargetPosition().y;
        playerZ = camera->GetTargetPosition().z;
        
        // piciorul stang
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY - 0.31f, playerZ)); 
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.085f, 0.0f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.25f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, legs_Color);
        }

        //piciorul drept
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY - 0.31f, playerZ)); 
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.085f, 0.0f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.25f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, legs_Color);
        }

        //trunchiul
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY, playerZ));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.32f, 0.35f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, body_arms_Color);
        }


        //capul
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY + 0.27f, playerZ)); 
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.17f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, head_hands_Color);
        }

        //bratul dreapt
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY + 0.075f, playerZ)); 
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.24f, 0.0f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.2f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, body_arms_Color);
        }

        //mana dreapta
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY - 0.09f, playerZ)); 
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.24f, 0.0f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.12f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, head_hands_Color);
        }

        //bratul stang
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY + 0.075f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.08f - 0.16f, 0.0f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.2f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, body_arms_Color);
        }

        //mana stanga
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY - 0.09f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(anglePlayer), glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.08f - 0.16f, 0.0f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.12f, 0.1f));
            RenderSimpleMesh(meshes["box"], shaders["MyShader"], modelMatrix, head_hands_Color);
        }

    }
}

void Tema2::DrawEnemy(float deltaTimeSeconds)
{   
    int count = 0;
    for (int i = 0; i < mazeSize; i++) {
        for (int j = 0; j < mazeSize; j++) {
            if (maze[randomMaze][i][j] == 2 && enemyStruct[count].available == 1 && i != playerX && j != playerZ) {
                
                // se realizeaza miscarea fiecarui inamic pe diagonala patratelei lui
                if (enemyStruct[count].offset >= 0.3f || enemyStruct[count].offset <= -0.3f) {
                    enemyStruct[count].dir = -enemyStruct[count].dir;
                }
                
                enemyStruct[count].offset += 0.28f * deltaTimeSeconds * enemyStruct[count].dir;
                
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyStruct[count].x + enemyStruct[count].offset, 0.4f, enemyStruct[count].y + enemyStruct[count].offset));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.55f, 0.55f, 0.55f));
                RenderSimpleMesh(meshes["sphere"], shaders["MyShader"], modelMatrix, glm::vec3(0, 0, 1));
                
            }
            if (maze[randomMaze][i][j] == 2) {
                count++;
            }
        }
    }
    
}

void Tema2::DrawHealth()
{
    {
        glm::mat3 modelMatrix = Tema2_transform2D::Translate(2, 2) * Tema2_transform2D::Scale(1.2f, 0.3f) * Tema2_transform2D::Translate(-0.5f, -0.5f);
        RenderMesh2D(meshes["healthFrame"], shaders["VertexColor"], modelMatrix);
    }

    {
        glm::mat3 modelMatrix = Tema2_transform2D::Translate(2, 2) * Tema2_transform2D::Scale(1.2f - healthDecreaseFactor, 0.3f) * Tema2_transform2D::Translate(-0.5f, -0.5f);
        RenderMesh2D(meshes["health"], shaders["VertexColor"], modelMatrix);
    }

}

void Tema2::DrawTime(float timeDecreaseFactor) 
{
    {
        glm::mat3 modelMatrix = Tema2_transform2D::Translate(2, 1.8f) * Tema2_transform2D::Scale(1.2f, 0.3f) * Tema2_transform2D::Translate(-0.5f, -0.5f);
        RenderMesh2D(meshes["healthFrame"], shaders["VertexColor"], modelMatrix);
    }

    {
        glm::mat3 modelMatrix = Tema2_transform2D::Translate(2, 1.8f) * Tema2_transform2D::Scale(1.2f - 2 * timeDecreaseFactor, 0.3f) * Tema2_transform2D::Translate(-0.5f, -0.5f);
        RenderMesh2D(meshes["health"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema2::Check_Player_Enemy_Collision() 
{
    for (int i = 0; i < nrEnemies; i++) {
        if (enemyStruct[i].available == 1 && abs(playerX - (enemyStruct[i].x + enemyStruct[i].offset)) < 0.25f && abs(playerZ - (enemyStruct[i].y + enemyStruct[i].offset)) < 0.25f) {
            enemyStruct[i].available = 0;
            healthDecreaseFactor += 0.2f;
            if (healthDecreaseFactor == 1.2f) {
                cout << "GAME OVER!" << endl;
                exit(-1);
            }
        }
    }

}

void Tema2::Check_Bullet_Enemy_Collision()
{
    for (int i = 0; i < nrEnemies; i++) {
        if (enemyStruct[i].available == 1 && abs(bulletX - (enemyStruct[i].x + enemyStruct[i].offset)) < 0.15f && abs(bulletY - (enemyStruct[i].y + enemyStruct[i].offset)) < 0.15f) {
            enemyStruct[i].available = 0;
            fire = 0;
        }
    }

}

void Tema2::Check_Win()
{
    if (playerX >= mazeSize || playerX <= 0 || -playerZ >= mazeSize || -playerZ <= 0) {
        cout << "YOU WON THE GAME!!" << endl;
        exit(-1);
    }

}

void Tema2::Update(float deltaTimeSeconds)
{
    DrawMaze();
    DrawEnemy(deltaTimeSeconds);
    DrawPlayer();

    glClear(GL_DEPTH_BUFFER_BIT);
   
    DrawHealth();
    DrawTime(timeDecreaseFactor);
    Check_Player_Enemy_Collision();
    Check_Bullet_Enemy_Collision();
    Check_Win();
    

    if (timer - deltaTimeSeconds > 0.02f) {
        timer -= deltaTimeSeconds;
        timeDecreaseFactor += factor * deltaTimeSeconds;
    }
    else {
        cout << "TIME OVER!" << endl;
        exit(-1);
    }

    if (fire == 1) {
        if (flagBullet == 0) {
            angleBullet = anglePlayer;
        }
        shootBullet(deltaTimeSeconds);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(bulletX, 0.4f, bulletY)); 
        modelMatrix = glm::rotate(modelMatrix, RADIANS(angleBullet), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.1f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 0.15f));
        RenderSimpleMesh(meshes["sphere"], shaders["MyShader"], modelMatrix, glm::vec3(1, 0, 0));
    }

}

void Tema2::shootBullet(float deltaTimeSeconds)
{
    /* 
     proiectilul poate atinge o anumita distanta daca nu intalneste nimic
     calculez acea distanta, implicit noile coordonate unde trebuie sa
     ajunga in cazul in care nu intalneste un inamic / obstacol
     */
    if (flagBullet == 0) {
        flagBullet = 1;
        float x = camera->GetTargetPosition().x;
        float z = camera->GetTargetPosition().z;
        bulletX = x;
        bulletY = z;
        deltaY = bulletSpeed * cos(RADIANS(angleBullet));
        deltaX = bulletSpeed * sin(RADIANS(angleBullet));
        newBulletX = bulletX + deltaX;
        newBulletY = bulletY + deltaY;
        speedBulletX = 1;
        speedBulletY = 1;

        // pentru a merge cu o viteza linniara, calculez un factor de scalara (o viteza)
        // pentru fiecare axa
        if (abs(newBulletX - bulletX) > abs(newBulletY - bulletY)) {
            speedBulletY = abs(newBulletY - bulletY) / abs(newBulletX - bulletX);
            if (speedBulletY == 0) { speedBulletY = 1; }
        }
        else {
            speedBulletX = abs(newBulletX - bulletX) / abs(newBulletY - bulletY);
            if (speedBulletX == 0) { speedBulletX = 1; }
        }
    }

    /* 
     * verific daca proiectilul a ajuns la tinta si daca da, dispare
     * altfel isi continua deplasarea
    */
    if (abs(bulletX - newBulletX) < 0.04 && abs(bulletY - newBulletY) < 0.04) {
        fire = false;
        flagBullet = 0;
    } 
    else {
        // fac deplasarea proiectilului daca nu exista coliziune
        if (bulletX < newBulletX) { bulletX += speedBulletX * 2 * deltaTimeSeconds; }
        if (bulletX > newBulletX) { bulletX -= speedBulletX * 2 * deltaTimeSeconds; }
        if (bulletY < newBulletY) { bulletY += speedBulletY * 2 * deltaTimeSeconds; }
        if (bulletY > newBulletY) { bulletY -= speedBulletY * 2 * deltaTimeSeconds; }
    }
    
}

void Tema2::FrameEnd()
{
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    shader->Use();
    int loc_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(loc_color, 1, glm::value_ptr(color));
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    mesh->Render();
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 2.0f;

    if (window->KeyHold(GLFW_KEY_W)) { camera->MoveForward(deltaTime * cameraSpeed); }

    if (window->KeyHold(GLFW_KEY_A)) { camera->TranslateRight(-deltaTime * cameraSpeed); }

    if (window->KeyHold(GLFW_KEY_S)) { camera->MoveForward(-deltaTime * cameraSpeed); }

    if (window->KeyHold(GLFW_KEY_D)) { camera->TranslateRight(deltaTime * cameraSpeed); }

    if (window->KeyHold(GLFW_KEY_Q)) { camera->TranslateUpward(-deltaTime * cameraSpeed); }

    if (window->KeyHold(GLFW_KEY_E)) { camera->TranslateUpward(deltaTime * cameraSpeed); }

}

void Tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_O) { projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar); }

    if (key == GLFW_KEY_P) { projectionMatrix = glm::perspective(RADIANS(FoV), window->props.aspectRatio, zNear, zFar); }
}

void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    float sensivityOX = 0.008f;
    float sensivityOY = 0.001f;

    if (thirdCamera == 0) {
        if (flag == 0) {
            camera->TranslateForward(1.5f);
        }
        camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
        camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
        flag = 1;
        flag2 = 0;
    }

    if (thirdCamera == 1) {
        flag = 0;
        if (flag2 == 0) {
            camera->TranslateForward(-1.5f);
        }
        flag2 = 1;
        camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
        camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
    }
    
    // unghiul player-ului l-am calculat folosind niste valori 
    // obtinute prin multiple incercari pentru a obtine
    // orientarea si viteza de rotatie corecte (totul a pornit
    // de la mouseX)
    anglePlayer = -((mouseX / 10 - 60) * 4.5 - 16);
    
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (button == 2) { thirdCamera = 1 - thirdCamera; }

    if (button == 1 && thirdCamera == 0) { fire = 1; }
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
