#include "DgViewer.h"

#define STB_IMAGE_IMPLEMENTATION
#include ".\\include\\STB\\stb_image.h"
void DgMesh::setupBuffers()
{
	if (mFaces.empty()) return;

	const bool hasTexCoord = !mTexels.empty();

	// 1) ���� ����: ������ �⺻���� �ϳ� �߰�, �߸��� �ε����� ������ fallback �غ�
	bool hadMatsInitially = !mMaterials.empty();
	size_t defaultMatIdx = 0;

	if (!hadMatsInitially) {
		mMaterials.emplace_back();           // �⺻ ���� ����
		// (�ɼ�) mMaterials.back().mName = "Default";
		defaultMatIdx = 0;
	}

	bool needFallback = false;
	for (const auto& f : mFaces) {
		if (f.mMtlIdx < 0 || f.mMtlIdx >= (int)mMaterials.size()) {
			needFallback = true; break;
		}
	}
	if (hadMatsInitially && needFallback) {
		defaultMatIdx = mMaterials.size();
		mMaterials.emplace_back();           // �ʱ� ������ �־����� �Ϻ� face�� -1/������ �� fallback �߰�
		// (�ɼ�) mMaterials.back().mName = "Default";
	}

	// 2) ������ ��Ŷ �غ�
	mVertexIndicesPerMtl.clear();
	mVertexIndicesPerMtl.resize(mMaterials.size());

	std::vector<float> vertexData;
	unsigned int nextIndex = 0;

	for (size_t i = 0; i < mFaces.size(); ++i)
	{
		const DgFace& face = mFaces[i];

		// ��ȿ ���� �ε��� ����(���ų� ���� ���̸� defaultMatIdx)
		size_t bucket = (face.mMtlIdx >= 0 && face.mMtlIdx < (int)mMaterials.size())
			? (size_t)face.mMtlIdx : defaultMatIdx;

		for (int j = 0; j < 3; ++j)
		{
			const int vIdx = face.mVertIdxs[j];
			const int nIdx = face.mNormalIdxs[j];

			const DgVertex& v = mVerts[vIdx];
			const DgNormal& n = mNormals[nIdx];

			// position
			vertexData.push_back((float)v.mPos[0]);
			vertexData.push_back((float)v.mPos[1]);
			vertexData.push_back((float)v.mPos[2]);

			// normal
			vertexData.push_back((float)n.mDir[0]);
			vertexData.push_back((float)n.mDir[1]);
			vertexData.push_back((float)n.mDir[2]);

			// texcoord (�ɼ�)
			if (hasTexCoord) {
				const int tIdx = face.mTexelIdxs[j];
				const DgTexel& t = mTexels[tIdx];
				vertexData.push_back((float)t.mST[0]);
				vertexData.push_back((float)t.mST[1]);
			}

			// ���� ��Ŷ�� �ε��� push
			mVertexIndicesPerMtl[bucket].push_back(nextIndex++);
		}
	}

	// 3) VAO/VBO ���ε�
	if (!mBuffersInitialized) {
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		mBuffersInitialized = true;
	}

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	const int stride = hasTexCoord ? 8 : 6;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	if (hasTexCoord) {
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(0);
}


//void DgMesh::setupBuffers()
//{
//	if (mFaces.empty()) return;
//
//	bool hasTexCoord = !mTexels.empty();  // �ؽ�ó ��ǥ ����
//
//	std::vector<float> vertexData;
//	mVertexIndicesPerMtl.clear();
//	mVertexIndicesPerMtl.resize(mMaterials.size());
//
//	unsigned int nextIndex = 0;
//
//	for (size_t i = 0; i < mFaces.size(); ++i)
//	{
//		const DgFace& face = mFaces[i];
//
//		for (int j = 0; j < 3; ++j)
//		{
//			int vIdx = face.mVertIdxs[j];
//			int nIdx = face.mNormalIdxs[j];
//
//			const DgVertex& v = mVerts[vIdx];
//			const DgNormal& n = mNormals[nIdx];
//
//			// [position: 3 floats]
//			vertexData.push_back((float)v.mPos[0]);
//			vertexData.push_back((float)v.mPos[1]);
//			vertexData.push_back((float)v.mPos[2]);
//
//			// [normal: 3 floats]
//			vertexData.push_back((float)n.mDir[0]);
//			vertexData.push_back((float)n.mDir[1]);
//			vertexData.push_back((float)n.mDir[2]);
//
//			// [texcoord: 2 floats] - only if available
//			if (hasTexCoord) {
//				int tIdx = face.mTexelIdxs[j];
//				const DgTexel& t = mTexels[tIdx];
//				vertexData.push_back((float)t.mST[0]);
//				vertexData.push_back((float)t.mST[1]);
//			}
//
//			// ������ �ε��� �߰�
//			mVertexIndicesPerMtl[face.mMtlIdx].push_back(nextIndex++);
//		}
//	}
//
//	if (!mBuffersInitialized) {
//		glGenVertexArrays(1, &mVAO);
//		glGenBuffers(1, &mVBO);
//		mBuffersInitialized = true;
//	}
//
//	glBindVertexArray(mVAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
//	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
//
//	// ���� ���̾ƿ�: position (0), normal (1)
//	int stride = hasTexCoord ? 8 : 6;
//
//	// layout(location = 0): position
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	// layout(location = 1): normal
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//	if (hasTexCoord)
//	{
//		// layout(location = 2): texcoord
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
//		glEnableVertexAttribArray(2);
//	}
//
//	glBindVertexArray(0);
//}

void DgMesh::render()
{
	if (!mBuffersInitialized)
		setupBuffers();

	// �ؽ�ó�� ������ �ؽ�ó ���� ���̴� ���
	bool hasTexture = false;
	for (const auto& mat : mMaterials) {
		if (mat.mTexId > 0) {
			hasTexture = true;
			break;
		}
	}

	glBindVertexArray(mVAO);
	for (size_t i = 0; i < mMaterials.size(); ++i)
	{
		const DgMaterial& mtl = mMaterials[i];

		// ���� ��� uniform ����
		glUniform3fv(glGetUniformLocation(mShaderId, "uKa"), 1, mtl.mKa);
		glUniform3fv(glGetUniformLocation(mShaderId, "uKd"), 1, mtl.mKd);
		glUniform3fv(glGetUniformLocation(mShaderId, "uKs"), 1, mtl.mKs);
		glUniform1f(glGetUniformLocation(mShaderId, "uNs"), mtl.mNs);

		// �ؽ�ó ������ ���� ó��
		if (hasTexture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mtl.mTexId);
			glUniform1i(glGetUniformLocation(mShaderId, "uTex"), 0);
		}

		// ������ �ε��� ���� ���� Ȯ��
		const std::vector<unsigned int>& indices = mVertexIndicesPerMtl[i];
		if (indices.empty()) continue;

		// EBO ���� �ӽ� �ε��� ���� (draw call����)
		GLuint tempEBO;
		glGenBuffers(1, &tempEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

		glDeleteBuffers(1, &tempEBO);
	}

	glBindVertexArray(0);
}

void DgMesh::computeNormal(int normalType)
{
	if (normalType == 0) // ���� ����
	{
		int numVerts = (int)mVerts.size();
		mNormals.clear();
		mNormals.assign(numVerts, DgNormal(0.0, 0.0, 0.0));

		int numFaces = (int)mFaces.size();
		for (int i = 0; i < numFaces; ++i)
		{
			int* vidx = mFaces[i].mVertIdxs;
			glm::vec3 p0 = glm::make_vec3(mVerts[vidx[0]].mPos);
			glm::vec3 p1 = glm::make_vec3(mVerts[vidx[1]].mPos);
			glm::vec3 p2 = glm::make_vec3(mVerts[vidx[2]].mPos);
			glm::vec3 e0 = p1 - p0;
			glm::vec3 e1 = p2 - p0;
			glm::vec3 n = glm::normalize(glm::cross(e0, e1));

			mNormals[vidx[0]].mDir[0] += n[0];
			mNormals[vidx[0]].mDir[1] += n[1];
			mNormals[vidx[0]].mDir[2] += n[2];

			mNormals[vidx[1]].mDir[0] += n[0];
			mNormals[vidx[1]].mDir[1] += n[1];
			mNormals[vidx[1]].mDir[2] += n[2];

			mNormals[vidx[2]].mDir[0] += n[0];
			mNormals[vidx[2]].mDir[1] += n[1];
			mNormals[vidx[2]].mDir[2] += n[2];

			mFaces[i].mNormalIdxs[0] = vidx[0];
			mFaces[i].mNormalIdxs[1] = vidx[1];
			mFaces[i].mNormalIdxs[2] = vidx[2];
		}

		for (int i = 0; i < numVerts; ++i)
		{
			glm::vec3 n = glm::make_vec3(mNormals[i].mDir);
			n = glm::normalize(n);
			mNormals[i].mDir[0] = n[0];
			mNormals[i].mDir[1] = n[1];
			mNormals[i].mDir[2] = n[2];
		}			
	}
	else if (normalType == 1) // �ﰢ�� ����
	{
		int numFaces = (int)mFaces.size();
		mNormals.clear();
		mNormals.assign(numFaces, DgNormal(0.0, 0.0, 0.0));

		for (int i = 0; i < numFaces; ++i)
		{
			int* vidx = mFaces[i].mVertIdxs;

			// �� �ﰢ������ ���� ���� ����
			glm::vec3 p0 = glm::make_vec3(mVerts[vidx[0]].mPos);
			glm::vec3 p1 = glm::make_vec3(mVerts[vidx[1]].mPos);
			glm::vec3 p2 = glm::make_vec3(mVerts[vidx[2]].mPos);
			glm::vec3 e0 = p1 - p0;
			glm::vec3 e1 = p2 - p0;
			glm::vec3 n = glm::normalize(glm::cross(e0, e1));

			mNormals[i].mDir[0] = n[0];
			mNormals[i].mDir[1] = n[1];
			mNormals[i].mDir[2] = n[2];
			mFaces[i].mNormalIdxs[0] = i;
			mFaces[i].mNormalIdxs[1] = i;
			mFaces[i].mNormalIdxs[2] = i;
		}
	}
}

bool import_obj_mtl(DgMesh* pMesh, const char* fname)
{
	std::ifstream file(fname);
	if (!file.is_open())
	{
		std::cerr << "\t���� ���� ���� ����: " << fname << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#') continue;

		std::istringstream iss(line);
		std::string tag;
		iss >> tag;

		if (tag == "newmtl")
		{
			pMesh->mMaterials.push_back(DgMaterial());
			iss >> pMesh->mMaterials.back().mName;
		}
		else if (tag == "Ka")
		{
			float* ka = pMesh->mMaterials.back().mKa;
			iss >> ka[0] >> ka[1] >> ka[2];
		}
		else if (tag == "Kd")
		{
			float* kd = pMesh->mMaterials.back().mKd;
			iss >> kd[0] >> kd[1] >> kd[2];
		}
		else if (tag == "Ks")
		{
			float* ks = pMesh->mMaterials.back().mKs;
			iss >> ks[0] >> ks[1] >> ks[2];
		}
		else if (tag == "Ns")
		{
			iss >> pMesh->mMaterials.back().mNs;
		}
		else if (tag == "map_Kd")
		{
			std::string texFile;
			iss >> texFile;

			int width, height, channels;
			stbi_set_flip_vertically_on_load(true);
			unsigned char* data = stbi_load(texFile.c_str(), &width, &height, &channels, 0);
			if (!data) {
				std::cerr << "\t�ؽ�ó �ε� ����: " << texFile << std::endl;
				continue;
			}

			glEnable(GL_TEXTURE_2D);
			GLuint texId;
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			GLenum format = (channels == 3) ? GL_RGB : (channels == 4) ? GL_RGBA : GL_RED;
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			stbi_image_free(data);
			pMesh->mMaterials.back().mTexId = texId;
		}
	}
	return true;
}

DgMesh* import_mesh_obj(const char* fname)
{
	// ���� ����
	std::ifstream file(fname);
	if (!file.is_open()) {
		std::cerr << "OBJ ���� ���� ����: " << fname << std::endl;
		return nullptr;
	}

	std::filesystem::path prevPath = std::filesystem::current_path();

	// ���� ���͸� ����(��, fname = "c:/models/bunny.obj")
	std::filesystem::path objPath(fname);					// objPath = c:/models/bunny.obj
	std::filesystem::path objDir = objPath.parent_path();	// objDir = c:/models/
	std::filesystem::current_path(objDir);					// ���� ���͸��� c:/models/ �� ����

	// �ﰢ �޽� ����
	DgMesh* pMesh = new DgMesh();
	int currMtlIdx = -1;

	// Obj ���� �Ľ�
	std::string line, tag;
	while (std::getline(file, line))
	{
		// �� ���̳� �ּ� ��ŵ
		if (line.empty() || line[0] == '#')
			continue;

		// ���� �ٿ��� �ױ� ����
		std::istringstream iss(line);
		iss >> tag;

		if (tag == "mtllib")	// mtllib dice.mtl
		{
			std::string mtlFile;
			iss >> mtlFile;
			if (!import_obj_mtl(pMesh, mtlFile.c_str()))
				std::cerr << "MTL ���� �ε� ����: " << mtlFile << std::endl;
		}
		else if (tag == "v")	// v	0.0		0.0		5.0
		{
			double x, y, z;
			iss >> x >> y >> z;
			pMesh->mVerts.emplace_back(x, y, z); // pMesh->mVerts.push_back(DgVertex(x, y, z)); �� ����
		}
		else if (tag == "vt")	// vt	0.25	0.5
		{
			double s, t;
			iss >> s >> t;
			pMesh->mTexels.emplace_back(s, t);
		}
		else if (tag == "vn")	// vn	0.0		0.0		1.0
		{
			double nx, ny, nz;
			iss >> nx >> ny >> nz;
			pMesh->mNormals.emplace_back(nx, ny, nz);
		}
		else if (tag == "usemtl")	// usemtl Dice
		{
			std::string mtlName;
			iss >> mtlName;
			currMtlIdx = -1;
			for (size_t i = 0; i < pMesh->mMaterials.size(); ++i) {
				if (pMesh->mMaterials[i].mName == mtlName) {
					currMtlIdx = static_cast<int>(i);
					break;
				}
			}
		}
		else if (tag == "f") // f	1/3/4	2/2/4	3/5/4
		{
			std::vector<int> vIdxs, tIdxs, nIdxs;
			std::string token;
			bool hasTexel = false, hasNormal = false;

			while (iss >> token)
			{
				int vi, ti, ni;
				size_t firstSlash = token.find('/');
				size_t secondSlash = token.find('/', firstSlash + 1);

				if (firstSlash == std::string::npos)		// f	1	2	3
				{
					vi = std::stoi(token) - 1;
					ti = -1;
					ni = -1;
				}
				else if (secondSlash == std::string::npos)	// f	1/3		2/2		3/5
				{
					vi = std::stoi(token.substr(0, firstSlash)) - 1;
					ti = std::stoi(token.substr(firstSlash + 1)) - 1;
					ni = -1;
					hasTexel = true;
				}
				else if (secondSlash == firstSlash + 1)		// f	1//4	2//4	3//4
				{
					vi = std::stoi(token.substr(0, firstSlash)) - 1;
					ti = -1;
					ni = std::stoi(token.substr(secondSlash + 1)) - 1;
					hasNormal = true;
				}
				else										// f	1/3/4	2/2/4	3/5/4
				{
					vi = std::stoi(token.substr(0, firstSlash)) - 1;
					ti = std::stoi(token.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1;
					ni = std::stoi(token.substr(secondSlash + 1)) - 1;
					hasTexel = true;
					hasNormal = true;
				}
				vIdxs.push_back(vi);
				tIdxs.push_back(ti);
				nIdxs.push_back(ni);
			}

			// �ٰ����� �ﰢ������ �����Ͽ� ó��
			for (size_t i = 1; i + 1 < vIdxs.size(); ++i)
			{
				int v0 = vIdxs[0], v1 = vIdxs[i], v2 = vIdxs[i + 1];
				int t0 = tIdxs[0], t1 = tIdxs[i], t2 = tIdxs[i + 1];
				int n0 = nIdxs[0], n1 = nIdxs[i], n2 = nIdxs[i + 1];

				// ������ ������ �ﰢ�� �������� �ڵ� ���
				//if (!hasNormal || n0 < 0 || n1 < 0 || n2 < 0)
				if (!hasNormal)
				{
					auto& p0 = pMesh->mVerts[v0].mPos;
					auto& p1 = pMesh->mVerts[v1].mPos;
					auto& p2 = pMesh->mVerts[v2].mPos;
					double e0[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
					double e1[3] = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };
					double n[3] = { e0[1] * e1[2] - e0[2] * e1[1], e0[2] * e1[0] - e0[0] * e1[2], e0[0] * e1[1] - e0[1] * e1[0] };
					double len = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
					if (len > 1e-8) {
						n[0] /= len; n[1] /= len; n[2] /= len;
					}
					pMesh->mNormals.emplace_back(n[0], n[1], n[2]);
					n0 = n1 = n2 = (int)pMesh->mNormals.size() - 1;
				}

				//if (hasTexel && t0 >= 0 && t1 >= 0 && t2 >= 0)
				if (hasTexel)
					pMesh->mFaces.emplace_back(v0, v1, v2, t0, t1, t2, n0, n1, n2, currMtlIdx);
				else
					pMesh->mFaces.emplace_back(v0, v1, v2, n0, n1, n2, currMtlIdx);
			}
		}
	}

	std::filesystem::current_path(prevPath);
	file.close();
	return pMesh;
}

GLuint load_shaders(const char* vertexPath, const char* fragmentPath)
{
	// ���̴� ���� ����
	std::ifstream vShaderFile(vertexPath);
	std::ifstream fShaderFile(fragmentPath);

	// ���� ���⿡ ������ ��� ���� ���
	if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
		std::cerr << "ERROR: Failed to open shader file(s)\n";
		return 0;
	}

	// ���� ������ ��Ʈ������ �о����
	std::stringstream vShaderStream, fShaderStream;
	vShaderStream << vShaderFile.rdbuf();  // ���� ���̴� �ڵ� �б�
	fShaderStream << fShaderFile.rdbuf();  // �����׸�Ʈ ���̴� �ڵ� �б�

	// ��Ʈ������ ���ڿ��� ��ȯ
	std::string vertexCode = vShaderStream.str();
	std::string fragmentCode = fShaderStream.str();

	// C ���ڿ� �����ͷ� ��ȯ (OpenGL �Լ� ȣ���)
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// ���̴� ��ü ����
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// ���̴� �ҽ� ���� �� ������
	glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
	glCompileShader(fragmentShader);

	// ���� ���̴� ������ ���� ���� Ȯ��
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "ERROR: Vertex Shader Compilation Failed\n" << infoLog << std::endl;
		return 0;
	}

	// �����׸�Ʈ ���̴� ������ ���� ���� Ȯ��
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "ERROR: Fragment Shader Compilation Failed\n" << infoLog << std::endl;
		return 0;
	}

	// ���α׷� ��ü ����
	GLuint programID = glCreateProgram();

	// ���̴��� ���α׷��� ����
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);

	// ���α׷� ��ũ
	glLinkProgram(programID);

	// ��ũ ���� ���� Ȯ��
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		std::cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << std::endl;
		return 0;
	}

	// ���̴� ��ü ���� (���α׷��� �̹� ����Ǿ����Ƿ� �ʿ� ����)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// ���� ���α׷� ID ��ȯ
	return programID;
}

