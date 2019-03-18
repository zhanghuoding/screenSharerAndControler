#include "qScreenRecorderResultReceiver.h"

#ifdef ANDROID_

int QScreenRecorderResultReceiver::requestId = 1;

QScreenRecorderResultReceiver::QScreenRecorderResultReceiver(int requestId)
{
    this->requestId = requestId;
    screenPicHasBeenShot = false;
    imageByte=new QByteArray;
    imageByte->resize(BUFFER_SIZE);
    dataLength=0;
}
QScreenRecorderResultReceiver::~QScreenRecorderResultReceiver()
{

}
void QScreenRecorderResultReceiver::handleActivityResult(int receiverRequestCode, \
                                                         int resultCode, const QAndroidJniObject &data)
{
    /*if(receiverRequestCode == requestId)
    {
        if(resultCode == RESULT_OK)
        {
            //some code here
        } else
        {
            //some code here
        }
    }*/
    //if(resultCode == -1 && receiverRequestCode == requestId)
    if(resultCode == -1 && receiverRequestCode == requestId)
    {
        screenPicHasBeenShot = true;
        //QAndroidJniObject mBundle("android/os/Bundle","()V;");

        //Bundle bundle=intent.getExtras();
        QAndroidJniObject mBundle=data.callObjectMethod("getExtras","()Landroid/os/Bundle;");
        //Person person=(Person) bundle.getSerializable("person");//获得传递的对象
        QAndroidJniObject jByteArrayOutputStream=mBundle.callObjectMethod(\
                    "getSerializable","(Ljava/lang/String;)Ljava/io/ByteArrayOutputStream;",\
                    QAndroidJniObject::fromString("byteOutputStream").object<jstring>());
        QAndroidJniEnvironment env;
        //jbyte* bytedata = (*env)->GetByteArrayElements(env,jBuffer,0);
        //unsigned char* buf=(unsigned char*)bBuffer;
        jbyte* tempByteData;// =env.GetByteArrayElements(\
                    jByteArrayOutputStream.callMethod("toByteArray","()[B;", 0);
        //memcpy(&gs_raw_data,bytedata,len);
        dataLength=sizeof(tempByteData);
        memcpy(imageByte->data(), tempByteData, dataLength);
    }
}
QByteArray *QScreenRecorderResultReceiver::getImageByteArray()
{
    return imageByte;
}
int QScreenRecorderResultReceiver::getDataLength()
{
    return dataLength;
}
bool QScreenRecorderResultReceiver::isScreenPicFinish()
{
    return screenPicHasBeenShot;
}


#endif //ANDROID
