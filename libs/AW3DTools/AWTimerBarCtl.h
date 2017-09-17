#ifndef __AW_TIMERBARCTL_H
#define __AW_TIMERBARCTL_H


#include "AWTransformController.h"


class AWExport AWTimerBarController : public AWTransformController
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWTimerBarController);

	AWTimerBarController();
	virtual			~AWTimerBarController();
	virtual int		calcTransform(float t, const AWMatrix4& parentTM);

	virtual void	restoreGuts(AWFile& file);
    virtual void    saveGuts(AWFile& file) const;
	virtual AWspace binaryStoreSize() const;

    virtual void    setInterval(float t)         {m_interval=t;}
    virtual float   getInterval()                {return m_interval;}

    virtual void    init(float t);

protected:
    float           m_timeout;      //not persisted
    float           m_interval;     //not persisted
};//class AWTimerBarController : public AWTransformController


inline void 
AWTimerBarController::init(float t)
{
    m_timeout = t + m_interval;
}//inline void AWTimerBarController::init(float t)


#endif //sentinel __AW_TIMERBARCTL_H