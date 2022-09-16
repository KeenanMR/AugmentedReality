// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPin.h"
#include "Castle.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Pawn.h"
#include "BaseEnemy.generated.h"

//Class used as the enemy in the application, in future development would be used as a base class for other enemy
UCLASS()
class AR_STARTER_PROJECT_API ABaseEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEnemy();

	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "MeshComp", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "Pin", VisibleAnywhere, BlueprintReadWrite)
		UARPin* PinComponent;

	//Setup for audio components
	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		USoundCue* deathAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		UAudioComponent* deathAudioComponent;

	//Public functions accessed externally
	void Damage(int takenDamage);

	//Getters and setters
	bool GetState();
	void SetCastle(ACastle& newCastle);
	void SetState(bool state);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	//Material variables
	UMaterial* colour, * selectedColour;

	//Bool variables
	bool selected = false;
	bool dying = false;

	//Number variables
	float alpha = 0;

	//Pointer to the castle which acts as the enemies target
	ACastle* castle;

	//Stats
	int health = 3, damage = 1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
