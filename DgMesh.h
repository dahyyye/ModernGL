#pragma once

/*!
 *	\class	DgVertex
 *	\brief	������ ǥ���ϴ� Ŭ����
 */
class DgVertex
{
public:
	/*! \brief ������ ��ǥ */
	double mPos[3];	

public:
	DgVertex(double x, double y, double z) {
		mPos[0] = x;
		mPos[1] = y;
		mPos[2] = z;
	}
	~DgVertex() {}
};

/*!
 *	\class	DgTexel
 *	\brief	�ؽ�ó ��ǥ�� ǥ���ϴ� Ŭ����
 */
class DgTexel
{
public:
	/*! \brief �ؽ�ó ��ǥ(s, t) */
	double mST[2];	

public:
	DgTexel(double s, double t) {
		mST[0] = s;
		mST[1] = t;
	}
	~DgTexel() {}
};

/*!
 *	\class	DgNormal
 *	\brief	������ ǥ���ϴ� Ŭ����
 */
class DgNormal
{
public:
	/*! \brief ������ ���� */
	double mDir[3];

public:
	DgNormal(double x, double y, double z) {
		mDir[0] = x;
		mDir[1] = y;
		mDir[2] = z;
	}
	~DgNormal() {}
};

/*!
 *	\class	DgFace
 *	\brief	�ﰢ���� ǥ���ϴ� Ŭ����
 */
class DgFace
{
public:
	int mMtlIdx;		/*! \brief �ﰢ���� ����ϴ� ������ �ε��� */
	int mVertIdxs[3];	/*! \brief �ﰢ���� �����ϴ� �� ������ �ε��� */
	int mTexelIdxs[3];	/*! \brief �ﰢ�� �� ������ �Ҵ�� �ؼ��� �ε��� */
	int mNormalIdxs[3];	/*! \brief �ﰢ�� �� ������ �Ҵ�� ������ �ε��� */

public:
	DgFace(int vidx0, int vidx1, int vidx2, int nidx0, int nidx1, int nidx2, int mtlIdx) {
		mVertIdxs[0] = vidx0;	mVertIdxs[1] = vidx1;	mVertIdxs[2] = vidx2;
		mTexelIdxs[0] = -1;		mTexelIdxs[1] = -1;		mTexelIdxs[2] = -1;
		mNormalIdxs[0] = nidx0;	mNormalIdxs[1] = nidx1;	mNormalIdxs[2] = nidx2;	
		mMtlIdx = mtlIdx;
	}
	DgFace(int vidx0, int vidx1, int vidx2, int tidx0, int tidx1, int tidx2, int nidx0, int nidx1, int nidx2, int mtlIdx) {
		mVertIdxs[0] = vidx0;	mVertIdxs[1] = vidx1;	mVertIdxs[2] = vidx2;
		mTexelIdxs[0] = tidx0;	mTexelIdxs[1] = tidx1;	mTexelIdxs[2] = tidx2;
		mNormalIdxs[0] = nidx0;	mNormalIdxs[1] = nidx1;	mNormalIdxs[2] = nidx2;	
		mMtlIdx = mtlIdx;
	}
	~DgFace() {}
};

/*!
 *	\class	DgMaterial
 *	\brief	�޽� ������ ǥ���ϴ� Ŭ����
 */
class DgMaterial
{
public:
	std::string mName;	/*! \brief ������ �̸� */
	float mKa[3];		/*! \brief �ֺ��� �ݻ� ��� */
	float mKd[3];		/*! \brief ���ݻ籤 �ݻ� ��� */
	float mKs[3];		/*! \brief ���ݻ籤 �ݻ� ��� */
	float mNs;			/*! \brief ���ݻ� ���� */
	GLuint mTexId;		/*! \brief ������ �ؽ�ó ���̵�(1���� ����) */

public:
	DgMaterial() {
		mKa[0] = 0.1f;	mKa[1] = 0.1f;	mKa[2] = 0.1f;
		mKd[0] = 0.7f;	mKd[1] = 0.7f;	mKd[2] = 0.7f;
		mKs[0] = 0.9f;	mKs[1] = 0.9f;	mKs[2] = 0.9f;
		mNs = 32.0f;
		mTexId = 0;
	}
	~DgMaterial() {}
};

/*!
 *	\class	DgMesh
 *	\brief	�ﰢ������ ������ �޽� ���� ǥ���ϴ� Ŭ����
 */
class DgMesh
{
public:
	std::string mName;					/*! \brief �޽� �̸� */
	std::vector<DgVertex> mVerts;		/*! \brief �޽��� �����ϴ� ���� �迭 */
	std::vector<DgTexel> mTexels;		/*! \brief �޽��� �����ϴ� �ؼ� �迭 */
	std::vector<DgNormal> mNormals;		/*! \brief �޽��� �����ϴ� ���� �迭 */
	std::vector<DgFace> mFaces;			/*! \brief �޽��� �����ϴ� �ﰢ�� �迭 */
	std::vector<DgMaterial> mMaterials;	/*! \brief �޽��� ����ϴ� ���� �迭 */

	std::vector<std::vector<unsigned int>> mVertexIndicesPerMtl;	/*! ������ �ﰢ�� ���� �ε��� �׷� */
	GLuint mShaderId;												/*! \brief �޽��� ����ϴ� ���̴� ���̵� */
	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;	
	bool mBuffersInitialized;

public:
	DgMesh() {
		mVAO = 0;
		mVBO = 0;
		mEBO = 0;
		mBuffersInitialized = false;
	}

	~DgMesh() {
		if (mVAO) glDeleteVertexArrays(1, &mVAO);
		if (mVBO) glDeleteBuffers(1, &mVBO);
		if (mEBO) glDeleteBuffers(1, &mEBO);
	};

	void setupBuffers();
	void computeNormal(int normalType);
	void render();
};

/*!
 *	\brief	OBJ ������ ����Ʈ�Ͽ� �޽� ���� �����Ѵ�.
 * 
 *	\param[in]	fname	���� �̸�
 */
DgMesh* import_mesh_obj(const char* fname);

/*!
 *	\brief	���̴� ���α׷��� �����Ѵ�.
 *
 *	\param[in]	vertexPath		���� ���̴� ������ ���
 *	\param[in]	fragmentPath	�����׸�Ʈ ���̴� ������ ���
 * 
 *	\return	������ ���α׷��� �ڵ��� ��ȯ�Ѵ�.
 */
GLuint load_shaders(const char* vertexPath, const char* fragmentPath);



