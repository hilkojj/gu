
#include "mesh.h"
#include "vert_attributes.h"

Mesh::Mesh(unsigned int nrOfVertices, unsigned int nrOfIndices, const VertAttributes &attributes)

    : nrOfVertices(nrOfVertices), nrOfIndices(nrOfIndices),

      vertices(nrOfVertices * attributes.getVertSize()),
      indices(nrOfIndices)
{
}
