#include "GLLoader.hpp"
#include <windows.h>

typedef void* (*loadProc)(const char* name);

static int openGLLib(void);
static void closeGLLib(void);
static int LoadOnlyNeededGLFuncs(loadProc load);
static void* LoadGLFunc(const char* namez);

bool_t GLLoader::LoadGL(void)
{
    bool_t status = false;

    if (openGLLib())
    {
        status = LoadOnlyNeededGLFuncs(&LoadGLFunc);
        closeGLLib();
    }

    return status;
}

// INTERNAL API
static HMODULE libGL;

typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNWGLGETPROCADDRESSPROC_PRIVATE getProcAddressPtr;

// OpenGL 1.0
PFNGLCULLFACEPROC glCullFace = NULL;
PFNGLFRONTFACEPROC glFrontFace = NULL;
PFNGLPOINTSIZEPROC glPointSize = NULL;
PFNGLPOLYGONMODEPROC glPolygonMode = NULL;
PFNGLSCISSORPROC glScissor = NULL;
PFNGLCLEARPROC glClear = NULL;
PFNGLCLEARCOLORPROC glClearColor = NULL;
PFNGLCLEARSTENCILPROC glClearStencil = NULL;
PFNGLCLEARDEPTHPROC glClearDepth = NULL;
PFNGLSTENCILMASKPROC glStencilMask = NULL;
PFNGLCOLORMASKPROC glColorMask = NULL;
PFNGLDEPTHMASKPROC glDepthMask = NULL;
PFNGLDISABLEPROC glDisable = NULL;
PFNGLDISABLEPROC glEnable = NULL;
PFNGLSTENCILFUNCPROC glStencilFunc = NULL;
PFNGLSTENCILOPPROC glStencilOp = NULL;
PFNGLDEPTHFUNCPROC glDepthFunc = NULL;
PFNGLGETERRORPROC glGetError = NULL;
PFNGLGETFLOATVPROC glGetFloatv = NULL;
PFNGLGETINTEGERVPROC glGetIntegerv = NULL;
PFNGLGETSTRINGPROC glGetString = NULL;
PFNGLDEPTHRANGEPROC glDepthRange = NULL;
PFNGLVIEWPORTPROC glViewport = NULL;

// OpenGL 1.1
PFNGLDRAWARRAYSPROC glDrawArrays = NULL;
PFNGLDRAWELEMENTSPROC glDrawElements = NULL;
PFNGLPOLYGONOFFSETPROC glPolygonOffset = NULL;
PFNGLBINDTEXTUREPROC glBindTexture = NULL;
PFNGLDELETETEXTURESPROC glDeleteTextures = NULL;

// OpenGL 1.2
// OpenGL 1.3

// OpenGL 1.4
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;
PFNGLBLENDCOLORPROC glBlendColor = NULL;
PFNGLBLENDEQUATIONPROC glBlendEquation = NULL;

// OpenGL 1.5
PFNGLDELETEQUERIESPROC glDeleteQueries = NULL;
PFNGLBEGINQUERYPROC glBeginQuery = NULL;
PFNGLENDQUERYPROC glEndQuery = NULL;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;

// OpenGL 2.0
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDETACHSHADERPROC glDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;

// OpenGL 2.1

// OpenGL 3.0
PFNGLBINDBUFFERBASEPROC glBindBufferBase = NULL;
PFNGLGETSTRINGIPROC glGetStringi = NULL;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;

// OpenGL 3.1
// OpenGL 3.2

// OpenGL 3.3
PFNGLDELETESAMPLERSPROC glDeleteSamplers = NULL;
PFNGLBINDSAMPLERPROC glBindSampler = NULL;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = NULL;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = NULL;

// OpenGL 4.0

// OpenGL 4.1
PFNGLSHADERBINARYPROC glShaderBinary = NULL;
PFNGLDEPTHRANGEFPROC glDepthRangef = NULL;
PFNGLCLEARDEPTHFPROC glClearDepthf = NULL;

// OpenGL 4.2

// OpenGL 4.3
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = NULL;

// OpenGL 4.4

