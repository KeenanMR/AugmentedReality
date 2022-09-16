// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPin.h"
#include "Bullet.h"
#include "BaseEnemy.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Pawn.h"
#include "BaseDefence.generated.h"

//Class used as the defence in the application, in future development would be used as a base class for other defences
UCLASS()
class AR_STARTER_PROJECT_API ABaseDefence : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseDefence();


	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "MeshComp", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "Pin", VisibleAnywhere, BlueprintReadWrite)
		UARPin* PinComponent;

	//Setup for audio components
	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		USoundCue* attackAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		UAudioComponent* attackAudioComponent;

	//Public functions accessed externally
	void Attack(ABaseEnemy& target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	//Material variables
	UMaterial* colour, * selectedColour;

	//Bool variables
	bool selected = false;

	//Number variables
	int damage = 1, range = 10;
	float fireCD = 0;

	//TArray of attacks from this tower
	TArray<ABullet*> shots;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
