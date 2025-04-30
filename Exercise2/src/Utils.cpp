#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace PolygonalLibrary
{
bool ImportMesh(PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(mesh))
        return false;

    if(!ImportCell1Ds(mesh))
        return false;

    if(!ImportCell2Ds(mesh))
        return false;

    return true;

}
// ***************************************************************************
bool ImportCell0Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell0Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;

    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell0Ds = listLines.size();

    if (mesh.NumCell0Ds == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DsId.reserve(mesh.NumCell0Ds);
    mesh.Cell0DsCoordinates = Eigen::MatrixXd::Zero(3, mesh.NumCell0Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2d coord;
		char separatore;

        converter >>  id >> separatore >>
		marker >> separatore >> 
		mesh.Cell0DsCoordinates(0, id) >> separatore >>
		mesh.Cell0DsCoordinates(1, id);

        mesh.Cell0DsId.push_back(id);

        /// Memorizza i marker
        if(marker != 0)
        {
            const auto it = mesh.MarkerCell0Ds.find(marker);
            if(it == mesh.MarkerCell0Ds.end())
            {
                mesh.MarkerCell0Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell0Ds[marker].push_back(id);
                it->second.push_back(id);
            }
        }

    }

    return true;
}
// ***************************************************************************
bool ImportCell1Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell1Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell1Ds = listLines.size();

    if (mesh.NumCell1Ds == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    mesh.Cell1DsExtrema = Eigen::MatrixXi(2, mesh.NumCell1Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;
		char separatore;

        converter >>  id >> separatore >>
		marker >>  separatore >>
		mesh.Cell1DsExtrema(0, id) >>  separatore >>
		mesh.Cell1DsExtrema(1, id);
		
        mesh.Cell1DsId.push_back(id);

        /// Memorizza i marker
        if(marker != 0)
        {
            const auto it = mesh.MarkerCell1Ds.find(marker);
            if(it == mesh.MarkerCell1Ds.end())
            {
                mesh.MarkerCell1Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell1Ds[marker].push_back(id);
                it->second.push_back(id);
            }
        }
    }

    return true;
}
// ***************************************************************************
bool ImportCell2Ds(PolygonalMesh& mesh)
{
    ifstream file;
    file.open("./Cell2Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell2Ds = listLines.size();

    if (mesh.NumCell2Ds == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DsId.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsVertices.resize(mesh.NumCell2Ds);
    mesh.Cell2DsEdges.resize(mesh.NumCell2Ds);
	
	unsigned int j=0;
    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
		unsigned int marker;
		unsigned int NumVertices;
		unsigned int NumEdges;
        char separatore;

        converter >>  id >> separatore >>  marker >> separatore >> NumVertices;
		
		vector<unsigned int> vertices(NumVertices);
		
        for(unsigned int i = 0; i <NumVertices ; i++)
            converter >> separatore >> vertices[i];
		
		converter >> separatore >> NumEdges;
		vector<unsigned int> edges(NumEdges);
		
        for(unsigned int i = 0; i < NumEdges; i++)
            converter >> separatore >> edges[i] ;

        mesh.Cell2DsId.push_back(id);
        mesh.Cell2DsVertices[j]=vertices;
        mesh.Cell2DsEdges[j]=edges;
		
		
		
		/// Memorizza i marker
        if(marker != 0)
        {
            const auto it = mesh.MarkerCell2Ds.find(marker);
            if(it == mesh.MarkerCell2Ds.end())
            {
                mesh.MarkerCell2Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell2Ds[marker].push_back(id);
                it->second.push_back(id);
            }
		}
		j++;
    }

    return true;
}
// ***************************************************************************
bool PolygonEdges(PolygonalMesh& mesh)
{
	const double epsilon = 1e-8;
	for (unsigned int i = 0; i < mesh.NumCell1Ds; ++i) {
        unsigned int origin_id = mesh.Cell1DsExtrema(0, i);
        unsigned int end_id = mesh.Cell1DsExtrema(1, i);

        double x1 = mesh.Cell0DsCoordinates(0, origin_id);
        double y1 = mesh.Cell0DsCoordinates(1, origin_id);
        double x2 = mesh.Cell0DsCoordinates(0, end_id);
        double y2 = mesh.Cell0DsCoordinates(1, end_id);
		
		
        double length = sqrt(std::pow(x2 - x1, 2) + pow(y2 - y1, 2));
        if (length <= epsilon) 		
            return false;
    
    }
    return true;
	
	
	
}
// ***************************************************************************
bool PolygonArea(PolygonalMesh& mesh)
{
	const double epsilon = 1e-16;
	for (unsigned int i = 0; i < mesh.NumCell2Ds; ++i) {
        const auto& vertices = mesh.Cell2DsVertices[i];
        double area = 0.0;

        for (size_t j = 0; j < vertices.size(); ++j) {
            unsigned int current = vertices[j];
            unsigned int next = vertices[(j + 1) % vertices.size()];

            double x1 = mesh.Cell0DsCoordinates(0, current);
            double y1 = mesh.Cell0DsCoordinates(1, current);
            double x2 = mesh.Cell0DsCoordinates(0, next);
            double y2 = mesh.Cell0DsCoordinates(1, next);

            area += x1 * y2 - x2 * y1;
        }

        area = std::fabs(area) / 2.0;
        if (area <= epsilon) 
            return false;
      
    }
    return true;
}
	
	
	

}

