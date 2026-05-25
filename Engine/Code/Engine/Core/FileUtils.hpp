#pragma once

#include <vector>
#include <string>

#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"

int FileReadToBuffer(std::vector<uint8_t>& out_buffer, std::string const& fileName);
int FileReadToString(std::string& out_string, std::string const& fileName);

bool FileWriteFromBuffer(std::vector<uint8_t> const& data, std::string const& filePathName);

//bool LoadObjFile(std::filesystem::path filePath, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_indices, bool& out_hasNormals, bool& out_hasUVs, Mat44 const& transform = Mat44::IDENTITY);