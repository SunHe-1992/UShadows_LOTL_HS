// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "US_Follower.generated.h"
UCLASS()
class USHADOWS_LOTL_HS_API AUS_Follower : public ACharacter
{
	GENERATED_BODY()

public:
	AUS_Follower();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	FVector PositionOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float FollowSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float ForwardOffset = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float RightOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float UpOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float SurroundRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float SurroundAngleDegrees = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float SurroundAngleDegreeSpeed = 50.0f;

	int followFormation = 1;
	void SetMaster(ACharacter* master);
private:
	UPROPERTY()
	ACharacter* TargetCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess
		= "true"))
	TObjectPtr<class USphereComponent> Collision;
protected:
	virtual void BeginPlay() override;
	FVector CalculatePointOnCircle(const FVector& Center, float Radius, float AngleDegrees);
};

