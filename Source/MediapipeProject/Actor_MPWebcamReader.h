// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define WEBCAMREADER

#include <vector>

#include "CoreMinimal.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"    
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Actor_MPWebcamReader.generated.h"

using namespace cv;
#define ENABLE_MEDIAPIPE
//#define _DEBUG_MSG

#ifdef ENABLE_MEDIAPIPE
#define HAND_LANDMARK_NUM 21
#define HAND_COORDINATE_NUM 3
#define POSE_LANDMARK_NUM 33
#define COORDINATE_NUM 3
#define MAX_HANDS 2
#define RIGHT_HAND 0
#define LEFT_HAND 1

// hand landmark define
#define WRIST 0
#define THUMB_CMC 1
#define THUMB_MCP 2
#define THUMB_IP 3
#define THUMB_TIP 4
#define INDEX_FINGER_MCP 5
#define INDEX_FINGER_PIP 6
#define INDEX_FINGER_DIP 7
#define INDEX_FINGER_TIP 8
#define MIDDLE_FINGER_MCP 9
#define MIDDLE_FINGER_PIP 10
#define MIDDLE_FINGER_DIP 11
#define MIDDLE_FINGER_TIP 12
#define RING_FINGER_MCP 13
#define RING_FINGER_PIP 14
#define RING_FINGER_DIP 15
#define RING_FINGER_TIP 16
#define PINKY_FINGER_MCP 17
#define PINKY_FINGER_PIP 18
#define PINKY_FINGER_DIP 19
#define PINKY_FINGER_TIP 20

struct HandsLandmarks {
	char* Label;
	float LandmarkCoords[HAND_LANDMARK_NUM][HAND_COORDINATE_NUM];
};

struct HolisticLandmarks {
	char* HandsLabel[MAX_HANDS];
	float HandsLandmarkCoords[MAX_HANDS][HAND_LANDMARK_NUM][COORDINATE_NUM]; //[hand index][landmark][coordinate]

	char* PoseLabel;
	float PoseLandmarkCoords[POSE_LANDMARK_NUM][COORDINATE_NUM];

};


typedef char* (__cdecl* MPP_START_GRPAH)(char*);
typedef void(__cdecl* MPP_RUN)();
typedef char* (__cdecl* MPP_SENDPACKET)(Mat, cv::Mat*);
typedef char* (__cdecl* MPP_GETGESTURES)(HolisticLandmarks**);
#endif


UCLASS()
class MEDIAPIPEPROJECT_API AActor_MPWebcamReader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_MPWebcamReader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // The device ID opened by the Video Stream
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
        int32 CameraID;

    // If the webcam images should be resized every frame
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
        bool ShouldResize;

    // The targeted resize width and height (width, height)
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
        FVector2D ResizeDeminsions;

    // The rate at which the color data array and video texture is updated (in frames per second)
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
        float RefreshRate;

    // The refresh timer
    UPROPERTY(BlueprintReadWrite, Category = Webcam)
        float RefreshTimer;

    // Blueprint Event called every time the video frame is updated
    UFUNCTION(BlueprintImplementableEvent, Category = Webcam)
        void OnNextVideoFrame();

    // OpenCV fields
    cv::Mat frame;
    cv::VideoCapture stream;
    cv::Size size;

    // OpenCV prototypes
    void UpdateFrame();
    void DoProcessing();
    void UpdateTexture();

    // If the stream has succesfully opened yet
    UPROPERTY(BlueprintReadOnly, Category = Webcam)
        bool isStreamOpen;

    // The videos width and height (width, height)
    UPROPERTY(BlueprintReadWrite, Category = Webcam)
        FVector2D VideoSize;

    // The current video frame's corresponding texture
    UPROPERTY(BlueprintReadOnly, Category = Webcam)
        UTexture2D* VideoTexture;

    // The current data array
    UPROPERTY(BlueprintReadOnly, Category = Webcam)
        TArray<FColor> Data;

    // API
    bool HasPoseLandmarkCoords();
    FVector GetHandsLandmarkCoords(int HandIndex, int LandmarkIndex);
    FVector GetPoseLandmarkCoords(int LandmarkIndex);
    
protected:

    // Use this function to update the texture rects you want to change:
    // NOTE: There is a method called UpdateTextureRegions in UTexture2D but it is compiled WITH_EDITOR and is not marked as ENGINE_API so it cannot be linked
    // from plugins.
    // FROM: https://wiki.unrealengine.com/Dynamic_Textures
    void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

    // Pointer to update texture region 2D struct
    FUpdateTextureRegion2D* VideoUpdateTextureRegion;

private:
    Mat output_frame_bufffer;
    FString Dll_Dir = FPaths::Combine(FPaths::ProjectDir(), TEXT("/DLL_folder/"));

    double start = 1, end = 1;
#ifdef ENABLE_MEDIAPIPE
    // dll API
    MPP_START_GRPAH p_FuncMPPStart = NULL; // Local DLL function pointer.
    MPP_SENDPACKET p_FuncMPPSendPacket = NULL;
    MPP_GETGESTURES p_FuncMPPGetGestures = NULL;
    MPP_RUN p_FuncMPPRun = NULL;

    // mediapip parameters

    bool isMPRunning = true;

    Mat temp;
    //float** temp_landmark = NULL;
    //HandsLandmarks *temp_landmark;

    int CoordnateArray[MAX_HANDS][HAND_LANDMARK_NUM][2];
    HandsLandmarks* mp_hands;
    HolisticLandmarks* mp_holistic;
#endif
};
