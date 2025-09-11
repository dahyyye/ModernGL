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

	// =========================
	// [추가]
	// SDF 
	int       mSdfShape;      // 0=Sphere, 1=Box, 2=Torus
	glm::vec3 mSdfMove;
	float     mSdfSphereR;
	glm::vec3 mSdfBoxB;
	glm::vec2 mSdfTorus;

	GLuint mSdfProg;
	GLuint mSdfVAO;
	bool   mSdfInit;

	void initSdfPass();
	void drawSdfPass(const glm::mat4& view, const glm::mat4& proj);
	// =========================


private:
	DgScene()
	{
		mOpen = true;
		mZoom = -30.0f;
		mStartPos = ImVec2(0.0f, 0.0f);
		mRotMat = glm::mat4(1.0f);
		mPan = glm::vec3(0.0f);
		// [추가]
		mSdfShape = 0;                          // Sphere
		mSdfMove = glm::vec3(0.0f, 0.0f, 0.0f);
		mSdfSphereR = 0.8f;
		mSdfBoxB = glm::vec3(0.4f, 0.5f, 0.3f); // half-size
		mSdfTorus = glm::vec2(0.8f, 0.25f);     // (R, r)
		// =========================
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
	//[추가]
	void renderSdfControls();
};
