#include "MSVPolyhedronRenderer.hpp"

MSVPolyhedronRenderer::MSVPolyhedronRenderer(ANTPolyhedron * p) {

  if (configuration_->usage == "h") {
    default_shader_path = "C:/Development/DNA_Nanomodeling_New/Adenita/Adenita/resource/shaders/";
  }
  
  p_ = p;


  //set arrays and faces
  unsigned int nFaces = boost::numeric_cast<unsigned int>(p_->GetNumFaces());
  unsigned int nVertices = boost::numeric_cast<unsigned int>(p_->GetNumVertices());
  positionArray_ = new float[3 * nVertices];
  indexArray_ = p_->GetIndices();

  for (unsigned int i = 0; i < nVertices; i++) {
    positionArray_[3 * i + 0] = p_->GetVertexById(i)->GetSBPosition()[0].getValue() * 100;
    positionArray_[3 * i + 1] = p_->GetVertexById(i)->GetSBPosition()[1].getValue() * 100;
    positionArray_[3 * i + 2] = p_->GetVertexById(i)->GetSBPosition()[2].getValue() * 100;
  }

  vsPath_ = default_shader_path + "polyhedron.vs";
  tcsPath_ = default_shader_path + "polyhedron.tcs";
  tesPath_ = default_shader_path + "polyhedron.tes";
  gsPath_ = default_shader_path + "polyhedron.gs";
  fsPath_ = default_shader_path + "polyhedron.fs";

  shaderFileWatcher_.addPath(vsPath_.c_str());
  shaderFileWatcher_.addPath(tcsPath_.c_str());
  shaderFileWatcher_.addPath(tesPath_.c_str());
  shaderFileWatcher_.addPath(gsPath_.c_str());
  shaderFileWatcher_.addPath(fsPath_.c_str());

  QObject::connect(&shaderFileWatcher_, SIGNAL(fileChanged(const QString &)), this, SLOT(loadShaders()));

  loadShaders();


}

MSVPolyhedronRenderer::~MSVPolyhedronRenderer() {

  ANTAuxiliary::log(string("MSVPolyhedronRenderer destroyed"));

  positionDataBuffer_.destroy();
  //delete configuration_;
}

void MSVPolyhedronRenderer::setUniformVector3(unsigned int p_location, const SBVector3& p_data) {

  program_->setUniformValue(p_location, QVector3D((float)p_data.v[0].getValue(), (float)p_data.v[1].getValue(), (float)p_data.v[2].getValue()));

}

void MSVPolyhedronRenderer::loadShaders()
{
  //shaders
  program_ = new QOpenGLShaderProgram();
  gl_ = new QOpenGLFunctions_4_3_Core();
  gl_->initializeOpenGLFunctions();
  vertexArrayObject_.create();

  QFile vsFile(vsPath_.c_str());
  QFile tcsFile(tcsPath_.c_str());
  QFile tesFile(tesPath_.c_str());
  QFile gsFile(gsPath_.c_str());
  QFile fsFile(fsPath_.c_str());

  vsFile.open(QIODevice::ReadOnly);
  tcsFile.open(QIODevice::ReadOnly);
  tesFile.open(QIODevice::ReadOnly);
  gsFile.open(QIODevice::ReadOnly);
  fsFile.open(QIODevice::ReadOnly);

  QTextStream vsIn(&vsFile);
  QTextStream tcsIn(&tcsFile);
  QTextStream tesIn(&tesFile);
  QTextStream gsIn(&gsFile);
  QTextStream fsIn(&fsFile);

  QString vertexShaderCode;
  while (!vsIn.atEnd()) {
    vertexShaderCode.append(vsIn.readLine() + "\n");
  }

  QString tesselationControlShaderCode;
  while (!tcsIn.atEnd()) {
    tesselationControlShaderCode.append(tcsIn.readLine() + "\n");
  }

  QString tesselationEvaluationShaderCode;
  while (!tesIn.atEnd()) {
    tesselationEvaluationShaderCode.append(tesIn.readLine() + "\n");
  }

  QString geometryShaderCode;
  while (!gsIn.atEnd()) {
    geometryShaderCode.append(gsIn.readLine() + "\n");
  }

  QString fragmentShaderCode;
  while (!fsIn.atEnd()) {
    fragmentShaderCode.append(fsIn.readLine() + "\n");
  }

  //ANTAuxiliary::log(vertexShaderCode.toStdString());
  //ANTAuxiliary::log(tesselationControlShaderCode.toStdString());
  //ANTAuxiliary::log(tesselationEvaluationShaderCode.toStdString());
  //ANTAuxiliary::log(geometryShaderCode.toStdString());
  //ANTAuxiliary::log(fragmentShaderCode.toStdString());

  vsFile.close();
  tcsFile.close();
  tesFile.close();
  gsFile.close();
  fsFile.close();

  // link

  bool b1 = program_->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderCode);
  bool b2 = program_->addShaderFromSourceCode(QOpenGLShader::TessellationControl, tesselationControlShaderCode);
  bool b3 = program_->addShaderFromSourceCode(QOpenGLShader::TessellationEvaluation, tesselationEvaluationShaderCode);
  bool b4 = program_->addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderCode);
  bool b5 = program_->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderCode);
  bool b6 = program_->link();
  bool b7 = program_->bind();

  innerTessellationLevelUniformLocation_ = program_->uniformLocation("innerTessellationLevel");
  outerTessellationLevelUniformLocation_ = program_->uniformLocation("outerTessellationLevel");

  projectionMatrixUniformLocation_ = program_->uniformLocation("Projection");
  viewMatrixUniformLocation_ = program_->uniformLocation("Modelview");

  lightPositionUniformLocation_ = program_->uniformLocation("lightPosition");
  diffuseMaterialUniformLocation_ = program_->uniformLocation("diffuseMaterial");
  ambientMaterialUniformLocation_ = program_->uniformLocation("ambientMaterial");


  numberOfIndices_ = 3 * boost::numeric_cast<int>(p_->GetNumFaces()); //depending on whether this is a triangle mesh
  positionDataBuffer_.create();

  gl_->glGenBuffers(1, &indexBuffer_);

  innerTessellationLevel_ = 1;
  outerTessellationLevel_ = 1;
}

