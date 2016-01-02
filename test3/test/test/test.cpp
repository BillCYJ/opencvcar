#include<opencv2\opencv.hpp>
#include <opencv2/core/core.hpp> 
#include<iostream>
#include<opencv2/nonfree/features2d.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<opencv2/opencv.hpp>
#include"cv.h"
#include"cxcore.h"


using namespace std;
using namespace cv; 


//��ȡ��ֵ������ֵ
int myOtsu(IplImage *frame)
//int CLICENSE_PLATE_RECONGNITIONDlg::myOtsu(IplImage *frame) 
{
    #define GrayScale 256//frame�Ҷȼ�
    int width = frame->width; 
    int height = frame->height; 
    int pixelCount[GrayScale]={0}; 
    float pixelPro[GrayScale]={0}; 
    int i, j, pixelSum = width * height, threshold = 0; 
    uchar* data = (uchar*)frame->imageData;

    //ͳ��ÿ���Ҷȼ������صĸ���  
    for(i = 0; i < height; i++) 
    { 
        for(j = 0;j < width;j++) 
        { 
            pixelCount[(int)data[i * width + j]]++; 
        } 
    } 

    //����ÿ���Ҷȼ���������Ŀռ����ͼ��ı���  
    for(i = 0; i < GrayScale; i++) 
    { 
        pixelPro[i] = (float)pixelCount[i] / pixelSum; 
    } 
    //�����Ҷȼ�[0,255],Ѱ�Һ��ʵ�threshold 

    float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0; 
    for(i = 0; i < GrayScale; i++) 
    { 
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0; 
        for(j = 0; j < GrayScale; j++) 
        { 
            if(j <= i)   //��������  
            { 
                w0 += pixelPro[j]; 
                u0tmp += j * pixelPro[j]; 
            } 
            else   //ǰ������  
            { 
                w1 += pixelPro[j]; 
                u1tmp += j * pixelPro[j]; 
            } 
        } 
        u0 = u0tmp / w0; 
        u1 = u1tmp / w1; 
        deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2)) ; 
        if(deltaTmp > deltaMax) 
        { 
            deltaMax = deltaTmp; 
            threshold = i; 
        } 
    } 
    return threshold;
}

