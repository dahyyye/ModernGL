#pragma once

class DgScene
{
public:
	bool mOpen;
	ImVec2 mSceneSize;
	DgFrmBuffer mFrameBuf;

	std::vector<float> mGroundVerts;
	GLuint mGroundVAO;
	GLuint mGroundVBO;

	std::vector<DgMesh*> mMeshList;
	std::vector<GLuint> mShaders;

	// 화면 조작을 위한 변수
	float mZoom;
	ImVec2 mStartPos;
	glm::mat4 mRotMat;
	glm::vec3 mPan;

private:
	DgScene()
	{
		mOpen = true;
		mZoom = -30.0f;
		mStartPos = ImVec2(0.0f, 0.0f);
		mRotMat = glm::mat4(1.0f);
		mPan = glm::vec3(0.0f);
	}
	~DgScene()
	{
		glDeleteVertexArrays(1, &mGroundVAO);
		glDeleteBuffers(1, &mGroundVBO);

		for (DgMesh* m : mMeshList)
			delete m;
		
		for (GLuint id : mShaders)
			glDeleteProgram(id);		
	}

public:
	static DgScene& instance() {
		static DgScene _inst;
		return _inst;
	}

	void setOpen(bool v) {
		mOpen = v;
	}

	bool isOpen() const {
		return mOpen;
	}

	void initOpenGL() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void createGroundMesh();

	void getSphereCoords(double x, double y, float* px, float* py, float* pz);
	void showWindow();
	void renderScene();
	void renderFps();
	void renderContextPopup();
	void processMouseEvent();
	void processKeyboardEvent();
};
