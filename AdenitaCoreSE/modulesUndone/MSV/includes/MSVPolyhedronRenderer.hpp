#pragma once 

#include "SAMSON.hpp"
#include "ANTPolyhedron.hpp"
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QFileSystemWatcher>
#include "SEConfig.hpp"


class MSVPolyhedronRenderer : public QObject {
  Q_OBJECT

public:

  MSVPolyhedronRenderer();
  MSVPolyhedronRenderer(ANTPolyhedron * p);
  MSVPolyhedronRenderer(const MSVPolyhedronRenderer &p);

	~MSVPolyhedronRenderer();																								
  
	void										render();																


  public slots:
  void                            loadShaders();

private:
  SEConfig * configuration_ = new SEConfig();
  ANTPolyhedron * p_;

  void														setUniformVector3(unsigned int p_location, const SBVector3& p_data);

  string vsPath_;
  string tcsPath_;
  string tesPath_;
  string gsPath_;
  string fsPath_;

  std::string default_shader_path = SB_ELEMENT_PATH + "/Resource/shaders/";
  QFileSystemWatcher shaderFileWatcher_;


  QOpenGLShaderProgram*						program_;
  QOpenGLVertexArrayObject				vertexArrayObject_;
  int															positionLocation_;
  int															numberOfIndices_;


  unsigned int*										indexArray_;
  float*													positionArray_;

  GLuint													indexBuffer_;

  unsigned int										projectionMatrixUniformLocation_;
  unsigned int										viewMatrixUniformLocation_;
  unsigned int										viewProjectionMatrixUniformLocation_;

  unsigned int										cameraEyePositionUniformLocation_;
  unsigned int										cameraTargetPositionUniformLocation_;
  unsigned int										cameraUpVectorUniformLocation_;
  unsigned int										cameraAngleUniformLocation_;

  unsigned int										lightPositionUniformLocation_;
  unsigned int										diffuseMaterialUniformLocation_;
  unsigned int										ambientMaterialUniformLocation_;

  unsigned int										globalClippingPointUniformLocation_;
  unsigned int										globalClippingNormalUniformLocation_;

  unsigned int										innerTessellationLevelUniformLocation_;
  unsigned int										outerTessellationLevelUniformLocation_;

  float														innerTessellationLevel_;
  float														outerTessellationLevel_;

  QOpenGLBuffer										positionDataBuffer_;
  QOpenGLFunctions_4_3_Core*			gl_;


};
