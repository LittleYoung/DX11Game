#pragma once
#include <DirectXMath.h>
#include <vector>
#include <windows.h>
using namespace DirectX;

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v) {}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	struct MeshData {
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	static void CreatePlaneMeshData(float xLength, float zLength, UINT xNum, UINT zNum, MeshData& meshData);
	static void CreateHillsMeshData(float xLength, float zLength, UINT xNum, UINT zNum, MeshData& meshData);
	static void CreateCylinderMeshData(float topRadius, float bottomRadius, float height, UINT stackCount, UINT sliceCount, MeshData& meshData);
	static void CreateCubeMeshData(float length, float width, float height, MeshData& meshData);
	static void CreateSphereMeshData(float radius, UINT numSubdivisions, MeshData& meshData);
	static void CreateFullScreenMeshData(MeshData& meshData);

	static void CreatePlaneQuadPatchIB(UINT patchRow, UINT patchCol, std::vector<UINT>& indices);
private:
	static void SubdivideTriangle(MeshData& meshData);

	static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
		UINT sliceCount, UINT stackCount, MeshData& meshData);
	static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
		UINT sliceCount, UINT stackCount, MeshData& meshData);
};