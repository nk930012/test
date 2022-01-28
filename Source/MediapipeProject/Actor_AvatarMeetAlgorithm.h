// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor_WebCamReader.h"
#include "AnimSkeletalControl.h"
#include "Math/Vector.h"
#include "Components/LineBatchComponent.h"
#include "Actor_AvatarMeetAlgorithm.generated.h"

UCLASS()
class MEDIAPIPEPROJECT_API AActor_AvatarMeetAlgorithm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_AvatarMeetAlgorithm();
	// TODO:: get all value
	FVector GetALLBody(EPoseLandmark PoseLandmark);
	FTransform GetALLBodyTran(EPoseLandmark PoseLandmark);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor_WebCamReader* WebcamReader;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UStaticMeshComponent*> TrackMeshArray1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USkeletalMeshComponent* SkeletalMeshComponent;
	//TODO::the weight parameter
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, FVector> WebPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, float> SKArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, float> WebArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, FVector> WebArrayVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TMap<EPoseLandmark, FVector> AvatarControl;

	// draw line
	TArray<FBatchedLine> lines;
private:
	UAnimSkeletalControl* ControlSkeletal;
	
	double start = 1, end = 1;
#define RIGHT_HAND 0
#define LEFT_HAND 1
	// TODO:: set WEB AND CON PARAMERTERS
	FVector RefSKPos;
	FVector WebcamUpperarm_L, WebcamUpperarm_R, WebcamEarCenter;
	FRotator FirstConRotator;
	FVector WebcamUpperarmCenter;
	// TODO:: set function
	//FVector FingerRotator(int fingerNum,FVector WebcamHand,int hand);
	FVector WebCal(FVector Temp);
	//FRotator CalFaceOrientation(EPoseLandmark PoseLandmark);
	FRotator CalFaceOrientation_new(FVector WebHead);
	void CalArray(EPoseLandmark PoseLandmark, int i);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
