/*
 *  Drawable3DOLD.cpp
 *  Phonosynth
 *
 *  Created by Greg on 12/20/08.
 *  Copyright 2008 Brown University. All rights reserved.
 *
 */




/* OLD
 Drawable3D::Drawable3D(int numVertices) :
 m_numVertices(numVertices),
 m_positions(NULL),
 m_texCoords(NULL),
 m_indices(NULL),
 m_vertexBufferPosition(NULL),
 m_vertexBufferTexCoords(NULL),
 m_indexBuffer(NULL),
 m_color(1.0f, 1.0f, 1.0f, 1.0f)
 { }
 
 
 Drawable3D::Drawable3D(const Drawable3D& rhs) :
 m_color(rhs.m_color),
 m_numVertices(rhs.m_numVertices),
 m_numIndices(rhs.m_numIndices)
 {
 
 m_vertexBufferPosition = NULL;
 m_vertexBufferTexCoords = NULL;
 m_indexBuffer = NULL;
 m_positions = NULL;
 m_texCoords = NULL;
 m_indices = NULL;
 
 // copy over positions
 if(rhs.m_vertexBufferPosition != NULL)
 m_vertexBufferPosition  = new VertexBuffer(*(rhs.m_vertexBufferPosition));
 else if (rhs.m_positions != NULL){
 m_positions = new Vertex::XYZ[m_numVertices];
 memcpy(m_positions, rhs.m_positions, m_numVertices * sizeof(Vertex::XYZ));        
 }
 
 // copy over tex coords
 if(rhs.m_vertexBufferTexCoords != NULL)
 m_vertexBufferTexCoords = new VertexBuffer(*(rhs.m_vertexBufferTexCoords));
 else if (rhs.m_texCoords != NULL){
 m_texCoords = new Vertex::UV[m_numVertices];
 memcpy(m_texCoords, rhs.m_texCoords, m_numVertices * sizeof(Vertex::UV));
 }
 
 // copy over indices
 if(rhs.m_indexBuffer != NULL)
 m_indexBuffer		    = new IndexBuffer(*(rhs.m_indexBuffer));
 else if (rhs.m_indices != NULL){
 m_indices = new unsigned short[m_numIndices];
 memcpy(m_indices, rhs.m_indices, m_numIndices * sizeof(unsigned short));
 }	
 }
 
 
 void
 Drawable3D::setPositions(Vertex::XYZ* positions)
 {
 m_positions = positions;
 }
 
 
 void
 Drawable3D::setTexCoords(Vertex::UV* texCoords)
 {
 m_texCoords = texCoords;
 }
 
 
 
 // performs a simple hit test in screen space ([-1, 1], [-1, 1] device coordinates)
 // using the click point (screenX, screenY) and the matrices viewMatrix and projectionMatrix
 double
 Drawable3D::hitTest(const float screenX, const float screenY,
 const Matrix& viewMatrix, const Matrix& projectionMatrix)
 {
 ASSERT(FUZZY_IN_RNG(screenX, -1.0f, 1.0f));
 ASSERT(FUZZY_IN_RNG(screenY, -1.0f, 1.0f));
 
 Vector3 avg(0.0f, 0.0f, 0.0f);
 FOR(i,0,m_numVertices){
 avg += project(m_positions[i], viewMatrix, projectionMatrix);
 }
 avg /= m_numVertices;
 
 return sqrt(SQR(screenY - avg.y) + SQR(screenX - avg.x));
 }
 
 
 
 
 
 void
 Drawable3D::draw(Drawer* d) const
 {
 d->setColor(m_color);
 
 // pass vertex position information
 if(m_vertexBufferPosition)
 d->setPositionSource(m_vertexBufferPosition, FMT_FLOAT3);
 else
 d->setPositionSource(m_positions, FMT_FLOAT3);
 
 // pass vertex texture coordinates
 if(m_vertexBufferTexCoords)
 d->setTexCoordSource(m_vertexBufferTexCoords, FMT_FLOAT2);
 else
 d->setTexCoordSource(m_texCoords, FMT_FLOAT2);
 
 // pass indices if indexed
 if(m_indexBuffer)
 d->draw(m_numIndices, m_indexBuffer, FMT_USHORT);
 else if(m_indices)
 d->draw(m_numIndices, m_indices, FMT_USHORT);
 else
 d->draw(m_numVertices, NULL);
 }
 
 
 
 void
 Drawable3D::genBuffers(const bool useAPI)
 {
 SAFE_DELETE_AND_CLEAR(m_vertexBufferPosition);
 SAFE_DELETE_AND_CLEAR(m_vertexBufferTexCoords);
 SAFE_DELETE_AND_CLEAR(m_indexBuffer);
 
 m_vertexBufferPosition  = new VertexBuffer();
 m_vertexBufferTexCoords = new VertexBuffer();
 m_indexBuffer           = new IndexBuffer();
 
 m_vertexBufferPosition->setData(m_numVertices * sizeof(Vertex::XYZ),
 m_positions,
 FMT_FLOAT3,
 useAPI,
 VB_STATIC);
 
 if(m_texCoords != NULL){
 m_vertexBufferTexCoords->setData(m_numVertices * sizeof(Vertex::UV),
 m_texCoords,
 FMT_FLOAT2,
 useAPI,
 VB_STATIC);
 }
 
 m_indexBuffer->setData(m_numIndices * sizeof(unsigned short),
 m_indices,
 FMT_USHORT,
 useAPI);
 
 SAFE_DELETE_AND_CLEAR_ARRAY(m_positions);
 SAFE_DELETE_AND_CLEAR_ARRAY(m_texCoords);
 SAFE_DELETE_AND_CLEAR_ARRAY(m_indices);
 }
 
 */





