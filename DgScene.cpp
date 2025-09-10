#include "DgViewer.h"

void DgScene::createGroundMesh()
{
	// -10���� 10���� 1.0 ���� �������� ���ڼ� ����
	for (float x = -10.0f; x <= 10.0f; x += 1.0f)
	{
		// ������: (x, 0, -10) ~ (x, 0, 10)
		mGroundVerts.push_back(x);
		mGroundVerts.push_back(0.0f);
		mGroundVerts.push_back(-10.0f);

		mGroundVerts.push_back(x);
		mGroundVerts.push_back(0.0f);
		mGroundVerts.push_back(10.0f);

		// ����: (-10, 0, x) ~ (10, 0, x)
		mGroundVerts.push_back(-10.0f);
		mGroundVerts.push_back(0.0f);
		mGroundVerts.push_back(x);

		mGroundVerts.push_back(10.0f);
		mGroundVerts.push_back(0.0f);
		mGroundVerts.push_back(x);
	}
	
	// VAO(Vertex Array Object)�� VBO(Vertex Buffer Object) ����
	glGenVertexArrays(1, &mGroundVAO);  // VAO 1�� ����
	glGenBuffers(1, &mGroundVBO);       // VBO 1�� ����

	// VAO ���ε� (���� ������ �� VAO�� �����)
	glBindVertexArray(mGroundVAO);
	{
		// VBO ���ε� �� ������ ���ε�
		glBindBuffer(GL_ARRAY_BUFFER, mGroundVBO);  // ���� Ÿ�� ����

		// ���� �����͸� GPU �޸𸮿� ����(������� �����Ƿ� STATIC_DRAW)
		glBufferData(GL_ARRAY_BUFFER, mGroundVerts.size() * sizeof(float), mGroundVerts.data(), GL_STATIC_DRAW);

		// ���� �Ӽ� ���� (location = 0, vec3 ��ġ ��ǥ)
		glVertexAttribPointer(
			0,                  // layout(location = 0)
			3,                  // vec3: 3���� float
			GL_FLOAT,           // ������ Ÿ��
			GL_FALSE,           // ����ȭ ���� (���� ��ġ�� ����ȭ���� ����)
			3 * sizeof(float),  // stride: �� ���� 3���� float (12 bytes)
			(void*)0            // ���� ������ (�迭 ù ��ġ����)
		);
		glEnableVertexAttribArray(0);  // location 0 ��� Ȱ��ȭ
	}
	glBindVertexArray(0);   // VAO ����ε� (���� �ٸ� ��ü ������ ������ ���� �ʵ���)
}

void DgScene::getSphereCoords(double x, double y, float* px, float* py, float* pz)
{
	*px = (2.0f * (float)x - mSceneSize[0]) / mSceneSize[0];
	*py = (-2.0f * (float)y + mSceneSize[1]) / mSceneSize[1];
	float r = (*px) * (*px) + (*py) * (*py);
	if (r >= 1.0f) {
		*px /= std::sqrtf(r);
		*py /= std::sqrtf(r);
		*pz = 0.0;
	}
	else
		*pz = std::sqrtf(1.0f - r);
}

void DgScene::showWindow()
{
	// ���� ���°� �ƴϸ� �����Ѵ�.
	if (!mOpen)	return;

	// ������ �÷���(window flag)�� �����Ѵ�.
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = true;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool no_docking = false;
	static bool unsaved_document = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;

	// ��� �����츦 �����ϰ�, collapsed�� ���¶�� �ٷ� �����Ѵ�.
	bool* openPtr = (no_close ? nullptr : &mOpen);
	if (!ImGui::Begin("SceneGL", openPtr, window_flags))
	{
		ImGui::End();
		return;
	}

	// ���콺 �̺�Ʈ�� ó��
	processMouseEvent();

	// Ű���� �̺�Ʈ�� ó��
	processKeyboardEvent();

	// ���� ������ ������
	renderScene();

	// Context �˾� �޴��� ������
	renderContextPopup();

	ImGui::End();
}

