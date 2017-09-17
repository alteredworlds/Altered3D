#include <math.h>
#include "AWNode.h"

#define VER_AWNODE_1	100
#define VER_AWNODE_2	101
#define VER_AWNODE_3	102
#define VER_AWNODE_4	103
#define VER_AWNODE_5	104


AWDEFINE_VERSIONABLE_PERSISTABLE(AWNode, CLSID_AWNODE, VER_AWNODE_5);


AWNode::AWNode(const AWCString& name, AWNode* parent, double life) :
	m_transformer(NULL),
	m_name(name),
	m_children(3),
	m_parent(parent),
    m_life(life),
    m_animStart(0.0),
	m_ownership(AW_NODE_OWNS_TRANSFORMER | AW_NODE_OWNS_CHILDREN)
	
{
   m_wsTM.identity();
}


AWNode::~AWNode()
{
	if (getOwnsTransformer())
	{
		delete m_transformer;
	}
	m_transformer = NULL;
	//child ownership now dealt with internal to m_children list
	m_children.destroy();
}//AWNode::~AWNode()


void	
AWNode::clearAndDestroy()
{
	if (getOwnsTransformer())
	{
		delete m_transformer;
	}
	m_transformer = NULL;
	//child ownership now dealt with internal to m_children list
	m_children.destroy();
}//void	AWNode::clearAndDestroy()


AWNode::AWNode(const AWNode& other)
 :
	m_transformer(NULL),
	m_parent(NULL),
	m_children(3),
	m_ownership(AW_NODE_OWNS_TRANSFORMER | AW_NODE_OWNS_CHILDREN)

{	//DEEP COPY
	*this=other;
}


void		 	 
AWNode::rotate(const AWQuat& rot)
{
	if (m_transformer)
	{
		m_transformer->rotate(rot);
	}
	rotateChildren(rot);
}//void AWNode::rotate(const AWQuat& rot)


void		 	 
AWNode::rotateChildren(const AWQuat& rot)
{
	//iterate through children and calculate their transforms
	for (int i = 0; i < (int)m_children.getNum(); i++)
	{
		((AWNode*)m_children[i])->rotate(rot);
	}//for (int i = 0; i < m_children.getNum(); i++)
}//void AWNode::rotateChildren(const AWQuat& rot)


int	 	 
AWNode::getDirection(AWPoint& dir)
{
	int retVal = 0;
	if (m_transformer)
	{
		m_transformer->getWSDirection(dir);
		retVal = 1;
	}//if (m_transformer)
	return retVal;
}//int AWNode::getDirection(AWPoint& dir)


int	 	 
AWNode::calcTransform(double t, const AWMatrix4& parentTM)
{
   int retVal = 0;
   m_wsTM = parentTM;
   double useTime = t - m_animStart;
   if (useTime < 0) useTime = 0;
   if (m_transformer)
   {
      retVal = m_transformer->calcTransform(useTime, m_wsTM);
      m_transformer->getWSTM(m_wsTM);
      AWTRACEHI4("Node %s WS posn\t(%f, %f, %f)\n", m_name.c_str(), 
                (double)m_wsTM[12] ,(double)m_wsTM[13], (double)m_wsTM[14]);
   }
   //now iterate through children and calculate their transforms
   for (int i = 0; i < getNumChildren(); i++)
   {
      getChild(i)->calcTransform(useTime, m_wsTM);
   }//for (int i = 0; i < m_children.getNum(); i++)
   return retVal;
}//int AWNode::calcTransform(double t, const AWMatrix4& parentTM)


void	 
AWNode::setTransform(const AWPoint& posn, const AWPoint& scale, 
					   const AWQuat& rotn)
{
	if (m_transformer)
	{
		m_transformer->setTransform(posn, scale, rotn);
	}
}//void AWNode::setTransform(AWPoint& posn, AWPoint& scale, AWQuat& rotn)


