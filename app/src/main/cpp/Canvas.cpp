#include "Canvas.h"
#include <GLES3/gl3.h>
#include <android/log.h>
#include <vector>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Canvas", __VA_ARGS__)
namespace Canvas {
    static GLuint prog=0, vao=0, vbo=0;
    static const char* VS =
        "#version 300 es\n"
        "layout(location=0) in vec2 a_pos;"
        "layout(location=1) in vec4 a_col;"
        "out vec4 v_col; uniform vec2 u_res;"
        "void main(){ vec2 p = a_pos / u_res * 2.0 - 1.0;"
        "gl_Position = vec4(p.x, -p.y, 0.0, 1.0); v_col = a_col; }";
    static const char* FS =
        "#version 300 es\nprecision mediump float;\n"
        "in vec4 v_col; out vec4 o; void main(){ o = v_col; }";
    static GLuint compile(GLenum t, const char* s){
        GLuint h=glCreateShader(t); glShaderSource(h,1,&s,nullptr);
        glCompileShader(h); GLint ok; glGetShaderiv(h,GL_COMPILE_STATUS,&ok);
        if(!ok) LOGI("shader fail"); return h; }
    void Init(){
        if(prog) return;
        GLuint vs=compile(GL_VERTEX_SHADER,VS);
        GLuint fs=compile(GL_FRAGMENT_SHADER,FS);
        prog=glCreateProgram(); glAttachShader(prog,vs); glAttachShader(prog,fs);
        glLinkProgram(prog); glDeleteShader(vs); glDeleteShader(fs);
        glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo); }
    struct V{float x,y,r,g,b,a;};
    static void flush(const std::vector<V>& v, GLenum m){
        if(v.empty()||!prog) return;
        glUseProgram(prog); glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(V),v.data(),GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(V),(void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,sizeof(V),(void*)(2*sizeof(float)));
        glDrawArrays(m,0,(GLsizei)v.size());
        glDisableVertexAttribArray(0); glDisableVertexAttribArray(1); }
    static void up(uint32_t c,float&r,float&g,float&b,float&a){
        a=((c>>24)&0xFF)/255.f; r=((c>>16)&0xFF)/255.f;
        g=((c>>8)&0xFF)/255.f;  b=(c&0xFF)/255.f; }
    void DrawLine(float x1,float y1,float x2,float y2,float th,uint32_t c){
        float r,g,b,a; up(c,r,g,b,a);
        std::vector<V> v={{x1,y1,r,g,b,a},{x2,y2,r,g,b,a}};
        glLineWidth(th); flush(v,GL_LINES); }
    void DrawBox(float x,float y,float w,float h,float th,uint32_t c){
        DrawLine(x,y,x+w,y,th,c); DrawLine(x+w,y,x+w,y+h,th,c);
        DrawLine(x+w,y+h,x,y+h,th,c); DrawLine(x,y+h,x,y,th,c); }
    void DrawText(float,float,const char*,uint32_t,float){}
}
