// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Scene component setup
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//Mesh component setup with collision
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	StaticMeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Constructor helpers to get assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>MaterialAsset(TEXT("Material'/Game/Mats/Yellow.Yellow'"));

	//Sets the mesh and material 
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	StaticMeshComponent->SetMaterial(0, MaterialAsset.Object);
	SetRootComponent(StaticMeshComponent);


}

//Sets the enemy taregt
void ABullet::SetTarget(ABaseEnemy& newTarget)
{
	target = &newTarget;
}

//Sets the damage the attack does
void ABullet::SetDamage(int newDamage)
{
	damage = newDamage;
}

//Basic spawn function
ABullet* ABullet::Spawn(UWorld* world)
{
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);
	FVector myLoc(0, 0, 0);

	ABullet* bullet = world->SpawnActor<ABullet>(myLoc, myRot, SpawnInfo);

	return bullet;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	alpha += DeltaTime;

	//Function to update the anchor each frame - stops the actors from drifting/disappearing in AR
	if (PinComponent->IsValidLowLevelFast())
	{
		if (PinComponent->GetTrackingState() == EARTrackingState::Tracking)
		{

			StaticMeshComponent->SetVisibility(true);
			auto tf = PinComponent->GetLocalToWorldTransform();
			tf.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
			tf.AddToTranslation(FVector(0, 0, 7));
			auto newTF = UKismetMathLibrary::TLerp(tf, target->GetTransform(), alpha);
			newTF.SetScale3D(FVector(0.05f, 0.05f, 0.05f));
			newTF.AddToTranslation(FVector(0, 0, 7));
			SetActorTransform(newTF);
		}

	}

	
	//if the target is destroyed or the target transform has been reached with no collision destory the attack
	if (target->IsPendingKill() || alpha >= 1)
	{
		this->Destroy();
	}

	//If the actor reaches the target
	if (IsOverlappingActor(target))
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("bullet hit")), true, true, FLinearColor(0, 0.66, 1, 1), 5);
		target->Damage(damage);
		this->Destroy();
	}

}

