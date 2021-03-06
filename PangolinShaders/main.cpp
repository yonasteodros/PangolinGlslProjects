#include <pangolin/pangolin.h>
#include <pangolin/gl/gldraw.h>
#include <chrono>
#include <pcl/io/png_io.h>
#include <pcl/io/pcd_io.h>
#include <Eigen/Core>


struct FBOTest
{
  FBOTest(int w, int h)
  {
    w_ = w;
    h_ = h;
    vao_ = 0;
    setup();
  }

  ~FBOTest()
  {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteRenderbuffers(1, &depth_);
    glDeleteTextures(1, &rgb_);
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(vbos_.size(), vbos_.data());
  }
  void setup()
  {
      glGenFramebuffers(1, &fbo_);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

      // Color
      glGenTextures(1, &rgb_);
      glBindTexture(GL_TEXTURE_2D, rgb_);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w_, h_, 0, GL_RGB, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rgb_, 0);
      glBindTexture(GL_TEXTURE_2D, 0);

      // Depth
      glGenRenderbuffers(1, &depth_);
      glBindRenderbuffer(GL_RENDERBUFFER, depth_);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w_, h_);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      std::vector<GLenum> drawbuffers = {GL_COLOR_ATTACHMENT0};
      glDrawBuffers(drawbuffers.size(), drawbuffers.data());

      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE){
        if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
          std::cerr<<"ERROR: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"<<std::endl;
        if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
          std::cerr<<"ERROR: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"<<std::endl;
        if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
          std::cerr<<"ERROR: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"<<std::endl;
        if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
          std::cerr<<"ERROR: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"<<std::endl;
        if (status == GL_FRAMEBUFFER_UNSUPPORTED)
          std::cerr<<"ERROR: GL_FRAMEBUFFER_UNSUPPORTED"<<std::endl;
        if (status == GL_FRAMEBUFFER_UNDEFINED)
          std::cerr<<"ERROR: GL_FRAMEBUFFER_UNDEFINED"<<std::endl;
        else
          std::cerr<<"Framebuffer fail, :"<<status<<std::endl;
      }

      std::string fbo_vert ="#version 450 core\n"
        "layout (location = 0) in vec3 position;"
        "layout (location = 1) in vec3 color;"
        "uniform mat4 mvp;"
        "out vec4 vColor;"
        "void main() {"
        "gl_Position = mvp * vec4(position, 1.0);"
        "vColor = vec4(color, 1.0);"
        "}";

      std::string fbo_frag ="#version 450 core\n"
        "in vec4 vColor;"
        "layout (location = 0) out vec4 fColor;"
        "void main() {"
        "fColor = vColor;"
        "}";

      prog_.AddShader(pangolin::GlSlVertexShader, fbo_vert, {}, {});
      prog_.AddShader(pangolin::GlSlFragmentShader, fbo_frag, {}, {});
      prog_.Link();

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void setup_vao(const std::vector<float>& xyz, const std::vector<uint8_t>& rgb)
  {
    count_ = xyz.size();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    vbos_.clear();
    vbos_.resize(2);
    glGenBuffers(2, vbos_.data());

    glBindBuffer(GL_ARRAY_BUFFER, vbos_[0]);
    glBufferData(GL_ARRAY_BUFFER, count_ * sizeof(GLfloat), xyz.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbos_[1]);
    glBufferData(GL_ARRAY_BUFFER, count_ * sizeof(GLbyte), rgb.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void draw()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    prog_.Bind();
    glBindVertexArray(vao_);
    glDrawArrays(GL_POINTS, 0, count_);
    glBindVertexArray(0);
    glFinish();
    prog_.Unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void blit()
  {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, w_, h_, 0, 0, w_, h_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  }

  pangolin::GlSlProgram& get_prog()
  {
    return prog_;
  }

  int w_, h_;
  GLuint vao_, fbo_, rgb_, depth_;
  std::vector<GLuint> vbos_;
  pangolin::GlSlProgram prog_;
  int count_;
};


int main(/*int argc, char* argv[]*/)
{
  int w=640;
  int h=480;
  pangolin::CreateWindowAndBind("Test", w, h);
  FBOTest fbtst(w, h);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glDepthMask(GL_TRUE); // Enable depth test (z-buffer)
  glDepthFunc(GL_LESS); // z buffering
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float fx=525.0;
  float fy=525.0;
  float cx=319.5;
  float cy=239.5;

  float x0=0.0;
  float y0=0.0;
  float skew=0.0;

  float right = w;
  float left = 0.0;
  float top = h;
  float bottom = 0.0;
  float near = 0.1;
  float far = 20.0;

  Eigen::Matrix4f eye = Eigen::Matrix4f::Identity();
  eye(1,1) = -1; // opengl is y-up, opencv y-down
  eye(2,2) = -1; // opengl is z-back, opencv z-forward

  Eigen::Matrix4f frustum2 = Eigen::Matrix4f::Zero();
  frustum2(0,0) = 2.0*fx/w;
  frustum2(0,1) = -2.0*skew/w;
  frustum2(0,2) = (w - 2.0*cx + 2.0*x0)/w;
  frustum2(1,1) = 2.0*fy/h;
  frustum2(1,2) = (-h + 2.0*cy + 2.0*y0)/h;
  frustum2(2,2) = -(far + near)/(far - near);
  frustum2(2,3) = -2.0*far*near/(far - near);
  frustum2(3,2) = -1.f;

  Eigen::Matrix4f mvp2 = frustum2 * eye;



  auto proj = pangolin::ProjectionMatrix(w, h, fx, fy, cx, cy, 0.1, 1000);
  auto mv = pangolin::ModelViewLookAt(0, 0, 0, 0, 0, 1, pangolin::AxisNegY);
  auto cam = pangolin::OpenGlRenderState(proj, mv);


  fbtst.get_prog().Bind();
  fbtst.get_prog().SetUniform("mvp", cam.GetProjectionModelViewMatrix());
  //fbtst.get_prog().SetUniform("mvp", mvp2);
  fbtst.get_prog().Unbind();

  std::cout<<"w "<<w<<" h "<<h<<" fx "<<fx<<" fy "<<fy<<" cx "<<cx<<" cy "<<cy<<std::endl;
  //std::cout<<"Projection mat "<<cam.GetProjectionMatrix()<<std::endl;
  //std::cout<<"ModelView mat "<<cam.GetModelViewMatrix()<<std::endl;
  //std::cout<<"MVP mat "<<cam.GetProjectionModelViewMatrix()<<std::endl;

  std::string cloud_fname = "/home/user/Desktop/circle1.pcd";
  pcl::PointCloud<pcl::PointXYZRGB> pcd;
  pcl::io::loadPCDFile (cloud_fname, pcd);
  std::vector<float> xyz;
  std::vector<uint8_t> rgb;
  for (auto p : pcd.points) {
    xyz.push_back(p.x);
    xyz.push_back(p.y);
    xyz.push_back(p.z);
    rgb.push_back(p.r);
    rgb.push_back(p.g);
    rgb.push_back(p.b);
  }
  fbtst.setup_vao(xyz, rgb);

  while (!pangolin::ShouldQuit()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fbtst.draw();
    fbtst.blit();
    pangolin::FinishFrame();
  }

}
