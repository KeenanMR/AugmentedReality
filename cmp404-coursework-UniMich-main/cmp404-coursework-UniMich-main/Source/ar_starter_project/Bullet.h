// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "Bullet.generated.h"

//Class used as the projectile in the application
UCLASS()
class AR_STARTER_PROJECT_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();


	//Setup for basic actor properties
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "MeshComp", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;


	UPROPERTY(Category = "Pin", VisibleAnywhere, BlueprintReadWrite)
		UARPin* PinComponent;

	//Public functions accessed externally
	void SetTarget(ABaseEnemy& newTarget);
	void SetDamage(int newDamage);
	static ABullet* Spawn(UWorld* world);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Number variables
	float alpha = 0;
	int damage = 1;

	//Pointer to the enemy that si the target
	ABaseEnemy* target;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
