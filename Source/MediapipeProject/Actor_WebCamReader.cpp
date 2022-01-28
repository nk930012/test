// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor_WebCamReader.h"
#include "WebcamReader.h"
float** temp_landmark = NULL;
float test_ret2 = NULL;

// Sets default values
AActor_WebCamReader::AActor_WebCamReader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#ifdef WEBCAMREADER
    // Initialize OpenCV and webcam properties
    // TODO:: set the avatar camera initialization
    CameraID = 0;
    RefreshRate = 30;
    isStreamOpen = false;
    VideoSize = FVector2D(0, 0);
    ShouldResize = false;
    ResizeDeminsions = FVector2D(640, 480); //640, 480 1920,1080 854,480
    RefreshTimer = 0.0f;
    stream = cv::VideoCapture();
    frame = cv::Mat();


#endif
#ifdef ENABLE_MEDIAPIPE
    // mediapipe init
    //  set mediapipe dll APIs
    void* DLLHandle;
    FString filePath = FPaths::Combine(Dll_Dir, TEXT("demo_run_graph_main_landmark.dll"));
    DLLHandle = FPlatformProcess::GetDllHandle(*filePath); // Retrieve the DLL.

    if (DLLHandle != NULL)
    {
        FString procStartGraph = "dll_api_MPP_start_graph";        
        p_FuncMPPStart = (MPP_START_GRPAH)FPlatformProcess::GetDllExport(DLLHandle, *procStartGraph); // Export the DLL function.

        FString procMPPSendPacket = "dll_api_MPP_SendPacket";
        p_FuncMPPSendPacket = (MPP_SENDPACKET)FPlatformProcess::GetDllExport(DLLHandle, *procMPPSendPacket); // Export the DLL function.

        FString procMPPGetGestures = "dll_api_MPP_GetGestures";
        p_FuncMPPGetGestures = (MPP_GETGESTURES)FPlatformProcess::GetDllExport(DLLHandle, *procMPPGetGestures); // Export the DLL function.

        FString procMPPRun = "dll_api_MPP_run";
        p_FuncMPPRun = (MPP_RUN)FPlatformProcess::GetDllExport(DLLHandle, *procMPPRun); // Export the DLL function.
    }
    else { UE_LOG(LogTemp, Warning, TEXT("DLL is NULL")); }
#endif
}

// Called when the game starts or when spawned
void AActor_WebCamReader::BeginPlay()
{
	Super::BeginPlay();

#ifdef WEBCAMREADER
    // Open the stream
    // TODO:: set the video path and open
    //String video = "C:/Users/kixvb/video/circle1920X1080.avi";//walk1920X1080.avi,circle1920X1080.avi,jump1920X1080.avi,WIN_20211208_17_1920X1080.avi,Blue bird_854x480p.avi
    // TODO:: open the avatar camera
    stream.open(CameraID);
    //stream.open(video);
    if (stream.isOpened())
    {
        // Initialize stream
        //  the setting MUST follow the order : FOURCC -> FPS -> WIDTH -> HEIGHT
        stream.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));;

        stream.set(cv::CAP_PROP_FPS, 30);
        stream.set(cv::CAP_PROP_FRAME_WIDTH, ResizeDeminsions.X);
        stream.set(cv::CAP_PROP_FRAME_HEIGHT, ResizeDeminsions.Y);
        
        isStreamOpen = true;
        UpdateFrame(); 
        size = cv::Size(ResizeDeminsions.X, ResizeDeminsions.Y);
        cv::resize(frame, frame, size);
        VideoSize = FVector2D(frame.cols, frame.rows);
        //ResizeDeminsions = FVector2D(frame.cols, frame.rows);
        VideoTexture = UTexture2D::CreateTransient(VideoSize.X, VideoSize.Y);

        VideoTexture->UpdateResource();
        VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, VideoSize.X, VideoSize.Y);

        // Initialize data array
        Data.Init(FColor(0, 0, 0, 255), VideoSize.X * VideoSize.Y);
        

        // Do first frame
        //DoProcessing();
        UpdateTexture();
        OnNextVideoFrame();
    }
#endif


