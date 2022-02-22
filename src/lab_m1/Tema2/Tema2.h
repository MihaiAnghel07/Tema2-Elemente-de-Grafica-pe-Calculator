#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "components/simple_scene.h"
#include "lab_m1/Tema2/Tema2_camera.h"
#include "lab_m1/Tema2/Tema2_object2D.h"
#include "lab_m1/Tema2/Tema2_transform2d.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void GenerateMaze();
        void GeneratePlayerStartPosition();
        void GenerateEnemiesPosition();
        void DrawMaze();
        void DrawPlayer();
        void DrawEnemy(float deltaTimeSeconds);
        void DrawHealth();
        void DrawTime(float deltaTimeSeconds);
        void Check_Player_Enemy_Collision();
        void Check_Bullet_Enemy_Collision();
        void Check_Win();
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void Tema2::shootBullet(float deltaTimeSeconds);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        implemented::Camera_Tema2* camera;
        glm::mat4 projectionMatrix;
        int ***maze;
        
        float FoV;
        float left, right, bottom, top;
        float zNear, zFar;
        
        int numberOfMazes;
        int mazeSize;
        int randomMaze;
        int startX;
        int startZ;
        int enemyX;
        int enemyY;
        float bulletX;
        float bulletY;
        float deltaX;
        float deltaY;
        float bulletSpeed;
        float newBulletX;
        float newBulletY;
        float speedBulletX;
        float speedBulletY;
        float timer;
        float timeDecreaseFactor;
        float healthDecreaseFactor;
        float factor;
        
        float playerX;
        float playerY;
        float playerZ;
        int thirdCamera;
        int flag, flag2;
        int fire;
        int flagBullet;
        int nrEnemies;
        
        float anglePlayer;
        float angleBullet;
        glm::vec3 body_arms_Color;
        glm::vec3 head_hands_Color;
        glm::vec3 legs_Color;

        typedef struct myEnemy {
            int available;
            float x, y;
            float offset;
            int dir;
        }myEnemy;
        myEnemy enemyStruct[100];
    };
}