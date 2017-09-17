#ifndef _AW_VERTEX_PROGRAM_H
#define _AW_VERTEX_PROGRAM_H


//#ifdef GL_NV_vertex_program
class AWVertexProgramBase
{
public:
	AWVertexProgramBase(GLenum tgt) 
		: m_target(tgt), m_valid(FALSE) {}
	
	virtual  ~AWVertexProgramBase()
	{ destroy(); }

   static AWBoolean initFnPointers();

   virtual AWBoolean getEnabled()=0;
   virtual void      setEnabled(AWBoolean v=TRUE)=0;

   void     enable()       { if (!getEnabled()) {glEnable(m_target);setEnabled();};}
   void     disable()      { if (getEnabled()) {glDisable(m_target);setEnabled(FALSE);}; }
	void     trackMatrix(int start, int matrix, int mapping=GL_IDENTITY_NV)
                  {glTrackMatrixNV(GL_VERTEX_PROGRAM_NV, start, matrix, mapping);}

   void     parameter(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
                  {glProgramParameter4fNV( GL_VERTEX_PROGRAM_NV, index, x, y, z, w );}
   void     parameter(GLuint index, const GLfloat *v)
                  {glProgramParameter4fvNV( GL_VERTEX_PROGRAM_NV, index, v);}
   void     parameters(GLuint index, GLsizei count, const GLfloat* v)
                  {glProgramParameters4fvNV(GL_VERTEX_PROGRAM_NV, index, count, v);}

   void     parameter(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
                  {glProgramParameter4dNV( GL_VERTEX_PROGRAM_NV, index, x, y, z, w );}
   void     parameter(GLuint index, const GLdouble *v)
                  {glProgramParameter4dvNV( GL_VERTEX_PROGRAM_NV, index, v);}
   void     parameters(GLuint index, GLsizei count, const GLdouble* v)
                  {glProgramParameters4dvNV(GL_VERTEX_PROGRAM_NV, index, count, v);}

	void     bind()
	{ if(!m_valid) gen(); glBindProgramNV(m_target, m_program); }
	
	void     unbind()
	{ glBindProgramNV(m_target, 0); }
		
	void     load(GLuint size, const GLubyte * prog_text)
	{
		if(!m_valid) gen();
		glLoadProgramNV(m_target, m_program, size, prog_text);
		GLint errpos;
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
		if (errpos != -1)
		{
			fprintf(stderr, "program error:\n");
			int bgn = errpos - 10;
			bgn < 0 ? 0 : bgn;
			const char * c = (const char *)(prog_text + bgn);
			for(int i = 0; i < 30; i++)
			{
				if(bgn+i >= int(size-1))
					break;
				fprintf(stderr, "%c", *c++);
			}
			fprintf(stderr, "\n");
		}
	}//void load(GLuint size, const GLubyte * prog_text)

	void     load(GLuint size, const char * prog_text)
	{
		if(!m_valid) gen();
		glLoadProgramNV(m_target, m_program, size, (const GLubyte *) prog_text);
		GLint errpos;
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
		if (errpos != -1)
		{
			fprintf(stderr, "program error:\n");
			int bgn = errpos - 10;
			bgn < 0 ? 0 : bgn;
			const char * c = (const char *)(prog_text + bgn);
			for(int i = 0; i < 30; i++)
			{
				if(bgn+i >= int(size-1))
					break;
				fprintf(stderr, "%c", *c++);
			}
			fprintf(stderr, "\n");
		}//if (errpos != -1)
	}//void load(GLuint size, const char * prog_text)

	void     load(const char* prog_text)
	{ load(strlen(prog_text), prog_text); }


	void     destroy()
	{ if(m_valid) glDeleteProgramsNV(1, &m_program); m_valid = FALSE;}
	
	AWBoolean     isValid() const { return m_valid; }
	
private:	
	void     gen() { glGenProgramsNV(1, &m_program); m_valid=TRUE;}
	
	AWBoolean   m_valid;
	GLenum      m_target;
	GLuint      m_program;
   
public:
   static PFNGLAREPROGRAMSRESIDENTNVPROC glAreProgramsResidentNV ;
   static PFNGLBINDPROGRAMNVPROC glBindProgramNV ;
   static PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV ;
   static PFNGLEXECUTEPROGRAMNVPROC glExecuteProgramNV ;
   static PFNGLGENPROGRAMSNVPROC glGenProgramsNV ;
   static PFNGLGETPROGRAMPARAMETERDVNVPROC glGetProgramParameterdvNV ;
   static PFNGLGETPROGRAMPARAMETERFVNVPROC glGetProgramParameterfvNV ;
   static PFNGLGETPROGRAMIVNVPROC glGetProgramivNV ;
   static PFNGLGETPROGRAMSTRINGNVPROC glGetProgramStringNV ;
   static PFNGLGETTRACKMATRIXIVNVPROC glGetTrackMatrixivNV ;
   static PFNGLGETVERTEXATTRIBDVNVPROC glGetVertexAttribdvNV ;
   static PFNGLGETVERTEXATTRIBFVNVPROC glGetVertexAttribfvNV ;
   static PFNGLGETVERTEXATTRIBIVNVPROC glGetVertexAttribivNV ;
   static PFNGLGETVERTEXATTRIBPOINTERVNVPROC glGetVertexAttribPointervNV ;
   static PFNGLISPROGRAMNVPROC glIsProgramNV ;
   static PFNGLLOADPROGRAMNVPROC glLoadProgramNV ;
   static PFNGLPROGRAMPARAMETER4DNVPROC glProgramParameter4dNV ;
   static PFNGLPROGRAMPARAMETER4DVNVPROC glProgramParameter4dvNV ;
   static PFNGLPROGRAMPARAMETER4FNVPROC glProgramParameter4fNV ;
   static PFNGLPROGRAMPARAMETER4FVNVPROC glProgramParameter4fvNV ;
   static PFNGLPROGRAMPARAMETERS4DVNVPROC glProgramParameters4dvNV ;
   static PFNGLPROGRAMPARAMETERS4FVNVPROC glProgramParameters4fvNV ;
   static PFNGLREQUESTRESIDENTPROGRAMSNVPROC glRequestResidentProgramsNV ;
   static PFNGLTRACKMATRIXNVPROC glTrackMatrixNV ;
   static PFNGLVERTEXATTRIBPOINTERNVPROC glVertexAttribPointerNV ;
   static PFNGLVERTEXATTRIB1DNVPROC glVertexAttrib1dNV ;
   static PFNGLVERTEXATTRIB1DVNVPROC glVertexAttrib1dvNV ;
   static PFNGLVERTEXATTRIB1FNVPROC glVertexAttrib1fNV ;
   static PFNGLVERTEXATTRIB1FVNVPROC glVertexAttrib1fvNV ;
   static PFNGLVERTEXATTRIB1SNVPROC glVertexAttrib1sNV ;
   static PFNGLVERTEXATTRIB1SVNVPROC glVertexAttrib1svNV ;
   static PFNGLVERTEXATTRIB2DNVPROC glVertexAttrib2dNV ;
   static PFNGLVERTEXATTRIB2DVNVPROC glVertexAttrib2dvNV ;
   static PFNGLVERTEXATTRIB2FNVPROC glVertexAttrib2fNV ;
   static PFNGLVERTEXATTRIB2FVNVPROC glVertexAttrib2fvNV ;
   static PFNGLVERTEXATTRIB2SNVPROC glVertexAttrib2sNV ;
   static PFNGLVERTEXATTRIB2SVNVPROC glVertexAttrib2svNV ;
   static PFNGLVERTEXATTRIB3DNVPROC glVertexAttrib3dNV ;
   static PFNGLVERTEXATTRIB3DVNVPROC glVertexAttrib3dvNV ;
   static PFNGLVERTEXATTRIB3FNVPROC glVertexAttrib3fNV ;
   static PFNGLVERTEXATTRIB3FVNVPROC glVertexAttrib3fvNV ;
   static PFNGLVERTEXATTRIB3SNVPROC glVertexAttrib3sNV ;
   static PFNGLVERTEXATTRIB3SVNVPROC glVertexAttrib3svNV ;
   static PFNGLVERTEXATTRIB4DNVPROC glVertexAttrib4dNV ;
   static PFNGLVERTEXATTRIB4DVNVPROC glVertexAttrib4dvNV ;
   static PFNGLVERTEXATTRIB4FNVPROC glVertexAttrib4fNV ;
   static PFNGLVERTEXATTRIB4FVNVPROC glVertexAttrib4fvNV ;
   static PFNGLVERTEXATTRIB4SNVPROC glVertexAttrib4sNV ;
   static PFNGLVERTEXATTRIB4SVNVPROC glVertexAttrib4svNV ;
   static PFNGLVERTEXATTRIB4UBVNVPROC glVertexAttrib4ubvNV ;
   static PFNGLVERTEXATTRIBS1DVNVPROC glVertexAttribs1dvNV ;
   static PFNGLVERTEXATTRIBS1FVNVPROC glVertexAttribs1fvNV ;
   static PFNGLVERTEXATTRIBS1SVNVPROC glVertexAttribs1svNV ;
   static PFNGLVERTEXATTRIBS2DVNVPROC glVertexAttribs2dvNV ;
   static PFNGLVERTEXATTRIBS2FVNVPROC glVertexAttribs2fvNV ;
   static PFNGLVERTEXATTRIBS2SVNVPROC glVertexAttribs2svNV ;
   static PFNGLVERTEXATTRIBS3DVNVPROC glVertexAttribs3dvNV ;
   static PFNGLVERTEXATTRIBS3FVNVPROC glVertexAttribs3fvNV ;
   static PFNGLVERTEXATTRIBS3SVNVPROC glVertexAttribs3svNV ;
   static PFNGLVERTEXATTRIBS4DVNVPROC glVertexAttribs4dvNV ;
   static PFNGLVERTEXATTRIBS4FVNVPROC glVertexAttribs4fvNV ;
   static PFNGLVERTEXATTRIBS4SVNVPROC glVertexAttribs4svNV ;
   static PFNGLVERTEXATTRIBS4UBVNVPROC glVertexAttribs4ubvNV ;
};//class AWVertexProgramBase



class AWVertexProgram : public AWVertexProgramBase
{
public:
	AWVertexProgram() 
		: AWVertexProgramBase(GL_VERTEX_PROGRAM_NV) {}
   virtual AWBoolean getEnabled()            {return m_enabled;}
   virtual void      setEnabled(AWBoolean v=TRUE) {m_enabled=v;}
protected:
   static AWBoolean m_enabled;
};//class AWVertexProgram : public AWVertexProgramBase		



class AWVertexStateProgram : public AWVertexProgramBase
{
public:
	AWVertexStateProgram() 
		: AWVertexProgramBase(GL_VERTEX_STATE_PROGRAM_NV) {}
   virtual AWBoolean getEnabled()            {return m_enabled;}
   virtual void      setEnabled(AWBoolean v) {m_enabled=v;}
protected:
   static AWBoolean m_enabled;
};//class AWVertexStateProgram : public AWVertexProgramBase



class AWLazyLoadVertexProgram : public AWVertexProgramBase
{
	public:
	AWLazyLoadVertexProgram(void (*vp_loader)() = 0) 
		: AWVertexProgramBase(GL_VERTEX_PROGRAM_NV), m_needsLoad(TRUE), 
        m_loader(vp_loader) {}

	void bind()
	{
		AWVertexProgramBase::bind();
		if(m_needsLoad && m_loader)
		{
			(*m_loader)();
			m_needsLoad = FALSE;
		}
	}//void bind()

	void reload() { m_needsLoad = TRUE; }

	private:
		AWBoolean m_needsLoad;
		void (*m_loader)();
};//class AWLazyLoadVertexProgram : public AWVertexProgramBase		


//#endif // GL_NV_vertex_program

#endif //sentinel _AW_VERTEX_PROGRAM_H