// OpenGL 4.5
PFNGLCREATEBUFFERSPROC glCreateBuffers = NULL;
PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage = NULL;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData = NULL;
PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange = NULL;
PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer = NULL;
PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers = NULL;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer = NULL;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture = NULL;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus = NULL;
PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers = NULL;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage = NULL;
PFNGLCREATETEXTURESPROC glCreateTextures = NULL;
PFNGLTEXTURESTORAGE1DPROC glTextureStorage1D = NULL;
PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D = NULL;
PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D = NULL;
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample = NULL;
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample = NULL;
PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D = NULL;
PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D = NULL;
PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D = NULL;
PFNGLTEXTUREPARAMETERFPROC glTextureParameterf = NULL;
PFNGLGENERATETEXTUREMIPMAPPROC glGenerateTextureMipmap = NULL;
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit = NULL;
PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays = NULL;
PFNGLCREATESAMPLERSPROC glCreateSamplers = NULL;
PFNGLCREATEQUERIESPROC glCreateQueries = NULL;

// OpenGL 4.6
PFNGLSPECIALIZESHADERPROC glSpecializeShader = NULL;


static int openGLLib(void)
{
    libGL = ::LoadLibraryW(L"opengl32.dll");
    if (libGL != NULL)
    {
        void (*tmp)(void);
        tmp = (void(*)(void)) ::GetProcAddress(libGL, "wglGetProcAddress");
        getProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE)tmp;
        return getProcAddressPtr != NULL;
    }

    return 0;
}

static void closeGLLib(void)
{
    if (libGL != NULL)
    {
        ::FreeLibrary((HMODULE)libGL);
        libGL = NULL;
    }
}

static void* LoadGLFunc(const char* namez)
{
    void* result = NULL;
    if (libGL == NULL) return NULL;

    if (getProcAddressPtr != NULL)
    {
        result = getProcAddressPtr(namez);
    }

    if (result == NULL) {
        result = (void*)::GetProcAddress((HMODULE)libGL, namez);
    }

    return result;
}

static void load_GL_VERSION_1_0(loadProc load);
static void load_GL_VERSION_1_1(loadProc load);
static void load_GL_VERSION_1_2(loadProc load);
static void load_GL_VERSION_1_3(loadProc load);
static void load_GL_VERSION_1_4(loadProc load);
static void load_GL_VERSION_1_5(loadProc load);
static void load_GL_VERSION_2_0(loadProc load);
static void load_GL_VERSION_2_1(loadProc load);
static void load_GL_VERSION_3_0(loadProc load);
static void load_GL_VERSION_3_1(loadProc load);
static void load_GL_VERSION_3_2(loadProc load);
static void load_GL_VERSION_3_3(loadProc load);
static void load_GL_VERSION_4_0(loadProc load);
static void load_GL_VERSION_4_1(loadProc load);
static void load_GL_VERSION_4_2(loadProc load);
static void load_GL_VERSION_4_3(loadProc load);
static void load_GL_VERSION_4_4(loadProc load);
static void load_GL_VERSION_4_5(loadProc load);
static void load_GL_VERSION_4_6(loadProc load);

static int LoadOnlyNeededGLFuncs(loadProc load)
{
    load_GL_VERSION_1_0(load);
    load_GL_VERSION_1_1(load);
    load_GL_VERSION_1_2(load);
    load_GL_VERSION_1_3(load);
    load_GL_VERSION_1_4(load);
    load_GL_VERSION_1_5(load);
    load_GL_VERSION_2_0(load);
    load_GL_VERSION_2_1(load);
    load_GL_VERSION_3_0(load);
    load_GL_VERSION_3_1(load);
    load_GL_VERSION_3_2(load);
    load_GL_VERSION_3_3(load);
    load_GL_VERSION_4_0(load);
    load_GL_VERSION_4_1(load);
    load_GL_VERSION_4_2(load);
    load_GL_VERSION_4_3(load);
    load_GL_VERSION_4_4(load);
    load_GL_VERSION_4_5(load);
    load_GL_VERSION_4_6(load);

    // TODO - add support for gl extensions

    return 1;
}

