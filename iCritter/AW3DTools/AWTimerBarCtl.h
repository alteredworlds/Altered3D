#ifndef __AW_TIMERBARCTL_H
#define __AW_TIMERBARCTL_H


#include "AWTransformController.h"


class AWExport AWTimerBarController : public AWTransformController
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWTimerBarController);

	AWTimerBarController();
	virtual			~AWTimerBarController();
	virtual int		calcTransform(double t, const AWMatrix4& parentTM);

	virtual void	restoreGuts(AWFile& file);
    virtual void    saveGuts(AWFile& file) const;
	virtual AWspace binaryStoreSize() const;

    virtual void    setInterval(double t)        {m_interval=t;}
    virtual double  getInterval()                {return m_interval;}

    virtual void    init(double t);

protected:
    double           m_timeout;      //not persisted
    double           m_interval;     //not persisted
};//class AWTimerBarController : public AWTransformController


inline void 
AWTimerBarController::init(double t)
{
    m_timeout = t + m_interval;
}//inline void AWTimerBarController::init(float t)


#endif //sentinel __AW_TIMERBARCTL_H