void DgScene::processMouseEvent()
{
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None))
	{
		// ���� �������� ���� ����� ����(0, 0)���� ���콺 ��ǥ(x, y)�� ���Ѵ�.
		ImVec2 pos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
		int x = (int)pos.x, y = (int)pos.y;
		ImVec2 delta = ImGui::GetIO().MouseDelta;

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))			// ���� ��ư�� Ŭ���� ���
		{
			mStartPos[0] = pos[0];
			mStartPos[1] = pos[1];
		}
		else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))		// ���� ��ư���� �巡���ϴ� ���
		{
			float px, py, pz, qx, qy, qz;
			getSphereCoords(mStartPos[0], mStartPos[1], &px, &py, &pz);
			getSphereCoords(pos[0], pos[1], &qx, &qy, &qz);
			glm::vec3 rotAxis = glm::cross(glm::vec3(px, py, pz), glm::vec3(qx, qy, qz));
			if (glm::length(rotAxis) > 0.000001f)
			{
				float angle = acos(px * qx + py * qy + pz * qz);
				mRotMat = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(rotAxis)) * mRotMat;
			}
			mStartPos[0] = pos[0];
			mStartPos[1] = pos[1];
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))		// Ŭ���ߴ� ���� ��ư�� ���� ���
		{
			mStartPos[0] = mStartPos[1] = 0.0;
		}
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))	// �߰� ��ư�� Ŭ���� ���
		{
			mStartPos[0] = pos[0];
			mStartPos[1] = pos[1];
		}
		else if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))	// �߰� ��ư���� �巡���ϴ� ���
		{
			float dx = (float)(pos[0] - mStartPos[0]) * 0.01f;
			float dy = (float)(mStartPos[1] - pos[1]) * 0.01f;
			mPan += glm::inverse(glm::mat3(mRotMat)) * glm::vec3(dx, dy, 0.0f);
			mStartPos[0] = pos[0];
			mStartPos[1] = pos[1];
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))	// Ŭ���ߴ� �߰� ��ư�� ���� ���
		{
		}
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))		// ������ ��ư�� Ŭ���� ���
		{
		}
		else if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))	// ������ ��ư���� �巡���ϴ� ��� 
		{
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))	// Ŭ���ߴ� ������ ��ư�� ���� ���
		{
		}
		else if (delta.x != 0.0f || delta.y != 0.0f)				// �׳� �����̴� ���
		{
		}

		// ����� ����/�ƿ��� �����Ѵ�.
		if (ImGui::GetIO().MouseWheel != 0.0f)
		{
			int dir = (ImGui::GetIO().MouseWheel > 0.0) ? 1 : -1;
			mZoom += (float)dir;
		}
	}
}

