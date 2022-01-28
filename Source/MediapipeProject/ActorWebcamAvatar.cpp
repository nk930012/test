// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorWebcamAvatar.h"

// Sets default values
AActorWebcamAvatar::AActorWebcamAvatar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PreFrameWebcamHipCenter = FVector::ZeroVector;
	AvatarRotationDegree = 0.0;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShapeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	
	for(int i = 0; i < (int)EPoseLandmark::Max; ++i)
	{
		
		UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPoseLandmark"));
		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *enumPtr->GetNameStringByValue(i));
		if (enumPtr->GetNameStringByValue(i).IsEmpty())
			continue;

		UE_LOG(LogTemp, Warning, TEXT("Mesh: %d"), i);
		FName name = *FString::Printf(TEXT("Track %i"), i);
		UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(name);
		if(i % 2 == 0)
			Mesh->SetStaticMesh(ShapeVisualAsset.Object);
		else
			Mesh->SetStaticMesh(CubeVisualAsset.Object);
		Mesh->SetWorldScale3D(FVector::OneVector * 0.1f);
		TrackMeshArray2.Add(Mesh);
		
		EPoseLandmark PoseLandmark = static_cast<EPoseLandmark>(i);
		PositionArray.Add(PoseLandmark, Mesh->GetComponentLocation());
		
		//TransformArray.Add(PoseLandmark, Mesh->GetComponentLocation());
		//RotationArray.Add(PoseLandmark, Mesh->GetComponentLocation());
	}
}



// Called when the game starts or when spawned
void AActorWebcamAvatar::BeginPlay()
{
	Super::BeginPlay();
	

	UAnimInstance* testTemp = SkeletalMeshComponent->GetAnimInstance();
	UAnimSkeletalControl* ControlTemp = Cast<UAnimSkeletalControl>(testTemp);
	if(WebcamReader == nullptr)
		UE_LOG(LogTemp, Error, TEXT("WebcamReader is nullptr"))	
	else if(UAnimSkeletalControl* Control = Cast<UAnimSkeletalControl>(SkeletalMeshComponent->GetAnimInstance()))
	{
		ControlSkeletal = Control;

		const FVector PersonUpperarm_L = ControlSkeletal->Positions[EPoseLandmark::Upperarm_L];
		const FVector PersonLowerarm_L = ControlSkeletal->Positions[EPoseLandmark::Lowerarm_L];
		const FVector PersonUpperarm_R = ControlSkeletal->Positions[EPoseLandmark::Upperarm_R];
		const FVector PersonLowerarm_R = ControlSkeletal->Positions[EPoseLandmark::Lowerarm_R];
		const FVector PersonUpperarmCenter = PersonUpperarm_L - ((PersonUpperarm_L- PersonUpperarm_R) * 0.5f);
		
		UE_LOG(LogTemp, Warning, TEXT("PersonUpperarm_L %s"), *PersonUpperarm_L.ToString())		
		UE_LOG(LogTemp, Warning, TEXT("PersonLowerarm_L %s"), *PersonLowerarm_L.ToString())		
		UE_LOG(LogTemp, Warning, TEXT("PersonUpperarm_R %s"), *PersonUpperarm_R.ToString())		
		UE_LOG(LogTemp, Warning, TEXT("PersonLowerarm_R %s"), *PersonLowerarm_R.ToString())
		
		AvatarTall = FVector::Dist(PersonLowerarm_R, PersonUpperarm_R);
		UE_LOG(LogTemp, Warning, TEXT("AvatarTall %f"), AvatarTall)
		AvatarTall += FVector::Dist(PersonUpperarm_R, PersonUpperarm_L);
		UE_LOG(LogTemp, Warning, TEXT("AvatarTall %f"), AvatarTall)
		AvatarTall += FVector::Dist(PersonUpperarm_L, PersonLowerarm_L);
		UE_LOG(LogTemp, Warning, TEXT("AvatarTall %f"), AvatarTall)
		AvatarUpperarmCenter = PersonUpperarmCenter;

		//WecamDeminsionRatio = WebcamReader->ResizeDeminsions.X / WebcamReader->ResizeDeminsions.Y;

		
		/*
		const FVector PersonHip_L = ControlSkeletal->Positions[EPoseLandmark::Hip_L];
		const FVector PersonHip_R = ControlSkeletal->Positions[EPoseLandmark::Hip_R];
		const FVector PersonKnee_L = ControlSkeletal->Positions[EPoseLandmark::Knee_L];
		const FVector PersonKnee_R = ControlSkeletal->Positions[EPoseLandmark::Knee_R];
		const FVector PersonFoot_L = ControlSkeletal->Positions[EPoseLandmark::Foot_L];
		const FVector PersonFoot_R = ControlSkeletal->Positions[EPoseLandmark::Foot_R];
		
		
		UE_LOG(LogTemp, Warning, TEXT("PersonFoot_L %s"), *PersonFoot_L.ToString())
		UE_LOG(LogTemp, Warning, TEXT("PersonFoot_R %s"), *PersonFoot_R.ToString())
		

		AvatarHeight = FVector::Dist(PersonUpperarm_L, PersonHip_L);
		UE_LOG(LogTemp, Warning, TEXT("AvatarHeight %f"), AvatarHeight)
		AvatarHeight += FVector::Dist(PersonHip_L, PersonKnee_L);
		UE_LOG(LogTemp, Warning, TEXT("AvatarHeight %f"), AvatarHeight)
		*/
		//AvatarHeight += FVector::Dist(PersonKnee_L, PersonFoot_L);
		//UE_LOG(LogTemp, Warning, TEXT("AvatarHeight %f"), AvatarHeight)
		

		
	}
}

