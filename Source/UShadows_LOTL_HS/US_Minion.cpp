// Fill out your copyright notice in the Description page of Project Settings.


#include "US_Minion.h"
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
// Sets default values
AUS_Minion::AUS_Minion()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
	PawnSense = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSense"));
	PawnSense->SensingInterval = .8f;
	PawnSense->SetPeripheralVisionAngle(45.f);
	PawnSense->SightRadius = 1500.f;
	PawnSense->HearingThreshold = 400.f;
	PawnSense->LOSHearingThreshold = 800.f;
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
	static ConstructorHelpers::FClassFinder<AUS_BasePickup>SpawnedPickupAsset(TEXT("/Game/Blueprints/BP_GoldCoinPickup"));
	if (SpawnedPickupAsset.Succeeded())
	{
		SpawnedPickup = SpawnedPickupAsset.Class;
	}
}

// Called when the game starts or when spawned
void AUS_Minion::BeginPlay()
{
	Super::BeginPlay();
	// Make sure AI Controller is spawned
	if (GetController() == nullptr)
	{
		// Spawn default AI controller if none exists
		SpawnDefaultController();
	}
	SetNextPatrolLocation();
}
void AUS_Minion::SetNextPatrolLocation()
{
	if (GetLocalRole() != ROLE_Authority) return;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	const auto LocationFound = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
		this, GetActorLocation(), PatrolLocation, PatrolRadius);
	if (LocationFound)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
	}
}


void AUS_Minion::Chase(APawn* Pawn)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Chase got stuck!"));
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Pawn);
	DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(), 25.f, 12, FColor::Red, true, 10.f, 0, 2.f);
	if (const auto GameMode = Cast<AUS_GameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->AlertMinions(this, Pawn->GetActorLocation(), AlertRadius);
	}
}
void AUS_Minion::OnPawnDetected(APawn* Pawn)
{
	if (!Pawn->IsA<AUS_Character>()) return;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Character detected!"));
	if (GetCharacterMovement()->MaxWalkSpeed != ChaseSpeed)
	{
		Chase(Pawn);
	}
}
void AUS_Minion::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Noise detected!"));
	GoToLocation(Location);
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
}

void AUS_Minion::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor->IsA<AUS_Character>()) return;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Character captured!"));
}
void AUS_Minion::OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health -= Damage;
	if (Health > 0) return;
	if (SpawnedPickup)
	{
		GetWorld()->SpawnActor<AUS_BasePickup>(SpawnedPickup, GetActorLocation(), GetActorRotation());
	}
	Destroy();
}

void AUS_Minion::GoToLocation(const FVector& Location)
{
	PatrolLocation = Location;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
}


// Called every frame
void AUS_Minion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() != ROLE_Authority) return;
	if (GetMovementComponent()->GetMaxSpeed() == ChaseSpeed) return;
	if ((GetActorLocation() - PatrolLocation).Size() < 500.f)
	{
		SetNextPatrolLocation();
	}
}


void AUS_Minion::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (GetLocalRole() != ROLE_Authority) return;
	OnTakeAnyDamage.AddDynamic(this, &AUS_Minion::OnDamage);
	OnActorBeginOverlap.AddDynamic(this, &AUS_Minion::OnBeginOverlap);
	GetPawnSense()->OnSeePawn.AddDynamic(this, &AUS_Minion::OnPawnDetected);
	GetPawnSense()->OnHearNoise.AddDynamic(this, &AUS_Minion::OnHearNoise);
}



// Called to bind functionality to input
void AUS_Minion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
