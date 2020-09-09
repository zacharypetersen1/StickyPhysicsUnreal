// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyPhysicsComponent.h"
#include "GameFramework/Actor.h"
#include "Engine.h"

// Sets default values for this component's properties
UStickyPhysicsComponent::UStickyPhysicsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UStickyPhysicsComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UStickyPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector IntersectionPoint;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (FStickyTriangle::TraceForTriangle(
		Triangle,
		IntersectionPoint,
		GetWorld(),
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation() + FVector(0, 0, -50000),
		ECollisionChannel::ECC_GameTraceChannel1))
	{
		DrawDebugPoint(GetWorld(), IntersectionPoint, 25, FColor::Blue);
		Triangle.DebugDraw(GetWorld(), FColor::Green);
		FVector BoundaryIntersectionPoint;
		Triangle.ProjectRayOntoBoundaries(BoundaryIntersectionPoint, FRay(IntersectionPoint, Triangle.ProjectDirectionOntoTriangle(GetOwner()->GetActorForwardVector())));
		DrawDebugPoint(GetWorld(), BoundaryIntersectionPoint, 25, FColor::Red);
		UE_LOG(LogTemp, Warning, TEXT("Intersection:%s"), *BoundaryIntersectionPoint.ToString());
	}
}