#ifdef ENABLE_MEDIAPIPE
    // start mediapipe graph
    if (p_FuncMPPStart != NULL)
    {
        FString graphPath = FPaths::Combine(Dll_Dir, TEXT("holistic_tracking_cpu.pbtxt"));
        char *ret = p_FuncMPPStart(TCHAR_TO_ANSI(*graphPath)); // Call the DLL function, with arguments corresponding to the signature and return type of the function.
        UE_LOG(LogTemp, Warning, TEXT("dll_api_MPP_start_graph %s"), UTF8_TO_TCHAR(ret));
        //p_FuncMPPRun();
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("API not found : dll_api_MPP_start_graph"));
    }
#endif
}

// Called every frame
void AActor_WebCamReader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    
#ifdef WEBCAMREADER
    
    RefreshTimer += DeltaTime;
    //UE_LOG(LogTemp, Warning, TEXT("RefreshTimer : %f, %f"), RefreshTimer, DeltaTime);
    if (isStreamOpen && RefreshTimer >= 1.0f / RefreshRate)
    {
        RefreshTimer -= 1.0f / RefreshRate;
        UpdateFrame();
        DoProcessing();
        UpdateTexture();
        //OnNextVideoFrame();
    }
    else { UE_LOG(LogTemp, Warning, TEXT("Not RefreshTimer ")); }
#endif
    //FPlatformProcess::Sleep(0.025f);
}

#ifdef WEBCAMREADER
void AActor_WebCamReader::UpdateFrame()
{

    start = (double)getTickCount();

    if (stream.isOpened())
    {   
        
        //stream.set(CV_CAP_PROP_FPS, 30);

        stream.read(frame);
        if (ShouldResize)
        {
            cv::resize(frame, frame, size);
        }
        //flip(frame, frame, 1);// horizonal flip
        frame.copyTo(output_frame_bufffer);
        //cv::resize(frame, frame, cv::Size(1920, 1080));

        //UE_LOG(LogTemp, Warning, TEXT("size: %f, %f"), output_frame_bufffer.cols, frame.cols);


    }
    else {
        isStreamOpen = false;
    }
}

