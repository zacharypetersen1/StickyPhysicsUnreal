// Fill out your copyright notice in the Description page of Project Settings.


#include "Engine.h"
#include "StickyTriangle.h"
#include "DrawDebugHelpers.h"
#include "Components/SceneComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

FStickyTriangle::FStickyTriangle()
{
}

FStickyTriangle::FStickyTriangle(FVector VertexPositionA, FVector VertexPositionB, FVector VertexPositionC, const USceneComponent* SetOwningComponent)
{
	ObjectVertexPositions.Add(VertexPositionA);
	ObjectVertexPositions.Add(VertexPositionB);
	ObjectVertexPositions.Add(VertexPositionC);
	ObjectFaceNormal = (VertexPositionA ^ VertexPositionB).GetSafeNormal();
	OwningComponent = SetOwningComponent;
}

FVector FStickyTriangle::GetVertexWorldPosition(int Index)
{
	if (!IsValid(OwningComponent))
		return FVector::ZeroVector;

	return OwningComponent->GetComponentTransform().TransformPosition(ObjectVertexPositions[Index]);
}

FVector FStickyTriangle::GetVertexWorldNormal(int Index)
{
	if (!IsValid(OwningComponent))
		return FVector::ZeroVector;

	return OwningComponent->GetComponentTransform().TransformVectorNoScale(ObjectVertexNormals[Index]);
}

FVector FStickyTriangle::GetWorldFaceNormal()
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting Normal Check...."));
	if (!IsValid(OwningComponent))
		return FVector::ZeroVector;

	UE_LOG(LogTemp, Warning, TEXT("FaceNormal:%s"), *ObjectFaceNormal.ToString());
	return OwningComponent->GetComponentTransform().TransformVectorNoScale(ObjectFaceNormal);
}

FVector FStickyTriangle::GetWorldInterpolatedNormal(const FVector &AtThisLocation)
{
	FVector BaryCentricCoords = FMath::ComputeBaryCentric2D(
		AtThisLocation,
		GetVertexWorldPosition(0),
		GetVertexWorldPosition(1),
		GetVertexWorldPosition(2)
	);
	return GetVertexWorldNormal(0) * BaryCentricCoords[0] +
		GetVertexWorldNormal(1) * BaryCentricCoords[0] +
		GetVertexWorldNormal(2) * BaryCentricCoords[0];
}

FVector FStickyTriangle::ProjectDirectionOntoTriangle(const FVector &Direction)
{
	return FVector::VectorPlaneProject(Direction, ObjectFaceNormal);
}

FVector FStickyTriangle::ProjectLocationOntoTriangle(const FVector &Location)
{
	return FVector::VectorPlaneProject(Location - ObjectVertexPositions[0], ObjectFaceNormal) + ObjectVertexPositions[0];
}

bool FStickyTriangle::ProjectRayOntoBoundaries(FVector &ResultIntersectionPoint, const FRay &Ray)
{
	float ShortestDistSquared = INFINITY;
	bool bFoundValidIntersection = false;

	// Construct plane that lies on each edge of triangle and is perpendicular to triangle and intersect ray with it
	for (int i = 0; i < 3; i++)
	{
		FVector VertPositionI = GetVertexWorldPosition(i);
		FPlane Plane = FPlane(VertPositionI, GetVertexWorldPosition((i + 1) % 3), VertPositionI + GetWorldFaceNormal());
		
		// Check if Ray is orthogonal to line
		if (!FVector::Orthogonal(Plane, Ray.Direction))
		{
			FVector IntersectionPoint = FMath::RayPlaneIntersection(Ray.Origin, Ray.Direction, Plane);
			DrawDebugPoint(GEngine->GetWorldContexts()[0].World(), IntersectionPoint, 25, FColor::Orange);
			float DistSquared = FVector::DistSquared(IntersectionPoint, Ray.Origin);
			if (DistSquared < ShortestDistSquared && (Ray.Direction | (IntersectionPoint - Ray.Origin)) > 0)
			{
				bFoundValidIntersection = true;
				ShortestDistSquared = DistSquared;
				ResultIntersectionPoint = IntersectionPoint;
			}
		}
	}
	return bFoundValidIntersection;
}

void FStickyTriangle::DebugDraw(const UWorld* World, const FColor &Color)
{
	for (int i = 0; i < 3; i++)
	{
		FTransform OwnerTransform = OwningComponent->GetComponentTransform();
		DrawDebugLine(World, 
			GetVertexWorldPosition(i), 
			GetVertexWorldPosition((i + 1) % 3),
			Color, false, -1.0f, 0, 5.0f);
	}
}

bool FStickyTriangle::TraceForTriangle(FStickyTriangle& Result, FVector& IntersectionPoint, const UWorld* World, const FVector& TraceStart, const FVector& TraceEnd, ECollisionChannel Channel)
{
	if (!World) return false;

	static FCollisionQueryParams Params;
	Params.bReturnFaceIndex = true;
	Params.bTraceComplex = true;

	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, Channel, Params))
	{
		const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(HitResult.Component);
		if (StaticMeshComponent)
		{
			const UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();

			if (!StaticMesh)
				return false;
			if (!StaticMesh->RenderData)
				return false;
			if (StaticMesh->RenderData->LODResources.Num() == 0)
				return false;

			FStaticMeshLODResources* Resources =
				&(StaticMesh->RenderData->LODResources[StaticMesh->LODForCollision]);

			const int Index1 = Resources->IndexBuffer.GetIndex(HitResult.FaceIndex * 3);
			const int Index2 = Resources->IndexBuffer.GetIndex(HitResult.FaceIndex * 3 + 1);
			const int Index3 = Resources->IndexBuffer.GetIndex(HitResult.FaceIndex * 3 + 2);

			const FStaticMeshVertexBuffers* VertexBuffers = &(Resources->VertexBuffers);

			const FVector VertexPosition1 = VertexBuffers->PositionVertexBuffer.VertexPosition(Index1);
			const FVector VertexPosition2 = VertexBuffers->PositionVertexBuffer.VertexPosition(Index2);
			const FVector VertexPosition3 = VertexBuffers->PositionVertexBuffer.VertexPosition(Index3);

			Result = FStickyTriangle(VertexPosition1, VertexPosition2, VertexPosition3, StaticMeshComponent);
			IntersectionPoint = HitResult.ImpactPoint;
			return true;
		}
	}
	return false;
}