static void load_GL_VERSION_1_0(loadProc load) 
{
    glCullFace = (PFNGLCULLFACEPROC)load("glCullFace");
    glFrontFace = (PFNGLFRONTFACEPROC)load("glFrontFace");
    glPointSize = (PFNGLPOINTSIZEPROC)load("glPointSize");
    glPolygonMode = (PFNGLPOLYGONMODEPROC)load("glPolygonMode");
    glScissor = (PFNGLSCISSORPROC)load("glScissor");
    glClear = (PFNGLCLEARPROC)load("glClear");
    glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
    glClearStencil = (PFNGLCLEARSTENCILPROC)load("glClearStencil");
    glClearDepth = (PFNGLCLEARDEPTHPROC)load("glClearDepth");
    glStencilMask = (PFNGLSTENCILMASKPROC)load("glStencilMask");
    glColorMask = (PFNGLCOLORMASKPROC)load("glColorMask");
    glDepthMask = (PFNGLDEPTHMASKPROC)load("glDepthMask");
    glDisable = (PFNGLDISABLEPROC)load("glDisable");
    glEnable = (PFNGLENABLEPROC)load("glEnable");
    glStencilFunc = (PFNGLSTENCILFUNCPROC)load("glStencilFunc");
    glStencilOp = (PFNGLSTENCILOPPROC)load("glStencilOp");
    glDepthFunc = (PFNGLDEPTHFUNCPROC)load("glDepthFunc");
    glGetError = (PFNGLGETERRORPROC)load("glGetError");
    glGetFloatv = (PFNGLGETFLOATVPROC)load("glGetFloatv");
    glGetIntegerv = (PFNGLGETINTEGERVPROC)load("glGetIntegerv");
    glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
    glDepthRange = (PFNGLDEPTHRANGEPROC)load("glDepthRange");
    glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
}

static void load_GL_VERSION_1_1(loadProc load)
{
    glDrawArrays = (PFNGLDRAWARRAYSPROC)load("glDrawArrays");
    glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements");
    glPolygonOffset = (PFNGLPOLYGONOFFSETPROC)load("glPolygonOffset");
    glBindTexture = (PFNGLBINDTEXTUREPROC)load("glBindTexture");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC)load("glDeleteTextures");
}

static void load_GL_VERSION_1_2(loadProc load)
{
    //
}

static void load_GL_VERSION_1_3(loadProc load)
{
    //
}

static void load_GL_VERSION_1_4(loadProc load)
{
    glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)load("glBlendFuncSeparate");
    glBlendColor = (PFNGLBLENDCOLORPROC)load("glBlendColor");
    glBlendEquation = (PFNGLBLENDEQUATIONPROC)load("glBlendEquation");
}

static void load_GL_VERSION_1_5(loadProc load)
{
    glDeleteQueries = (PFNGLDELETEQUERIESPROC)load("glDeleteQueries");
    glBeginQuery = (PFNGLBEGINQUERYPROC)load("glBeginQuery");
    glEndQuery = (PFNGLENDQUERYPROC)load("glEndQuery");
    glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)load("glGetQueryObjectiv");
    glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers");
}

static void load_GL_VERSION_2_0(loadProc load)
{
    glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)load("glBlendEquationSeparate");
    glAttachShader = (PFNGLATTACHSHADERPROC)load("glAttachShader");
    glCompileShader = (PFNGLCOMPILESHADERPROC)load("glCompileShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)load("glCreateProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)load("glCreateShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load("glDeleteProgram");
    glDeleteShader = (PFNGLDELETESHADERPROC)load("glDeleteShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)load("glDetachShader");
    glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)load("glDisableVertexAttribArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load("glEnableVertexAttribArray");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load("glGetProgramInfoLog");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)load("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load("glGetShaderInfoLog");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)load("glLinkProgram");
    glShaderSource = (PFNGLSHADERSOURCEPROC)load("glShaderSource");
    glUseProgram = (PFNGLUSEPROGRAMPROC)load("glUseProgram");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load("glVertexAttribPointer");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load("glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)load("glUniform1i");
}

static void load_GL_VERSION_2_1(loadProc load)
{
    //
}

static void load_GL_VERSION_3_0(loadProc load)
{
    glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase");
    glGetStringi = (PFNGLGETSTRINGIPROC)load("glGetStringi");
    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)load("glBindRenderbuffer");
    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)load("glDeleteRenderbuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)load("glBindFramebuffer");
    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)load("glDeleteFramebuffers");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays");
}

