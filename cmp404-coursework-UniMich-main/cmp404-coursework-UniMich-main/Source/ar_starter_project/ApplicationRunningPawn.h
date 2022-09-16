// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Bullet.h"
#include "Anchor.h"
#include "UIWidget.h"
#include "ARPlaneActor.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "MenuClass.h"
#include "BaseEnemy.h"
#include "BaseLand.h"
#include "BaseDefence.h"
#include "Blueprint/UserWidget.h"
#include "Portal.h"
#include "Castle.h"
#include "ARPin.h"
#include "ARSessionConfig.h"
#include "Math/TransformNonVectorized.h"
#include "ApplicationRunningPawn.generated.h"

//Class that acts as the player in the application and is responsible for most functions
UCLASS()
class AR_STARTER_PROJECT_API AApplicationRunningPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AApplicationRunningPawn();

	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "CamComp", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* CameraComponent;

	//Setup for audio components
	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		USoundCue* backgroundAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		UAudioComponent* backgroundAudioComponent;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		USoundCue* spawnAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere, BlueprintReadWrite)
		UAudioComponent* spawnAudioComponent;

	//Setup for widget components - Menu
	UPROPERTY(Category = "Type", BlueprintReadWrite)
		class UMenuClass* menu;

	TSubclassOf<class UUserWidget> menuClass;

	//Setup for widget components - UI
	UPROPERTY(Category = "UI", BlueprintReadWrite)
		class UUIWidget* UI;

	TSubclassOf<class UUserWidget> UIClass;

	virtual void PostInitializeComponents() override;



protected:

	//Member functions
	void UpdatePlaneActors();
	AARPlaneActor* SpawnPlaneActor();
	FColor GetPlaneColor(int index);
	void ResetARCoreSession();

	virtual void OnScreenTouch(const ETouchIndex::Type fingerIndex, const FVector screenPos);
	virtual void OnScreenHeld(const ETouchIndex::Type fingerIndex, const FVector screenPo);
	virtual void OnScreenReleased(const ETouchIndex::Type fingerIndex, const FVector screenPo);
	bool WorldHitTest(const FVector2D& screenPos, FHitResult& hitResult);

	void SpawnAnchor(const FVector screenPo);
	void SpawnLand();
	void SpawnDefence(const FVector screenPo);
	void SpawnCastle(const FVector screenPo);
	void SpawnPortal(const FVector screenPo);
	void SpawnEnemies();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Backup config incase menu widget fails to start
	UARSessionConfig* Config;

	//Base plane actor for geometry detection
	AARPlaneActor* planeActor;

	//Map of geometry planes
	TMap<UARPlaneGeometry*, AARPlaneActor*> PlaneActors;

	//Index for plane colours adn array of colours
	int planeIndex = 0;
	TArray<FColor> PlaneColors;

	//Pawn/Actor Arrays
	TArray<ABaseDefence*> defences;
	TArray<ABaseLand*> lands;
	TArray<ABaseEnemy*> enemies;

	//Actors for single entities 
	AAnchor* anchor;
	ACastle* castle;
	APortal* portal;
	
	//Bool variables
	bool actorSelected = false;
	bool anchorSet = false, portalSet = false, castleSet = false;
	bool anchorSpawned = false, portalSpawned = false, castleSpawned = false, landSpawned = false;

	//Not used but would hold the selected actor
	AActor* selectedActor;

	//Number variables
	float spawnTimer = 0, spawnCD = 3;
	int playerGold = 20;
	
	//World rotation variable
	FRotator worldRotation;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