int main()
{
	/*
	char*path = "f:\\1111.jpg";
	IplImage*frame = cvLoadImage(path);
	if(!frame)return 0;
	cvNamedWindow("frame",1);
	cvShowImage("frame",frame);
    //��ֵ�˲�
	cvSmooth(frame,frame,CV_MEDIAN);
	//�Ҷ�ͼ
	IplImage*gray=cvCreateImage(cvGetSize(frame),frame->depth,1);
	cvCvtColor(frame,gray,CV_BGR2GRAY);
	cvNamedWindow("gray",1);
	cvShowImage("gray",gray);
	//��Ե���
	IplImage*temp=cvCreateImage(cvGetSize(gray),IPL_DEPTH_16S,1);
	//x�����ݶȣ���ֱ��Ե
	cvSobel(gray,temp,2,0,3);
	IplImage*sobel=cvCreateImage(cvGetSize(temp),IPL_DEPTH_8U,1);
	cvConvertScale(temp,sobel,1,0);
	cvShowImage("sobel",sobel);
	//��ֵ��
	IplImage*threshold=cvCreateImage(cvGetSize(sobel),gray->depth,1);
	cvThreshold(sobel,threshold,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
	cvNamedWindow("threshold",1);
	cvShowImage("threshold",threshold);
	//��̬ѧ�仯
	IplConvKernel*kernal;
	IplImage*morph=cvCreateImage(cvGetSize(threshold),threshold->depth,1);
	//�Զ���1*3�ĺ˽���x��������͸�ʴ
	kernal=cvCreateStructuringElementEx(3,1,1,0,CV_SHAPE_RECT);
	cvDilate(threshold,morph,kernal,2);//x������ͨ����
	cvErode(morph,morph,kernal,4);//x��ʴȥ����Ƭ
	cvDilate(morph,morph,kernal,4);//x���ͻظ���̬
	//�Զ���3*1�ĺ˽���y��������͸�ʴ
	kernal=cvCreateStructuringElementEx(1,3,0,1,CV_SHAPE_RECT);
	cvErode(morph,morph,kernal,1);//y��ʴȥ����Ƭ
	cvDilate(morph,morph,kernal,3);//y���ͻظ���̬
	cvNamedWindow("erode",1);
	cvShowImage("erode",morph);
	//�������
	IplImage * frame_draw = cvCreateImage(cvGetSize(frame), frame->depth, frame->nChannels);
    cvCopy(frame, frame_draw);
    CvMemStorage * storage = cvCreateMemStorage(0);  
    CvSeq * contour = 0;   
    int count = cvFindContours(morph,storage,&contour,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);   
    CvSeq * _contour = contour;   
    for( ;contour != 0; contour = contour->h_next)
    {  		
	    double tmparea = fabs(cvContourArea(contour));  		 
	    CvRect aRect = cvBoundingRect( contour, 0 ); 
	    if(tmparea > ((frame->height*frame->width)/10))   
	    {  
		    cvSeqRemove(contour,0); //ɾ�����С���趨ֵ������,1/10   
		    continue;  
	    } 
	    if (aRect.width < (aRect.height*2))  
	    {  
		    cvSeqRemove(contour,0); //ɾ����߱���С���趨ֵ������   
		    continue;  
	    }
	    if ((aRect.width/aRect.height) > 4 )
	    {  
		    cvSeqRemove(contour,0); //ɾ����߱���С���趨ֵ������   
		    continue;  
	    }
	    if((aRect.height * aRect.width) < ((frame->height * frame->width)/100))
	    {  
		    cvSeqRemove(contour,0); //ɾ����߱���С���趨ֵ������   
		    continue;  
	    }
	    CvScalar color = CV_RGB( 255, 0, 0); 
	    cvDrawContours(frame_draw, contour, color, color, 0, 1, 8 );//�����ⲿ���ڲ�������
    }
    cvNamedWindow("����", 1);
    cvShowImage("����", frame_draw);
	waitKey();*/
	//ͼ��Ԥ����������ɫ������
	char*path = "f:\\111.jpg";
	IplImage*SrcImage = cvLoadImage(path);
	if(!SrcImage)return 0;
    IplImage *imgH=NULL,*imgS=NULL,*imgV=NULL,*imgHSV=NULL,*imgGray=NULL;
    imgHSV=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,3);
    imgH=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    imgS=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    imgV=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    cvCvtColor(SrcImage,imgHSV,CV_BGR2HSV); // תΪHSVͼ��
    cvSplit(imgHSV,imgH,imgS,imgV,NULL);    //�����ͨ���ֱ��Ƶ�������ͨ��ͼ����
    cvInRangeS(imgH,cvScalar(94,0,0,0),cvScalar(115,0,0,0),imgH);   //ѡȡ����ͨ��������ֵ��Χ���ڷ�Χ�ڵ���
    cvInRangeS(imgS,cvScalar(90,0,0,0),cvScalar(255,0,0,0),imgS);   //��1��������0������ɫ���ж�ֵ����
    cvInRangeS(imgV,cvScalar(36,0,0,0),cvScalar(255,0,0,0),imgV);
    IplImage *imgTemp=NULL,*imgHsvBinnary=NULL;
    imgTemp=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    imgHsvBinnary=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    cvAnd(imgH,imgS,imgTemp);
    cvAnd(imgTemp,imgV,imgHsvBinnary);  //H,S,V����ͨ���ֱ�λ���룬�����õĵ�ͨ��ͼ�񱣴浽imgHsvBinnary��
    //��̬ѧȥ��
    //����ṹԪ��
    IplConvKernel *element=0;   //�Զ����
    int values[2]={255,255};
    int rows=2,cols=1,anchor_x=0,anchor_y=1;
    element = cvCreateStructuringElementEx(cols,rows,anchor_x,anchor_y,CV_SHAPE_CUSTOM,values); 
    cvDilate(imgHsvBinnary,imgHsvBinnary,element,1);    //���͸�ʴ
    cvErode(imgHsvBinnary,imgHsvBinnary,element,2);     //��θ�ʴ��2�Σ�����������
    //cvNamedWindow("imgh1");
    //cvShowImage("imgh1",imgHsvBinnary);
    //��ֵ�ָ�
    imgGray=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    cvCvtColor(SrcImage,imgGray,CV_RGB2GRAY);   //��ԭͼתΪ��ͨ���Ļ�ɫͼ��
    IplImage *imgRgbBinnary;
    imgRgbBinnary=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    int Thresold=myOtsu(imgGray);   //���ô�����ɫͼ����ֵ
    cvThreshold(imgGray,imgRgbBinnary,Thresold,255,CV_THRESH_OTSU); //���ô����ֵ���ж�ֵ��
	//cvThreshold(imgGray,imgRgbBinnary,0,255,CV_THRESH_OTSU); //���ô����ֵ���ж�ֵ��
    //cvNamedWindow("imgh2");
    //cvShowImage("imgh2",imgRgbBinnary);     //��ʾ��ֵ��ͼ��
    //���ƶ�λ
    //�ж�λ�����ݳ��Ƶ������ͼ���������ж�λ��
    int hop_num=10; //�ַ����������������ֵ
    int num=0;      //��������Ĵ���
    int begin=0;    //�����Ƿ�ʼ
    int mark_Row[2]={0},k=0;//��һ�α�ǳ��ƵĿ�ʼ���������
    int mark_Row1[2]={0};   //�ڶ��α��
    //��һ�ζ�λ
    for(int i=SrcImage->height-1;i>=0;i--)
    {
        num=0;
        for(int j=0;j<SrcImage->width-1;j++)
        {
            if(cvGet2D(imgHsvBinnary,i,j).val[0]!=cvGet2D(imgHsvBinnary,i,j+1).val[0])  //�������е�ֵ��������Ϊһ������
               {
                    num++;
               }
        }
        if(num>hop_num)
            {
                mark_Row[k]=i;
                k=1;
             }
    }
    //cvLine(SrcImage,cvPoint(0,mark_Row[0]),cvPoint(SrcImage->width,mark_Row[0]),CV_RGB(255,255,0));   //��ԭͼ�л�������ǵ�����
    //cvLine(SrcImage,cvPoint(0,mark_Row[1]),cvPoint(SrcImage->width,mark_Row[1]),CV_RGB(255,255,0));
    //�ж�λ
    int mark_col[2]={0},mark_col1[2]={0},num_col=0,k_col=0;
    int a[100]={0},Thresold_col=7;
    for(int j=0;j<SrcImage->width;j++)
    {
        num_col=0;
        for(int i=mark_Row[1];i<mark_Row[0];i++)    //ֻɨ���Ѿ���ǵ�����֮���ͼ��
            if(cvGet2D(imgHsvBinnary,i,j).val[0]>0)
                num_col++;
                if(num_col>Thresold_col)
                {
                    mark_col[k_col]=j;
                    k_col=1;
                }
    }
    int i=0;
    //cvLine(SrcImage,cvPoint(mark_col[0],0),cvPoint(mark_col[0],SrcImage->height),CV_RGB(255,0,0));
    //cvLine(SrcImage,cvPoint(mark_col[1],0),cvPoint(mark_col[1],SrcImage->height),CV_RGB(255,0,0));
    IplImage *imgLicense;
    int license_Width1=(mark_col[1]-mark_col[0]);
    int license_Height1 =mark_Row[0]-mark_Row[1];
    if(license_Width1/license_Height1<3)    //���ݳ��ƵĿ�Ⱥ͸߶ȱȶԳ��������������
    {
        int real_height1 = license_Width1/3;    //���ƵĿ�Ⱥ͸߶ȱȴ��Ϊ3:1
        mark_Row[1] = mark_Row[0]-real_height1;
        license_Height1 = real_height1;
    }
    //�ڶ��ζ�λ���ڵ�һ�ζ�λ�Ļ���֮�ϣ�
    k=0;
    for(int i=mark_Row[0];i>mark_Row[1];i--)
    {
        num=0;
        for(int j=mark_col[0];j<mark_col[1];j++)
        {
            if(cvGet2D(imgHsvBinnary,i,j).val[0]!=cvGet2D(imgHsvBinnary,i,j+1).val[0])  //�������е�ֵ��������Ϊһ������
               {
                    num++;
               }
        }
        if(num>8)
            {
                mark_Row1[k]=i;
                k=1;
             }
    }
    k_col=0;
    for(int j=mark_col[0];j<mark_col[1];j++)
    {
        num_col=0;
        for(int i=mark_Row1[1];i<mark_Row1[0];i++)  //ֻɨ���Ѿ���ǵ�����֮���ͼ��
            if(cvGet2D(imgHsvBinnary,i,j).val[0]>0)
                num_col++;
                if(num_col>6)
                {
                    mark_col1[k_col]=j;
                    k_col=1;
                }
    }
    int license_Width=(mark_col1[1]-mark_col1[0]);
    int license_Height =mark_Row1[0]-mark_Row1[1];
    if(license_Width/license_Height<3)  //���ݿ�Ⱥ͸߶ȱ��ٴ�����
    {
        int real_height = license_Width/3;  //���ƵĿ�Ⱥ͸߶ȱȴ��Ϊ3:1
        mark_Row1[1] = mark_Row1[0]-real_height;
        license_Height = real_height;
    }
	IplImage*SrcLicenseimg1;
	IplImage*SrcLicenseimg2;
    cvSetImageROI(SrcImage,cvRect(mark_col1[0],mark_Row1[1],license_Width,license_Height)); //��������������ΪROI����
    cvSetImageROI(imgRgbBinnary,cvRect(mark_col1[0],mark_Row1[1],license_Width,license_Height));
    cvSetImageROI(imgHsvBinnary,cvRect(mark_col1[0],mark_Row1[1],license_Width,license_Height));
    imgLicense=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,SrcImage->nChannels);  //������ʾ�ĳ���ͼƬ
    SrcLicenseimg1=cvCreateImage(cvGetSize(imgRgbBinnary),imgRgbBinnary->depth,imgRgbBinnary->nChannels);
    SrcLicenseimg2=cvCreateImage(cvGetSize(imgHsvBinnary),imgHsvBinnary->depth,imgHsvBinnary->nChannels);
    cvCopy(SrcImage,imgLicense,0);
    cvCopy(imgRgbBinnary,SrcLicenseimg1,0); //���������򿽱�����Ӧ��ͼ����
    cvCopy(imgHsvBinnary,SrcLicenseimg2,0);
    //cvNamedWindow("SrcLicenseimg1");  //��ʾ���ƵĶ�ֵ����ͼƬ
    //cvShowImage("SrcLicenseimg1",SrcLicenseimg1);
    //cvNamedWindow("SrcLicenseimg2");  
    //cvShowImage("SrcLicenseimg2",SrcLicenseimg2);
    cvResetImageROI(SrcImage);  //ȡ��ROI����
    cvResetImageROI(imgRgbBinnary);
    cvResetImageROI(imgHsvBinnary);
	cvNamedWindow("license");
    cvShowImage("license",imgLicense);
    //cvResize(imgLicense,TheImage_plate);
    //ShowImage(TheImage_plate,IDC_LICENSE_AREA); //��ʾ����
    cvLine(SrcImage,cvPoint(mark_col1[0],mark_Row1[0]),cvPoint(mark_col1[1],mark_Row1[0]),CV_RGB(255,0,0)); //��ԭͼ���л�����������
    cvLine(SrcImage,cvPoint(mark_col1[1],mark_Row1[0]),cvPoint(mark_col1[1],mark_Row1[1]),CV_RGB(255,0,0));
    cvLine(SrcImage,cvPoint(mark_col1[1],mark_Row1[1]),cvPoint(mark_col1[0],mark_Row1[1]),CV_RGB(255,0,0));
    cvLine(SrcImage,cvPoint(mark_col1[0],mark_Row1[1]),cvPoint(mark_col1[0],mark_Row1[0]),CV_RGB(255,0,0));
    cvNamedWindow("src");
    cvShowImage("src",SrcImage);
    //cvResize( SrcImage, TheImage );    //��ʾԭͼ���Ѿ������˳�������
    //ShowImage( TheImage, IDC_VIDEO_SHOW );
	waitKey();
}