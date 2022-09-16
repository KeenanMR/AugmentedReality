// Fill out your copyright notice in the Description page of Project Settings.


#include "Anchor.h"

// Sets default values
AAnchor::AAnchor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Scene component setup
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//Mesh component setup
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Constructor helpers to get assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>BaseColourMat(TEXT("Material'/Game/Mats/Black.Black'"));

	//Sets the material variables
	colour = BaseColourMat.Object;


	//Sets the mesh and material 
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetMaterial(0, colour);
	SetRootComponent(StaticMeshComponent);

}



//Hides the anchor from vieew
void AAnchor::Hide()
{
	hidden = true;
}

// Called when the game starts or when spawned
void AAnchor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAnchor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//Function to update the anchor each frame - stops the actors from drifting/disappearing in AR
	if (PinComponent->IsValidLowLevelFast())
	{
		if (PinComponent->GetTrackingState() == EARTrackingState::Tracking)
		{
			if(!hidden)
			StaticMeshComponent->SetVisibility(true);
			else
			StaticMeshComponent->SetVisibility(false);

			auto tf = PinComponent->GetLocalToWorldTransform();
			tf.SetScale3D(FVector(0.1f, 0.1f, 0.1f));
			SetActorTransform(tf);
		}

	}

}

