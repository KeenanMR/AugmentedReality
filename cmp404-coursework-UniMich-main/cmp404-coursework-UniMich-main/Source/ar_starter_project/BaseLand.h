// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPin.h"
#include "GameFramework/Pawn.h"
#include "BaseLand.generated.h"

//Class used as the land in the application, in future development would be used as a base class for other land
UCLASS()
class AR_STARTER_PROJECT_API ABaseLand : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseLand();

	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "MeshComp", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "Pin", VisibleAnywhere, BlueprintReadWrite)
		UARPin* PinComponent;




	//Setters
	UFUNCTION()
	void SetOffset(FVector newOffset);

	UFUNCTION()
		void SetScale(FVector newScale);

	//Not Used
	UFUNCTION()
		void GenerateMatrix();

	UFUNCTION()
		void MulitplyWorldTransfrom(FTransform originTransform);

	UFUNCTION()
		void InitialMatrix(FVector initialPosition);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Material variables
	UMaterial* colour, * selectedColour;

	//Bool variables
	bool selected = false;

	//Unused variabels for attempted matrix movement
	FRotator rotation;
	FMatrix transformationMatrix,scaleMatrix,rotMatrix,transMatrix,initMatrix;
	FTransform finalTransform;

		


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
