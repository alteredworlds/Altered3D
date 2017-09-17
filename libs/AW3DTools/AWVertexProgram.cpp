#include "AWGLRenderer.h"
#include "AWVertexProgram.h"

//#ifdef GL_NV_vertex_program
BOOL AWVertexProgram::m_enabled = FALSE;
BOOL AWVertexStateProgram::m_enabled = FALSE;

PFNGLAREPROGRAMSRESIDENTNVPROC AWVertexProgramBase::glAreProgramsResidentNV = NULL;
PFNGLBINDPROGRAMNVPROC AWVertexProgramBase::glBindProgramNV = NULL;
PFNGLDELETEPROGRAMSNVPROC AWVertexProgramBase::glDeleteProgramsNV = NULL;
PFNGLEXECUTEPROGRAMNVPROC AWVertexProgramBase::glExecuteProgramNV = NULL;
PFNGLGENPROGRAMSNVPROC AWVertexProgramBase::glGenProgramsNV = NULL;
PFNGLGETPROGRAMPARAMETERDVNVPROC AWVertexProgramBase::glGetProgramParameterdvNV = NULL;
PFNGLGETPROGRAMPARAMETERFVNVPROC AWVertexProgramBase::glGetProgramParameterfvNV = NULL;
PFNGLGETPROGRAMIVNVPROC AWVertexProgramBase::glGetProgramivNV = NULL;
PFNGLGETPROGRAMSTRINGNVPROC AWVertexProgramBase::glGetProgramStringNV = NULL;
PFNGLGETTRACKMATRIXIVNVPROC AWVertexProgramBase::glGetTrackMatrixivNV = NULL;
PFNGLGETVERTEXATTRIBDVNVPROC AWVertexProgramBase::glGetVertexAttribdvNV = NULL;
PFNGLGETVERTEXATTRIBFVNVPROC AWVertexProgramBase::glGetVertexAttribfvNV = NULL;
PFNGLGETVERTEXATTRIBIVNVPROC AWVertexProgramBase::glGetVertexAttribivNV = NULL;
PFNGLGETVERTEXATTRIBPOINTERVNVPROC AWVertexProgramBase::glGetVertexAttribPointervNV = NULL;
PFNGLISPROGRAMNVPROC AWVertexProgramBase::glIsProgramNV = NULL;
PFNGLLOADPROGRAMNVPROC AWVertexProgramBase::glLoadProgramNV = NULL;
PFNGLPROGRAMPARAMETER4DNVPROC AWVertexProgramBase::glProgramParameter4dNV = NULL;
PFNGLPROGRAMPARAMETER4DVNVPROC AWVertexProgramBase::glProgramParameter4dvNV = NULL;
PFNGLPROGRAMPARAMETER4FNVPROC AWVertexProgramBase::glProgramParameter4fNV = NULL;
PFNGLPROGRAMPARAMETER4FVNVPROC AWVertexProgramBase::glProgramParameter4fvNV = NULL;
PFNGLPROGRAMPARAMETERS4DVNVPROC AWVertexProgramBase::glProgramParameters4dvNV = NULL;
PFNGLPROGRAMPARAMETERS4FVNVPROC AWVertexProgramBase::glProgramParameters4fvNV = NULL;
PFNGLREQUESTRESIDENTPROGRAMSNVPROC AWVertexProgramBase::glRequestResidentProgramsNV = NULL;
PFNGLTRACKMATRIXNVPROC AWVertexProgramBase::glTrackMatrixNV = NULL;
PFNGLVERTEXATTRIBPOINTERNVPROC AWVertexProgramBase::glVertexAttribPointerNV = NULL;
PFNGLVERTEXATTRIB1DNVPROC AWVertexProgramBase::glVertexAttrib1dNV = NULL;
PFNGLVERTEXATTRIB1DVNVPROC AWVertexProgramBase::glVertexAttrib1dvNV = NULL;
PFNGLVERTEXATTRIB1FNVPROC AWVertexProgramBase::glVertexAttrib1fNV = NULL;
PFNGLVERTEXATTRIB1FVNVPROC AWVertexProgramBase::glVertexAttrib1fvNV = NULL;
PFNGLVERTEXATTRIB1SNVPROC AWVertexProgramBase::glVertexAttrib1sNV = NULL;
PFNGLVERTEXATTRIB1SVNVPROC AWVertexProgramBase::glVertexAttrib1svNV = NULL;
PFNGLVERTEXATTRIB2DNVPROC AWVertexProgramBase::glVertexAttrib2dNV = NULL;
PFNGLVERTEXATTRIB2DVNVPROC AWVertexProgramBase::glVertexAttrib2dvNV = NULL;
PFNGLVERTEXATTRIB2FNVPROC AWVertexProgramBase::glVertexAttrib2fNV = NULL;
PFNGLVERTEXATTRIB2FVNVPROC AWVertexProgramBase::glVertexAttrib2fvNV = NULL;
PFNGLVERTEXATTRIB2SNVPROC AWVertexProgramBase::glVertexAttrib2sNV = NULL;
PFNGLVERTEXATTRIB2SVNVPROC AWVertexProgramBase::glVertexAttrib2svNV = NULL;
PFNGLVERTEXATTRIB3DNVPROC AWVertexProgramBase::glVertexAttrib3dNV = NULL;
PFNGLVERTEXATTRIB3DVNVPROC AWVertexProgramBase::glVertexAttrib3dvNV = NULL;
PFNGLVERTEXATTRIB3FNVPROC AWVertexProgramBase::glVertexAttrib3fNV = NULL;
PFNGLVERTEXATTRIB3FVNVPROC AWVertexProgramBase::glVertexAttrib3fvNV = NULL;
PFNGLVERTEXATTRIB3SNVPROC AWVertexProgramBase::glVertexAttrib3sNV = NULL;
PFNGLVERTEXATTRIB3SVNVPROC AWVertexProgramBase::glVertexAttrib3svNV = NULL;
PFNGLVERTEXATTRIB4DNVPROC AWVertexProgramBase::glVertexAttrib4dNV = NULL;
PFNGLVERTEXATTRIB4DVNVPROC AWVertexProgramBase::glVertexAttrib4dvNV = NULL;
PFNGLVERTEXATTRIB4FNVPROC AWVertexProgramBase::glVertexAttrib4fNV = NULL;
PFNGLVERTEXATTRIB4FVNVPROC AWVertexProgramBase::glVertexAttrib4fvNV = NULL;
PFNGLVERTEXATTRIB4SNVPROC AWVertexProgramBase::glVertexAttrib4sNV = NULL;
PFNGLVERTEXATTRIB4SVNVPROC AWVertexProgramBase::glVertexAttrib4svNV = NULL;
PFNGLVERTEXATTRIB4UBVNVPROC AWVertexProgramBase::glVertexAttrib4ubvNV = NULL;
PFNGLVERTEXATTRIBS1DVNVPROC AWVertexProgramBase::glVertexAttribs1dvNV = NULL;
PFNGLVERTEXATTRIBS1FVNVPROC AWVertexProgramBase::glVertexAttribs1fvNV = NULL;
PFNGLVERTEXATTRIBS1SVNVPROC AWVertexProgramBase::glVertexAttribs1svNV = NULL;
PFNGLVERTEXATTRIBS2DVNVPROC AWVertexProgramBase::glVertexAttribs2dvNV = NULL;
PFNGLVERTEXATTRIBS2FVNVPROC AWVertexProgramBase::glVertexAttribs2fvNV = NULL;
PFNGLVERTEXATTRIBS2SVNVPROC AWVertexProgramBase::glVertexAttribs2svNV = NULL;
PFNGLVERTEXATTRIBS3DVNVPROC AWVertexProgramBase::glVertexAttribs3dvNV = NULL;
PFNGLVERTEXATTRIBS3FVNVPROC AWVertexProgramBase::glVertexAttribs3fvNV = NULL;
PFNGLVERTEXATTRIBS3SVNVPROC AWVertexProgramBase::glVertexAttribs3svNV = NULL;
PFNGLVERTEXATTRIBS4DVNVPROC AWVertexProgramBase::glVertexAttribs4dvNV = NULL;
PFNGLVERTEXATTRIBS4FVNVPROC AWVertexProgramBase::glVertexAttribs4fvNV = NULL;
PFNGLVERTEXATTRIBS4SVNVPROC AWVertexProgramBase::glVertexAttribs4svNV = NULL;
PFNGLVERTEXATTRIBS4UBVNVPROC AWVertexProgramBase::glVertexAttribs4ubvNV = NULL;