// Called every frame
void AActorWebcamAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ControlSkeletal && WebcamReader->HasPoseLandmarkCoords())
	{
		// TODO::Save the Initial data
		const FVector WebcamHead = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Head);
		const FVector WebcamUpperarm_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Upperarm_L);		
		const FVector WebcamLowerarm_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Lowerarm_L);		
		const FVector WebcamUpperarm_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Upperarm_R);
		const FVector WebcamLowerarm_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Lowerarm_R);
		const FVector WebcamUpperarmCenter = WebcamUpperarm_R + ((WebcamUpperarm_L- WebcamUpperarm_R) * 0.5f);
						
		float WebcamTall = FVector::Dist(WebcamLowerarm_R, WebcamUpperarm_R);
		WebcamTall += FVector::Dist(WebcamUpperarm_R, WebcamUpperarm_L);
		WebcamTall += FVector::Dist(WebcamUpperarm_L, WebcamLowerarm_L);

		float R_ARM_D = FVector::Dist(WebcamLowerarm_R, WebcamUpperarm_R);

		/*
		const FVector WebcamHip_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hip_L);
		const FVector WebcamHip_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hip_R);
		const FVector WebcamKnee_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Knee_L);
		const FVector WebcamKnee_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Knee_R);
		const FVector WebcamFoot_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Foot_L);
		const FVector WebcamFoot_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Foot_R);
		//const FVector WebcamHeel_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Heel_L);
		//const FVector WebcamHeel_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Heel_R);
		
		//float WebcamWidth = FVector::Dist(WebcamHeel_R, WebcamFoot_R);
		float WebcamWidth = FVector::Dist(WebcamFoot_R, WebcamKnee_R);
		WebcamWidth += FVector::Dist(WebcamKnee_R, WebcamHip_R);

		WebcamWidth += FVector::Dist(WebcamHip_R, WebcamHip_L);
		WebcamWidth += FVector::Dist(WebcamHip_L, WebcamKnee_L);

		WebcamWidth += FVector::Dist(WebcamKnee_L, WebcamFoot_L);
		//WebcamWidth += FVector::Dist(WebcamFoot_L, WebcamHeel_L);
		*/

		const FVector WebcamHand_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hand_L);
		const FVector WebcamHand_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hand_R);

		
		float WebcamWidth = WebcamTall;
		WebcamWidth += FVector::Dist(WebcamHand_L, WebcamLowerarm_L);
		WebcamWidth += FVector::Dist(WebcamHand_R, WebcamLowerarm_R);
		
		

		const FVector WebcamHip_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hip_L);
		const FVector WebcamHip_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hip_R);
		const FVector WebcamKnee_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Knee_L);
		const FVector WebcamKnee_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Knee_R);
		const FVector WebcamFoot_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Foot_L);
		const FVector WebcamFoot_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Foot_R);

		// TODO::Calculate the Ratio
		float WebcamHeight = FVector::Dist(WebcamUpperarm_L, WebcamHip_L);
		WebcamHeight += FVector::Dist(WebcamHip_L, WebcamKnee_L);
		//WebcamHeight += FVector::Dist(WebcamKnee_L, WebcamFoot_L);
		WebcamHeight += FVector::Dist(WebcamUpperarm_R, WebcamHip_R);
		WebcamHeight += FVector::Dist(WebcamHip_R, WebcamKnee_R);
		//WebcamHeight += FVector::Dist(WebcamKnee_R, WebcamFoot_R);
		//------------------------------- --------------------------------------------------------------------------
		float TallRatio = AvatarTall / WebcamTall;
		
		float WidthRatio = AvatorWidget / WebcamWidth;
		//float HeightRatio = AvatorHight / WebcamHeight;
		float HeightRatio = AvatarHeight * 2.0 / WebcamHeight;

		//UE_LOG(LogTemp, Warning, TEXT("AvatorWidget: %.6f"), AvatorWidget);
		//UE_LOG(LogTemp, Warning, TEXT("WebcamWidth: %.6f"), WebcamWidth);

		/*
		UE_LOG(LogTemp, Warning, TEXT("WidthRatio: %.6f"), WidthRatio);
		
		UE_LOG(LogTemp, Warning, TEXT("AvatarHeight: %.6f"), AvatarHeight);
		UE_LOG(LogTemp, Warning, TEXT("WebcamHeight: %.6f"), WebcamHeight);
		UE_LOG(LogTemp, Warning, TEXT("HeightRatio: %.6f"), HeightRatio);

		UE_LOG(LogTemp, Warning, TEXT("TallRatio: %.6f"), TallRatio);

		UE_LOG(LogTemp, Warning, TEXT("R_ARM_D: %.6f"), R_ARM_D);
		*/
		//const FVector WebcamHip_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hip_R);
		//const FVector WebcamHip_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hip_L);


		

		float WebcamArmDist = FVector::Dist(WebcamHand_L, WebcamLowerarm_L);
		WebcamArmDist += FVector::Dist(WebcamLowerarm_R, WebcamUpperarm_R);
		WebcamArmDist += FVector::Dist(WebcamUpperarm_R, WebcamUpperarm_L);
		WebcamArmDist += FVector::Dist(WebcamUpperarm_L, WebcamLowerarm_L);
		WebcamArmDist += FVector::Dist(WebcamLowerarm_R, WebcamHand_R);
		UE_LOG(LogTemp, Warning, TEXT("WebcamArmDist: %.6f"), WebcamArmDist);


		const FVector WebcamHipCenter = WebcamHip_R + ((WebcamHip_L - WebcamHip_R) * 0.5f);

		//-----------------------------------------------------------------------------------------------------------------------------------------//rotation radian


		const FVector WebcamHipVectorNormal = (WebcamHip_L - WebcamHip_R).GetSafeNormal();
		float CosineValue = FVector::DotProduct(WebcamHipVectorNormal, FVector(1, 0, 0));
		float RotationDegree = (180.f) / PI * FMath::Acos(CosineValue);

		/*
		UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~WebcamHipVectorNormal: %s"), *WebcamHipVectorNormal.ToString());

		UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CosineValue: %.6f"), CosineValue);

		UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~RotationDegree: %.6f"), RotationDegree);
		*/

		//float SinValue = FVector::CrossProduct(WebcamHipVectorNormal, FVector(1, 0, 0));
		float SinValue = WebcamHipVectorNormal.Z;
		float RotationDegreeSin = (180.f) / PI * FMath::Asin(SinValue);
		/*
		UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~SinValue: %.6f"), SinValue);

		UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~RotationDegree: %.6f"), RotationDegreeSin);
		*/
		/*
		if (SinValue < 0)
			AvatarRotationDegree  = - RotationDegree;
		else
			AvatarRotationDegree = RotationDegree;
		*/
		
		AvatarRotationDegree = (SinValue < 0) ? (-RotationDegree) : RotationDegree;

		//UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~AvatarRotationDegree: %.6f"), AvatarRotationDegree);


		//ControlSkeletal->GetRotation(0.0, 0.0, AvatarRotationDegree);
		//-----------------------------------------------------------------------------------------------------------------------------------------



		
		
		/*
		UE_LOG(LogTemp, Warning, TEXT("WebcamHipCenter: %s"), *WebcamHipCenter.ToString());
		UE_LOG(LogTemp, Warning, TEXT("WebcamUpperarmCenter: %s"), *WebcamUpperarmCenter.ToString())
		*/
		const FVector PersonHip_R = ControlSkeletal->Positions[EPoseLandmark::Hip_R];
		const FVector PersonHip_L = ControlSkeletal->Positions[EPoseLandmark::Hip_L];
		const FVector PersonHipCenter = PersonHip_L - ((PersonHip_L - PersonHip_R) * 0.5f);


		//UE_LOG(LogTemp, Warning, TEXT("PersonHipCenter %s"), *PersonHipCenter.ToString());

			//----------------------------------------------------------------------------------------------------------
			/*
			if(TallRatio > 0)
			{
				for(int i = 0; i < (int)EPoseLandmark::Max; ++i)
				{
					UE_LOG(LogTemp, Warning, TEXT("%d %s"),i, *WebcamReader->GetPoseLandmarkCoords(i).ToString())
					EPoseLandmark PoseLandmark = static_cast<EPoseLandmark>(i);
					if(ControlSkeletal->Positions.Contains(PoseLandmark))
					{
						FVector Temp = (WebcamUpperarmCenter - WebcamReader->GetPoseLandmarkCoords(i));
						float X = Temp.X * TallRatio * WecamDeminsionRatio * -1;
						float Y = Temp.Z * TallRatio * WecamDeminsionRatio;
						float Z = Temp.Y * TallRatio;
						PositionArray[PoseLandmark] = AvatarUpperarmCenter + FVector(X, Y, Z);
						ControlSkeletal->Positions[PoseLandmark] = FMath::Lerp(ControlSkeletal->Positions[PoseLandmark], PositionArray[PoseLandmark], 0.35f);
						TrackMeshArray2[i]->SetRelativeLocation(PositionArray[PoseLandmark]);
					}
				}
			}*/
		float WebcamUpperarm_diff = abs(WebcamUpperarm_L.Z - WebcamUpperarm_R.Z);

		if (mediapipeTall == NULL)
		{
			mediapipeTall = FVector::Dist(WebcamUpperarm_R, WebcamUpperarm_L) *4.5;
		}
		else
		{
			//position未更新
			if (WebcamUpperarm_diff < 0.01)
			{
				mediapipeTall = FVector::Dist(WebcamUpperarm_R, WebcamUpperarm_L) *4.5;
			}
			//to do...position更新，重新計算mediapipeTall

		}

		if (PreFrameWebcamHipCenter == FVector::ZeroVector)
		{
			HipCenterOffset = FVector::ZeroVector;
		}
		else
		{
			HipCenterOffset.X = (WebcamHipCenter.X - PreFrameWebcamHipCenter.X) / mediapipeTall * AvatorWidget;//just for X-axis transformation
			HipCenterOffset.Z = ( - (WebcamHipCenter.Y - PreFrameWebcamHipCenter.Y) / mediapipeTall / WebcamReader->ResizeDeminsions.X * WebcamReader->ResizeDeminsions.Y )*AvatorHight;
		}

		PreFrameWebcamHipCenter = WebcamHipCenter;

		UE_LOG(LogTemp, Warning, TEXT("WebcamHipCenter.X: %.6f"), WebcamHipCenter.X);

			
		/*
		UE_LOG(LogTemp, Warning, TEXT("HipCenterOffset---------------------------- %s"), *HipCenterOffset.ToString());

		UE_LOG(LogTemp, Warning, TEXT("WebcamHipCenter.X: %.6f"), WebcamHipCenter.X);
		UE_LOG(LogTemp, Warning, TEXT("PreFrameWebcamHipCenter.X: %.6f"), PreFrameWebcamHipCenter.X);
		
		UE_LOG(LogTemp, Warning, TEXT("mediapipeTall: %.6f"), mediapipeTall);
		UE_LOG(LogTemp, Warning, TEXT("WebcamUpperarm_R---Z: %.6f"), WebcamUpperarm_R.Z);
		UE_LOG(LogTemp, Warning, TEXT("WebcamUpperarm_L---Z: %.6f"), WebcamUpperarm_L.Z);
		UE_LOG(LogTemp, Warning, TEXT("WebcamUpperarm_diff---Z: %.6f"), WebcamUpperarm_diff);
		*/
		int TrackMeshArray2Index = 0;
		// TODO:: body Control
		for(int i = 0; i < (int)EPoseLandmark::Max; ++i)
		{
			
			UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPoseLandmark"));
			if (enumPtr->GetNameStringByValue(i).IsEmpty())
				continue;

			EPoseLandmark PoseLandmark = static_cast<EPoseLandmark>(i);
			//if((i == 15) || (i == 16))
				//UE_LOG(LogTemp, Warning, TEXT("%d %s"), i, *WebcamReader->GetPoseLandmarkCoords(i).ToString())

			FVector Temp;
			if ((i / 100) != 0)
			{

				FVector TempTip;
				FVector TipDipVector;
				if ((i / 100) == 1) //left hand
				{
					
					Temp = WebcamReader->GetHandsLandmarkCoords(1, i % 100);
					Temp.Z = Temp.Z + WebcamHand_L.Z;

					//save finger tip data for adjust tip degree
					TempTip = WebcamReader->GetHandsLandmarkCoords(1, (i + 1) % 100);
					TempTip.Z = TempTip.Z + WebcamHand_L.Z;

					TipDipVector = (Temp - TempTip).GetSafeNormal();

				}
				else if ((i / 100) == 2) //right hand
				{					

					Temp = WebcamReader->GetHandsLandmarkCoords(0, i % 100);
					Temp.Z = Temp.Z + WebcamHand_R.Z;

					//save finger tip data for adjust tip degree
					TempTip = WebcamReader->GetHandsLandmarkCoords(0, (i + 1) % 100);
					TempTip.Z = TempTip.Z + WebcamHand_R.Z;

					TipDipVector = (TempTip - Temp).GetSafeNormal();

				}

				float TipRotationDegreeY = (180.f) / PI * FMath::Asin(TipDipVector.Y);
				float TipRotationDegreeX = (180.f) / PI * FMath::Asin(TipDipVector.X);
				float TipRotationDegreeZ = (180.f) / PI * FMath::Asin(TipDipVector.Z);

				
				//UE_LOG(LogTemp, Warning, TEXT("%d TipRotationDegreeY: %.6f"),i, TipRotationDegreeY);
				//UE_LOG(LogTemp, Warning, TEXT("%d TipRotationDegreeX: %.6f"), i, TipRotationDegreeX);
				//UE_LOG(LogTemp, Warning, TEXT("%d TipRotationDegreeZ: %.6f"), i, TipRotationDegreeZ);
				ControlSkeletal->Rotations.Add(PoseLandmark, FRotator(TipRotationDegreeY, TipRotationDegreeX, TipRotationDegreeZ));
			}
			
			else //if ((i / 100) == 0) pose
			{
				

				Temp = WebcamReader->GetPoseLandmarkCoords(i);
			}
			

				
				
				/*
				Temp.X = Temp.X / mediapipeTall;
				Temp.Y = Temp.Y / mediapipeTall / WebcamReader->ResizeDeminsions.X * WebcamReader->ResizeDeminsions.Y;
				Temp.Z = Temp.Z / mediapipeTall;
				*/
				/*
				float X = (Temp.X - 0.5) * AvatorWidget;
				float Y = (-1 * Temp.Z) * AvatorWidget * 0.5;
				float Z = (1 - Temp.Y) * AvatorHight;
				*/
				//float X = (Temp.X - WebcamHipCenter.X) * TallRatio;
				//float X = (Temp.X - WebcamHipCenter.X) * WidthRatio;			
				//float Y = (-1 * Temp.Z) * TallRatio * 0.5;
				//float Y = (-1 * Temp.Z) * WidthRatio * 0.5;
				//float Z = (1 - Temp.Y) * TallRatio;
				//float Z = (1 - Temp.Y) * HeightRatio;
				/*
				Temp.X = Temp.X / mediapipeTall;
				Temp.Y = Temp.Y / mediapipeTall / WebcamReader->ResizeDeminsions.X * WebcamReader->ResizeDeminsions.Y;
				Temp.Z = Temp.Z / mediapipeTall;

				float X = (Temp.X - WebcamHipCenter.X) * AvatorWidget;
				float Y = (-1 * Temp.Z) * AvatorWidget * 0.5;
				float Z = (1 - Temp.Y) * AvatorHight;
				*/
				if (RotationDegree <= 90.0)
					Temp.X = (Temp.X - WebcamHipCenter.X) / mediapipeTall;
				else
					Temp.X = (WebcamHipCenter.X - Temp.X) / mediapipeTall;

				Temp.Y = (Temp.Y - WebcamHead.Y + mediapipeTall * 0.15) / mediapipeTall / WebcamReader->ResizeDeminsions.X * WebcamReader->ResizeDeminsions.Y;
				Temp.Z = Temp.Z / mediapipeTall;

				float X = Temp.X * AvatorWidget;
				float Y = (-1 * Temp.Z) * AvatorWidget * 0.5 * 0.5;//* 0.5
				float Z = (1 - Temp.Y) * AvatorHight;


				PositionArray[PoseLandmark] = FVector(X, Y, Z);

				
				
				//TrackMeshArray2[i]->SetRelativeLocation(PositionArray[PoseLandmark]);
				//TrackMeshArray2[TrackMeshArray2Index]->SetRelativeLocation(PositionArray[PoseLandmark]);
				TrackMeshArray2Index++;

				if (ControlArray.Contains(PoseLandmark))
				{
					if (ControlSkeletal->Positions.Contains(PoseLandmark))
					{
						//UE_LOG(LogTemp, Warning, TEXT("*********"));
						ControlSkeletal->Positions[PoseLandmark] = FMath::Lerp(ControlSkeletal->Positions[PoseLandmark], PositionArray[PoseLandmark], 0.35f);
					}

					else
					{

						//UE_LOG(LogTemp, Warning, TEXT("%%%%%%%%%%"));
						ControlSkeletal->Positions.Add(PoseLandmark, PositionArray[PoseLandmark]);
					}

					/*
					if (ControlSkeletal->Rotations.Contains(PoseLandmark))
					{
						//UE_LOG(LogTemp, Warning, TEXT("*********"));
						ControlSkeletal->Rotations[PoseLandmark] = ;
					}
					*/

					//UE_LOG(LogTemp, Warning, TEXT("%d == %s"), i, *ControlSkeletal->GetPosition(PoseLandmark).ToString());
				}




				/*
				if (ControlSkeletal->Positions[EPoseLandmark::Hip_R].Y == ControlSkeletal->Positions[EPoseLandmark::Hip_L].Y)
				{
					UE_LOG(LogTemp, Warning, TEXT("***********----------------------------------Hip_L.Y: %.6f"), ControlSkeletal->Positions[EPoseLandmark::Hip_L].Y);
					UE_LOG(LogTemp, Warning, TEXT("***********----------------------------------Hip_R.Y: %.6f"), ControlSkeletal->Positions[EPoseLandmark::Hip_R].Y);
				}

				*/
				//UE_LOG(LogTemp, Warning, TEXT("R_ARM_D: %.6f"), R_ARM_D);
			/*
			}
			else 
			{
				//FVector TempHandL = WebcamReader->GetHandsLandmarkCoords(1,i%100);
				UE_LOG(LogTemp, Warning, TEXT("%d %s"), i, *WebcamReader->GetHandsLandmarkCoords(1, i % 100).ToString());
			}

			*/
		}

		//UE_LOG(LogTemp, Warning, TEXT("%s"), *WebcamHand_L.ToString());
		
		
	}		
}

