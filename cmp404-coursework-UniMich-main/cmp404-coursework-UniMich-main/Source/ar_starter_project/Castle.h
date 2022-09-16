// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPin.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "Castle.generated.h"

UCLASS()
class AR_STARTER_PROJECT_API ACastle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACastle();

	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "MeshComp", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "Pin", VisibleAnywhere, BlueprintReadWrite)
		UARPin* PinComponent;

	//Setup for audio components
	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		USoundCue* damageAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		UAudioComponent* damageAudioComponent;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		USoundCue* lossAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		UAudioComponent* lossAudioComponent;

	//Public functions accessed externally
	void Damage(int takenDamage);

	//Getter
	int GetHealth();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	//Material variables
	UMaterial* colour, * selectedColour;

	//Bool variables
	bool selected = false;

	//Number variables
	int health = 10;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
