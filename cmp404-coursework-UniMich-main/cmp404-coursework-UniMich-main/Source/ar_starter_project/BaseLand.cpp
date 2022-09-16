// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLand.h"

// Sets default values
ABaseLand::ABaseLand()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Scene component setup
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//Mesh component setup with collision
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Constructor helpers to get assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>BaseColourMat(TEXT("Material'/Game/Mats/Land.Land'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>SelectedColourMat(TEXT("Material'/Game/Mats/Land_Selected.Land_Selected'"));

	//Sets the material variables
	colour = BaseColourMat.Object;
	selectedColour = SelectedColourMat.Object;
	
	//Sets the mesh and material 
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetMaterial(0, colour);
	SetRootComponent(StaticMeshComponent);

}



// Called when the game starts or when spawned
void ABaseLand::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void ABaseLand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Function to update the anchor each frame - stops the actors from drifting/disappearing in AR
	if (PinComponent->IsValidLowLevelFast())
	{
		if (PinComponent->GetTrackingState() == EARTrackingState::Tracking)
		{
			StaticMeshComponent->SetVisibility(true);
			auto tf = PinComponent->GetLocalToWorldTransform();
			tf.SetScale3D(FVector(0.2f, 0.2f, 0.2f));
			SetActorTransform(tf);
		}
	}
}

// Called to bind functionality to input
void ABaseLand::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//Unsed Maxtrix functions
void ABaseLand::InitialMatrix(FVector initialPosition)
{
	initMatrix = FMatrix::Identity;
	initMatrix.M[3][0] = initialPosition.X;
	initMatrix.M[3][1] = initialPosition.Y;
	initMatrix.M[3][2] = initialPosition.Z;

}

void ABaseLand::SetOffset(FVector newTF)
{
	transMatrix = FMatrix::Identity;
	transMatrix.M[3][0] = newTF.X;
	transMatrix.M[3][1] = newTF.Y;
	transMatrix.M[3][2] = newTF.Z;
}

void ABaseLand::SetScale(FVector newScale)
{
	scaleMatrix = FMatrix::Identity;
	scaleMatrix.M[0][0] = newScale.X;
	scaleMatrix.M[1][1] = newScale.Y;
	scaleMatrix.M[2][2] = newScale.Z;
}

void ABaseLand::GenerateMatrix()
{
	transformationMatrix = scaleMatrix * transformationMatrix * initMatrix;
}

void ABaseLand::MulitplyWorldTransfrom(FTransform originTransform)
{
	finalTransform = FTransform(transformationMatrix) * originTransform;
}


