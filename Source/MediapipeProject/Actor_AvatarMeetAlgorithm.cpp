// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_AvatarMeetAlgorithm.h"

// Sets default values
AActor_AvatarMeetAlgorithm::AActor_AvatarMeetAlgorithm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// TODO:: set TrackMeshArray1
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShapeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	for (int i = 0; i < (int)EPoseLandmark::Max; ++i)
	{
		UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPoseLandmark"));
		if (enumPtr->GetNameStringByValue(i).IsEmpty())
			continue;
		FName name = *FString::Printf(TEXT("Track %i"), i);
		UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(name);
		if (i % 2 == 0)
			Mesh->SetStaticMesh(ShapeVisualAsset.Object);  //觀察右邊座標用的圓形物件
		else
			Mesh->SetStaticMesh(CubeVisualAsset.Object);   //觀察左邊座標用的圓形物件
		Mesh->SetWorldScale3D(FVector::OneVector * 0.1f);
		TrackMeshArray1.Add(Mesh);

	}
}
FVector AActor_AvatarMeetAlgorithm::GetALLBody(EPoseLandmark PoseLandmark)
{
	if (ControlSkeletal->Positions.Contains(PoseLandmark))
	{
		return FVector(ControlSkeletal->Positions[PoseLandmark]);
	}
	return FVector::ZeroVector;
}


FTransform AActor_AvatarMeetAlgorithm::GetALLBodyTran(EPoseLandmark PoseLandmark)
{
	if (ControlSkeletal->Transforms.Contains(PoseLandmark))
	{
		return FTransform(ControlSkeletal->Transforms[PoseLandmark]);
	}
	return FTransform::Identity;
}

