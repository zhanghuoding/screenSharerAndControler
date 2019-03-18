#include "deviceScreen.h"

DeviceScreen::DeviceScreen()
{
    m_dentisty=getDentisy();
    dentistyChanged();

}

DeviceScreen::~DeviceScreen()
{

}

//获取屏幕像素密度
double DeviceScreen::getDentisy()
{
    return QAndroidJniObject::callStaticMethod<double>("com/tommego/work/Screen","getDentisy","()D");
}