void MSVPolyhedronRenderer::render() {

  /*static int indexArray[] = {
    2, 1, 0,
    3, 2, 0,
    4, 3, 0,
    5, 4, 0,
    1, 5, 0,
    11, 6, 7,
    11, 7, 8,
    11, 8, 9,
    11, 9, 10,
    11, 10, 6,
    1, 2, 6,
    2, 3, 7,
    3, 4, 8,
    4, 5, 9,
    5, 1, 10,
    2, 7, 6,
    3, 8, 7,
    4, 9, 8,
    5, 10, 9,
    1, 6, 10 };

  static float positionArray[] = {
    0.000f*1000.0f, 0.000f*1000.0f, 1.000f*1000.0f,
    0.894f*1000.0f, 0.000f*1000.0f, 0.447f*1000.0f,
    0.276f*1000.0f, 0.851f*1000.0f, 0.447f*1000.0f,
    -0.724f*1000.0f, 0.526f*1000.0f, 0.447f*1000.0f,
    -0.724f*1000.0f, -0.526f*1000.0f, 0.447f*1000.0f,
    0.276f*1000.0f, -0.851f*1000.0f, 0.447f*1000.0f,
    0.724f*1000.0f, 0.526f*1000.0f, -0.447f*1000.0f,
    -0.276f*1000.0f, 0.851f*1000.0f, -0.447f*1000.0f,
    -0.894f*1000.0f, 0.000f*1000.0f, -0.447f*1000.0f,
    -0.276f*1000.0f, -0.851f*1000.0f, -0.447f*1000.0f,
    0.724f*1000.0f, -0.526f*1000.0f, -0.447f*1000.0f,
    0.000f*1000.0f, 0.000f*1000.0f, -1.000f*1000.0f };

  numberOfIndices_ = 60;*/


  program_->bind();
  vertexArrayObject_.bind();

  // set uniform matrices

  program_->setUniformValue(projectionMatrixUniformLocation_, QMatrix4x4(SAMSON::getActiveCamera()->getProjectionMatrixTranspose()));
  program_->setUniformValue(viewMatrixUniformLocation_, QMatrix4x4(SAMSON::getActiveCamera()->getViewMatrixTranspose()));

  setUniformVector3(lightPositionUniformLocation_, SBVector3(-1, 1, 1.2));
  setUniformVector3(diffuseMaterialUniformLocation_, SBVector3(1, 1, 0));
  setUniformVector3(ambientMaterialUniformLocation_, SBVector3(0.1, 0.1, 0.1));

  program_->setUniformValue(innerTessellationLevelUniformLocation_, innerTessellationLevel_);
  program_->setUniformValue(outerTessellationLevelUniformLocation_, outerTessellationLevel_);

  program_->enableAttributeArray("position");

  positionDataBuffer_.bind();
  positionDataBuffer_.setUsagePattern(QOpenGLBuffer::DynamicDraw);
  positionDataBuffer_.allocate(positionArray_, 3 * sizeof(float) * boost::numeric_cast<int>(p_->GetNumVertices()));
  program_->setAttributeBuffer("position", GL_FLOAT, 0, 3);
  positionDataBuffer_.release();

  gl_->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
  gl_->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*numberOfIndices_, indexArray_, GL_DYNAMIC_DRAW);

  gl_->glPatchParameteri(GL_PATCH_VERTICES, 3);
  gl_->glDrawElements(GL_PATCHES, numberOfIndices_, GL_UNSIGNED_INT, 0);

  program_->disableAttributeArray("position");

  gl_->glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl_->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  vertexArrayObject_.release();
  program_->release();
}