// Called when the game starts or when spawned
void AActor_AvatarMeetAlgorithm::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i < (int)EPoseLandmark::Max; ++i)
	{
		UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPoseLandmark"));
		if (enumPtr->GetNameStringByValue(i).IsEmpty())
			continue;

		EPoseLandmark SKLandmark = static_cast<EPoseLandmark>(i);
		SKArray.Add(SKLandmark, 0.f);
		WebArray.Add(SKLandmark, 0.f);
		WebArrayVector.Add(SKLandmark, FVector(0.f, 0.f, 0.f));
		AvatarControl.Add(SKLandmark, FVector(0.f, 0.f, 0.f));
		WebPosition.Add(SKLandmark, FVector(0.f, 0.f, 0.f));

	}

	if (WebcamReader == nullptr)
		UE_LOG(LogTemp, Error, TEXT("WebcamReader is nullptr"))

	else if (UAnimSkeletalControl* Control = Cast<UAnimSkeletalControl>(SkeletalMeshComponent->GetAnimInstance()))
	{
		ControlSkeletal = Control;
		
		FVector ConU_L = ControlSkeletal->Positions[EPoseLandmark::Upperarm_L];
		FVector ConU_R = ControlSkeletal->Positions[EPoseLandmark::Upperarm_R];
		RefSKPos = (ConU_L + ConU_R) * 0.5f;
		FirstConRotator = ControlSkeletal->Transforms[EPoseLandmark::Head].GetRotation().Rotator();
		//TODO::CONTROL SK RATE
		for (int i = 0; i < (int)EPoseLandmark::Max; ++i)
		{
			EPoseLandmark PoseLandmark = static_cast<EPoseLandmark>(i);
			if (ControlSkeletal->Positions.Contains(PoseLandmark) || ControlSkeletal->Transforms.Contains(PoseLandmark))
			{
				if (i == 0 || i == (int)EPoseLandmark::Upperarm_L || i == (int)EPoseLandmark::Upperarm_R)
				{
					FVector temp;
					if (ControlSkeletal->Transforms.Contains(PoseLandmark))
						temp = ControlSkeletal->Transforms[PoseLandmark].GetLocation();
					else
						temp = ControlSkeletal->Positions[PoseLandmark];
					SKArray[PoseLandmark] = (temp - RefSKPos).Size();
				}
				else if (i < (int)EPoseLandmark::Body_MAX)
				{
					EPoseLandmark PoseLandmark_root = static_cast<EPoseLandmark>(i - 2);
					SKArray[PoseLandmark] = (ControlSkeletal->Positions[PoseLandmark] - ControlSkeletal->Positions[PoseLandmark_root]).Size();
				}
				else if (i >= (int)EPoseLandmark::WRIST_L && i < (int)EPoseLandmark::Hand_L_MAX)
				{
					int temp_point = 2, fingerNum;
					fingerNum = i - (int)(EPoseLandmark::WRIST_L);
					if (fingerNum >= 5)
					{
						temp_point = temp_point + 3 + ((fingerNum - 1) / 4 - 1) * 4;
					}
					if (fingerNum == temp_point || fingerNum == 1)
					{
						EPoseLandmark PoseLandmark_root = static_cast<EPoseLandmark>(EPoseLandmark::Hand_L);
						SKArray[PoseLandmark] = (ControlSkeletal->Positions[PoseLandmark] - ControlSkeletal->Positions[PoseLandmark_root]).Size();
						EPoseLandmark PoseLandmark_other = static_cast<EPoseLandmark>((int)EPoseLandmark::WRIST_R + fingerNum);
						SKArray[PoseLandmark_other] = SKArray[PoseLandmark];
					}
					else
					{
						if (ControlSkeletal->Transforms.Contains(PoseLandmark))
						{
							EPoseLandmark PoseLandmark_root = static_cast<EPoseLandmark>(i - (fingerNum - temp_point));
								SKArray[PoseLandmark] = (ControlSkeletal->Transforms[PoseLandmark].GetLocation() - ControlSkeletal->Positions[PoseLandmark_root]).Size();
								EPoseLandmark PoseLandmark_other = static_cast<EPoseLandmark>((int)EPoseLandmark::WRIST_R + fingerNum);
								SKArray[PoseLandmark_other] = SKArray[PoseLandmark];
						}

					}

				}

			}

		}

	}
}
/*FVector AActor_AvatarMeetAlgorithm::FingerRotator(int fingerNum,FVector WebcamHand,int hand)
{
	FVector Temp,TipDipVector;
	Temp = WebcamReader->GetHandsLandmarkCoords(hand, fingerNum);
	Temp.Z = Temp.Z + WebcamHand.Z;
	//save finger tip data for adjust tip degree
	TipDipVector = WebcamReader->GetHandsLandmarkCoords(hand, fingerNum+1);
	TipDipVector.Z = TipDipVector.Z + WebcamHand.Z;

	TipDipVector = (TipDipVector - Temp).GetSafeNormal();
	float TipRotationDegreeY = (180.f) / PI * FMath::Asin(TipDipVector.Y);
	float TipRotationDegreeX = (180.f) / PI * FMath::Asin(TipDipVector.X);
	float TipRotationDegreeZ = (180.f) / PI * FMath::Asin(TipDipVector.Z);

	return FVector(FVector(TipDipVector));
}*/

FVector AActor_AvatarMeetAlgorithm::WebCal(FVector Temp)
{
	// TODO:: Calculate WEB position
	float rate=30;//to Skeletal x:y:z~=[4:1:6]
	Temp.X = (Temp.X - WebcamUpperarmCenter.X) ;
	float X = Temp.X * rate*4;
	Temp.Y = (Temp.Y - WebcamUpperarmCenter.Y +  (WebcamEarCenter-WebcamUpperarmCenter).Size())  / WebcamReader->ResizeDeminsions.X * WebcamReader->ResizeDeminsions.Y;
	float Z = (1 - Temp.Y) * rate*6;
	Temp.Z = Temp.Z - WebcamUpperarmCenter.Z;//for meeting mode
	float Y = (-1 * Temp.Z) * rate;
	return FVector(FVector(X, Y, Z));
}


