// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor_WebCamReader.h"
#include "AnimSkeletalControl.h"
#include "GameFramework/Actor.h"
#include "ActorWebcamAvatar.generated.h"

UCLASS()
class MEDIAPIPEPROJECT_API AActorWebcamAvatar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorWebcamAvatar();

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	AActor_WebCamReader* WebcamReader;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvatorHight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvatorWidget;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<UStaticMeshComponent*> TrackMeshArray2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<EPoseLandmark,FVector> PositionArray;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<EPoseLandmark, FRotator> RotationArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<EPoseLandmark, FTransform> TransformArray;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EPoseLandmark> ControlArray;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector HipCenterOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AvatarRotationDegree;

private:
	UAnimSkeletalControl* ControlSkeletal;

	float WecamDeminsionRatio;
	float AvatarTall;
	FVector AvatarUpperarmCenter;
	float AvatarHeight;
	float mediapipeTall;
	FVector AvatarHipCenter;
	FVector PreFrameWebcamHipCenter;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
