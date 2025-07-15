#pragma once
#include "Common.hpp"
#include "IncludesOC.hpp"



void extractFaceData(const TopoDS_Face& face, std::vector<p3>& positions, std::vector<unsigned int>& trIndices, std::vector<unsigned int>& wireIndices
	, std::vector<p3>& normals)
{
	unsigned int indexOffset = positions.size(); //Works bc a face won't have a repeated vertex

	BRepMesh_IncrementalMesh mesh(face, 0.01);
	TopLoc_Location loc;
	Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, loc);
	if (triangulation.IsNull()) return;

	const gp_Trsf& trsf = loc.Transformation();

	//positions
	const int nbNodes = triangulation->NbNodes();
	positions.reserve(trIndices.size() + nbNodes);

	for (int i = 1; i <= nbNodes; ++i)
	{
		const gp_Pnt pnt = triangulation->Node(i).Transformed(trsf);
		const p3 pt{ static_cast<float>(pnt.X()), static_cast<float>(pnt.Y()), static_cast<float>(pnt.Z()) };
		positions.insert(positions.end(), pt);
	}

	//trIndices
	const int nbTris = triangulation->NbTriangles();
	trIndices.reserve(trIndices.size() + nbTris * 3);
	normals.reserve(normals.size() + nbTris * 3);

	for (int i = 1; i <= nbTris; ++i)
	{
		int n1, n2, n3;
		triangulation->Triangle(i).Get(n1, n2, n3);
		trIndices.insert(trIndices.end(), {
			static_cast<unsigned int>(indexOffset + n1 - 1),
			static_cast<unsigned int>(indexOffset + n2 - 1),
			static_cast<unsigned int>(indexOffset + n3 - 1)
			});

		//normals
		const p3& v1 = positions[n1 - 1];
		const p3& v2 = positions[n2 - 1];
		const p3& v3 = positions[n3 - 1];
		const p3 faceNormal = normalize3(cross3(v2 - v1, v3 - v1));
		normals.insert(normals.end(), { faceNormal, faceNormal, faceNormal });
	}


	//wire indices
	trIndices.reserve(trIndices.size() * 2);
	for (unsigned int i = 0; i < trIndices.size(); i += 3)
	{
		wireIndices.insert(wireIndices.end(),
			{
				trIndices[i],trIndices[i + 1],
				trIndices[i + 1],trIndices[i + 2],
				trIndices[i + 2],trIndices[i]
			});
	}
}


void createRectangle(p3 corner1, p3 corner2, p3 planeNormal, TopoDS_Shape& shape)
{
	gp_Pnt A(corner1.x, corner1.y, corner1.z);
	gp_Pnt C(corner2.x, corner2.y, corner2.z);
	gp_Vec AC(A, C);
	gp_Dir N(planeNormal.x, planeNormal.y, planeNormal.z);

	// Step 1: Choose one axis perpendicular to normal
	gp_Vec U = (std::abs(N.X()) < 0.9) ? gp_Vec(1, 0, 0) : gp_Vec(0, 1, 0);
	gp_Vec X = N ^ U; // X axis in the plane
	gp_Vec Y = N ^ X; // Y axis in the plane
	X.Normalize();
	Y.Normalize();

	// Step 2: Project AC onto local axes
	double xLen = AC * X;
	double yLen = AC * Y;

	gp_Pnt B = A.Translated(X * xLen);
	gp_Pnt D = A.Translated(Y * yLen);

	// Create edges A-B-C-D-A
	TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(A, B);
	TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(B, C);
	TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(C, D);
	TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(D, A);

	TopoDS_Wire squareWire = BRepBuilderAPI_MakeWire(edge1, edge2, edge3, edge4);
	TopoDS_Face face = BRepBuilderAPI_MakeFace(squareWire);

	shape = face;
}

void createPolyline(const std::vector<p3>& positions, TopoDS_Shape& shape)
{
	if (positions.size() < 4) // 3 points + repeated first
		throw std::runtime_error("Polyline must have at least 3 unique points (4 with repeat)");

	// Validate closure
	if (positions.front().x != positions.back().x ||
		positions.front().y != positions.back().y ||
		positions.front().z != positions.back().z)
	{
		throw std::runtime_error("Polyline must be closed (first point repeated at end)");
	}

	// Build edges from consecutive points
	std::vector<TopoDS_Edge> edges;
	for (size_t i = 0; i < positions.size() - 1; ++i)
	{
		const p3& p1 = positions[i];
		const p3& p2 = positions[i + 1];

		gp_Pnt gp1(p1.x, p1.y, p1.z);
		gp_Pnt gp2(p2.x, p2.y, p2.z);

		edges.emplace_back(BRepBuilderAPI_MakeEdge(gp1, gp2));
	}

	// Build wire from edges
	BRepBuilderAPI_MakeWire wireBuilder;
	for (const auto& edge : edges)
		wireBuilder.Add(edge);

	if (!wireBuilder.IsDone())
		throw std::runtime_error("Failed to build wire from polyline edges");

	TopoDS_Wire wire = wireBuilder.Wire();

	// Build face from wire
	TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

	// Output as shape
	shape = face;
}

void createCircle(const p3& center, const float radius, TopoDS_Shape& shape, p3 normal_ = {0,1,0})
{
	// Define circle axis system: XZ plane → normal = (0, 1, 0)
	gp_Pnt gpCenter(center.x, center.y, center.z);
	gp_Dir normal(normal_.x, normal_.y, normal_.z);

	gp_Ax2 axis(gpCenter, normal);

	// Define the circle in this plane
	gp_Circ circle(axis, static_cast<Standard_Real>(radius));

	// Create edge from circle curve
	TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);

	// Build wire from edge
	TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);

	// Build face from wire
	TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

	// Output as shape
	shape = face;
}


int faceCounter(const TopoDS_Shape& shape)
{
	int count = 0;
	for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next())
	{
		++count;
		if (count >= 2)
			return 2; // Early exit, no need to count more
	}

	return count; // 0 if none, 1 if exactly one
}


//overwrites shape
void extrudeSingleFace(TopoDS_Shape& shape, const p3& direction)
{
	

	TopExp_Explorer exp(shape, TopAbs_FACE);
	const TopoDS_Face& face = TopoDS::Face(exp.Current());

	// Extrude
	gp_Vec vec(direction.x, direction.y, direction.z);
	shape = BRepPrimAPI_MakePrism(face, vec);

}




void importIGES(const std::string& filepath, TopoDS_Shape& shape)
{
	std::string basePath = "Resources/OpenCascade/";
	string finalPath = basePath + filepath;
	IGESControl_Reader reader;
	IFSelect_ReturnStatus status = reader.ReadFile(finalPath.c_str());
	if (status != IFSelect_RetDone)
		return;

	reader.TransferRoots();
	shape = reader.OneShape();
}




