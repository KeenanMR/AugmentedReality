// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPin.h"
#include "GameFramework/Actor.h"
#include "Anchor.generated.h"

//Anchor class is used to create a world origin, objects are spawned in reference to this.
UCLASS()
class AR_STARTER_PROJECT_API AAnchor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnchor();

	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "MeshComp", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "Pin", VisibleAnywhere, BlueprintReadWrite)
		UARPin* PinComponent;

	//Public functions accessed externally
	void Hide();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Material variables
	UMaterial* colour,*selectedColour;

	//Bool variables
	bool selected = false, hidden = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