BOOL
AWVertexProgramBase::initFnPointers()
{
   glAreProgramsResidentNV = (PFNGLAREPROGRAMSRESIDENTNVPROC)GLRenderer::getFunctionByName("glAreProgramsResidentNV");
   glBindProgramNV = (PFNGLBINDPROGRAMNVPROC)GLRenderer::getFunctionByName("glBindProgramNV");
   glDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)GLRenderer::getFunctionByName("glDeleteProgramsNV");
   glExecuteProgramNV = (PFNGLEXECUTEPROGRAMNVPROC)GLRenderer::getFunctionByName("glExecuteProgramNV");
   glGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)GLRenderer::getFunctionByName("glGenProgramsNV");
   glGetProgramParameterdvNV = (PFNGLGETPROGRAMPARAMETERDVNVPROC)GLRenderer::getFunctionByName("glGetProgramParameterdvNV");
   glGetProgramParameterfvNV = (PFNGLGETPROGRAMPARAMETERFVNVPROC)GLRenderer::getFunctionByName("glGetProgramParameterfvNV");
   glGetProgramivNV = (PFNGLGETPROGRAMIVNVPROC)GLRenderer::getFunctionByName("glGetProgramivNV");
   glGetProgramStringNV = (PFNGLGETPROGRAMSTRINGNVPROC)GLRenderer::getFunctionByName("glGetProgramStringNV");
   glGetTrackMatrixivNV = (PFNGLGETTRACKMATRIXIVNVPROC)GLRenderer::getFunctionByName("glGetTrackMatrixivNV");
   glGetVertexAttribdvNV = (PFNGLGETVERTEXATTRIBDVNVPROC)GLRenderer::getFunctionByName("glGetVertexAttribdvNV");
   glGetVertexAttribfvNV = (PFNGLGETVERTEXATTRIBFVNVPROC)GLRenderer::getFunctionByName("glGetVertexAttribfvNV");
   glGetVertexAttribivNV = (PFNGLGETVERTEXATTRIBIVNVPROC)GLRenderer::getFunctionByName("glGetVertexAttribivNV");
   glGetVertexAttribPointervNV = (PFNGLGETVERTEXATTRIBPOINTERVNVPROC)GLRenderer::getFunctionByName("glGetVertexAttribPointervNV");
   glIsProgramNV = (PFNGLISPROGRAMNVPROC)GLRenderer::getFunctionByName("glIsProgramNV");
   glLoadProgramNV = (PFNGLLOADPROGRAMNVPROC)GLRenderer::getFunctionByName("glLoadProgramNV");
   glProgramParameter4dNV = (PFNGLPROGRAMPARAMETER4DNVPROC)GLRenderer::getFunctionByName("glProgramParameter4dNV");
   glProgramParameter4dvNV = (PFNGLPROGRAMPARAMETER4DVNVPROC)GLRenderer::getFunctionByName("glProgramParameter4dvNV");
   glProgramParameter4fNV = (PFNGLPROGRAMPARAMETER4FNVPROC)GLRenderer::getFunctionByName("glProgramParameter4fNV");
   glProgramParameter4fvNV = (PFNGLPROGRAMPARAMETER4FVNVPROC)GLRenderer::getFunctionByName("glProgramParameter4fvNV");
   glProgramParameters4dvNV = (PFNGLPROGRAMPARAMETERS4DVNVPROC)GLRenderer::getFunctionByName("glProgramParameters4dvNV");
   glProgramParameters4fvNV = (PFNGLPROGRAMPARAMETERS4FVNVPROC)GLRenderer::getFunctionByName("glProgramParameters4fvNV");
   glRequestResidentProgramsNV = (PFNGLREQUESTRESIDENTPROGRAMSNVPROC)GLRenderer::getFunctionByName("glRequestResidentProgramsNV");
   glTrackMatrixNV = (PFNGLTRACKMATRIXNVPROC)GLRenderer::getFunctionByName("glTrackMatrixNV");
   glVertexAttribPointerNV = (PFNGLVERTEXATTRIBPOINTERNVPROC)GLRenderer::getFunctionByName("glVertexAttribPointerNV");
   glVertexAttrib1dNV = (PFNGLVERTEXATTRIB1DNVPROC)GLRenderer::getFunctionByName("glVertexAttrib1dNV");
   glVertexAttrib1dvNV = (PFNGLVERTEXATTRIB1DVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib1dvNV");
   glVertexAttrib1fNV = (PFNGLVERTEXATTRIB1FNVPROC)GLRenderer::getFunctionByName("glVertexAttrib1fNV");
   glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib1fvNV");
   glVertexAttrib1sNV = (PFNGLVERTEXATTRIB1SNVPROC)GLRenderer::getFunctionByName("glVertexAttrib1sNV");
   glVertexAttrib1svNV = (PFNGLVERTEXATTRIB1SVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib1svNV");
   glVertexAttrib2dNV = (PFNGLVERTEXATTRIB2DNVPROC)GLRenderer::getFunctionByName("glVertexAttrib2dNV");
   glVertexAttrib2dvNV = (PFNGLVERTEXATTRIB2DVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib2dvNV");
   glVertexAttrib2fNV = (PFNGLVERTEXATTRIB2FNVPROC)GLRenderer::getFunctionByName("glVertexAttrib2fNV");
   glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib2fvNV");
   glVertexAttrib2sNV = (PFNGLVERTEXATTRIB2SNVPROC)GLRenderer::getFunctionByName("glVertexAttrib2sNV");
   glVertexAttrib2svNV = (PFNGLVERTEXATTRIB2SVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib2svNV");
   glVertexAttrib3dNV = (PFNGLVERTEXATTRIB3DNVPROC)GLRenderer::getFunctionByName("glVertexAttrib3dNV");
   glVertexAttrib3dvNV = (PFNGLVERTEXATTRIB3DVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib3dvNV");
   glVertexAttrib3fNV = (PFNGLVERTEXATTRIB3FNVPROC)GLRenderer::getFunctionByName("glVertexAttrib3fNV");
   glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib3fvNV");
   glVertexAttrib3sNV = (PFNGLVERTEXATTRIB3SNVPROC)GLRenderer::getFunctionByName("glVertexAttrib3sNV");
   glVertexAttrib3svNV = (PFNGLVERTEXATTRIB3SVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib3svNV");
   glVertexAttrib4dNV = (PFNGLVERTEXATTRIB4DNVPROC)GLRenderer::getFunctionByName("glVertexAttrib4dNV");
   glVertexAttrib4dvNV = (PFNGLVERTEXATTRIB4DVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib4dvNV");
   glVertexAttrib4fNV = (PFNGLVERTEXATTRIB4FNVPROC)GLRenderer::getFunctionByName("glVertexAttrib4fNV");
   glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib4fvNV");
   glVertexAttrib4sNV = (PFNGLVERTEXATTRIB4SNVPROC)GLRenderer::getFunctionByName("glVertexAttrib4sNV");
   glVertexAttrib4svNV = (PFNGLVERTEXATTRIB4SVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib4svNV");
   glVertexAttrib4ubvNV = (PFNGLVERTEXATTRIB4UBVNVPROC)GLRenderer::getFunctionByName("glVertexAttrib4ubvNV");
   glVertexAttribs1dvNV = (PFNGLVERTEXATTRIBS1DVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs1dvNV");
   glVertexAttribs1fvNV = (PFNGLVERTEXATTRIBS1FVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs1fvNV");
   glVertexAttribs1svNV = (PFNGLVERTEXATTRIBS1SVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs1svNV");
   glVertexAttribs2dvNV = (PFNGLVERTEXATTRIBS2DVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs2dvNV");
   glVertexAttribs2fvNV = (PFNGLVERTEXATTRIBS2FVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs2fvNV");
   glVertexAttribs2svNV = (PFNGLVERTEXATTRIBS2SVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs2svNV");
   glVertexAttribs3dvNV = (PFNGLVERTEXATTRIBS3DVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs3dvNV");
   glVertexAttribs3fvNV = (PFNGLVERTEXATTRIBS3FVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs3fvNV");
   glVertexAttribs3svNV = (PFNGLVERTEXATTRIBS3SVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs3svNV");
   glVertexAttribs4dvNV = (PFNGLVERTEXATTRIBS4DVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs4dvNV");
   glVertexAttribs4fvNV = (PFNGLVERTEXATTRIBS4FVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs4fvNV");
   glVertexAttribs4svNV = (PFNGLVERTEXATTRIBS4SVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs4svNV");
   glVertexAttribs4ubvNV = (PFNGLVERTEXATTRIBS4UBVNVPROC)GLRenderer::getFunctionByName("glVertexAttribs4ubvNV");
   return TRUE;
}//BOOL AWVertexProgramBase::initFnPointers()



//#endif