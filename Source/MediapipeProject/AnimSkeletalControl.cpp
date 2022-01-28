// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimSkeletalControl.h"

FVector UAnimSkeletalControl::GetPosition(EPoseLandmark PoseLandmark)
{
	if(Positions.Contains(PoseLandmark))
	{
		return Positions[PoseLandmark]; 
	}
	return  FVector::ZeroVector;
}

FRotator UAnimSkeletalControl::GetRotation(EPoseLandmark PoseLandmark)
{
	if (Rotations.Contains(PoseLandmark))
	{
		return Rotations[PoseLandmark];
	}
		
	return  FRotator::ZeroRotator;
}

FTransform UAnimSkeletalControl::GetTransform(EPoseLandmark PoseLandmark)
{
	if (Transforms.Contains(PoseLandmark))
	{
		return Transforms[PoseLandmark];
	}
	return FTransform::Identity;
}



bool UAnimSkeletalControl::CheckPosition(EPoseLandmark PoseLandmark)
{
	if(Positions.Contains(PoseLandmark))
		return true;
	return false;
}