void AActor_WebCamReader::DoProcessing()
{
    // TODO: Do any processing here!
#ifdef ENABLE_MEDIAPIPE
    // send packet to mediapipe and get result   

    //UE_LOG(LogTemp, Warning, TEXT("Dir : %s"), *(FPaths::ProjectDir()));
    char* gesture = "None";
    //UE_LOG(LogTemp, Warning, TEXT("LAndmark  add: %d"), &test_ret);
    if (isStreamOpen && frame.data)
    {
        if (p_FuncMPPSendPacket != NULL)
        {
            char* ret = p_FuncMPPSendPacket(frame, &temp); // Call the DLL function, with arguments corresponding to the signature and return type of the function.
            //UE_LOG(LogTemp, Warning, TEXT("ret : %s"), UTF8_TO_TCHAR(*ret)); //run status check
            
            gesture = p_FuncMPPGetGestures(&mp_holistic);
            //UE_LOG(LogTemp, Warning, TEXT("gesture : %s"), UTF8_TO_TCHAR(gesture));

            //UE_LOG(LogTemp, Warning, TEXT("LAndmark : %f"), mp_hands[0].LandmarkCoords[9][0]); //run status check

            //UE_LOG(LogTemp, Warning, TEXT("dll_api_MPP_SendPacket %f"), temp_landmark[9][0]);
            for (int i = 0; i < HAND_LANDMARK_NUM; i++) {

                //UE_LOG(LogTemp, Warning, TEXT("Land mark x: %d"), strcmp(mp_hands[RIGHT_HAND].Label, "Right"));
                if (!strcmp(mp_holistic->HandsLabel[RIGHT_HAND], "Right")) {
                    if (mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][0] > 0 && mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][0] < 1) {
                        mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][0] = (int)(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][0] * VideoSize.X);
                    }
                    else { mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][0] = VideoSize.X + 10; }
                    if (mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][1] > 0 && mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][1] < 1) {
                        mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][1] = (int)(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][1] * VideoSize.Y);
                    }
                    else { mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][1] = VideoSize.Y + 10; }
                    //UE_LOG(LogTemp, Warning, TEXT("R Land mark x: %d"), CoordnateArray[RIGHT_HAND][9][0]);
                }
                else {
                    mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][0] = -1;
                    mp_holistic->HandsLandmarkCoords[RIGHT_HAND][i][1] = -1;
                }

                if (!strcmp(mp_holistic->HandsLabel[LEFT_HAND], "Left")) {
                    if (mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][0] > 0 && mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][0] < 1) {
                        mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][0] = (int)(mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][0] * VideoSize.X);
                    }
                    else { mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][0] = VideoSize.X + 10; }
                    if (mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][1] > 0 && mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][1] < 1) {
                        mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][1] = (int)(mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][1] * VideoSize.Y);
                    }
                    else { mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][1] = VideoSize.Y + 10; }
                }
                else {
                    mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][0] = -1;
                    mp_holistic->HandsLandmarkCoords[LEFT_HAND][i][1] = -1;
                }
            }
            
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("API not foubd : dll_api_MPP_SendPacket"));
        }
    }


    // draw hand position

        //UE_LOG(LogTemp, Warning, TEXT("Label: %s, %s"), UTF8_TO_TCHAR(mp_hands[RIGHT_HAND].Label), UTF8_TO_TCHAR(mp_hands[LEFT_HAND].Label));
    
        // draw right hand
        if (!strcmp(mp_holistic->HandsLabel[RIGHT_HAND], "Right")) {
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][MIDDLE_FINGER_MCP][0], mp_holistic->HandsLandmarkCoords[RIGHT_HAND][MIDDLE_FINGER_MCP][1]), 10, Scalar(0, 0, 255), -1);

            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][THUMB_TIP][0], mp_holistic->HandsLandmarkCoords[RIGHT_HAND][THUMB_TIP][1]), 10, Scalar(255, 255, 0), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][INDEX_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[RIGHT_HAND][INDEX_FINGER_TIP][1]), 10, Scalar(255, 255, 0), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][MIDDLE_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[RIGHT_HAND][MIDDLE_FINGER_TIP][1]), 10, Scalar(255, 255, 0), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][RING_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[RIGHT_HAND][RING_FINGER_TIP][1]), 10, Scalar(255, 255, 0), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[RIGHT_HAND][PINKY_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[RIGHT_HAND][PINKY_FINGER_TIP][1]), 10, Scalar(255, 255, 0), -1);
        }
    
        //draw left hand
        if (!strcmp(mp_holistic->HandsLabel[LEFT_HAND], "Left")) {
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[LEFT_HAND][MIDDLE_FINGER_MCP][0], mp_holistic->HandsLandmarkCoords[LEFT_HAND][MIDDLE_FINGER_MCP][1]), 10, Scalar(0, 0, 255), -1);

            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[LEFT_HAND][THUMB_TIP][0], mp_holistic->HandsLandmarkCoords[LEFT_HAND][THUMB_TIP][1]), 10, Scalar(255, 0, 255), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[LEFT_HAND][INDEX_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[LEFT_HAND][INDEX_FINGER_TIP][1]), 10, Scalar(255, 0, 255), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[LEFT_HAND][MIDDLE_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[LEFT_HAND][MIDDLE_FINGER_TIP][1]), 10, Scalar(255, 0, 255), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[LEFT_HAND][RING_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[LEFT_HAND][RING_FINGER_TIP][1]), 10, Scalar(255, 0, 255), -1);
            circle(output_frame_bufffer, Point(mp_holistic->HandsLandmarkCoords[LEFT_HAND][PINKY_FINGER_TIP][0], mp_holistic->HandsLandmarkCoords[LEFT_HAND][PINKY_FINGER_TIP][1]), 10, Scalar(255, 0, 255), -1);
        }

        UE_LOG(LogTemp, Warning, TEXT("Label: %s"), UTF8_TO_TCHAR(mp_holistic->PoseLabel));
        //UE_LOG(LogTemp, Warning, TEXT("pos: %s"), (int)(mp_holistic->PoseLandmarkCoords[11][0] * VideoSize.X), (int)(mp_holistic->PoseLandmarkCoords[11][1] * VideoSize.Y), (int)(mp_holistic->PoseLandmarkCoords[11][1] * VideoSize.Y));
        // draw body
        if (!strcmp(mp_holistic->PoseLabel, "Pose")) {
            int line_thickness = 4;
            circle(output_frame_bufffer, Point((int)(mp_holistic->PoseLandmarkCoords[11][0] * VideoSize.X), (int)(mp_holistic->PoseLandmarkCoords[11][1] * VideoSize.Y)), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[12][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[12][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[13][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[13][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[14][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[14][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[15][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[15][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[16][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[16][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);

            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[23][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[23][1] * VideoSize.Y), 10, Scalar(255, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[24][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[24][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[25][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[25][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[26][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[26][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[27][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[27][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[28][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[28][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[31][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[31][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[32][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[32][1] * VideoSize.Y), 10, Scalar(10, 105, 230), -1);




            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[11][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[11][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[12][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[12][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[11][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[11][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[13][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[13][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[14][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[14][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[12][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[12][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[15][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[15][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[13][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[13][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[14][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[14][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[16][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[16][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);


            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[23][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[23][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[24][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[24][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[25][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[25][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[23][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[23][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[25][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[25][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[27][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[27][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[24][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[24][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[26][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[26][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[28][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[28][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[26][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[26][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);

            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[24][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[24][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[12][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[12][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[23][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[23][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[11][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[11][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);

            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[28][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[28][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[32][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[32][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);
            line(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[27][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[27][1] * VideoSize.Y),
                Point(mp_holistic->PoseLandmarkCoords[31][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[31][1] * VideoSize.Y), Scalar(255, 255, 255), line_thickness);


            // face
            circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[0][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[0][1] * VideoSize.Y), 10, Scalar(105, 10, 230), -1);
            //circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[7][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[7][1] * VideoSize.Y), 10, Scalar(105, 10, 230), -1);
            //circle(output_frame_bufffer, Point(mp_holistic->PoseLandmarkCoords[8][0] * VideoSize.X, mp_holistic->PoseLandmarkCoords[8][1] * VideoSize.Y), 10, Scalar(105, 10, 230), -1);
            //circle(output_frame_bufffer, Point((mp_holistic->PoseLandmarkCoords[7][0] + mp_holistic->PoseLandmarkCoords[8][0]) * VideoSize.X/2, (mp_holistic->PoseLandmarkCoords[7][1] + mp_holistic->PoseLandmarkCoords[8][1]) * VideoSize.Y/2), 10, Scalar(10, 230,10), -1);
        }
        
#endif 
    // get end time
    end = (double)getTickCount();

    //add fps info
    flip(output_frame_bufffer, output_frame_bufffer, 1);// horizonal flip
    double fps = 1, diff = 1;
    std::ostringstream str;
    diff = (end - start) / getTickFrequency();
    //UE_LOG(LogTemp, Warning, TEXT("time diff %f"), diff);
    fps = 1 / diff;
    str << "FPS : " << fps;
    putText(output_frame_bufffer,
        str.str(),
        Point(10, 25),
        FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(0, 255, 0), //font color
        2);
#ifdef _DEBUG_MSG
    std::ostringstream DebugMsg;
    DebugMsg << "Resolusion : " << output_frame_bufffer.cols<<"*"<< frame.cols;
    putText(output_frame_bufffer,
        DebugMsg.str(),
        Point(int(VideoSize.X * 0.6) , 25),
        FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(255, 0, 0), //font color
        2);
#endif
#ifdef ENABLE_MEDIAPIPE
    // draw gesture info
    std::ostringstream str_gesture;
    str_gesture << "gestures : " << gesture;
    cv::putText(output_frame_bufffer,
        str_gesture.str(),
        cv::Point(10, 50),
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(0, 255, 0), //font color
        2);
#endif   
}

void AActor_WebCamReader::UpdateTexture()
{
    if (isStreamOpen && output_frame_bufffer.data)
    {
        // Copy Mat data to Data array
        for (int y = 0; y < VideoSize.Y; y++)
        {
            for (int x = 0; x < VideoSize.X; x++)
            {
                int i = x + (y * VideoSize.X);
                Data[i].B = output_frame_bufffer.data[i * 3 + 0];
                Data[i].G = output_frame_bufffer.data[i * 3 + 1];
                Data[i].R = output_frame_bufffer.data[i * 3 + 2];
            }
        }

        // Update texture 2D
        UpdateTextureRegions(VideoTexture, (int32)0, (uint32)1, VideoUpdateTextureRegion, (uint32)(4 * VideoSize.X), (uint32)4, (uint8*)Data.GetData(), false);
    }
}

bool AActor_WebCamReader::HasPoseLandmarkCoords()
{
    if(mp_holistic != nullptr && !strcmp(mp_holistic->PoseLabel, "Pose"))
        return true;
    return false;
}

bool AActor_WebCamReader::HasHandsLandmarkCoords(int HandIndex)
{
    if (mp_holistic != nullptr)
    {
        if (HandIndex == RIGHT_HAND && !strcmp(mp_holistic->HandsLabel[RIGHT_HAND], "Right"))
            return true;
        if (HandIndex == LEFT_HAND && !strcmp(mp_holistic->HandsLabel[LEFT_HAND], "Left"))
            return true;
 
    }
    return false;
}

FVector AActor_WebCamReader::GetHandsLandmarkCoords(int HandIndex, int LandmarkIndex)
{
    if(mp_holistic != nullptr)
    {
        if(HandIndex == RIGHT_HAND && strcmp(mp_holistic->HandsLabel[RIGHT_HAND], "Right"))
            return FVector::ZeroVector;
        if(HandIndex == LEFT_HAND && strcmp(mp_holistic->HandsLabel[LEFT_HAND], "Left"))
            return FVector::ZeroVector;
        const float X = (mp_holistic->HandsLandmarkCoords[HandIndex][LandmarkIndex][0]) / VideoSize.X;/// VideoSize.X
        const float Y = (mp_holistic->HandsLandmarkCoords[HandIndex][LandmarkIndex][1]) / VideoSize.Y;/// VideoSize.Y
        const float Z = mp_holistic->HandsLandmarkCoords[HandIndex][LandmarkIndex][2];
        return FVector(X,Y,Z);
    }
    return FVector::ZeroVector;
}

FVector AActor_WebCamReader::GetPoseLandmarkCoords(int LandmarkIndex)
{
    if(mp_holistic != nullptr && !strcmp(mp_holistic->PoseLabel, "Pose"))
    {
        const float X = mp_holistic->PoseLandmarkCoords[LandmarkIndex][0];
        const float Y = mp_holistic->PoseLandmarkCoords[LandmarkIndex][1];
        const float Z = mp_holistic->PoseLandmarkCoords[LandmarkIndex][2];
        return FVector(X,Y,Z);
    }
    return FVector::ZeroVector;
}

void AActor_WebCamReader::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
    //UE_LOG(LogTemp, Warning, TEXT("UpdateTextureRegions\n"));
    if (Texture->Resource)
    {
        struct FUpdateTextureRegionsData
        {
            FTexture2DResource* Texture2DResource;
            int32 MipIndex;
            uint32 NumRegions;
            FUpdateTextureRegion2D* Regions;
            uint32 SrcPitch;
            uint32 SrcBpp;
            uint8* SrcData;
        };

        FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

        RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
        RegionData->MipIndex = MipIndex;
        RegionData->NumRegions = NumRegions;
        RegionData->Regions = Regions;
        RegionData->SrcPitch = SrcPitch;
        RegionData->SrcBpp = SrcBpp;
        RegionData->SrcData = SrcData;

        ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)([RegionData, bFreeData](FRHICommandListImmediate& RHICmdList) {

            for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
            {
                int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
                if (RegionData->MipIndex >= CurrentFirstMip)
                {
                    RHIUpdateTexture2D(
                        RegionData->Texture2DResource->GetTexture2DRHI(),
                        RegionData->MipIndex - CurrentFirstMip,
                        RegionData->Regions[RegionIndex],
                        RegionData->SrcPitch,
                        RegionData->SrcData
                        + RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
                        + RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
                    );
                }
            }
            if (bFreeData)
            {
                FMemory::Free(RegionData->Regions);
                FMemory::Free(RegionData->SrcData);
            }

            });
    }
}
//*/
#endif