static void load_GL_VERSION_3_1(loadProc load)
{
    //
}

static void load_GL_VERSION_3_2(loadProc load)
{
    //
}

static void load_GL_VERSION_3_3(loadProc load)
{
    glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)load("glDeleteSamplers");
    glBindSampler = (PFNGLBINDSAMPLERPROC)load("glBindSampler");
    glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)load("glSamplerParameteri");
    glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)load("glSamplerParameterf");
}

static void load_GL_VERSION_4_0(loadProc load)
{
    //
}

static void load_GL_VERSION_4_1(loadProc load)
{
    glShaderBinary = (PFNGLSHADERBINARYPROC)load("glShaderBinary");
    glDepthRangef = (PFNGLDEPTHRANGEFPROC)load("glDepthRangef");
    glClearDepthf = (PFNGLCLEARDEPTHFPROC)load("glClearDepthf");
}

static void load_GL_VERSION_4_2(loadProc load)
{
    //
}

static void load_GL_VERSION_4_3(loadProc load)
{
    glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)load("glDebugMessageCallback");
}

static void load_GL_VERSION_4_4(loadProc load)
{
    //
}

static void load_GL_VERSION_4_5(loadProc load)
{
    glCreateBuffers = (PFNGLCREATEBUFFERSPROC)load("glCreateBuffers");
    glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)load("glNamedBufferStorage");
    glCopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC)load("glCopyNamedBufferSubData");
    glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)load("glMapNamedBufferRange");
    glUnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)load("glUnmapNamedBuffer");
    glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)load("glCreateFramebuffers");
    glNamedFramebufferRenderbuffer = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC)load("glNamedFramebufferRenderbuffer");
    glNamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)load("glNamedFramebufferTexture");
    glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)load("glCheckNamedFramebufferStatus");
    glCreateRenderbuffers = (PFNGLCREATERENDERBUFFERSPROC)load("glCreateRenderbuffers");
    glNamedRenderbufferStorage = (PFNGLNAMEDRENDERBUFFERSTORAGEPROC)load("glNamedRenderbufferStorage");
    glCreateTextures = (PFNGLCREATETEXTURESPROC)load("glCreateTextures");
    glTextureStorage1D = (PFNGLTEXTURESTORAGE1DPROC)load("glTextureStorage1D");
    glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)load("glTextureStorage2D");
    glTextureStorage3D = (PFNGLTEXTURESTORAGE3DPROC)load("glTextureStorage3D");
    glTextureStorage2DMultisample = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)load("glTextureStorage2DMultisample");
    glTextureStorage3DMultisample = (PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)load("glTextureStorage3DMultisample");
    glTextureSubImage1D = (PFNGLTEXTURESUBIMAGE1DPROC)load("glTextureSubImage1D");
    glTextureSubImage2D = (PFNGLTEXTURESUBIMAGE2DPROC)load("glTextureSubImage2D");
    glTextureSubImage3D = (PFNGLTEXTURESUBIMAGE3DPROC)load("glTextureSubImage3D");
    glTextureParameterf = (PFNGLTEXTUREPARAMETERFPROC)load("glTextureParameterf");
    glGenerateTextureMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC)load("glGenerateTextureMipmap");
    glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)load("glBindTextureUnit");
    glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)load("glCreateVertexArrays");
    glCreateSamplers = (PFNGLCREATESAMPLERSPROC)load("glCreateSamplers");
    glCreateQueries = (PFNGLCREATEQUERIESPROC)load("glCreateQueries");
}

static void load_GL_VERSION_4_6(loadProc load)
{
    glSpecializeShader = (PFNGLSPECIALIZESHADERPROC)load("glSpecializeShader");
}