AWNode& 
AWNode::operator=(const AWNode& other)
{
	clearAndDestroy();
	//make a full copy of the transformer
	m_transformer	= other.m_transformer?
		(AWTransformController*)other.m_transformer->copy() : NULL;
	setOwnsTransformer(TRUE); //node now owns this new copy
	//parent pointer also copied over
	m_parent		= other.m_parent;
    m_life          = other.m_life;
	//make a full copy of each child. This node will now own these children
	for (int child = 0; child < other.getNumChildren(); child++)
	{
		addChild((AWNode*)other.m_children[child]->copy());
	}
	setOwnsChildren(TRUE);
	m_name			= other.m_name;
	return *this;
}


AWNode& 
AWNode::clone(const AWNode& other)
{	//does a deep copy of the transformer
	clearAndDestroy();
	m_transformer	= (AWTransformController*)other.m_transformer->copy();
	setOwnsTransformer(TRUE); 
	m_name			= other.m_name;
	m_parent        = other.m_parent;
    m_life          = other.m_life; 
	//make a CLONE of each child
	for (int child = 0; child < (int)other.m_children.getNum(); child++)
	{
		addChild(((AWNode*)other.m_children[child])->clone());
	}
	setOwnsChildren(TRUE);
	return *this;
}//AWNode& AWNode::clone(const AWNode& other)


AWNode*  
AWNode::clone() const
{//return a NEW shallow copy of this object
	AWNode* retVal = new AWNode;
	retVal->clone(*this);
	return retVal;
}//AWNode* AWNode::clone() const


void
AWNode::offset(const AWPoint& offset)
{
	if (m_transformer)
	{
		m_transformer->offset(offset);
	}//if (m_transformer)
}//void AWNode::offset(const AWPoint& offset)


void	
AWNode::scale(const AWPoint& scale)
{	//now scale the position keys of any attached
	//transformation controller
	if (m_transformer)
	{
		m_transformer->scale(scale);
	}
    //now scale all this node's CHILDREN
    for (int child=0; child < getNumChildren(); child++)
    {
        getChild(child)->scale(scale);
    }
}//void	AWNode::scale(const AWPoint& scale)


//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWNode
//
//////////////////////////////////////////////////////////
void	
AWNode::restoreGuts(AWFile& file)
{
	clearAndDestroy();
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (version <= VER_AWNODE_5)
	{
		AWPersistable::restoreGuts(file);
		file >> m_name;
		AWTRACE1("loading AWNode - %s\n", m_name.c_str());
		file >> m_transformer;
		file >> m_parent;
		file >> m_ownership;
		if (version <= VER_AWNODE_2)
		{	//something really wierd happened for these file versions
			//luckily we ALWAYS owned this data 
			m_ownership = AW_NODE_OWNS_TRANSFORMER|AW_NODE_OWNS_CHILDREN; 
		}//if (version <= VER_AWNODE_2)
		m_children.read(file, version <= VER_AWNODE_3);
		if (version >= VER_AWNODE_2)
		{
            if (version < VER_AWNODE_5)
            {
                float tmp;
                file >> tmp;
                m_life = (double)tmp;
            }
			else
                file >> m_life;
		}//if (version >= VER_AWNODE_2)
		else
            m_life = AW_INFINITE_LIFE;
	}//if (version <= VER_AWNODE_5)
}//void	AWNode::restoreGuts(AWFile& file)


void        
AWNode::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file << m_name;
	file << m_transformer;
	file << m_parent;
	file << m_ownership;
	file << m_children;
	file << m_life;
}//void AWNode::saveGuts(AWFile& file) const


AWspace     
AWNode::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		m_name.binaryStoreSize() +
		AWPERSISTABLE_POINTER_SIZE(m_transformer) +	
		AWPERSISTABLE_POINTER_SIZE(m_parent) +
		sizeof (m_ownership) +
		sizeof (m_life) +
		m_children.getStoreSize();
	return retVal;
}//AWspace AWNode::binaryStoreSize() const