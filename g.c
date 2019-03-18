#include "androidinterface.h"

AndroidInterface * AndroidInterface::m_pInstance = NULL;

AndroidInterface *AndroidInterface::GetInstance()
{
	if(m_pInstance == NULL){ m_pInstance = new AndroidInterface(); atexit(Release); } return m_pInstance; } void AndroidInterface::Release() { if (m_pInstance) { delete m_pInstance; m_pInstance = NULL; } } void AndroidInterface::startRecord() { QAndroidJniObject::callStaticMethod<void>("com/tl/screen/MainActivity", "startRecord", "()V"); } void AndroidInterface::stopRecord() { QAndroidJniObject::callStaticMethod<void>("com/tl/screen/MainActivity", "stopRecord", "()V"); } AndroidInterface::AndroidInterface(QObject *parent) : QObject(parent) { }
