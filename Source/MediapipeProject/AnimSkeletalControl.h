// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimSkeletalControl.generated.h"

/*
UENUM(BlueprintType)
enum class EHandLandmark : uint8
{
	WRIST UMETA(DisplayName = "WRIST"),
	THUMB_CMC UMETA(DisplayName = "THUMB_CMC"),
	THUMB_MCP UMETA(DisplayName = "THUMB_MCP"),
	THUMB_IP UMETA(DisplayName = "THUMB_IP"),
	THUMB_TIP UMETA(DisplayName = "THUMB_TIP"),
	INDEX_FINGER_MCP UMETA(DisplayName = "INDEX_FINGER_MCP"),
	INDEX_FINGER_PIP UMETA(DisplayName = "INDEX_FINGER_PIP"),
	INDEX_FINGER_DIP UMETA(DisplayName = "INDEX_FINGER_DIP"),
	INDEX_FINGER_TIP UMETA(DisplayName = "INDEX_FINGER_TIP"),
	MIDDLE_FINGER_MCP UMETA(DisplayName = "MIDDLE_FINGER_MCP"),
	MIDDLE_FINGER_PIP UMETA(DisplayName = "MIDDLE_FINGER_PIP"),
	MIDDLE_FINGER_DIP UMETA(DisplayName = "MIDDLE_FINGER_DIP"),
	MIDDLE_FINGER_TIP UMETA(DisplayName = "MIDDLE_FINGER_TIP"),
	RING_FINGER_MCP UMETA(DisplayName = "RING_FINGER_MCP"),
	RING_FINGER_PIP UMETA(DisplayName = "RING_FINGER_PIP"),
	RING_FINGER_DIP UMETA(DisplayName = "RING_FINGER_DIP"),
	RING_FINGER_TIP UMETA(DisplayName = "RING_FINGER_TIP"),
	PINKY_FINGER_MCP UMETA(DisplayName = "PINKY_FINGER_MCP"),
	PINKY_FINGER_PIP UMETA(DisplayName = "PINKY_FINGER_PIP"),
	PINKY_FINGER_DIP UMETA(DisplayName = "PINKY_FINGER_DIP"),
	PINKY_FINGER_TIP UMETA(DisplayName = "PINKY_FINGER_TIP"),
};
*/
#define POSE_LANDMARK_ 0



UENUM(BlueprintType)
enum class EPoseLandmark : uint8
{
	// TODO:: set EPoseLandmark definition
	Head UMETA(DisplayName = "Head"),
	EAR_L = 7	UMETA(DisplayName = "EAR_L"),
	EAR_R = 8	UMETA(DisplayName = "EAR_R"),
	Upperarm_L = 11	UMETA(DisplayName = "Upperarm_L"),
	Upperarm_R = 12	UMETA(DisplayName = "Upperarm_R"),
	Lowerarm_L = 13	UMETA(DisplayName = "Lowerarm_L"),
	Lowerarm_R = 14	UMETA(DisplayName = "Lowerarm_R"),
	Hand_L = 15		UMETA(DisplayName = "Hand_L"),
	Hand_R = 16		UMETA(DisplayName = "Hand_R"),
	Hip_L = 23		UMETA(DisplayName = "Hip_L"),
	Hip_R = 24		UMETA(DisplayName = "Hip_R"),
	Knee_L = 25		UMETA(DisplayName = "Knee_L"),
	Knee_R = 26		UMETA(DisplayName = "Knee_R"),
	Foot_L = 27		UMETA(DisplayName = "Foot_L"),
	Foot_R = 28		UMETA(DisplayName = "Foot_R"),
	//Heel_L = 29		UMETA(DisplayName = "Heel_L"),
	//Heel_R = 30		UMETA(DisplayName = "Heel_R"),
	FootTip_L = 31		UMETA(DisplayName = "FootTip_L"),
	FootTip_R = 32		UMETA(DisplayName = "FootTip_R"),
	Body_MAX			UMETA(DisplayName = "The max num of Body"),

	WRIST_L = 100		UMETA(DisplayName = "WRIST_L"),
	THUMB_MCP_L = 102	UMETA(DisplayName = "THUMB_MCP_L"),
	THUMB_DIP_L = 103	UMETA(DisplayName = "THUMB_DIP_L"),
	INDEX_MCP_L = 105	UMETA(DisplayName = "INDEX_MCP_L"),
	INDEX_DIP_L = 107	UMETA(DisplayName = "INDEX_DIP_L"),
	MIDDLE_MCP_L = 109	UMETA(DisplayName = "MIDDLE_MCP_L"),
	MIDDLE_DIP_L = 111	UMETA(DisplayName = "MIDDLE_DIP_L"),
	RING_MCP_L = 113	UMETA(DisplayName = "RING_MCP_L"),
	RING_DIP_L = 115	UMETA(DisplayName = "RING_DIP_L"),
	PINKY_MCP_L = 117	UMETA(DisplayName = "PINKY_MCP_L"),
	PINKY_DIP_L = 119	UMETA(DisplayName = "PINKY_DIP_L"),
	Hand_L_MAX			UMETA(DisplayName = "The max num of Hand_L"),

	WRIST_R = 200		UMETA(DisplayName = "WRIST_R"),
	THUMB_MCP_R = 202	UMETA(DisplayName = "THUMB_MCP_R"),
	THUMB_DIP_R = 203	UMETA(DisplayName = "THUMB_DIP_R"),
	INDEX_MCP_R = 205	UMETA(DisplayName = "INDEX_MCP_R"),
	INDEX_DIP_R = 207	UMETA(DisplayName = "INDEX_DIP_R"),
	MIDDLE_MCP_R = 209	UMETA(DisplayName = "MIDDLE_MCP_R"),
	MIDDLE_DIP_R = 211	UMETA(DisplayName = "MIDDLE_DIP_R"),
	RING_MCP_R = 213	UMETA(DisplayName = "RING_MCP_R"),
	RING_DIP_R = 215	UMETA(DisplayName = "RING_DIP_R"),
	PINKY_MCP_R = 217	UMETA(DisplayName = "PINKY_MCP_R"),
	PINKY_DIP_R = 219	UMETA(DisplayName = "PINKY_DIP_R"),
	Hand_R_MAX			UMETA(DisplayName = "The max num of Hand_R"),


	Max,
};
/**
 *
 */
UCLASS()
class MEDIAPIPEPROJECT_API UAnimSkeletalControl : public UAnimInstance
{
	GENERATED_BODY()

public:
	// TODO:: set control Rig function to get Position, Rotations and Transforms
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, FVector> Positions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, FRotator> Rotations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, FTransform> Transforms;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)//(VisibleAnywhere, BlueprintReadOnly)
	//AActorWebcamAvatar* WebcamAvatar;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//AActor_WebCamReader* WebcamReader;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//AActor_WebCamReader* WebcamReader;

	UFUNCTION(BlueprintPure)
		FVector GetPosition(EPoseLandmark PoseLandmark);

	UFUNCTION(BlueprintPure)
		bool CheckPosition(EPoseLandmark PoseLandmark);

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//FRotator PelvisDegree;

	UFUNCTION(BlueprintPure)
		FRotator GetRotation(EPoseLandmark PoseLandmark);

	UFUNCTION(BlueprintPure)
		FTransform GetTransform(EPoseLandmark PoseLandmark);



};