FRotator AActor_AvatarMeetAlgorithm::CalFaceOrientation_new(FVector WebHead)
{
	FVector Toward = WebHead - WebcamEarCenter;
	FVector BodyFace = FVector(0, 0.7, 0);
	Toward.Y = -Toward.Y;
	FRotator FaceRotator_con;
	BodyFace.Y = -BodyFace.Y;
	FVector BodyFace_T = FVector(BodyFace.X, 0, BodyFace.Y);
	FVector Toward_T = FVector(Toward.X, 0, Toward.Z);//FVector(Toward.X, BodyFace.Y, Toward.Z)
	//Pitch
	float tempP = (FVector::DotProduct(BodyFace_T, Toward_T)) / (BodyFace_T.Size() * Toward_T.Size());
	tempP = FMath::Acos(tempP) * 180.0 / PI;
	BodyFace_T.Normalize();
	Toward_T.Normalize();
	float ans_angle = FVector::DotProduct((BodyFace_T - Toward_T), (WebcamUpperarm_L - WebcamUpperarm_R));
	if (ans_angle < 0)
		tempP = -tempP;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("tempP:%f,ans_angle:%f"), tempP, ans_angle));
	//Yaw
	BodyFace_T = FVector(0, BodyFace.Z, BodyFace.Y);
	Toward_T = FVector(0, Toward.Y, Toward.Z);
	float tempY = (FVector::DotProduct(BodyFace_T, Toward_T)) / (BodyFace_T.Size() * Toward_T.Size());
	tempY = FMath::Acos(tempY) * 180.0 / PI;
	BodyFace_T.Normalize();
	Toward_T.Normalize();
	ans_angle = FVector::DotProduct((BodyFace_T - Toward_T), (WebHead - (WebcamUpperarm_L + WebcamUpperarm_R) * 0.5f));
	if (tempY > 90)
		tempY = 90;
	if (ans_angle < 0)
		tempY = -tempY;
	FaceRotator_con.Pitch = FirstConRotator.Pitch + tempY * 1.5f;//上下轉 + tempY
	FaceRotator_con.Yaw = FirstConRotator.Yaw + tempP * 1.5f;//+ tempP;//左右轉
	FaceRotator_con.Roll = FirstConRotator.Roll + 0;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("tempP:%f,tempY:%f"), tempP, tempY));
	////// TODO::draw head line
	/*
	ULineBatchComponent* const LineBatchComponent = GetWorld()->PersistentLineBatcher;//DrawLine objection
	LineBatchComponent->BatchedLines.Empty();
	lines.Empty();
	BodyFace.Normalize();
	FVector startL,endL;
	startL = FVector(250,720, -20);//(-150, 400, 190);
	end2 = startL + FVector(0, BodyFace.Y, BodyFace.Z) * 100;//FVector end = startL + FVector(0, BodyFace.Y, BodyFace.Z) * 100;
	FBatchedLine line = FBatchedLine(startL,
		endL,
		FLinearColor(1, 0, 0, 1),  //设置颜色、透明度
		2, // 设置显示时间。设为0，表示永久显示
		4, //Thickness
		4
	);
	lines.Add(line);
	Toward.Normalize();
	end2 = startL + FVector(Toward.X, Toward.Z, 0) * 100;//end = startL + FVector(0, Toward.Z, Toward.Y) * 100;
	line = FBatchedLine(startL,
		endL,
		FLinearColor(0, 0, 1, 1),  //设置颜色、透明度
		2, // 设置显示时间。设为0，表示永久显示
		4, //Thickness
		4
	);
	lines.Add(line);
	end2 = startL + FVector(0, Toward.Z, Toward.Y) * 100;//end = startL + FVector(0, Toward.Z, Toward.Y) * 100;
	line = FBatchedLine(startL,
		endL,
		FLinearColor(0, 1, 0, 1),  //设置颜色、透明度
		2, // 设置显示时间。设为0，表示永久显示
		4, //Thickness
		4
	);
	lines.Add(line);
	LineBatchComponent->DrawLines(lines);
	*/
	return FRotator(FaceRotator_con);
}

