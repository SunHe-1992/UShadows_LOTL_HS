// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "US_PlayerState.h"
#include "GameFramework/Character.h"
#include "US_Character.generated.h"

UCLASS()
class USHADOWS_LOTL_HS_API AUS_Character : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stealth", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPawnNoiseEmitterComponent> NoiseEmitter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Data", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable;
	struct FUS_CharacterStats* CharacterStats;
	UFUNCTION(NetMulticast, Reliable)
	void SprintStart_Client();
	UFUNCTION(NetMulticast, Reliable)
	void SprintEnd_Client();



public:
	// Sets default values for this character's properties
	AUS_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Move(const struct FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintEnd(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void Interact_Server();
private:
	UPROPERTY()
	AActor* InteractableActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UUS_WeaponProjectileComponent> Weapon;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void UpdateCharacterStats(int32 CharacterLevel);

	void Spawn(int index, int total);

	UFUNCTION(Server, Reliable)
	void SprintStart_Server();
	UFUNCTION(Server, Reliable)
	void SprintEnd_Server();

	// Getters
	/** Returns the SpringArmComponent used to connect the Camera to the character Capsule component. */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns the CameraComponent used as a main camera for the character. */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Getter for the character statistics based on the level
	FORCEINLINE FUS_CharacterStats* GetCharacterStats() const { return CharacterStats; }

	FORCEINLINE UUS_WeaponProjectileComponent* GetWeapon() const { return Weapon; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AUS_Follower> SpawnableMinions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn System")
	int32 NumMinionsAtStart = 5;
};