void DgScene::renderScene()
{
	// ���� ������(3D Scene)�� ������ ���Ͽ�, ������ ���۸� �����Ѵ�.
	ImVec2 sceneSize = ImGui::GetContentRegionAvail();
	if (sceneSize[0] != mSceneSize[0] || sceneSize[1] != mSceneSize[1])
	{
		mSceneSize = sceneSize;
		mFrameBuf.rescaleFrameBuffer((int)sceneSize[0], (int)sceneSize[1]);
	}

	// ��� Į�� ���ۿ� ������
	mFrameBuf.bind();
	{
		glViewport(0, 0, (GLsizei)mSceneSize[0], (GLsizei)mSceneSize[1]);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				

		// ���� ��ȯ ��� ����
		glm::mat4 projMat = glm::perspective(glm::radians(30.0f), mSceneSize[0] / mSceneSize[1], 1.0f, 1000.0f);

		// ���� ��ȯ ���
		glm::mat4 viewMat(1.0f);                                                // ���� ��� �ʱ�ȭ, M = I
		viewMat = glm::translate(viewMat, glm::vec3(0.0, 0.0, mZoom));           // �� ��ȯ, M = I * T
		viewMat = viewMat * mRotMat;                                             // ȸ�� ��ȯ, M = I * T * R
		viewMat = glm::translate(viewMat, glm::vec3(mPan[0], mPan[1], mPan[2]));   // Pan ��ȯ, M = I * T * R * Pan

		// �ٴ� ������
		{
			// �𵨸� ��ȯ ���(���� ���)
			glm::mat4 modelMat(1.0f);
			glUseProgram(mShaders[0]);

			// ���� ���̴��� �Ķ���� ����
			glUniformMatrix4fv(glGetUniformLocation(mShaders[0], "uModel"), 1, GL_FALSE, glm::value_ptr(modelMat));
			glUniformMatrix4fv(glGetUniformLocation(mShaders[0], "uView"), 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(glGetUniformLocation(mShaders[0], "uProjection"), 1, GL_FALSE, glm::value_ptr(projMat));

			// �ٴ� ��� �׸���
			glBindVertexArray(mGroundVAO);
			glDrawArrays(GL_LINES, 0, mGroundVerts.size() / 3);
			glBindVertexArray(0);
			glUseProgram(0);
		}

		// �� ������
		for (DgMesh* pMesh : mMeshList)
		{
			// �𵨸� ��ȯ ���(���� ���)
			glm::mat4 modelMat(1.0f);

			// ���̴� ���α׷� ����
			GLuint shaderProgram = pMesh->mShaderId;
			glUseProgram(shaderProgram);

			// ���� ���̴��� �Ķ���� ����
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(modelMat));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uView"), 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, glm::value_ptr(projMat));

			// ������ �Ӽ��� ���� ��ġ ����
			glm::vec3 viewPos = glm::vec3(glm::inverse(viewMat)[3]);
			glm::vec3 lightPos = glm::vec3(glm::inverse(viewMat)[3]);
			glUniform3fv(glGetUniformLocation(shaderProgram, "uViewPos"), 1, glm::value_ptr(viewPos));
			glUniform3fv(glGetUniformLocation(shaderProgram, "uLightPos"), 1, glm::value_ptr(lightPos));
			glUniform3fv(glGetUniformLocation(shaderProgram, "uLightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));


			// �� �����Ӹ��� �ð� �� ���
			float timeValue = static_cast<float>(glfwGetTime());

			// uTime ��ġ ���
			GLint timeLoc = glGetUniformLocation(shaderProgram, "uTime");

			// �������� �� ����
			glUseProgram(shaderProgram);
			glUniform1f(timeLoc, timeValue);

			// �� ������ �ϱ�
			pMesh->render();
			glUseProgram(0);
		}

		// FPS ������
		renderFps();					
	}
	mFrameBuf.unbind();

	ImTextureID textureID = (void*)(uintptr_t)mFrameBuf.getFrameTexture();
	ImGui::Image(textureID, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
}

void DgScene::renderFps()
{
	// ����� �������� ��ġ�� ������ �����Ѵ�.
	const float D = 10.0f;
	static int corner = 3;
	float W = ImGui::GetWindowSize().x;
	float H = ImGui::GetWindowSize().y;

	if (corner != -1)
	{
		ImVec2 pos = ImGui::GetWindowPos(), pivot;
		switch (corner)
		{
		case 0: pivot = ImVec2(0.0f, 0.0f); pos += ImVec2(D, D); break;
		case 1: pivot = ImVec2(1.0f, 0.0f); pos += ImVec2(W - D, D); break;
		case 2: pivot = ImVec2(0.0f, 1.0f); pos += ImVec2(D, H - D); break;
		case 3: pivot = ImVec2(1.0f, 1.0f); pos += ImVec2(W - D, H - D); break;
		}
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f);

	// ���콺 ��ǥ�� ���Ѵ�.
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 pos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();

	// �����츦 �����ϰ� �޽����� ����Ѵ�.
	bool open = true;
	bool* p_open = &open;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.0f);
	if (ImGui::Begin("Example: Simple overlay", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::Text("Rendering Speed: %.1f FPS", io.Framerate);
		ImGui::Separator();
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%d,%d)", (int)pos.x, (int)pos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void DgScene::processKeyboardEvent()
{
	// Ű���� �̺�Ʈ�� ó���Ѵ�.
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None))
	{
		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			glfwSetWindowShouldClose(ImGuiManager::instance().mWindow, true);
		}		
	}
}
void DgScene::renderContextPopup()
{
	if (ImGui::BeginPopupContextWindow("SceneContext", ImGuiPopupFlags_MouseButtonRight))
	{
		static const char* kShaderLabel[] = {
			"0: Black",
			"1: Toon", 
			"2: Phong", 
			"3: Texture",
			"4: Effect-1", 
			"5: Effect-2",
			"6: Normals", 
			"7: Fresnel RGB",
			"8: Effect-5", 
			"9: Effect-6"
		};
		const int labelCount = IM_ARRAYSIZE(kShaderLabel);
		const int shaderCount = (int)mShaders.size();
		const int showCount = (shaderCount < labelCount) ? shaderCount : labelCount;

		ImGui::TextUnformatted("Shaders");
		ImGui::Separator();

		if (shaderCount == 0) {
			ImGui::TextDisabled("No shaders loaded");
			ImGui::EndPopup();
			return;
		}
		
		// --- per-mesh ---
		for (size_t i = 0; i < mMeshList.size(); ++i)
		{
			DgMesh* M = mMeshList[i];
			if (!M) continue;

			ImGui::PushID((int)i);
			char menuLabel[128];
			if (!M->mName.empty()) 
				snprintf(menuLabel, sizeof(menuLabel), "%s", M->mName.c_str());
			else                   
				snprintf(menuLabel, sizeof(menuLabel), "Mesh %zu", i);

			if (ImGui::BeginMenu(menuLabel))
			{
				// 1~9 ��
				for (int s = 0; s < showCount; ++s)
				{
					bool selected = (M->mShaderId == mShaders[s]);
					if (ImGui::MenuItem(kShaderLabel[s], nullptr, selected))
						M->mShaderId = mShaders[s];
				}				
				ImGui::EndMenu();
			}
			ImGui::PopID();
		}

		ImGui::EndPopup();
	}
}