void AActor_AvatarMeetAlgorithm::CalArray(EPoseLandmark PoseLandmark, int i)
{
	// TODO:: Calculate WEB BODY Vector
	if (i == (int)EPoseLandmark::Head)
	{
		FTransform ConTran;
		WebArrayVector[PoseLandmark] = RefSKPos - WebPosition[PoseLandmark];//*0.66;
		WebArray[PoseLandmark] = (WebArrayVector[PoseLandmark]).Size();
		WebArrayVector[PoseLandmark] = WebArrayVector[PoseLandmark] * (SKArray[PoseLandmark]) / (WebArray[PoseLandmark]);
		AvatarControl[PoseLandmark] = RefSKPos - WebArrayVector[PoseLandmark];
		ConTran.SetLocation(FMath::Lerp(ControlSkeletal->Transforms[PoseLandmark].GetLocation(), AvatarControl[PoseLandmark], 0.35f));
		if ((WebcamEarCenter.X > 0.0) && (WebcamEarCenter.Y > 0.0) && (WebcamEarCenter.X < 1.0) && (WebcamEarCenter.Y < 1.0))
		{
			FRotator Head_rotator = CalFaceOrientation_new(WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Head));
			FQuat ConHead_ro2 = Head_rotator.Quaternion();
			ConTran.SetRotation(FMath::Lerp(ControlSkeletal->Transforms[PoseLandmark].GetRotation(), ConHead_ro2, 0.35f));
			//FRotator HeadOrientation =CalFaceOrientation(WebcamEarCenter,PoseLandmark);
		}
		else
			ConTran.SetRotation(ControlSkeletal->Transforms[PoseLandmark].GetRotation());
		ConTran.SetScale3D(FVector(1, 1, 1));
		ControlSkeletal->Transforms[PoseLandmark] = ConTran;
	}
	else if (i > (int)EPoseLandmark::Head && i < (int)EPoseLandmark::Body_MAX)
	{
		if (i == (int)EPoseLandmark::Upperarm_L || i == (int)EPoseLandmark::Upperarm_R)
		{
			WebArrayVector[PoseLandmark] = RefSKPos - WebPosition[PoseLandmark];
			WebArray[PoseLandmark] = (WebArrayVector[PoseLandmark]).Size();
			WebArrayVector[PoseLandmark] = WebArrayVector[PoseLandmark] * (SKArray[PoseLandmark]) / (WebArray[PoseLandmark]);
			AvatarControl[PoseLandmark] = RefSKPos - WebArrayVector[PoseLandmark];
			//TrackMeshArray1[i]->SetRelativeLocation(AvatarControl[PoseLandmark]);
		}
		else
		{
			EPoseLandmark PoseLandmark_temp = static_cast<EPoseLandmark>(i - 2);
			WebArrayVector[PoseLandmark] = WebPosition[PoseLandmark_temp] - WebPosition[PoseLandmark];
			WebArray[PoseLandmark] = (WebArrayVector[PoseLandmark]).Size();
			WebArrayVector[PoseLandmark] = WebArrayVector[PoseLandmark] * (SKArray[PoseLandmark]) / (WebArray[PoseLandmark]);
			AvatarControl[PoseLandmark] = AvatarControl[static_cast<EPoseLandmark>(i - 2)] - WebArrayVector[PoseLandmark];
			//TrackMeshArray1[i]->SetRelativeLocation(AvatarControl[PoseLandmark]);
		}
		if(i == (int)EPoseLandmark::Hand_L || i == (int)EPoseLandmark::Hand_R)
			ControlSkeletal->Positions[PoseLandmark] = FMath::Lerp(ControlSkeletal->Positions[PoseLandmark], AvatarControl[PoseLandmark], 0.35f);
	}
	else if (i >= (int)EPoseLandmark::WRIST_L && i < (int)EPoseLandmark::Hand_L_MAX)
	{
		// TODO:: Calculate Finger left Vector
		int temp_point = 2;
		int fingerNum = i - (int)(EPoseLandmark::WRIST_L);
		if (fingerNum >= 5)
		{
			temp_point = temp_point + 3 + ((fingerNum - 1) / 4 - 1) * 4;
		}
		if (fingerNum == temp_point)
		{
			//WebArrayVector[PoseLandmark]=WebPosition[PoseLandmark];
			//WebPosition[PoseLandmark]=WebPosition[PoseLandmark]+WebPosition[EPoseLandmark::Hand_L];
			WebArrayVector[PoseLandmark] = WebPosition[EPoseLandmark::Hand_L] - WebPosition[PoseLandmark];
			WebArray[PoseLandmark] = (WebArrayVector[PoseLandmark]).Size();
			WebArrayVector[PoseLandmark] = WebArrayVector[PoseLandmark] * (SKArray[PoseLandmark]) / (WebArray[PoseLandmark]);
			AvatarControl[PoseLandmark] = AvatarControl[EPoseLandmark::Hand_L] - WebArrayVector[PoseLandmark];
			//ControlSkeletal->Positions[PoseLandmark] = FMath::Lerp(ControlSkeletal->Positions[PoseLandmark], AvatarControl[PoseLandmark], 0.35f);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("l=>%d  av%s"),i,*AvatarControl[PoseLandmark].ToString()));
		}
		else
		{
			//WebPosition[PoseLandmark]=WebPosition[PoseLandmark]+WebPosition[EPoseLandmark::Hand_L];
			EPoseLandmark PoseLandmark_temp = static_cast<EPoseLandmark>((int)(EPoseLandmark::WRIST_L)+temp_point);
			WebArrayVector[PoseLandmark] = WebPosition[PoseLandmark_temp] - WebPosition[PoseLandmark];
			WebArray[PoseLandmark] = WebArray[PoseLandmark_temp] * (SKArray[PoseLandmark] / SKArray[PoseLandmark_temp]);
			WebArrayVector[PoseLandmark] = WebArrayVector[PoseLandmark] * (SKArray[PoseLandmark]) / (WebArray[PoseLandmark]);
			AvatarControl[PoseLandmark] = AvatarControl[PoseLandmark_temp] - WebArrayVector[PoseLandmark];
			if (ControlSkeletal->Transforms.Contains(PoseLandmark))
			{
				FTransform ConTran;
				ConTran.SetLocation(FMath::Lerp(ControlSkeletal->Transforms[PoseLandmark].GetLocation(), AvatarControl[PoseLandmark], 0.35f));
				ConTran.SetScale3D(FVector(1, 1, 1));
				// TODO::WEBCAM head rotation
				FVector temp_con = (WebcamReader->GetHandsLandmarkCoords(LEFT_HAND, fingerNum + 1) - WebcamReader->GetHandsLandmarkCoords(LEFT_HAND, fingerNum));
				temp_con.Normalize();
				ConTran.SetRotation(FQuat(temp_con, PI / 2));
				ControlSkeletal->Transforms[PoseLandmark] = ConTran;
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("L=>%d,%s "), i, *AvatarControl[PoseLandmark].ToString()));
			}
		}
	}
	else if (i >= (int)EPoseLandmark::WRIST_R && i < (int)EPoseLandmark::Hand_R_MAX)
	{
		// TODO:: Calculate Finger right Vector
		int temp_point = 2;
		int fingerNum = i - (int)(EPoseLandmark::WRIST_R);
		if (fingerNum >= 5)
		{
			temp_point = temp_point + 3 + ((fingerNum - 1) / 4 - 1) * 4;
		}
		if (fingerNum == temp_point)
		{
			WebArrayVector[PoseLandmark] = WebPosition[EPoseLandmark::Hand_R] - WebPosition[PoseLandmark];
			WebArray[PoseLandmark] = (WebArrayVector[PoseLandmark]).Size();
			WebArrayVector[PoseLandmark] = WebArrayVector[PoseLandmark] * (SKArray[PoseLandmark]) / (WebArray[PoseLandmark]);
			AvatarControl[PoseLandmark] = AvatarControl[EPoseLandmark::Hand_R] - WebArrayVector[PoseLandmark];
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("r=>%d  av%s=>%s"),i,*AvatarControl[PoseLandmark].ToString(), *ControlSkeletal->Positions[EPoseLandmark::Hand_R].ToString()));
		}
		else
		{
			EPoseLandmark PoseLandmark_temp = static_cast<EPoseLandmark>((int)(EPoseLandmark::WRIST_R)+temp_point);
			WebArrayVector[PoseLandmark] = WebPosition[PoseLandmark_temp] - WebPosition[PoseLandmark];
			WebArray[PoseLandmark] = WebArray[PoseLandmark_temp] * (SKArray[PoseLandmark] / SKArray[PoseLandmark_temp]);
			WebArrayVector[PoseLandmark] = WebArrayVector[PoseLandmark] * (SKArray[PoseLandmark]) / (WebArray[PoseLandmark]);
			AvatarControl[PoseLandmark] =  AvatarControl[PoseLandmark_temp] - WebArrayVector[PoseLandmark];
			if (ControlSkeletal->Transforms.Contains(PoseLandmark))
			{
				FTransform ConTran;
				ConTran.SetLocation(FMath::Lerp(ControlSkeletal->Transforms[PoseLandmark].GetLocation(), AvatarControl[PoseLandmark], 0.35f));
				ConTran.SetScale3D(FVector(1, 1, 1));
				FVector temp_con = (WebcamReader->GetHandsLandmarkCoords(RIGHT_HAND, fingerNum + 1) - WebcamReader->GetHandsLandmarkCoords(RIGHT_HAND, fingerNum));
				temp_con.Normalize();
				ConTran.SetRotation(FQuat(temp_con, -PI / 2));
				ControlSkeletal->Transforms[PoseLandmark] = ConTran;
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("R=>%d,%s "), i, *AvatarControl[PoseLandmark].ToString()));
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%d=>%s  rate%f"), i,*AvatarControl[PoseLandmark].ToString(),WebArray[PoseLandmark]));
	
}

