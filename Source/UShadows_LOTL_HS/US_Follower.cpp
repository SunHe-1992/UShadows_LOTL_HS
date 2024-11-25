// Fill out your copyright notice in the Description page of Project Settings.


#include "US_Follower.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "US_Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/SphereComponent.h"
#include "US_GameMode.h"
#include "US_BasePickup.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Math/UnrealMathUtility.h"
// Sets default values
AUS_Follower::AUS_Follower()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetSphereRadius(100);
	Collision->SetupAttachment(RootComponent);
	GetCapsuleComponent()->InitCapsuleSize(60.f, 96.0f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -91.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>	SkeletalMeshAsset(TEXT("/Game/KayKit/Skeletons/skeleton_minion"));

	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

}

void AUS_Follower::SetMaster(ACharacter* master)
{
	TargetCharacter = master;
}

// Called when the game starts or when spawned
void AUS_Follower::BeginPlay()
{
	Super::BeginPlay();
	// Find the player character
	//TargetCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	//TargetCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}



void AUS_Follower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetCharacter)
	{
		SurroundAngleDegrees += DeltaTime * SurroundAngleDegreeSpeed;
		// Calculate target position with offset
		//FVector TargetLocation = TargetCharacter->GetActorLocation() + PositionOffset;

	/*	FVector TargetLocation = TargetCharacter->GetActorLocation() +
			TargetCharacter->GetActorForwardVector() * ForwardOffset +
			TargetCharacter->GetActorRightVector() * RightOffset +
			TargetCharacter->GetActorUpVector() * UpOffset;*/

		FVector TargetLocation = CalculatePointOnCircle(TargetCharacter->GetActorLocation(), SurroundRadius, SurroundAngleDegrees);

			// Interpolate towards target location
			FVector NewLocation = FMath::VInterpTo(
				GetActorLocation(),
				TargetLocation,
				DeltaTime,
				FollowSpeed
			);

		SetActorLocation(NewLocation);

		this->SetActorRotation(TargetCharacter->GetActorRotation());
	}
}

FVector AUS_Follower::CalculatePointOnCircle(const FVector& Center, float Radius, float AngleDegrees)
{
	// Convert angle to radians
	float AngleRadians = FMath::DegreesToRadians(AngleDegrees);

	// Calculate X and Y offsets
	float X = Center.X + Radius * FMath::Cos(AngleRadians);
	float Y = Center.Y + Radius * FMath::Sin(AngleRadians);

	return FVector(X, Y, Center.Z);
}