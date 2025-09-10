// OpenGL 3.3 Core Profile�� ����� 3DViewer
#include "DgViewer.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3_mt.lib")
#pragma comment(lib, "glew32.lib")

int main(int argc, char **argv) 
{
    // GLFW �����츦 �����ϰ�, ImGui�� �ʱ�ȭ �Ѵ�.
    if (!ImGuiManager::instance().init(1280, 1024))
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }
    
    // GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) 
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // OpenGL �ʱ�ȭ �� ���̴� �ε�
    DgScene& scene = DgScene::instance();
    scene.initOpenGL();
    scene.mShaders.push_back(load_shaders(".\\shaders\\ground.vert", ".\\shaders\\ground.frag"));      // 0
    scene.mShaders.push_back(load_shaders(".\\shaders\\toon.vert", ".\\shaders\\toon.frag"));          // 1
    scene.mShaders.push_back(load_shaders(".\\shaders\\phong.vert", ".\\shaders\\phong.frag"));        // 2
    scene.mShaders.push_back(load_shaders(".\\shaders\\texture.vert", ".\\shaders\\texture.frag"));    // 3
    scene.mShaders.push_back(load_shaders(".\\shaders\\phong.vert", ".\\shaders\\effect-1.frag"));     // 4
    scene.mShaders.push_back(load_shaders(".\\shaders\\phong.vert", ".\\shaders\\effect-2.frag"));     // 5
    scene.mShaders.push_back(load_shaders(".\\shaders\\phong.vert", ".\\shaders\\effect-3.frag"));     // 6 ���� Į��
    scene.mShaders.push_back(load_shaders(".\\shaders\\phong.vert", ".\\shaders\\effect-4.frag"));     // 7 ���ݻ� RGB
    scene.mShaders.push_back(load_shaders(".\\shaders\\phong.vert", ".\\shaders\\effect-5.frag"));     // 8 
    scene.mShaders.push_back(load_shaders(".\\shaders\\phong.vert", ".\\shaders\\effect-6.frag"));     // 9 
    
    // �ٴ� ��� �޽��� ����
    scene.createGroundMesh();

    // �޽��� ����Ʈ�Ͽ� ��鿡 �߰�
    DgMesh *pMesh1 = ::import_mesh_obj(".\\model\\bunny_8327v.obj");
    scene.mMeshList.push_back(pMesh1);
    pMesh1->mName = "Bunny";
    pMesh1->computeNormal(0);
    pMesh1->mShaderId = scene.mShaders[2];

    /*DgMesh *pMesh2 = ::import_mesh_obj(".\\model\\camel.obj");
    scene.mMeshList.push_back(pMesh2);
    pMesh2->mName = "Camel";
    pMesh2->computeNormal(0);
    pMesh2->mShaderId = scene.mShaders[2];*/
    
    // ���� ���� ����
    while (!glfwWindowShouldClose(ImGuiManager::instance().mWindow))
    {
        glfwPollEvents();        
        ImGuiManager::instance().begin();
        {
            // ���� �޴� ������
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Exit"))
                    {
                        glfwSetWindowShouldClose(ImGuiManager::instance().mWindow, true);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            
            // SceneGL ������ ���
            DgScene::instance().showWindow();
        }
        ImGuiManager::instance().end();        
    }

    ImGuiManager::instance().cleanUp();
    return 0;
}