// Called every frame
void AActor_AvatarMeetAlgorithm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	start = (double)getTickCount();

	if (ControlSkeletal && WebcamReader->HasPoseLandmarkCoords())
	{
		WebcamUpperarm_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Upperarm_L);
		WebcamUpperarm_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Upperarm_R);

		const FVector WebcamHand_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hand_L);
		const FVector WebcamHand_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::Hand_R);

		const FVector WebcamEar_L = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::EAR_L);
		const FVector WebcamEar_R = WebcamReader->GetPoseLandmarkCoords((int)EPoseLandmark::EAR_R);

		WebcamEarCenter = ((WebcamEar_L + WebcamEar_R) * 0.5f);
		WebcamUpperarmCenter = WebcamUpperarm_R + ((WebcamUpperarm_L - WebcamUpperarm_R) * 0.5f);
		
		

		for (int i = 0; i < (int)EPoseLandmark::Max; ++i)
		{

			UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPoseLandmark"));
			if (enumPtr->GetNameStringByValue(i).IsEmpty())
				continue;

			EPoseLandmark PoseLandmark = static_cast<EPoseLandmark>(i);

			if (ControlSkeletal->Positions.Contains(PoseLandmark) || ControlSkeletal->Transforms.Contains(PoseLandmark))
			{
				FVector Temp;
				if (i > (int)EPoseLandmark::WRIST_L && i <= (int)EPoseLandmark::Hand_L_MAX)//left hand
				{
					int fingerNum = i - (int)(EPoseLandmark::WRIST_L);
					//Temp = (WebcamReader->GetHandsLandmarkCoords(LEFT_HAND, fingerNum) );
                    				
					Temp = WebcamReader->GetHandsLandmarkCoords(LEFT_HAND, fingerNum);
					Temp.Z = Temp.Z + WebcamHand_L.Z;

				}
				else if (i > (int)EPoseLandmark::WRIST_R && i <= (int)EPoseLandmark::Hand_R_MAX) //right hand
				{
					int fingerNum = i - (int)(EPoseLandmark::WRIST_R);
					//Temp = (WebcamReader->GetHandsLandmarkCoords(RIGHT_HAND, fingerNum) - WebcamReader->GetHandsLandmarkCoords(RIGHT_HAND, 0));
					
					Temp = WebcamReader->GetHandsLandmarkCoords(RIGHT_HAND, fingerNum);
					Temp.Z = Temp.Z + WebcamHand_R.Z;

				}
				else if (i > (int)EPoseLandmark::Head && i <= (int)EPoseLandmark::Body_MAX)
				{
					Temp = WebcamReader->GetPoseLandmarkCoords(i);
				}
				else if (i == (int)EPoseLandmark::Head)
				{
					Temp = (WebcamEarCenter - (WebcamReader->GetPoseLandmarkCoords(i) - WebcamEarCenter) * 0.5);
				}

				if ((Temp.X > 0.0) && (Temp.Y > 0.0) && (Temp.X < 1.0) && (Temp.Y < 1.0))
				{
					WebPosition[PoseLandmark] = WebCal(Temp);
					CalArray(PoseLandmark, i);
				}

			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("eye-shoulder=>%s  rate%f"), *(WebcamEarCenter-WebcamUpperarmCenter).ToString(),(WebcamEarCenter-WebcamUpperarmCenter).Size()));

	end = (double)getTickCount();
	double fps = 1, diff = 1;
	std::ostringstream str;
	diff = (end - start) / getTickFrequency();
	//UE_LOG(LogTemp, Warning, TEXT("time diff %f"), diff);
	fps = 1 / diff;
	str << "FPS : " << fps;
}