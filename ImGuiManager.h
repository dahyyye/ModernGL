#pragma once

/*!
 *	\class	ImGuiManager
 *	\brief	GLFW + OpenGL3.3 ����� ImGui �������α׷� ������ ���� �Ŵ��� �̱���(singleton) Ŭ����
 *
 *	\author ������(shyun@dongguk.edu)
 *	\date	2024-03-04
 */
class ImGuiManager
{
public:
	/*! \biref	���� ������ ������ */
	GLFWwindow *mWindow;

private:
	/*! 
	 *	\biref ������ 
	 */
	ImGuiManager();

	/*! 
	 *	\biref �Ҹ��� 
	 */
	~ImGuiManager();

public:
	/*!
	 *	\biref	Ŭ���� ��ü�� ��ȯ�Ѵ�.
	 *
	 *	\return Ŭ���� ��ü�� ��ȯ�Ѵ�.
	 */
	static ImGuiManager& instance()
	{
		static ImGuiManager _inst;
		return _inst;
	}

	/*! 
	 *	\biref	GLFW �����츦 �����ϰ� ImGui�� �ʱ�ȭ �Ѵ�.	
	 */
	bool init(int winWidth, int winHeight);

	/*! 
	 *	\biref	���ο� �������� �����Ѵ�.	
	 */
	void begin();

	/*! 
	 *	\biref	�������� ������ �������Ѵ�. 
	 */
	void end();

	/*! 
	 *	\biref	������ ������� ImGui�� �����Ѵ�. 
	 */
	void cleanUp();
};

/*!
 *	\class	DgFrmBuffer
 *	\brief	OpenGL ������ ���۸� ǥ���ϴ� Ŭ����
 *
 *	\author ������(shyun@dongguk.edu)
 *	\date	2023-12-02
 */
class DgFrmBuffer
{
public:
	/*! \biref ������ ���� ��ü �ڵ� */
	unsigned int mFrameBufObj;

	/*! \biref ������ �ؽ�ó ���̵� */
	unsigned int mTexture;

	/*! \biref ������ ���� ��ü �ڵ� */
	unsigned int mRenderBufObj;

public:
	/*! \biref ������ */
	DgFrmBuffer(int width = 100, int height = 100) {
		// ������ ������ �ڵ��� �����ϰ� ���ε��Ѵ�.
		glGenFramebuffers(1, &mFrameBufObj);
		glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufObj);

		// �ؽ�ó �ڵ��� �����Ͽ� ���ε��Ѵ�.
		glGenTextures(1, &mTexture);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

		// ������ ������ �ڵ��� �����ϰ� ���ε��Ѵ�.
		glGenRenderbuffers(1, &mRenderBufObj);
		glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufObj);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBufObj);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		// ���۸� �����Ѵ�.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	/*! \biref �Ҹ��� */
	~DgFrmBuffer() {
		glDeleteFramebuffers(1, &mFrameBufObj);
		glDeleteTextures(1, &mTexture);
		glDeleteRenderbuffers(1, &mRenderBufObj);
	}

	/*!
	 *	\biref	������ �ؽ�ó�� ��ȯ�Ѵ�.
	 *
	 *	\return	������ �ؽ�ó�� ��ȯ�Ѵ�.
	 */
	unsigned int getFrameTexture() {
		return mTexture;
	}

	/*!
	 *	\biref	������ ������ ũ�⸦ �缳���Ѵ�.
	 *
	 *	\param[in]	width	������ �ʺ�
	 *	\param[in]	height	������ ����
	 */
	void rescaleFrameBuffer(int width, int height) {
		glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufObj);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufObj);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBufObj);
	}

	/*!
	 *	\biref	������ ���۸� ���ε��Ѵ�.
	 */
	void bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufObj);
	}

	/*!
	 *	\biref	������ ���۸� ����ε��Ѵ�.
	 */
	void unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};
