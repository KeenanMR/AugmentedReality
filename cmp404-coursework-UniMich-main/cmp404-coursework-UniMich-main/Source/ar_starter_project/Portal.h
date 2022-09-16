// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPin.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

//Class used as the enemy spawn in the application
UCLASS()
class AR_STARTER_PROJECT_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "MeshComp", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "Pin", VisibleAnywhere, BlueprintReadWrite)
		UARPin* PinComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//Material variables
	UMaterial* colour, *selectedColour;

	//Bool variables
	bool selected = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
