#include "FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <map>
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>
#include <filesystem>

int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename)
{
	FILE* file = nullptr;

	errno_t returnValue = fopen_s(&file, filename.c_str(), "rb");
	if (returnValue != EINVAL)
	{
		if (file == NULL)
		{
			ERROR_RECOVERABLE("FILE was NULL");
			return -1;
		}
		if (filename.c_str() == NULL)
		{
			ERROR_AND_DIE("Couldn't open specified file: ");
		}
	}

	fseek(file, 0, SEEK_END);

	size_t length = size_t(ftell(file));

	if (length == 0)
	{
		ERROR_AND_DIE("Length was 0");
	}

	if (outBuffer.size() < length)
	{
		outBuffer.resize(length);
	}

	fseek(file, 0, SEEK_SET);

	const size_t ret_code = fread(outBuffer.data(), sizeof(uint8_t), length, file);
	if (ret_code != length)
	{
		ERROR_AND_DIE("Read the wrong number of bytes");
	}

	int ret_code_int = fclose(file);
	if (ret_code_int != 0)
	{
		ERROR_AND_DIE("Error closing file");
	}

	fclose(file);
	return int(length);
}

int FileReadToString(std::string& out_string, std::string const& fileName)
{
	std::vector<uint8_t> buffer;
	int code = FileReadToBuffer(buffer, fileName);
	if (code != -1)
	{
		out_string = std::string(buffer.begin(), buffer.end());
		out_string += '\0';
		return 0;
	}
	ERROR_AND_DIE("Could not load file " + fileName);
	return -1;
}

bool FileWriteFromBuffer(std::vector<uint8_t> const& buffer, std::string const& filePathName)
{
	std::filesystem::path filePath(filePathName);

	// Create the parent directories if they do not exist
	std::filesystem::create_directories(filePath.parent_path());

	FILE* file = nullptr;
	errno_t result = fopen_s(&file, filePathName.c_str(), "wb");

	//result = 0 means it exists

	//this means it doesnt exist
	if (result != 0 || file == nullptr)
	{
		return false;
	}

	fwrite(buffer.data(), 1, buffer.size(), file);

	fclose(file);

	return true;
}
/*
bool LoadObjFile(std::filesystem::path filePath, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_indices, bool& out_hasNormals, bool& out_hasUVs, Mat44 const& transform)
{
	std::vector<Vec3> positions;
	std::vector<Vec2> uvs;
	std::vector<Vec3> normals;

	std::map<std::string, size_t> uniqueVerts;

	std::vector<Vertex_PCUTBN> verts;
	std::vector<unsigned int> indices;

	std::string fileString;

	int result = FileReadToString(fileString, filePath.string());
	if (result == -1)
	{
		return false;
	}

	Mat44 normalTransform = transform;
	normalTransform = normalTransform.GetOrthonormalInverse();
	normalTransform.Transpose();
	normalTransform.SetTranslation3D(Vec3(0.f, 0.f, 0.f));

	std::vector<std::string> lines = SplitStringOnDelimiter(fileString, '\n');
	for (std::string const& line : lines)
	{
		std::string processedLine = RemoveAllSubstr(line, "\r");

		std::vector<std::string> tokens = SplitStringOnDelimiter(processedLine, ' ');
		//Position data
		if (tokens[0] == "v")
		{
			Vec3 vertex = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			positions.push_back(vertex);
		}
		else if (tokens[0] == "vt")
		{
			Vec2 uv = Vec2(std::stof(tokens[1]), std::stof(tokens[2]));
			uvs.push_back(uv);
		}
		else if (tokens[0] == "vn")
		{
			Vec3 normal = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			normals.push_back(normal);
		}
		else if (tokens[0] == "f")
		{
			std::vector<unsigned int> faceIndices;
			for (size_t i = 1; i < tokens.size(); ++i)
			{


				std::vector<std::string> strIndices = SplitStringOnDelimiter(tokens[i], '/');
				size_t vertIndex = 0;
				if (!strIndices[0].empty())
				{
					vertIndex = std::stoul(strIndices[0]);
				}
				else
				{
					ERROR_RECOVERABLE("Face has no vertex index in file : " + fileString);
				}

				std::optional<size_t> uvIndex;
				if (strIndices.size() > 1)
					if (!strIndices[1].empty())
					{
						uvIndex = std::stoul(strIndices[1]);
					}

				std::optional<size_t> normalIndex;
				if (strIndices.size() > 2)
					if (!strIndices[2].empty())
					{
						normalIndex = std::stoul(strIndices[2]);
					}

				std::string vertKey = Stringf("%zu/%zu/%zu", vertIndex, uvIndex, normalIndex);
				if (uniqueVerts.find(vertKey) == uniqueVerts.end())
				{
					uniqueVerts[vertKey] = verts.size();
					Vertex_PCUTBN vert;
					Vec3 pos = transform.TransformPosition3D(positions[vertIndex - 1]);
					vert.m_position = pos;
					if (uvIndex.has_value())
					{
						vert.m_uvTexCoords = uvs[uvIndex.value() - 1];
					}
					else
					{
						vert.m_uvTexCoords = Vec2(0.0f, 0.0f);
					}
					if (normalIndex.has_value())
					{
						Vec3 norm = normalTransform.TransformPosition3D(normals[normalIndex.value() - 1]);
						vert.m_normal = norm;
					}
					else
					{
						//#TODO : Calculate normals if they are not provided
						vert.m_normal = Vec3(0.0f, 0.0f, 0.0f);
					}
					verts.push_back(vert);
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
				else
				{
					faceIndices.push_back((unsigned int)uniqueVerts[vertKey]);
				}
			}

			//Triangulate the face
			if (faceIndices.size() >= 3)
			{
				unsigned int rootIndex = faceIndices[0];
				for (size_t j = 2; j < faceIndices.size(); ++j)
				{
					indices.push_back(faceIndices[j - 1]);
					indices.push_back(rootIndex);
					indices.push_back(faceIndices[j]);
				}
			}
			else
			{
				ERROR_RECOVERABLE("Face has less than 3 vertices in file : " + fileString);
			}
		}

		if (!verts.empty())
		{
			out_verts = verts;
			out_indices = indices;
			out_hasNormals = !normals.empty();
			out_hasUVs = !uvs.empty();
		}
		else
		{
			return false;
		}
	}

	return true;
}
*/